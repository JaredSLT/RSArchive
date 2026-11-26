#!/usr/bin/env python3
"""Mechanical Fernflower/JDK fixes so archived client source compiles.

Does not rename game logic. Edits are the decompiler artifacts javac rejects:
forward static inits, dummy int-as-boolean, undeclared locals, LiveConnect, etc.
"""

from __future__ import annotations

import os
import re
import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import jdk  # noqa: E402

JAVA = jdk.java_bin()
JAVAC = jdk.javac_bin()
JAR = jdk.jar_bin()
STUB_SRC = ROOT / "tools" / "stub-src"
STUB_JAR = ROOT / "tools" / "stub.jar"
MS_STUB_JAR = ROOT / "tools" / "rs-deob" / "template" / "rs2-2011" / "lib" / "stub.jar"

FIELD_RE = re.compile(
    r"(?P<full>"
    r"(?P<prefix>(?:^[ \t]*@.+\n|^[ \t]*//.+\n)*)"
    r"(?P<indent>[ \t]*)"
    r"(?P<mods>(?:(?:public|protected|private|static|final|volatile|transient)\s+)+)"
    r"(?P<type>[\w.$]+(?:\s*<[^>\n]+>)?(?:\s*\[\s*\])*)\s+"
    r"(?P<name>[A-Za-z_]\w*)"
    r"(?P<eq>\s*=\s*(?P<init>[^;]+))?;"
    r")",
    re.M,
)

LITERAL_RE = re.compile(
    r"""^(?:
        null|true|false|
        -?\d+[lLfFdD]?|
        -?\d+\.\d+[fFdD]?|
        "(?:\\.|[^"\\])*"|
        '(?:\\.|.)'
    )$""",
    re.X,
)


def log(msg: str) -> None:
    print(msg, flush=True)


def rebuild_stub_jar() -> Path:
    """Build a jdk.jsobject patch jar that contains only netscape.javascript."""
    work = ROOT / "work" / "stub-build"
    if work.exists():
        shutil.rmtree(work)
    js_src = work / "js-src"
    js_pkg = js_src / "netscape" / "javascript"
    js_pkg.mkdir(parents=True)
    for name in ("JSObject.java", "JSException.java"):
        shutil.copy2(STUB_SRC / "netscape" / "javascript" / name, js_pkg / name)
    classes = work / "classes"
    classes.mkdir(parents=True)
    subprocess.check_call(
        [
            str(JAVAC), "-encoding", "UTF-8",
            "--patch-module", f"jdk.jsobject={js_src}",
            "-Xlint:-options", "-d", str(classes),
            str(js_pkg / "JSObject.java"),
            str(js_pkg / "JSException.java"),
        ]
    )
    STUB_JAR.parent.mkdir(parents=True, exist_ok=True)
    subprocess.check_call([str(JAR), "cf", str(STUB_JAR), "-C", str(classes), "."])
    return STUB_JAR


