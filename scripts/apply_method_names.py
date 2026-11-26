#!/usr/bin/env python3
"""Apply one revision's frozen method rename slice and compile it.

Reads work/method-standard/<engine>-<rev>.json. Copies each file it will
change into work/method-standard/backup/ before editing. On a failed compile,
restores those files. Does not touch fernflower, deob jars, or curated remaps.
"""
from __future__ import annotations

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
from collections import Counter, defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import jdk  # noqa: E402
import unify_method_names as um  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
STANDARD = ROOT / "work" / "method-standard"
BACKUP = STANDARD / "backup"
JAVA_HOME = jdk.java_home()
IDENT = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
SIG = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|native|synchronized|abstract|strictfp)[ \t]+)*"
    r"(?:[\w.$]+(?:[ \t]*<[^>;{}]*>)?(?:[ \t]*\[[ \t]*\])*[ \t]+)?"
    r"([A-Za-z_$][\w$]*)[ \t]*\(([^)]*)\)"
)
SKIP = {
    "if", "for", "while", "switch", "catch", "do", "try", "else", "return",
    "new", "synchronized",
}
JDK_KEEP = {
    "init", "start", "stop", "destroy", "paint", "update", "run",
    "keyPressed", "keyReleased", "keyTyped",
    "mouseClicked", "mousePressed", "mouseReleased", "mouseDragged",
    "mouseMoved", "mouseEntered", "mouseExited", "mouseWheelMoved",
    "focusGained", "focusLost",
}


def rev_dir(engine: str, rev: int) -> Path:
    hits = sorted((ROOT / "builds" / engine).glob(f"{rev}-*"))
    if not hits:
        raise SystemExit(f"no directory for {engine} {rev}")
    return hits[0]


def java_files(root: Path) -> list[Path]:
    out = []
    for path in root.rglob("*.java"):
        if "build" in path.parts or "fernflower" in path.parts or "runtime-home" in path.parts:
            continue
        out.append(path)
    return out


def source_root(engine: str, rev: int, folder: Path) -> Path:
    for rel in (Path("src/src/main/java"), Path("src/client/src/main/java")):
        p = folder / rel
        if p.is_dir():
            return p
    raise SystemExit(f"no Java sources for {engine} {rev}")


def decl_names(root: Path) -> Counter:
    counts: Counter = Counter()
    for path in java_files(root):
        for line in path.read_text(errors="ignore").splitlines():
            m = SIG.match(line)
            if not m or m.group(1) in SKIP or m.group(1)[:1].isupper():
                continue
            counts[m.group(1)] += 1
    return counts


CLASS_HEAD = re.compile(
    r"\b(?:class|interface|enum)\s+(\w+)"
    r"(?:\s+extends\s+([\w.]+))?"
    r"(?:\s+implements\s+([^{]+))?"
)
FIELD_LINE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|volatile|transient)[ \t]+)+"
    r"([\w.]+(?:\s*\[\s*\])*)\s+(\w+)\s*(?:=|;)"
)


def index_classes(root: Path) -> dict[str, dict]:
    """Simple class name -> superclass and member-field types."""
    classes: dict[str, dict] = {}
    for path in java_files(root):
        current = None
        for line in path.read_text(errors="ignore").splitlines():
            if line.strip().startswith("//"):
                continue
            head = CLASS_HEAD.search(line)
            if head and "{" in line or (head and not line.strip().endswith(";")):
                if head and "class " in line or (head and "interface " in line) or (head and "enum " in line):
                    current = head.group(1)
                    sup = head.group(2).split(".")[-1] if head.group(2) else None
                    ifaces = []
                    if head.group(3):
                        ifaces = [
                            part.split(".")[-1].strip()
                            for part in head.group(3).split(",")
                            if part.strip()
                        ]
                    classes.setdefault(current, {"supers": set(), "ifaces": set(), "fields": {}})
                    if sup:
                        classes[current]["supers"].add(sup)
                    if ifaces:
                        classes[current]["ifaces"].update(ifaces)
                    continue
            if not current:
                continue
            fm = FIELD_LINE.match(line)
            if not fm:
                continue
            ftype = fm.group(1).split(".")[-1].replace("[]", "")
            classes[current]["fields"].setdefault(fm.group(2), ftype)
    return classes


CAST_HEAD = re.compile(r"^\(\s*([\w.\[\]]+)\s*\)")


def cast_type_before(text: str, ident_at: int) -> str | None:
    """Type of `((Type)expr).method` when the receiver is not a plain identifier."""
    j = ident_at - 1
    while j >= 0 and text[j].isspace():
        j -= 1
    if j < 0 or text[j] != ".":
        return None
    j -= 1
    while j >= 0 and text[j].isspace():
        j -= 1
    if j < 0 or text[j] != ")":
        return None
    depth = 0
    k = j
    while k >= 0:
        if text[k] == ")":
            depth += 1
        elif text[k] == "(":
            depth -= 1
            if depth == 0:
                break
        k -= 1
    if k < 0 or text[k] != "(":
        return None
    match = CAST_HEAD.match(text[k + 1:j].lstrip())
    if not match:
        return None
    return match.group(1).split(".")[-1].replace("[]", "")


LOCAL_DECL = re.compile(r"(?:^|[^.\w])([A-Z][\w]*)\s+([A-Za-z_]\w*)")


def local_ranges(text: str) -> list[tuple[int, int, dict[str, str]]]:
    """Method line ranges and the local variable types declared in each one."""
    ranges: list[tuple[int, int, dict[str, str]]] = []
    lines = text.splitlines()
    depth = 0
    in_block = False
    pending = None
    pending_code = ""
    current: dict[str, str] | None = None
    current_start = 0
    body_depth = None

    def take(code: str, into: dict[str, str]) -> None:
        for match in LOCAL_DECL.finditer(code):
            typ, name = match.group(1), match.group(2)
            if name not in into:
                into[name] = typ
            elif into[name] != typ:
                into[name] = ""

    for lineno, line in enumerate(lines, start=1):
        code, in_block = code_line(line, in_block)
        if current is not None:
            take(code, current)
        elif pending is None and SIG.match(code):
            name = SIG.match(code).group(1)
            if not name[:1].isupper() and name not in SKIP:
                pending = lineno
                pending_code = code
        opened = False
        for ch in code:
            if ch == "{":
                if pending is not None and current is None:
                    current = {}
                    take(pending_code, current)
                    current_start = pending
                    body_depth = depth
                    pending = None
                    opened = True
                depth += 1
            elif ch == "}":
                depth = max(0, depth - 1)
                if current is not None and body_depth is not None and depth == body_depth:
                    ranges.append((current_start, lineno, current))
                    current = None
                    body_depth = None
        if opened and current is not None:
            brace = code.find("{")
            if brace >= 0:
                take(code[brace + 1:], current)
    return ranges


def paren_arity(text: str, end: int) -> int | None:
    """Argument count of the call or declaration that starts after an identifier."""
    n = len(text)
    k = end
    while k < n and text[k].isspace():
        k += 1
    if k >= n or text[k] != "(":
        return None
    k += 1
    depth = 0
    commas = 0
    nonempty = False
    while k < n:
        ch = text[k]
        if ch == "/" and k + 1 < n and text[k + 1] == "/":
            k = text.find("\n", k)
            k = n if k < 0 else k
            continue
        if ch in "\"'":
            quote = ch
            k += 1
            while k < n and text[k] != "\n":
                if text[k] == "\\":
                    k += 2
                    continue
                if text[k] == quote:
                    k += 1
                    break
                k += 1
            nonempty = True
            continue
        if ch == "(":
            depth += 1
        elif ch == ")":
            if depth == 0:
                return commas + 1 if nonempty else 0
            depth -= 1
        elif ch == "," and depth == 0:
            commas += 1
            nonempty = True
        elif not ch.isspace():
            nonempty = True
        k += 1
    return None


def local_type_at(ranges: list[tuple[int, int, dict[str, str]]], line: int, name: str) -> str | None:
    found = None
    for start, end, mapping in ranges:
        if start <= line <= end:
            found = mapping.get(name) or None
    return found