def install_stub(proj: Path) -> None:
    lib = proj / "lib"
    lib.mkdir(exist_ok=True)
    if MS_STUB_JAR.exists():
        shutil.copy2(MS_STUB_JAR, lib / "stub.jar")
        peer_src = STUB_SRC / "com" / "ms" / "awt" / "WComponentPeer.java"
        if peer_src.exists():
            pwork = ROOT / "work" / "peer-build"
            if pwork.exists():
                shutil.rmtree(pwork)
            pclasses = pwork / "classes"
            pclasses.mkdir(parents=True)
            subprocess.check_call(
                [str(JAVAC), "-encoding", "UTF-8", "-d", str(pclasses), str(peer_src)]
            )
            subprocess.check_call(
                [str(JAR), "uf", str(lib / "stub.jar"), "-C", str(pclasses), "."]
            )
    shutil.copy2(STUB_JAR, lib / "jsobject.jar")
    gradle = proj / "build.gradle"
    if gradle.exists():
        text = gradle.read_text()
        text = text.replace("JavaVersion.VERSION_1_8", "JavaVersion.VERSION_11")
        text = text.replace(
            "jdk.jsobject=${file('lib/stub.jar')}",
            "jdk.jsobject=${file('lib/jsobject.jar')}",
        )
        text = re.sub(
            r"\n\s*options\.compilerArgs \+= \['--add-reads', 'jdk\.jsobject=java\.desktop'\]",
            "",
            text,
        )
        if "lib:stub" not in text and "name: 'stub'" not in text:
            dep = "\ndependencies {\n    compileOnly name: 'stub'\n}\n"
            if "dependencies {" in text:
                text = text.replace(
                    "dependencies {",
                    "dependencies {\n    compileOnly name: 'stub'\n",
                    1,
                )
            elif re.search(r"plugins\s*\{", text):
                text, n = re.subn(
                    r"(plugins\s*\{.*?\n\}\n)",
                    r"\1" + dep,
                    text,
                    count=1,
                    flags=re.S,
                )
                if n == 0:
                    text += dep
            else:
                text += dep
        if "--patch-module" not in text:
            text += (
                "\ntasks.withType(JavaCompile) {\n"
                "    options.compilerArgs += ['--patch-module', \"jdk.jsobject=${file('lib/jsobject.jar')}\"]\n"
                "}\n"
            )
        gradle.write_text(text)
    settings = proj / "settings.gradle"
    if settings.exists():
        st = settings.read_text()
        if "flatDir" not in st:
            st += (
                "\ndependencyResolutionManagement {\n"
                "    repositories {\n"
                "        mavenCentral()\n"
                "        flatDir { dirs 'lib' }\n"
                "    }\n"
                "}\n"
            )
            settings.write_text(st)
    java_root = proj / "src" / "main" / "java"
    if java_root.exists():
        audio_src = STUB_SRC / "sun" / "audio" / "AudioPlayer.java"
        dest = java_root / "sun" / "audio" / "AudioPlayer.java"
        if audio_src.exists():
            needs = any("sun.audio" in p.read_text(errors="ignore") for p in java_root.rglob("*.java"))
            if needs:
                dest.parent.mkdir(parents=True, exist_ok=True)
                shutil.copy2(audio_src, dest)
        peer_dest = java_root / "com" / "ms" / "awt" / "WComponentPeer.java"
        if peer_dest.exists():
            peer_dest.unlink()


def is_literal(init: str) -> bool:
    return bool(LITERAL_RE.match(init.strip()))


def _semi_outside_strings(text: str, start: int) -> int:
    i = start
    n = len(text)
    quote = None
    depth = 0
    while i < n:
        ch = text[i]
        if quote:
            if ch == "\\" and i + 1 < n:
                i += 2
                continue
            if ch == quote:
                quote = None
            i += 1
            continue
        if ch in "\"'":
            quote = ch
            i += 1
            continue
        if ch == "(":
            depth += 1
        elif ch == ")":
            depth -= 1
        elif ch == ";" and depth <= 0:
            return i
        i += 1
    return -1


def find_static_assigned_fields(text: str) -> list[dict]:
    pat = re.compile(
        r"(?P<prefix>(?:^[ \t]*@.+\n|^[ \t]*//.+\n)*)"
        r"(?P<indent>[ \t]*)"
        r"(?P<mods>(?:(?:public|protected|private|static|final|volatile|transient)\s+)+)"
        r"(?P<type>[\w.$]+(?:\s*<[^>\n]+>)?(?:\s*\[\s*\])*)\s+"
        r"(?P<name>[A-Za-z_]\w*)\s*=",
        re.M,
    )
    out = []
    pos = 0
    while True:
        m = pat.search(text, pos)
        if not m:
            break
        if "static" not in m.group("mods"):
            pos = m.end()
            continue
        semi = _semi_outside_strings(text, m.end())
        if semi < 0:
            break
        init = text[m.end() : semi].strip()
        out.append(
            {
                "start": m.start(),
                "end": semi + 1,
                "prefix": m.group("prefix"),
                "indent": m.group("indent") or "\t",
                "mods": m.group("mods"),
                "type": m.group("type"),
                "name": m.group("name"),
                "init": init,
            }
        )
        pos = semi + 1
    return out