def unique_field_types(classes: dict) -> dict[str, str]:
    """Field name -> type, when every declaration of that name has one type."""
    types: dict[str, set[str]] = defaultdict(set)
    for info in classes.values():
        for fname, ftype in info.get("fields", {}).items():
            if ftype:
                types[fname].add(ftype)
    return {name: next(iter(found)) for name, found in types.items() if len(found) == 1}


def classes_in_text(text: str) -> list[str]:
    found = []
    for line in text.splitlines():
        if line.strip().startswith("//"):
            continue
        head = CLASS_HEAD.search(line)
        if head and ("class " in line or "interface " in line or "enum " in line):
            found.append(head.group(1))
    return found


PRIMITIVE = {"void", "int", "long", "boolean", "byte", "short", "float", "double", "char"}
RET_WORD = re.compile(
    r"([A-Za-z_][\w$]*)(?:\s*<[^>;{}]*>)?(?:\s*\[\s*\])*\s+([A-Za-z_$][\w$]*)\s*\("
)


def index_returns(root: Path) -> dict[tuple[str, str, int], str]:
    """(class, method, arity) -> simple return type, when that pair is unique."""
    found: dict[tuple[str, str, int], str] = {}
    bad: set[tuple[str, str, int]] = set()
    for path in java_files(root):
        lines = path.read_text(errors="ignore").splitlines()
        for meth in um.parse_methods(path):
            cls = class_before(lines, meth["line"]) or ""
            line = lines[meth["line"] - 1]
            rtype = None
            for cand in RET_WORD.finditer(line):
                if cand.group(2) == meth["name"]:
                    rtype = cand.group(1)
            if not rtype or rtype in PRIMITIVE:
                continue
            key = (cls, meth["name"], arity_of(meth["params"]))
            if key in found and found[key] != rtype:
                bad.add(key)
            else:
                found[key] = rtype
    for key in bad:
        found.pop(key, None)
    return found


def index_methods(root: Path) -> dict[str, dict[str, list[str]]]:
    """class -> method name -> parameter keys, in source order."""
    out: dict[str, dict[str, list[str]]] = defaultdict(lambda: defaultdict(list))
    for path in java_files(root):
        lines = path.read_text(errors="ignore").splitlines()
        for meth in um.parse_methods(path):
            cls = class_before(lines, meth["line"]) or ""
            out[cls][meth["name"]].append(meth["params"])
    return out


FN_NAME = re.compile(r"^fn([0-9a-f]{8})(b\d*)?$")
PARAM_NAME = re.compile(r"^(.*?)([a-z_][\w$]*)$")


def simple_params(params: str) -> str:
    """Ignore package qualifiers so Component and java.awt.Component match."""
    parts = []
    for raw in (params or "").split(","):
        if not raw:
            continue
        match = PARAM_NAME.match(raw)
        if not match or not match.group(1):
            parts.append(raw)
            continue
        parts.append(match.group(1).split(".")[-1] + match.group(2))
    return ",".join(parts)


def sig_key(params: str) -> str:
    """Signature key. Slice params have no spaces; source params do."""
    return re.sub(r"\s+", "", simple_params(params))


def arity_of(params: str) -> int:
    text = (params or "").strip()
    if not text:
        return 0
    return text.count(",") + 1


def normalize_proposed(name: str) -> str:
    """generify treats fn+8hex as obfuscated. The public form is call+8hex."""
    match = FN_NAME.match(name)
    if not match:
        return name
    return "call" + match.group(1) + (match.group(2) or "")


def drop_broken_overrides(by_class: dict[str, dict[str, str]], renames: list[tuple], classes: dict, methods: dict) -> int:
    """Drop a rename that would leave an override named differently from its parent.

    A parent declaration that is not taking the same name wins. The override
    keeps the name it already shares with that parent.
    """
    parents: dict[str, list[str]] = {}
    for name, info in classes.items():
        pars = list(info.get("supers") or [])
        pars.extend(info.get("ifaces") or [])
        parents[name] = pars
    explicit = {(cls, old, sig_key(params)): new for cls, old, params, new in renames}
    children: dict[str, list[str]] = defaultdict(list)
    for name, pars in parents.items():
        for parent in pars:
            children[parent].append(name)

    def declared(cls: str, old: str) -> set[str]:
        return {sig_key(p) for p in methods.get(cls, {}).get(old, [])}

    def slot_of(start: str, old: str, signature: str) -> set[str]:
        up: set[str] = set()
        stack = [start]
        while stack:
            cur = stack.pop()
            if cur in up:
                continue
            up.add(cur)
            stack.extend(parents.get(cur, []))
        declarers = {cls for cls in up if signature in declared(cls, old)}
        found = set(declarers)
        seen = set(declarers)
        stack = list(declarers)
        while stack:
            cur = stack.pop()
            for child in children.get(cur, []):
                if child in seen:
                    continue
                seen.add(child)
                stack.append(child)
                if signature in declared(child, old):
                    found.add(child)
        return found

    drop: set[tuple[str, str]] = set()
    seen_slots: set[tuple] = set()
    for cls, old, params, new in renames:
        signature = sig_key(params)
        members = frozenset(slot_of(cls, old, signature))
        slot_key = (old, signature, members)
        if slot_key in seen_slots:
            continue
        seen_slots.add(slot_key)
        names = {explicit[(c, old, signature)] for c in members if (c, old, signature) in explicit}
        if len(names) > 1:
            drop.update((c, old) for c in members)
            continue
        blocked = False
        for owner in members:
            if (owner, old, signature) not in explicit:
                continue
            stack = list(parents.get(owner, []))
            seen: set[str] = set()
            while stack:
                cur = stack.pop()
                if cur in seen:
                    continue
                seen.add(cur)
                if signature not in declared(cur, old):
                    stack.extend(parents.get(cur, []))
                    continue
                if explicit.get((cur, old, signature)) != new:
                    blocked = True
                break
            if blocked:
                break
        if blocked:
            drop.update((c, old) for c in members if (c, old, signature) in explicit)
    dropped = 0
    for cls, old in drop:
        if old in by_class.get(cls, {}):
            del by_class[cls][old]
            dropped += 1
    renames[:] = [item for item in renames if (item[0], item[1]) not in drop]
    return dropped


def propagate_overrides(by_class: dict[str, dict[str, str]], renames: list[tuple], classes: dict, methods: dict) -> int:
    """Give a subclass override the same name as the superclass method.

    Only when the subclass has a single method of that name and the same
    parameter list. Overloads stay untouched.
    """
    children: dict[str, list[str]] = defaultdict(list)
    for name, info in classes.items():
        for parent in info.get("supers") or []:
            children[parent].append(name)
        for iface in info.get("ifaces") or []:
            children[iface].append(name)
    added = 0
    for cls, old, params, new in renames:
        stack = list(children.get(cls, []))
        seen: set[str] = set()
        while stack:
            sub = stack.pop()
            if sub in seen:
                continue
            seen.add(sub)
            stack.extend(children.get(sub, []))
            overloads = methods.get(sub, {}).get(old, [])
            if sig_key(params) not in {sig_key(p) for p in overloads} or len(overloads) != 1:
                continue
            current = by_class[sub].get(old)
            if current is None:
                by_class[sub][old] = new
                added += 1
    return added


def propagate_overload_overrides(overload: dict[tuple[str, str, int], str], classes: dict, methods: dict) -> int:
    """Rename a subclass override that shares one arity of an overloaded name.

    The other arities on that subclass keep their names.
    """
    children: dict[str, list[str]] = defaultdict(list)
    for name, info in classes.items():
        for parent in list(info.get("supers") or []) + list(info.get("ifaces") or []):
            children[parent].append(name)
    added = 0
    for (cls, old, arity), new in list(overload.items()):
        signature = None
        for params in methods.get(cls, {}).get(old, []):
            if arity_of(params) == arity:
                signature = sig_key(params)
                break
        if signature is None:
            continue
        stack = list(children.get(cls, []))
        seen: set[str] = set()
        while stack:
            sub = stack.pop()
            if sub in seen:
                continue
            seen.add(sub)
            stack.extend(children.get(sub, []))
            overs = methods.get(sub, {}).get(old, [])
            if signature not in {sig_key(p) for p in overs} or len(overs) <= 1:
                continue
            key = (sub, old, arity)
            if key in overload and overload[key] != new:
                continue
            if key not in overload:
                overload[key] = new
                added += 1
    return added