def _topo_static_fields(fields: list[dict]) -> list[dict]:
    """Assign a static field before others that mention it in their initializer."""
    names = {f["name"] for f in fields}
    remaining = list(fields)
    out: list[dict] = []
    while remaining:
        progressed = False
        done = {f["name"] for f in out}
        for i, f in enumerate(remaining):
            deps = [
                d
                for d in re.findall(r"\b([A-Za-z_]\w*)\b", f["init"])
                if d in names and d != f["name"]
            ]
            if any(d not in done for d in deps):
                continue
            out.append(f)
            remaining.pop(i)
            progressed = True
            break
        if not progressed:
            out.extend(remaining)
            break
    return out


def relocate_static_inits(text: str) -> str | None:
    """Move non-constant static field inits into a static {} so forward refs compile."""
    fields = [f for f in find_static_assigned_fields(text) if not is_literal(f["init"])]
    if not fields:
        return None
    assigns = []
    pieces = []
    last = 0
    indent = "\t"
    for f in fields:
        pieces.append(text[last : f["start"]])
        indent = f["indent"]
        pieces.append(f"{f['prefix']}{indent}{f['mods']}{f['type']} {f['name']};")
        last = f["end"]
    for f in _topo_static_fields(fields):
        assigns.append(f"{indent}\t{f['name']} = {f['init']};")
    tail = text[last:]
    block = "\n\n" + indent + "static {\n" + "\n".join(assigns) + "\n" + indent + "}\n"
    return "".join(pieces) + block + tail


def inject_class_forname(path: Path) -> int:
    text = path.read_text(errors="ignore")
    if re.search(r"\bstatic\s+Class\s+a\s*\(\s*String", text):
        return 0
    helper = (
        "\n\tprivate static Class a(String name) {\n"
        "\t\ttry {\n"
        "\t\t\treturn Class.forName(name);\n"
        "\t\t} catch (ClassNotFoundException e) {\n"
        "\t\t\tthrow new NoClassDefFoundError(name);\n"
        "\t\t}\n"
        "\t}\n"
    )
    m = re.search(r"(public(?:\s+final)?(?:\s+abstract)?\s+class\s+\w+[^{]*\{)", text)
    if not m:
        m = re.search(r"(abstract\s+class\s+\w+[^{]*\{)", text)
    if not m:
        return 0
    path.write_text(text[: m.end()] + helper + text[m.end() :])
    return 1


def fix_super_first(lines: list[str], idx: int) -> bool:
    line = lines[idx]
    if "super();" not in line:
        return False
    j = idx
    while j > 0:
        s = lines[j]
        if re.search(r"\b(?:public|protected|private)\s+\w+\s*\(", s) or re.search(
            r"^\s*(?:public|protected|private)?\s*\w+\s*\([^;]*\)\s*\{", s
        ):
            break
        j -= 1
    brace = j
    while brace < idx and "{" not in lines[brace]:
        brace += 1
    if brace >= idx:
        return False
    super_line = lines.pop(idx)
    if not super_line.endswith("\n"):
        super_line += "\n"
    lines.insert(brace + 1, super_line)
    return True


def hoist_try_scoped_local(lines: list[str], idx: int, name: str) -> bool:
    """Hoist a local declared inside try {} so uses after the try compile."""
    decl_idx = None
    decl_type = None
    for i in range(idx - 1, -1, -1):
        s = lines[i]
        if re.search(r"^\s*(?:public|protected|private).*\(", s) and "{" in s:
            break
        m = re.search(
            rf"(?:^|\s)((?:[\w.]+)(?:\s*\[\s*\])*)\s+{re.escape(name)}\s*=",
            s,
        )
        if m:
            decl_idx = i
            decl_type = re.sub(r"\s+", "", m.group(1))
            break
        m = re.search(rf"\b{re.escape(name)}\s*=\s*new\s+([\w.]+)\s*\[", s)
        if m:
            decl_idx = i
            decl_type = m.group(1) + "[]"
            break
    if decl_idx is None:
        return False
    try_idx = None
    for i in range(decl_idx, -1, -1):
        if re.search(r"\btry\b", lines[i]):
            try_idx = i
            break
        if re.search(r"^\s*(?:public|protected|private).*\(", lines[i]) and "{" in lines[i]:
            break
    if try_idx is None:
        return False
    for i in range(max(0, try_idx - 8), try_idx):
        if re.search(rf"\b{re.escape(name)}\s*=\s*null\s*;", lines[i]):
            return False
    indent = lines[try_idx][: len(lines[try_idx]) - len(lines[try_idx].lstrip())]
    lines.insert(try_idx, f"{indent}{decl_type} {name} = null;\n")
    new_decl = decl_idx + 1
    lines[new_decl] = re.sub(
        rf"((?:[\w.]+)(?:\s*\[\s*\])*)\s+{re.escape(name)}\s*=",
        f"{name} =",
        lines[new_decl],
        count=1,
    )
    return True