CLASS_WORD = re.compile(r"\b(?:class|interface|enum)\s+([A-Za-z_][\w]*)")


def code_line(line: str, in_block: bool) -> tuple[str, bool]:
    """Drop comments, strings, and character literals so braces stay structural."""
    out = []
    i = 0
    n = len(line)
    while i < n:
        ch = line[i]
        if in_block:
            end = line.find("*/", i)
            if end < 0:
                return "".join(out), True
            i = end + 2
            in_block = False
            continue
        if ch == "/" and i + 1 < n and line[i + 1] == "/":
            break
        if ch == "/" and i + 1 < n and line[i + 1] == "*":
            in_block = True
            i += 2
            continue
        if ch == '"':
            i += 1
            while i < n:
                if line[i] == "\\":
                    i += 2
                    continue
                if line[i] == '"':
                    i += 1
                    break
                i += 1
            out.append(" ")
            continue
        if ch == "'":
            i += 1
            while i < n and line[i] != "\n":
                if line[i] == "\\":
                    i += 2
                    continue
                if line[i] == "'":
                    i += 1
                    break
                i += 1
            out.append(" ")
            continue
        out.append(ch)
        i += 1
    return "".join(out), in_block


def class_before(lines: list[str], line_no: int) -> str | None:
    """Enclosing class at line_no, ignoring classes that have already closed."""
    depth = 0
    stack: list[tuple[int, str]] = []
    pending = None
    in_block = False
    for line in lines[:line_no]:
        code, in_block = code_line(line, in_block)
        found = CLASS_WORD.search(code)
        if found:
            pending = found.group(1)
        for ch in code:
            if ch == "{":
                if pending is not None:
                    stack.append((depth, pending))
                    pending = None
                depth += 1
            elif ch == "}":
                depth = max(0, depth - 1)
                while stack and stack[-1][0] >= depth:
                    stack.pop()
                pending = None
    return stack[-1][1] if stack else None


def rewrite(text: str, accept, call_type=None) -> str:
    """Replace identifiers. accept(word, receiver) returns the new name or None.

    receiver is the identifier before a dot, or None for an unqualified name.
    call_type(word, receiver, line, arity) returns the class a call returns,
    so that ``obj.method().next()`` can see ``next`` on that class. Strings,
    character literals, and comments are copied through.
    """
    out = []
    i = 0
    n = len(text)
    prev = None
    dotted = False
    line = 1
    bracket = 0
    held: tuple | None = None
    paren_stack: list[tuple] = []
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            end = text.find("\n", i)
            end = n if end < 0 else end + 1
            out.append(text[i:end])
            line += text[i:end].count("\n")
            i = end
            prev = None
            dotted = False
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append(text[i:end])
            line += text[i:end].count("\n")
            i = end
            prev = None
            dotted = False
            continue
        if ch == '"':
            j = i + 1
            while j < n:
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == '"':
                    j += 1
                    break
                j += 1
            out.append(text[i:j])
            line += text[i:j].count("\n")
            i = j
            prev = None
            dotted = False
            continue
        if ch == "'":
            j = i + 1
            while j < n and text[j] != "\n":
                if text[j] == "\\":
                    j += 2
                    continue
                if text[j] == "'":
                    j += 1
                    break
                j += 1
            out.append(text[i:j])
            line += text[i:j].count("\n")
            i = j
            prev = None
            dotted = False
            continue
        m = IDENT.match(text, i)
        if m:
            word = m.group(0)
            receiver = prev if dotted else None
            if receiver is None and dotted:
                receiver = cast_type_before(text, i)
            arity = paren_arity(text, m.end())
            new = accept(word, receiver, line, arity)
            out.append(new if new else word)
            if bracket == 0:
                rtype = call_type(word, receiver, line, arity) if call_type and arity is not None else None
                prev = rtype or (new if new else word)
                dotted = False
            i = m.end()
            continue
        out.append(ch)
        if ch == "\n":
            line += 1
        if ch == "(" and bracket == 0:
            paren_stack.append((prev, dotted))
            prev = None
            dotted = False
            i += 1
            continue
        if ch == ")" and bracket == 0:
            if paren_stack:
                prev, dotted = paren_stack.pop()
            i += 1
            continue
        if ch == "[":
            if bracket == 0:
                held = (prev, dotted)
            bracket += 1
            i += 1
            continue
        if ch == "]":
            bracket = max(0, bracket - 1)
            if bracket == 0 and held is not None:
                prev, dotted = held
                held = None
            i += 1
            continue
        if bracket:
            i += 1
            continue
        if ch == ".":
            dotted = True
        elif not ch.isspace():
            prev = None
            dotted = False
        i += 1
    return "".join(out)


def load_slice(engine: str, rev: int) -> list[dict]:
    path = STANDARD / f"{engine}-{rev}.json"
    if not path.exists():
        return []
    return [m for m in json.loads(path.read_text()) if m.get("rename") and not m.get("skip")]


def avoid_signature_collisions(members: list[dict]) -> list[dict]:
    """Do not give a class two methods with the same name and parameter list.

    A rename that would land on a method which is staying put gets a b suffix.
    """
    by_file: dict[Path, list[dict]] = defaultdict(list)
    for mem in members:
        by_file[Path(mem["file"])].append(mem)
    for rel, group in by_file.items():
        path = ROOT / rel
        if not path.exists():
            continue
        decls = um.parse_methods(path)
        planned = {m["line"]: m for m in group}
        # Names that are already in the file and are not being renamed away.
        assigned: dict[tuple, int] = {}
        for meth in decls:
            mem = planned.get(meth["line"])
            if mem is None or mem["proposed"] == meth["name"]:
                assigned[(meth["name"], meth["params"])] = meth["line"]
        for meth in decls:
            mem = planned.get(meth["line"])
            if mem is None or mem["proposed"] == meth["name"]:
                continue
            key = (mem["proposed"], meth["params"])
            if key not in assigned or assigned[key] == meth["line"]:
                assigned[key] = meth["line"]
                continue
            base = mem["proposed"]
            n = 2
            while True:
                suffix = "b" if n == 2 else f"b{n}"
                cand = base + suffix
                if (cand, meth["params"]) not in assigned:
                    mem["proposed"] = cand
                    mem["suffix"] = suffix
                    assigned[(cand, meth["params"])] = meth["line"]
                    break
                n += 1
    return members