def inject_dummy_method(path: Path, name: str) -> bool:
    text = path.read_text(errors="ignore")
    if re.search(rf"\bvoid\s+{re.escape(name)}\s*\(\s*\)", text):
        return False
    helper = f"\n\tprivate static void {name}() {{}}\n"
    m = re.search(r"(public(?:\s+final)?(?:\s+abstract)?\s+class\s+\w+[^{]*\{)", text)
    if not m:
        m = re.search(r"((?:public\s+)?(?:final\s+)?class\s+\w+[^{]*\{)", text)
    if not m:
        return False
    path.write_text(text[: m.end()] + helper + text[m.end() :])
    return True


def wrap_at_caret(line: str, col: int, kind: str) -> str | None:
    """Wrap the expression starting near col."""
    if col < 1:
        col = 1
    i = col - 1
    if i >= len(line):
        i = len(line) - 1
    # skip spaces
    while i < len(line) and line[i] in " \t":
        i += 1
    start = i
    while start > 0 and (line[start - 1].isalnum() or line[start - 1] in "_."):
        start -= 1
    rest = line[start:]
    m = re.match(r"[A-Za-z_][\w]*(?:\s*\.\s*[A-Za-z_][\w]*)*(?:\s*\[[^\]]+\])?", rest)
    if not m:
        m = re.match(r"\([^)]+\)", rest)
    if not m:
        return None
    expr = m.group(0)
    if kind == "boolean":
        if "!= 0" in expr:
            return None
        wrapped = f"({expr} != 0)"
    elif kind == "objectarray":
        wrapped = f"((Object[])({expr}))"
    elif kind == "byte":
        wrapped = f"((byte)({expr}))"
    elif kind == "int":
        wrapped = f"((int)({expr}))"
    else:
        return None
    return line[:start] + wrapped + line[start + len(expr) :]


def parse_errors(log_text: str) -> list[tuple[Path, int, int, str, str]]:
    """(path, line, col, message, extra). col from caret if present."""
    lines = log_text.splitlines()
    out = []
    for i, line in enumerate(lines):
        if line.startswith(" "):
            continue
        m = re.match(r"^(.+\.java):(\d+): error: (.+)$", line)
        if not m:
            continue
        col = 1
        extra = ""
        if i + 2 < len(lines) and "^" in lines[i + 2]:
            col = lines[i + 2].find("^") + 1
        if i + 1 < len(lines):
            extra = "\n".join(lines[i + 1 : i + 5])
        out.append((Path(m.group(1)), int(m.group(2)), col, m.group(3), extra))
    return out