def apply_rev(engine: str, rev: int) -> tuple[int, list[str]]:
    folder = rev_dir(engine, rev)
    root = source_root(engine, rev, folder)
    members = avoid_signature_collisions(load_slice(engine, rev))
    suffixed = [m for m in members if m.get("suffix")]
    if suffixed:
        slice_path = STANDARD / f"{engine}-{rev}.json"
        slice_path.write_text(json.dumps(members, indent=2) + "\n")
        print(f"{engine}-{rev}: suffixed {len(suffixed)} colliding signatures")
    if not members:
        print(f"{engine}-{rev}: no renames")
        return 0, []
    counts = decl_names(root)
    notes = []
    by_class: dict[str, dict[str, str]] = defaultdict(dict)
    renames: list[tuple] = []
    for mem in members:
        old, new = mem["old"], normalize_proposed(mem["proposed"])
        mem["proposed"] = new
        if old in JDK_KEEP or old == new:
            continue
        path = ROOT / mem["file"]
        if not path.exists():
            notes.append(f"missing {path}")
            continue
        lines = path.read_text(errors="ignore").splitlines()
        cls = class_before(lines, mem["line"]) or ""
        by_class[cls][old] = new
        renames.append((cls, old, mem["params"], new))
    classes = index_classes(root)
    methods = index_methods(root)
    method_returns = index_returns(root)
    fields_unique = unique_field_types(classes)

    def lookup_return(owner: str | None, name: str, arity: int) -> str | None:
        if not owner:
            return None
        hits = []
        stack = [owner]
        seen: set[str] = set()
        while stack:
            cur = stack.pop()
            if not cur or cur in seen:
                continue
            seen.add(cur)
            rtype = method_returns.get((cur, name, arity))
            if rtype:
                hits.append(rtype)
                continue
            info = classes.get(cur, {})
            stack.extend(info.get("supers") or [])
            stack.extend(info.get("ifaces") or [])
        if len(set(hits)) == 1 and hits[0] in classes:
            return hits[0]
        return None
    dropped = drop_broken_overrides(by_class, renames, classes, methods)
    # (int) and (long) have the same argument count. A call site does not say
    # which one it is, so an arity that belongs to two signatures is left as it
    # is. A unique arity can be renamed without touching the other overloads.
    multi: list[tuple] = []
    kept: list[tuple] = []
    ambiguous = 0
    for item in renames:
        cls, old, params, new = item
        known = methods.get(cls, {}).get(old, [])
        if len(known) <= 1:
            kept.append(item)
            continue
        arity = arity_of(params)
        same = [p for p in known if arity_of(p) == arity]
        typed = [p for p in same if sig_key(p) == sig_key(params)]
        if len(same) != 1 or len(typed) != 1:
            by_class.get(cls, {}).pop(old, None)
            ambiguous += 1
            continue
        multi.append(item)
        kept.append(item)
    renames[:] = kept
    overrides = propagate_overrides(by_class, renames, classes, methods)
    overload: dict[tuple[str, str, int], str] = {}
    for cls, old, params, new in multi:
        if (cls, old, params, new) not in renames:
            continue
        arity = arity_of(params)
        key = (cls, old, arity)
        if key in overload and overload[key] != new:
            overload[key] = ""
        else:
            overload[key] = new
        by_class.get(cls, {}).pop(old, None)
    overload = {key: name for key, name in overload.items() if name}
    overrides += propagate_overload_overrides(overload, classes, methods)
    flat_unique = {
        old: new
        for mapping in by_class.values()
        for old, new in mapping.items()
        if counts[old] == 1 and len(old) > 3
    }

    def backup(path: Path) -> None:
        dest = BACKUP / path.relative_to(ROOT)
        if dest.exists():
            return
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, dest)

    changed = 0
    for path in java_files(root):
        text = path.read_text(errors="ignore")
        local = classes_in_text(text)
        fields: dict[str, str] = {}
        supers: set[str] = set()
        for cls in local:
            info = classes.get(cls, {})
            for fname, ftype in info.get("fields", {}).items():
                if fname in fields and fields[fname] != ftype:
                    fields[fname] = ""
                else:
                    fields.setdefault(fname, ftype)
            stack = list(info.get("supers") or [])
            seen = set()
            while stack:
                parent = stack.pop()
                if not parent or parent in seen:
                    continue
                supers.add(parent)
                seen.add(parent)
                stack.extend(classes.get(parent, {}).get("supers") or [])
        local_methods = {
            old
            for cls in local
            for old in by_class.get(cls, {})
        }
        ranges = local_ranges(text)

        def accept(word, receiver, line=0, arity=None, _path=path):
            if word in flat_unique and not any(key[1] == word for key in overload):
                return flat_unique[word]

            def renamed_on(owner: str | None) -> str | None:
                if not owner:
                    return None
                if arity is not None and (owner, word, arity) in overload:
                    return overload[(owner, word, arity)]
                # A blanket entry renames every arity. That is only safe when
                # the class has one method of this name.
                if word in by_class.get(owner, {}) and len(methods.get(owner, {}).get(word, [])) <= 1:
                    return by_class[owner][word]
                return None

            def inherited(owner: str | None) -> str | None:
                found = renamed_on(owner)
                if found:
                    return found
                if not owner or owner not in classes:
                    return None
                hits = []
                stack = list(classes[owner].get("supers") or [])
                stack.extend(classes[owner].get("ifaces") or [])
                seen: set[str] = set()
                while stack:
                    cur = stack.pop()
                    if not cur or cur in seen:
                        continue
                    seen.add(cur)
                    name = renamed_on(cur)
                    if name:
                        hits.append(name)
                    info = classes.get(cur, {})
                    stack.extend(info.get("supers") or [])
                    stack.extend(info.get("ifaces") or [])
                if len(set(hits)) == 1:
                    return hits[0]
                return None

            if receiver == "super":
                hits = [name for parent in supers if (name := renamed_on(parent))]
                if len(set(hits)) == 1:
                    return hits[0]
                return None
            ftype = local_type_at(ranges, line, receiver) if receiver else None
            if not ftype:
                if receiver in fields and fields[receiver]:
                    ftype = fields[receiver]
                elif receiver:
                    ftype = fields_unique.get(receiver) or None
            owner = None
            if receiver in classes and renamed_on(receiver):
                owner = receiver
            elif ftype:
                owner = ftype
            elif receiver in classes:
                owner = receiver
            if owner is None and receiver in (None, "this"):
                owners = [cls for cls in local if renamed_on(cls) or (cls, word, arity) in overload]
                if len(set(owners)) == 1:
                    owner = owners[0]
            found = inherited(owner)
            if found:
                return found
            if receiver in (None, "this") and word in local_methods:
                hits = [by_class[c][word] for c in local if word in by_class.get(c, {})]
                if len(set(hits)) == 1:
                    return hits[0]
            return None

        def call_type(word, receiver, line, arity, _path=path):
            owner = None
            if receiver in (None, "this"):
                owner = local[0] if len(local) == 1 else None
            else:
                ftype = local_type_at(ranges, line, receiver) if receiver else None
                if not ftype and receiver in fields and fields[receiver]:
                    ftype = fields[receiver]
                elif not ftype and receiver:
                    ftype = fields_unique.get(receiver) or None
                if ftype in classes:
                    owner = ftype
                elif receiver in classes:
                    owner = receiver
            return lookup_return(owner, word, arity)

        new = rewrite(text, accept, call_type)
        if new != text:
            backup(path)
            path.write_text(new)
            changed += 1
    print(
        f"{engine}-{rev}: renames={len(members)} unique={len(flat_unique)} "
        f"classes={len(by_class)} dropped={dropped} ambiguous={ambiguous} "
        f"overrides={overrides} files_written={changed}"
    )
    for note in notes:
        print(" ", note)
    return changed, notes


def compile_rev(engine: str, rev: int) -> int:
    folder = rev_dir(engine, rev)
    proj = folder / "src"
    if rev == 742 and (proj / "client" / "build.gradle").exists():
        task = ":client:compileJava"
    else:
        task = "compileJava"
    logf = ROOT / "logs" / f"method-names-{engine}-{rev}.log"
    env = os.environ.copy()
    env["JAVA_HOME"] = str(JAVA_HOME)
    env["PATH"] = str(JAVA_HOME / "bin") + os.pathsep + env.get("PATH", "")
    home = Path("/tmp/rsarchive-method-names") / f"{engine}-{rev}"
    home.mkdir(parents=True, exist_ok=True)
    env["GRADLE_USER_HOME"] = str(home)
    gradlew = proj / "gradlew"
    os.chmod(gradlew, 0o755)
    with logf.open("w") as fh:
        rc = subprocess.run(
            [str(gradlew), "--no-daemon", task],
            cwd=proj, env=env, stdout=fh, stderr=subprocess.STDOUT,
        ).returncode
    print(f"{engine}-{rev}: compile exit {rc} log {logf}")
    return rc


def restore_rev(engine: str, rev: int) -> int:
    folder = rev_dir(engine, rev)
    prefix = folder.relative_to(ROOT).as_posix() + "/"
    n = 0
    if not BACKUP.is_dir():
        return 0
    for path in BACKUP.rglob("*"):
        if not path.is_file():
            continue
        rel = path.relative_to(BACKUP).as_posix()
        if not rel.startswith(prefix):
            continue
        target = ROOT / rel
        target.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(path, target)
        n += 1
    print(f"{engine}-{rev}: restored {n} files")
    return n


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--engine", required=True, choices=("rsc", "rs2", "rs3"))
    ap.add_argument("--rev", required=True, type=int)
    ap.add_argument("--compile", action="store_true")
    ap.add_argument("--restore", action="store_true")
    args = ap.parse_args()
    if args.restore:
        restore_rev(args.engine, args.rev)
        return 0
    apply_rev(args.engine, args.rev)
    if not args.compile:
        return 0
    rc = compile_rev(args.engine, args.rev)
    if rc != 0:
        restore_rev(args.engine, args.rev)
        return rc
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