def apply_fixes(errors: list[tuple[Path, int, int, str, str]], src_java: Path) -> int:
    changed = 0
    forward_files: set[Path] = set()
    class_forname_files: set[Path] = set()
    simplepeer = None

    for path, line_no, col, msg, extra in errors:
        if not path.exists():
            cands = list(src_java.rglob(path.name))
            path = cands[0] if cands else path
        if not path.exists():
            continue
        idx = line_no - 1
        text_lines = path.read_text(errors="ignore").splitlines(True)
        if idx < 0 or idx >= len(text_lines):
            continue
        line = text_lines[idx]

        if "illegal forward reference" in msg:
            forward_files.add(path)
            continue
        if "method a(String)" in extra or "method a(String)" in msg:
            class_forname_files.add(path)
            continue
        if "overridden method is static,final" in extra and "init()" in msg:
            simplepeer = src_java / "deob" / "SimplePeer.java"
            if not simplepeer.exists():
                hits = list(src_java.rglob("SimplePeer.java"))
                simplepeer = hits[0] if hits else None
            continue
        if "call to super must be first" in msg:
            if fix_super_first(text_lines, idx):
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if "is already defined" in msg:
            mm = re.search(r"\b([A-Za-z_][\w$]*)\s*\(", line)
            if mm:
                text_lines[idx] = line.replace(mm.group(1) + "(", mm.group(1) + "_dup(", 1)
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if "might not have been initialized" in msg:
            vm = re.search(r"variable (\w+) might not have been initialized", msg)
            if vm:
                whole = "".join(text_lines)
                name = vm.group(1)
                def _init_repl(m: re.Match) -> str:
                    decl = m.group(0)
                    if "= " in decl:
                        return decl
                    if "boolean" in decl:
                        return decl.replace(";", " = false;")
                    if re.search(r"\[\s*\]", decl) or not re.search(
                        r"\b(?:int|boolean|byte|short|long|float|double|char)\b", decl
                    ):
                        return decl.replace(";", " = null;")
                    return decl.replace(";", " = 0;")

                nw, n = re.subn(
                    rf"(?:(?:public|protected|private|static|final)\s+)*([\w.]+)((?:\s*\[\s*\])*)\s+{re.escape(name)}\s*;",
                    _init_repl,
                    whole,
                    count=1,
                )
                if n:
                    path.write_text(nw)
                    changed += 1
            continue
        if "overridden method does not throw" in extra or "overridden method does not throw" in msg:
            nl = re.sub(r"\s+throws\s+Exception\b", "", line)
            if nl != line:
                text_lines[idx] = nl
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if "incompatible types: <null> cannot be converted" in msg:
            nl = re.sub(
                r"try \{ (return [^;]+;) \} catch \(Exception ignored\) \{ return null; \}",
                r"\1",
                line,
            )
            if nl != line:
                text_lines[idx] = nl
                whole = "".join(text_lines)
                for name in re.findall(r"this\.(\w+)\s*\(", nl):
                    whole = re.sub(
                        rf"(\b{re.escape(name)}\s*\([^)]*\))\s*throws\s+Exception\s*\{{",
                        r"\1 {",
                        whole,
                    )
                path.write_text(whole)
                changed += 1
            continue
        if "unreported exception" in msg:
            if "try {" in line:
                continue
            indent = re.match(r"[ \t]*", line).group(0)
            stmt = line.strip()
            if stmt.startswith("return "):
                # strip throws Exception from same-class callees instead of wrapping the return
                callees = re.findall(r"this\.(\w+)\s*\(", stmt)
                whole = "".join(text_lines)
                nstrip = 0
                for name in callees:
                    whole, k = re.subn(
                        rf"(\b{re.escape(name)}\s*\([^)]*\))\s*throws\s+Exception\s*\{{",
                        r"\1 {",
                        whole,
                    )
                    nstrip += k
                if nstrip:
                    path.write_text(whole)
                    changed += 1
                    continue
                continue
            text_lines[idx] = f"{indent}try {{ {stmt} }} catch (Exception ignored) {{}}\n"
            path.write_text("".join(text_lines))
            changed += 1
            continue
        if "missing return statement" in msg:
            sig = ""
            for j in range(idx, -1, -1):
                if re.search(r"\b(?:public|protected|private).*\(", text_lines[j]) or re.search(
                    r"^\s+\w+\s+\w+\s*\(", text_lines[j]
                ):
                    sig = text_lines[j]
                    break
            if "void" in sig.split("(")[0]:
                continue
            if re.search(r"\bboolean\b", sig.split("(")[0]):
                ret = "false"
            elif re.search(r"\b(?:int|byte|short|long|char|float|double)\b", sig.split("(")[0]):
                ret = "0"
            else:
                ret = "null"
            indent = re.match(r"[ \t]*", line).group(0) + "\t"
            text_lines[idx] = f"{indent}return {ret};\n" + line
            path.write_text("".join(text_lines))
            changed += 1
            continue
        if "illegal start of expression" in msg or "not a statement" in msg or "';' expected" in msg:
            nl = re.sub(r"\)\s+throws\s+\w+\s*\{", ") {", line)
            if nl != line:
                text_lines[idx] = nl
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if "int cannot be converted to boolean" in msg:
            nl = wrap_at_caret(line, col, "boolean")
            if nl and nl != line:
                text_lines[idx] = nl
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if re.search(r"cannot be converted to Object\[]", msg):
            nl = re.sub(
                r"Object\[\]\s+(\w+)\s*=\s*\(\(Object\[\]\)\((\w+)\)\)\.(\w+)",
                r"Object[] \1 = ((Object[])(\2.\3))",
                line,
            )
            if nl == line:
                nl = re.sub(
                    r"Object\[\]\s+(\w+)\s*=\s*(\w+\.\w+)",
                    r"Object[] \1 = ((Object[])(\2))",
                    line,
                )
            if nl != line:
                text_lines[idx] = nl
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if "possible lossy conversion from int to byte" in msg:
            nl = line
            if "((byte)" not in line and re.search(r"=\s*[^;]+;", line):
                nl = re.sub(
                    r"=\s*([^;]+);",
                    lambda m: f"= ((byte)({m.group(1).strip()}));",
                    line,
                    count=1,
                )
            if nl == line:
                nl = wrap_at_caret(line, col, "byte") or line
            if nl != line:
                text_lines[idx] = nl
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if "possible lossy conversion from float to int" in msg:
            nl = re.sub(r"(\w+\[[^\]]+\](?:\+\+|--))", r"((int)(\1))", line)
            if nl == line:
                nl = wrap_at_caret(line, col, "int")
            if nl and nl != line:
                text_lines[idx] = nl
                path.write_text("".join(text_lines))
                changed += 1
            continue
        if "cannot find symbol" in msg and "getPeer()" in extra:
            nl = re.sub(
                r"\(WComponentPeer\)(.+?)\.getPeer\(\)",
                r"WComponentPeer.from(\1)",
                line,
            )
            if nl == line:
                nl = re.sub(r"(\w+(?:\.\w+)*)\.getPeer\(\)", r"WComponentPeer.from(\1)", line)
            if nl != line:
                text_lines[idx] = nl
                if "import com.ms.awt.WComponentPeer;" not in path.read_text(errors="ignore"):
                    joined = "".join(text_lines)
                    joined = joined.replace("\npublic ", "\nimport com.ms.awt.WComponentPeer;\n\npublic ", 1)
                    path.write_text(joined)
                else:
                    path.write_text("".join(text_lines))
                changed += 1
            continue
        if "cannot find symbol" in msg:
            mm = re.search(r"symbol:\s+method (\w+)\(\)", extra)
            if mm and re.search(rf"\b{re.escape(mm.group(1))}\s*\(\s*\)\s*;", line):
                if inject_dummy_method(path, mm.group(1)):
                    changed += 1
                continue
            vm = re.search(r"symbol:\s+variable (\w+)", extra)
            if vm:
                name = vm.group(1)
                if hoist_try_scoped_local(text_lines, idx, name):
                    path.write_text("".join(text_lines))
                    changed += 1
                    continue
                am = re.match(rf"\s*{re.escape(name)}\s*=\s*new\s+([\w.]+)\s*\[", line)
                if am:
                    try_idx = idx
                    while try_idx > 0 and "try" not in text_lines[try_idx]:
                        try_idx -= 1
                    decl_line = f"{text_lines[try_idx][:len(text_lines[try_idx]) - len(text_lines[try_idx].lstrip())]}{am.group(1)}[] {name} = null;\n"
                    already = any(re.search(rf"\b{re.escape(am.group(1))}\[]\s+{re.escape(name)}\b", x) for x in text_lines)
                    if not already:
                        text_lines.insert(try_idx, decl_line)
                        path.write_text("".join(text_lines))
                        changed += 1
                    continue
                whole = "".join(text_lines)
                if re.search(rf"\b(?:byte|int|boolean|long|short|char|float|double|[\w.]+)(?:\s*\[\s*\])*\s+{re.escape(name)}\b", whole):
                    continue
                if "new byte[" in "".join(text_lines[max(0, idx - 5) : idx + 3]):
                    text_lines[idx] = re.sub(
                        rf"{re.escape(name)}\s*=",
                        f"byte[] {name} =",
                        line,
                        count=1,
                    )
                    if text_lines[idx] != line:
                        path.write_text("".join(text_lines))
                        changed += 1
            continue

    for path in forward_files:
        text = path.read_text(errors="ignore")
        nt = relocate_static_inits(text)
        if nt and nt != text:
            path.write_text(nt)
            changed += 1

    for path in class_forname_files:
        changed += inject_class_forname(path)

    if simplepeer and simplepeer.exists():
        t = simplepeer.read_text(errors="ignore")
        nt = re.sub(
            r"public static final native void init\(\);",
            "public static final native void initClass();",
            t,
        )
        nt = re.sub(r"static \{\s*init\(\);\s*\}", "static {\n      initClass();\n   }", nt)
        if nt != t:
            simplepeer.write_text(nt)
            changed += 1

    return changed


def compile_project(proj: Path, logf: Path) -> int:
    gradlew = proj / "gradlew"
    env = os.environ.copy()
    env["JAVA_HOME"] = str(JAVA.parent.parent)
    env["PATH"] = str(JAVA.parent) + os.pathsep + env.get("PATH", "")
    gh = Path("/tmp/rsarchive-compile-now") / f"gradle-{proj.parent.name}"
    gh.mkdir(parents=True, exist_ok=True)
    env["GRADLE_USER_HOME"] = str(gh)
    if gradlew.exists():
        os.chmod(gradlew, 0o755)
        cmd = [str(gradlew.resolve()), "--no-daemon", "compileJava"]
    else:
        cmd = ["gradle", "--no-daemon", "compileJava"]
    with logf.open("w") as fh:
        return subprocess.run(cmd, cwd=proj, env=env, stdout=fh, stderr=subprocess.STDOUT).returncode


def fix_project(proj: Path, logf: Path, attempts: int = 8) -> bool:
    src_java = proj / "src" / "main" / "java"
    if not src_java.exists():
        # 742 uses src/client
        cands = list(proj.rglob("src/main/java"))
        src_java = cands[0] if cands else src_java
    install_stub(proj)
    for attempt in range(attempts):
        rc = compile_project(proj, logf)
        text = logf.read_text(errors="ignore")
        if rc == 0 or "BUILD SUCCESSFUL" in text:
            return True
        errors = parse_errors(text)
        n = apply_fixes(errors, src_java)
        log(f"  attempt {attempt + 1}: {len(errors)} errors, {n} fixes")
        if n == 0:
            return False
    return False


def main() -> int:
    rebuild_stub_jar()
    targets = []
    if len(sys.argv) > 1:
        targets = [Path(a) for a in sys.argv[1:]]
    else:
        for fam in ("rsc", "rs2", "rs3"):
            root = ROOT / "builds" / fam
            for d in sorted(root.iterdir()):
                src = d / "src"
                if (src / "build.gradle").exists() or (src / "settings.gradle").exists():
                    targets.append(src)
    ok = fail = 0
    logs = ROOT / "logs"
    logs.mkdir(exist_ok=True)
    for proj in targets:
        key = proj.parent.name if proj.name == "src" else proj.name
        logf = logs / f"compile-{key}.log"
        log(f"compile {key}")
        if fix_project(proj, logf):
            log(f"OK {key}")
            # refresh status.json if present
            status = proj.parent / "status.json"
            if status.exists():
                import json
                st = json.loads(status.read_text())
                st["compiled"] = True
                status.write_text(json.dumps(st, indent=2) + "\n")
            ok += 1
        else:
            log(f"FAIL {key}")
            fail += 1
    log(f"done ok={ok} fail={fail}")
    return 0 if fail == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
