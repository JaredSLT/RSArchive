#!/usr/bin/env python3
"""Build cross-revision remaps with a shared human vocabulary.

Sources of names, in priority order:
  1. RuneWiki rs2-317 / rs2-254 remaps (old-engine members)
  2. Pazaz RSC-177 OriginalClass/OriginalMember
  3. Pazaz RT4-Client (rev 530) OriginalClass/OriginalMember
  4. LostCity RS742 named source (strings + @ObfuscatedName)

Vanilla gamepacks are matched to those named trees by distinctive string
literals so the same function keeps the same name in every year.
"""
from __future__ import annotations

import json
import re
import struct
import zipfile
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REFS = ROOT / "work" / "refs"
REMAP_DIR = ROOT / "tools" / "rs-deob" / "remap"
OUT_DIR = ROOT / "work" / "unified-remaps"
BUILDS = ROOT / "builds"

# Simple names that must be identical across every RS2/RSC year.
SYNONYMS = {
    "ClientPlayer": "PlayerEntity",
    "ClientNpc": "NpcEntity",
    "ClientEntity": "PathingEntity",
    "Player": "PlayerEntity",
    "Npc": "NpcEntity",
    "NPCType": "NpcType",
    "IsaacRandom": "Isaac",
    "JagString": "JString",
    "Pix32": "Sprite",
    "Pix8": "IndexedSprite",
    "SoftwareRaster": "Pix2D",
    "GlRaster": "Pix2D",
    "Rasteriser": "Pix3D",
    "PureJavaRasteriser": "Pix3D",
    "IndexedImage": "IndexedSprite",
    "ImageRGB": "Sprite",
    "Game": "Client",
    "mudclient": "MudClient",
    "client": "Client",
    "signlink": "SignLink",
    "browsercontrol": "BrowserControl",
    "IfType": "Component",
}

# Do not rename jaclib/jaggl/jagex3 native buffers to Packet.
PACKAGE_KEEP = ("jaclib/", "jaggl/", "jagex3/", "com/sun/", "javax/")

JUNK_STRINGS = {
    "",
    "null",
    "true",
    "false",
    "java",
    "lang",
    "error",
    "ok",
    "utf-8",
    "iso-8859-1",
    "main",
    "client",
    "runescape",
}


@dataclass
class ObfMethod:
    name: str
    desc: str
    strings: set[str] = field(default_factory=set)


@dataclass
class ObfField:
    name: str
    desc: str


@dataclass
class ObfClass:
    name: str
    super_name: str | None
    strings: set[str] = field(default_factory=set)
    methods: list[ObfMethod] = field(default_factory=list)
    fields: list[ObfField] = field(default_factory=list)


@dataclass
class NamedMember:
    kind: str  # field|method
    orig_owner: str
    orig_name: str
    orig_desc: str
    new_name: str


@dataclass
class NamedClass:
    orig: str
    new: str  # dotted or internal, we store dotted FQCN
    strings: set[str] = field(default_factory=set)
    members: list[NamedMember] = field(default_factory=list)
    methods_by_sig: dict[tuple[str, str], str] = field(default_factory=dict)


def u1(b, i):
    return b[i], i + 1


def u2(b, i):
    return struct.unpack(">H", b[i : i + 2])[0], i + 2


def u4(b, i):
    return struct.unpack(">I", b[i : i + 4])[0], i + 4


def parse_cp(data: bytes):
    count, i = u2(data, 8)
    cp = [None]
    n = 1
    while n < count:
        tag = data[i]
        i += 1
        if tag in (1, 2):  # Utf8, or legacy CONSTANT_Unicode (Java 1.1)
            ln, i = u2(data, i)
            raw = data[i : i + ln]
            i += ln
            try:
                s = raw.decode("utf-8")
            except UnicodeDecodeError:
                s = raw.decode("latin1")
            cp.append(("Utf8", s))
        elif tag in (7, 8, 16, 19, 20):
            v, i = u2(data, i)
            cp.append((tag, v))
        elif tag in (3, 4, 9, 10, 11, 12, 17, 18):
            a, i = u2(data, i)
            b2, i = u2(data, i)
            cp.append((tag, a, b2))
        elif tag in (5, 6):  # Long / Double: 8 bytes, skip next cp slot
            _hi, i = u4(data, i)
            _lo, i = u4(data, i)
            cp.append((tag, _hi, _lo))
            n += 1
            cp.append(None)
        elif tag == 15:
            k = data[i]
            i += 1
            v, i = u2(data, i)
            cp.append((tag, k, v))
        else:
            raise RuntimeError(f"unknown cp tag {tag}")
        n += 1
    return cp, i


def utf(cp, idx):
    if not idx or idx >= len(cp) or cp[idx] is None:
        return None
    e = cp[idx]
    if e[0] == "Utf8":
        return e[1]
    if e[0] == 7:
        return utf(cp, e[1])
    if e[0] == 8:
        return utf(cp, e[1])
    return None


def skip_attrs(data, i):
    n, i = u2(data, i)
    for _ in range(n):
        i += 2
        ln, i = u4(data, i)
        i += ln
    return i


def find_code(data, i, cp):
    n, i = u2(data, i)
    code = None
    for _ in range(n):
        ni, i = u2(data, i)
        ln, i = u4(data, i)
        body = data[i : i + ln]
        i += ln
        if utf(cp, ni) == "Code":
            code = body
    return code, i


def ldc_strings(code: bytes, cp) -> set[str]:
    if not code or len(code) < 8:
        return set()
    # skip max_stack, max_locals
    clen = struct.unpack(">I", code[4:8])[0]
    bytecode = code[8 : 8 + clen]
    out = set()
    i = 0
    wide = False
    while i < len(bytecode):
        op = bytecode[i]
        i += 1
        if op in (18,):  # ldc
            idx = bytecode[i]
            i += 1
            s = utf(cp, idx)
            if s:
                out.add(s)
        elif op in (19, 20):  # ldc_w, ldc2_w
            idx = (bytecode[i] << 8) | bytecode[i + 1]
            i += 2
            s = utf(cp, idx)
            if s:
                out.add(s)
        elif op in (16, 188):
            i += 1
        elif op in (
            17,
            179,
            180,
            181,
            182,
            183,
            184,
            178,
            187,
            189,
            192,
            193,
            167,
            168,
            198,
            199,
            153,
            154,
            155,
            156,
            157,
            158,
            159,
            160,
            161,
            162,
            163,
            164,
            165,
            166,
            132,
        ):
            i += 2
        elif op in (185, 186, 200, 201):
            i += 4 if op != 185 else 4
            if op == 185:
                pass
            if op == 186:
                i += 0
        elif op == 197:
            i += 3
        elif op == 170:  # tableswitch
            while i % 4:
                i += 1
            i += 4
            low = struct.unpack(">i", bytecode[i : i + 4])[0]
            i += 4
            high = struct.unpack(">i", bytecode[i : i + 4])[0]
            i += 4
            i += 4 * (high - low + 1)
        elif op == 171:  # lookupswitch
            while i % 4:
                i += 1
            i += 4
            npairs = struct.unpack(">i", bytecode[i : i + 4])[0]
            i += 4
            i += 8 * npairs
        elif op == 196:
            wide = True
            continue
        elif wide:
            if op == 132:
                i += 4
            else:
                i += 2
            wide = False
        wide = False
    return out


def parse_class_bytes(data: bytes) -> ObfClass | None:
    if data[:4] != b"\xca\xfe\xba\xbe":
        return None
    try:
        cp, i = parse_cp(data)
    except Exception:
        return None
    _acc, i = u2(data, i)
    this_i, i = u2(data, i)
    super_i, i = u2(data, i)
    inf_n, i = u2(data, i)
    i += 2 * inf_n
    this_name = utf(cp, this_i)
    super_name = utf(cp, super_i)
    if not this_name or "$" in this_name:
        # still parse inners later if needed; skip anonymous for matching
        pass
    fields = []
    fn, i = u2(data, i)
    for _ in range(fn):
        _a, i = u2(data, i)
        ni, i = u2(data, i)
        di, i = u2(data, i)
        i = skip_attrs(data, i)
        fields.append(ObfField(utf(cp, ni) or "?", utf(cp, di) or "?"))
    methods = []
    strings = set()
    mn, i = u2(data, i)
    for _ in range(mn):
        _a, i = u2(data, i)
        ni, i = u2(data, i)
        di, i = u2(data, i)
        code, i = find_code(data, i, cp)
        mname = utf(cp, ni) or "?"
        mdesc = utf(cp, di) or "?"
        mstrings = set()
        if code:
            try:
                mstrings = {s for s in ldc_strings(code, cp) if isinstance(s, str)}
            except Exception:
                mstrings = set()
        strings |= mstrings
        methods.append(ObfMethod(mname, mdesc, mstrings))
    return ObfClass(this_name, super_name, strings, methods, fields)


def load_jar_classes(jar: Path) -> dict[str, ObfClass]:
    out: dict[str, ObfClass] = {}
    with zipfile.ZipFile(jar) as z:
        for name in z.namelist():
            if not name.endswith(".class"):
                continue
            if name.startswith("META-INF/"):
                continue
            try:
                cls = parse_class_bytes(z.read(name))
            except Exception:
                continue
            if cls and cls.name:
                out[cls.name] = cls
    return out


def distinctive(strings: set[str]) -> set[str]:
    out = set()
    for s in strings:
        if not s or s in JUNK_STRINGS:
            continue
        if len(s) < 4:
            continue
        if s.startswith("(") or s.startswith("L") and s.endswith(";"):
            continue
        if s.startswith("[") or "/" in s and s.endswith(";"):
            continue
        if re.fullmatch(r"[0-9.\-]+", s):
            continue
        if re.fullmatch(r"[a-z]{1,3}", s):
            continue
        out.add(s)
    return out


def canonicalize_simple(name: str) -> str:
    if not name:
        return name
    simple = name.split(".")[-1].split("/")[-1]
    if simple in SYNONYMS:
        prefix = name[: -len(simple)] if name.endswith(simple) else ""
        # Buffer->Packet is wrong for jaclib.memory.Buffer
        if simple == "Buffer" and ("jaclib" in name or "Native" in name or "Heap" in name):
            return name
        return prefix + SYNONYMS[simple]
    return name


def canonicalize_member(name: str) -> str:
    return SYNONYMS.get(name, name)


def parse_lostcity_remap(path: Path) -> dict[str, NamedClass]:
    classes: dict[str, NamedClass] = {}
    if not path.exists():
        return classes
    for raw in path.read_text(errors="ignore").splitlines():
        line = raw.strip()
        if not line or line.startswith("#") or "=" not in line:
            continue
        left, right = line.split("=", 1)
        right = canonicalize_simple(right)
        if "(" in left:
            owner, rest = left.split(".", 1)
            name = rest[: rest.index("(")]
            desc = rest[rest.index("(") :]
            nc = classes.setdefault(owner, NamedClass(owner, owner))
            new = canonicalize_member(right)
            nc.members.append(NamedMember("method", owner, name, desc, new))
            nc.methods_by_sig[(name, desc)] = new
        elif "." in left:
            owner, fname = left.split(".", 1)
            nc = classes.setdefault(owner, NamedClass(owner, owner))
            nc.members.append(NamedMember("field", owner, fname, "", canonicalize_member(right)))
        else:
            nc = classes.setdefault(left, NamedClass(left, right))
            nc.new = right
            nc.orig = left
    return classes


ORIGINAL_CLASS = re.compile(r'@OriginalClass\("([^"]+)"\)')
ORIGINAL_MEMBER = re.compile(
    r"@OriginalMember\(\s*owner\s*=\s*\"([^\"]+)\"\s*,\s*name\s*=\s*\"([^\"]+)\"\s*,\s*descriptor\s*=\s*\"([^\"]+)\"\s*\)",
    re.S,
)
JAVA_CLASS = re.compile(
    r"package\s+([\w.]+)\s*;|public\s+(?:final\s+)?(?:abstract\s+)?(?:class|interface|enum)\s+(\w+)"
)
JAVA_STRINGS = re.compile(r"\"((?:\\.|[^\"\\])*)\"")
JAVA_METHOD = re.compile(
    r"(?:public|private|protected)\s+(?:static\s+)?(?:final\s+)*(?:[\w.<>\[\]]+)\s+(\w+)\s*\(([^)]*)\)"
)


def strip_owner(owner: str) -> str:
    # client!wa  /  mudclient!a/a/a  /  signlink!ll
    if "!" in owner:
        owner = owner.split("!", 1)[1]
    return owner.replace(".", "/")


def parse_original_java_tree(root: Path) -> dict[str, NamedClass]:
    classes: dict[str, NamedClass] = {}
    for path in root.rglob("*.java"):
        text = path.read_text(errors="ignore")
        oc = ORIGINAL_CLASS.search(text)
        pkg = None
        cls = None
        for m in JAVA_CLASS.finditer(text):
            if m.group(1):
                pkg = m.group(1)
            if m.group(2):
                cls = m.group(2)
        if not cls:
            continue
        fqcn = f"{pkg}.{cls}" if pkg else cls
        fqcn = canonicalize_simple(fqcn)
        # RT4 splits Packet (isaac/bits) off Buffer (g1/p1). Unify with 317/742:
        # Packet = byte buffer, PacketBit = isaac subclass.
        if cls == "Buffer" and (pkg or "").endswith("rt4"):
            fqcn = f"{pkg}.Packet" if pkg else "Packet"
        elif cls == "Packet" and (pkg or "").endswith("rt4"):
            fqcn = f"{pkg}.PacketBit" if pkg else "PacketBit"
        strings = distinctive({unescape_java(s) for s in JAVA_STRINGS.findall(text)})
        orig = strip_owner(oc.group(1)) if oc else None
        named = NamedClass(orig or fqcn.replace(".", "/"), fqcn, strings)
        for om in ORIGINAL_MEMBER.finditer(text):
            owner, name, desc = om.group(1), om.group(2), om.group(3)
            owner = strip_owner(owner)
            # Find the following identifier (refactored name)
            after = text[om.end() : om.end() + 400]
            ident = re.search(
                r"(?:public|private|protected|static|final|native|synchronized|\s)+([\w.<>\[\]]+)\s+(\w+)\s*(\(|;|=)",
                after,
            )
            if not ident:
                continue
            new_name = canonicalize_member(ident.group(2))
            kind = "method" if ident.group(3) == "(" else "field"
            if name == "<init>" or name == "<clinit>":
                continue
            named.members.append(NamedMember(kind, owner, name, desc if kind == "method" else "", new_name))
            if kind == "method":
                named.methods_by_sig[(name, desc)] = new_name
            # Members may have been moved; still record under original owner class.
            if owner != named.orig:
                other = classes.setdefault(owner, NamedClass(owner, canonicalize_simple(owner)))
                other.members.append(NamedMember(kind, owner, name, desc if kind == "method" else "", new_name))
                if kind == "method":
                    other.methods_by_sig[(name, desc)] = new_name
        if orig:
            existing = classes.get(orig)
            if existing and existing.new not in SYNONYMS.values() and "/" not in existing.new.replace("rt4.", ""):
                # keep existing if it already has a human FQCN
                if "." in existing.new or existing.new[:1].isupper():
                    existing.strings |= strings
                    existing.members.extend(named.members)
                    continue
            classes[orig] = named
        else:
            classes.setdefault(named.orig, named)
    return classes


def unescape_java(s: str) -> str:
    return (
        s.replace("\\n", "\n")
        .replace("\\t", "\t")
        .replace('\\"', '"')
        .replace("\\\\", "\\")
    )


OBF_ANN = re.compile(r"@ObfuscatedName\(\"([^\"]+)\"\)")


def parse_named_source_tree(root: Path) -> list[NamedClass]:
    """Parse already-named Java (742, 317 src) for string fingerprints."""
    out = []
    for path in root.rglob("*.java"):
        if path.name in {"ObfuscatedName.java", "OriginalClass.java", "OriginalMember.java", "OriginalArg.java", "Pc.java"}:
            continue
        text = path.read_text(errors="ignore")
        pkg = None
        cls = None
        for m in JAVA_CLASS.finditer(text):
            if m.group(1):
                pkg = m.group(1)
            if m.group(2):
                cls = m.group(2)
        if not cls:
            continue
        fqcn = f"{pkg}.{cls}" if pkg else cls
        fqcn = canonicalize_simple(fqcn)
        strings = distinctive({unescape_java(s) for s in JAVA_STRINGS.findall(text)})
        orig = None
        m = OBF_ANN.search(text)
        if m and "(" not in m.group(1) and "." not in m.group(1):
            orig = m.group(1)
        nc = NamedClass(orig or cls, fqcn, strings)
        out.append(nc)
    return out


def score_match(a: set[str], b: set[str]) -> int:
    return len(a & b)


def match_classes(
    obf: dict[str, ObfClass], named: dict[str, NamedClass]
) -> dict[str, NamedClass]:
    """Map obfuscated class internal name -> NamedClass."""
    named_list = [n for n in named.values() if n.strings]
    used_named = set()
    mapping: dict[str, NamedClass] = {}

    # Exact original-name hits first.
    by_orig = {n.orig: n for n in named.values() if n.orig}
    for oname, oc in obf.items():
        if oname in by_orig:
            mapping[oname] = by_orig[oname]
            used_named.add(id(by_orig[oname]))

    # Unobfuscated keepers.
    for oname, oc in obf.items():
        if oname in mapping:
            continue
        simple = oname.split("/")[-1]
        if simple in {"client", "signlink", "mudclient", "link", "browsercontrol", "jagmisc"}:
            fq = canonicalize_simple(simple[0].upper() + simple[1:] if simple != "client" else "Client")
            if simple == "client":
                fq = "Client"
            elif simple == "mudclient":
                fq = "MudClient"
            elif simple == "signlink":
                fq = "SignLink"
            mapping[oname] = NamedClass(oname, fq, distinctive(oc.strings))

    candidates = []
    for oname, oc in obf.items():
        if oname in mapping:
            continue
        if any(oname.startswith(p) for p in PACKAGE_KEEP):
            continue
        ds = distinctive(oc.strings)
        if len(ds) < 2:
            continue
        best = None
        best_score = 0
        second = 0
        for nc in named_list:
            if id(nc) in used_named:
                continue
            sc = score_match(ds, nc.strings)
            if sc > best_score:
                second = best_score
                best_score = sc
                best = nc
            elif sc > second:
                second = sc
        if best and (
            (best_score >= 1 and second == 0)
            or (best_score >= 2 and best_score >= second + 2)
        ):
            candidates.append((best_score, oname, best))
    candidates.sort(reverse=True)
    for _sc, oname, best in candidates:
        if oname in mapping or id(best) in used_named:
            continue
        mapping[oname] = best
        used_named.add(id(best))
    return mapping


def match_methods(oc: ObfClass, nc: NamedClass) -> list[tuple[str, str]]:
    """Return remap lines for methods/fields."""
    lines = []
    # Fields: unique descriptor wins; else copy by OriginalMember owner match.
    fields_by_desc: dict[str, list[ObfField]] = defaultdict(list)
    for f in oc.fields:
        fields_by_desc[f.desc].append(f)
    named_fields = [m for m in nc.members if m.kind == "field" and m.orig_owner == oc.name]
    if not named_fields:
        named_fields = [m for m in nc.members if m.kind == "field"]
    used_fields = set()
    for nf in named_fields:
        key = (oc.name, nf.orig_name)
        # Prefer exact original name if this named class came from this revision.
        exact = next((f for f in oc.fields if f.name == nf.orig_name and f.name not in used_fields), None)
        if exact:
            lines.append(f"{oc.name}.{exact.name}={nf.new_name}")
            used_fields.add(exact.name)
    # Methods via strings then original name.
    named_methods = [m for m in nc.members if m.kind == "method"]
    used_methods = set()
    for om in oc.methods:
        if om.name in {"<init>", "<clinit>"}:
            continue
        ds = distinctive(om.strings)
        best = None
        best_score = 0
        for nm in named_methods:
            if not ds:
                continue
            # named methods don't carry strings here; skip
        # exact original name+desc from same-revision annotations
        if (om.name, om.desc) in nc.methods_by_sig:
            new = nc.methods_by_sig[(om.name, om.desc)]
            lines.append(f"{oc.name}.{om.name}{om.desc}={new}")
            used_methods.add((om.name, om.desc))
    return lines


def emit_remap(
    obf: dict[str, ObfClass],
    class_map: dict[str, NamedClass],
    extra_member_lines: list[str] | None = None,
) -> str:
    lines = []
    seen = set()
    for oname, nc in sorted(class_map.items(), key=lambda kv: kv[1].new):
        if "$" in oname:
            continue
        if any(oname.startswith(p) for p in PACKAGE_KEEP):
            continue
        new = nc.new
        # default-package PascalCase if the named class is a raw obfuscated leftover
        simple = new.split(".")[-1]
        if re.fullmatch(r"class\d+", simple) or re.fullmatch(r"[a-z]{1,3}", simple):
            continue
        key = f"{oname}={new}"
        if key not in seen:
            lines.append(key)
            seen.add(key)
        oc = obf.get(oname)
        if oc:
            for ln in match_methods(oc, nc):
                if ln not in seen:
                    lines.append(ln)
                    seen.add(ln)
    if extra_member_lines:
        for ln in extra_member_lines:
            if ln not in seen:
                lines.append(ln)
                seen.add(ln)
    return "\n".join(lines) + "\n"


def transfer_members(
    src_obf: dict[str, ObfClass],
    src_named: dict[str, NamedClass],
    dst_obf: dict[str, ObfClass],
    dst_class_map: dict[str, NamedClass],
) -> list[str]:
    """Copy member names from a named revision onto a matched destination revision."""
    # Invert src: NamedClass id -> orig
    src_by_new: dict[str, tuple[str, NamedClass]] = {}
    for orig, nc in src_named.items():
        src_by_new[nc.new] = (orig, nc)

    lines = []
    for dst_orig, nc in dst_class_map.items():
        src = src_by_new.get(nc.new)
        if not src:
            continue
        src_orig, src_nc = src
        src_cls = src_obf.get(src_orig)
        dst_cls = dst_obf.get(dst_orig)
        if not src_cls or not dst_cls:
            continue
        # Map src methods (human names from src_nc) onto dst methods via strings.
        src_human = {}
        for m in src_nc.members:
            if m.kind == "method":
                src_human[(m.orig_name, m.orig_desc)] = m.new_name
        src_methods_named: list[tuple[ObfMethod, str]] = []
        for sm in src_cls.methods:
            human = src_human.get((sm.name, sm.desc))
            if not human:
                # try src_named methods_by_sig
                human = src_nc.methods_by_sig.get((sm.name, sm.desc))
            if human:
                src_methods_named.append((sm, human))
        used = set()
        for dm in dst_cls.methods:
            if dm.name in {"<init>", "<clinit>"}:
                continue
            ds = distinctive(dm.strings)
            best = None
            best_sc = 0
            second = 0
            for sm, human in src_methods_named:
                sc = score_match(ds, distinctive(sm.strings))
                if sc > best_sc:
                    second = best_sc
                    best_sc = sc
                    best = human
                elif sc > second:
                    second = sc
            if (
                best
                and not best.startswith("_flow")
                and best_sc >= 1
                and best_sc > second
                and (dm.name, dm.desc) not in used
            ):
                lines.append(f"{dst_orig}.{dm.name}{dm.desc}={best}")
                used.add((dm.name, dm.desc))
        # Unique-descriptor fallback is too aggressive (()V → load, etc.).
        # Do not guess fields from descriptor uniqueness: dummy booleans
        # and names like `length` collide and break compilation.
    return lines


def apply_synonyms_to_remap_file(text: str) -> str:
    out = []
    for line in text.splitlines():
        if "=" not in line:
            out.append(line)
            continue
        left, right = line.split("=", 1)
        right = canonicalize_simple(right)
        right = canonicalize_member(right)
        out.append(f"{left}={right}")
    return "\n".join(out) + "\n"


def name_leftovers(obf: dict[str, ObfClass], mapped: dict[str, NamedClass]) -> dict[str, NamedClass]:
    """Give remaining classes a stable human-ish name from distinctive strings / hierarchy."""
    used = {nc.new.split(".")[-1] for nc in mapped.values()}
    used.update(SYNONYMS.values())
    extras = {}
    for oname, oc in obf.items():
        if oname in mapped or "$" in oname:
            continue
        if any(oname.startswith(p) for p in PACKAGE_KEEP):
            continue
        simple = oname.split("/")[-1]
        if simple[:1].isupper() and not re.fullmatch(r"[A-Z]{6,}", simple):
            # already looks like a real name (OpenGL, Isaac, etc.)
            extras[oname] = NamedClass(oname, canonicalize_simple(simple.replace("-", "_")))
            continue
        ds = sorted(distinctive(oc.strings), key=len, reverse=True)
        label = None
        for s in ds:
            # Only English phrases; skip crc tables and obfuscated tokens.
            if " " not in s and "." not in s:
                continue
            if re.search(r"\d{4,}", s):
                continue
            words = re.findall(r"[A-Za-z]{3,}", s)
            if len(words) < 2:
                continue
            slug = "".join(w[:1].upper() + w[1:] for w in words)[:40]
            if 6 <= len(slug) <= 40 and slug.isidentifier() and slug not in used:
                label = slug
                break
        if not label:
            continue  # leave unnamed; UniqueRenamer still produces classN
        used.add(label)
        extras[oname] = NamedClass(oname, label, distinctive(oc.strings))
    return extras


def write_out(name: str, text: str) -> Path:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    path = OUT_DIR / name
    path.write_text(text)
    return path


def report(title: str, obf: dict[str, ObfClass], mapped: dict[str, NamedClass]):
    real = [n for n in obf if "$" not in n and not any(n.startswith(p) for p in PACKAGE_KEEP)]
    human = 0
    for n, nc in mapped.items():
        simple = nc.new.split(".")[-1]
        if not re.match(r"^(Unmapped|class)\d+", simple) and not re.fullmatch(r"[a-z]{1,3}", simple):
            human += 1
    print(f"{title}: classes={len(real)} mapped={len(mapped)} humanish={human}")


def main():
    print("Loading named references...")
    named_317 = parse_lostcity_remap(REMAP_DIR / "rs2-317.txt")
    named_254 = parse_lostcity_remap(REMAP_DIR / "rs2-254.txt")
    rsc177 = parse_original_java_tree(REFS / "RSC-177")
    rt4 = parse_original_java_tree(REFS / "RT4-Client")
    src_742 = parse_named_source_tree(BUILDS / "rs2" / "742-2012-11-07" / "src")
    named_742 = {}
    for nc in src_742:
        named_742[nc.orig] = nc

    # Attach 317 strings from original jar so we can match other years to it.
    jar_317 = BUILDS / "rs2" / "317-2005-06-13" / "original" / "gamepack.jar"
    obf_317 = load_jar_classes(jar_317)
    for orig, nc in named_317.items():
        if orig in obf_317:
            nc.strings = distinctive(obf_317[orig].strings)

    jar_254 = BUILDS / "rs2" / "254-2004-09-07" / "original" / "gamepack.jar"
    obf_254 = load_jar_classes(jar_254)
    for orig, nc in named_254.items():
        if orig in obf_254:
            nc.strings = distinctive(obf_254[orig].strings)

    # RT4 strings from its own source already; also try matching against 530 vanilla.
    builds = {
        "rsc-115": BUILDS / "rsc" / "115-2001-12-24" / "original" / "gamepack.jar",
        "rsc-159": BUILDS / "rsc" / "159-2002-11-25" / "original" / "gamepack.jar",
        "rsc-177": BUILDS / "rsc" / "177-2003-10-31" / "original" / "gamepack.jar",
        "rs2-254": jar_254,
        "rs2-317": jar_317,
        "rs2-377": BUILDS / "rs2" / "377-2006-05-02" / "original" / "gamepack.jar",
        "rs2-468": BUILDS / "rs2" / "468-2007-08-08" / "original" / "gamepack.jar",
        "rs2-474": BUILDS / "rs2" / "474-2007-10-19" / "original" / "gamepack.jar",
        "rs2-508": BUILDS / "rs2" / "508-2008-08-05" / "original" / "gamepack.jar",
        "rs2-530": BUILDS / "rs2" / "530-2009-01-28" / "original" / "gamepack.jar",
        "rs2-614": BUILDS / "rs2" / "614-2010-07-13" / "original" / "gamepack.jar",
        "rs2-667": BUILDS / "rs2" / "667-2011-09-29" / "original" / "gamepack.jar",
    }

    stats = {}

    # RSC
    for key, jar in (("rsc-177", builds["rsc-177"]), ("rsc-159", builds["rsc-159"]), ("rsc-115", builds["rsc-115"])):
        obf = load_jar_classes(jar)
        mapped = match_classes(obf, rsc177)
        if key == "rsc-177":
            # Prefer annotation orig names.
            for orig, nc in rsc177.items():
                if orig in obf:
                    mapped[orig] = nc
        extra = transfer_members(
            load_jar_classes(builds["rsc-177"]) if key != "rsc-177" else obf,
            rsc177,
            obf,
            mapped,
        )
        leftovers = name_leftovers(obf, mapped)
        mapped.update(leftovers)
        text = emit_remap(obf, mapped, extra)
        write_out(f"{key}.txt", text)
        report(key, obf, mapped)
        stats[key] = {"classes": len(obf), "mapped": len(mapped), "lines": text.count("\n")}

    # 254 / 317: take existing remaps, apply synonyms, fill leftovers
    for key, named, obf in (
        ("rs2-254", named_254, obf_254),
        ("rs2-317", named_317, obf_317),
    ):
        mapped = dict(named)
        if "client" in mapped and mapped["client"].new.split(".")[-1] in {"client", "Client"}:
            mapped["client"].new = "jagex2.client.Client"
        leftovers = name_leftovers(obf, mapped)
        mapped.update(leftovers)
        existing = apply_synonyms_to_remap_file((REMAP_DIR / f"{key}.txt").read_text())
        extra_lines = existing.splitlines()
        text = emit_remap(obf, mapped)
        # Prefer the existing (complete) member map, with synonyms, plus leftover classes.
        merged = extra_lines[:]
        have = {ln.split("=", 1)[0] for ln in merged if "=" in ln}
        for ln in text.splitlines():
            if "=" in ln and ln.split("=", 1)[0] not in have:
                merged.append(ln)
        out = "\n".join(merged) + "\n"
        write_out(f"{key}.txt", out)
        report(key, obf, mapped)
        stats[key] = {"lines": out.count("\n")}

    # 377: keep the existing class map (complete) and copy 317 members onto it
    obf_377 = load_jar_classes(builds["rs2-377"])
    existing_377 = parse_lostcity_remap(REMAP_DIR / "rs2-377.txt")
    for nc in existing_377.values():
        # Keep only method remaps from the seed file; guessed fields collide.
        nc.members = [m for m in nc.members if m.kind == "method"]
    mapped_377 = dict(existing_377)
    for orig, nc in mapped_377.items():
        if orig in obf_377:
            nc.strings |= distinctive(obf_377[orig].strings)
        nc.new = canonicalize_simple(nc.new)
        if nc.new == "client":
            nc.new = "jagex2.client.Client"
    for orig, nc in match_classes(obf_377, named_317).items():
        if orig not in mapped_377:
            mapped_377[orig] = nc
    extra_377 = [
        ln
        for ln in (REMAP_DIR / "rs2-377.txt").read_text().splitlines()
        if ln.startswith("MFMVIYHT.")
    ]
    leftovers = name_leftovers(obf_377, mapped_377)
    mapped_377.update(leftovers)
    text = emit_remap(obf_377, mapped_377, extra_377)
    write_out("rs2-377.txt", text)
    report("rs2-377", obf_377, mapped_377)
    stats["rs2-377"] = {"mapped": len(mapped_377), "lines": text.count("\n")}

    # 530 from RT4 annotations (exact)
    obf_530 = load_jar_classes(builds["rs2-530"])
    mapped_530 = {}
    for orig, nc in rt4.items():
        if orig in obf_530:
            mapped_530[orig] = nc
    mapped_530.update(match_classes(obf_530, rt4))
    leftovers = name_leftovers(obf_530, mapped_530)
    mapped_530.update(leftovers)
    for nc in mapped_530.values():
        nc.members = []
        nc.methods_by_sig = {}
    # Class names only. RT4 member maps include relocated statics that
    # collide if applied in-place.
    text = emit_remap(obf_530, mapped_530, None)
    write_out("rs2-530.txt", text)
    report("rs2-530", obf_530, mapped_530)
    stats["rs2-530"] = {"mapped": len(mapped_530), "lines": text.count("\n")}

    # 468 / 474 / 508 from 530
    named_530_for_match = {}
    for orig, nc in mapped_530.items():
        if orig in obf_530:
            nc.strings = distinctive(obf_530[orig].strings) | nc.strings
        named_530_for_match[orig] = nc

    for key in ("rs2-468", "rs2-474", "rs2-508"):
        obf = load_jar_classes(builds[key])
        mapped = match_classes(obf, named_530_for_match)
        leftovers = name_leftovers(obf, mapped)
        mapped.update(leftovers)
        text = emit_remap(obf, mapped, None)
        write_out(f"{key}.txt", text)
        report(key, obf, mapped)
        stats[key] = {"mapped": len(mapped), "lines": text.count("\n")}

    # 614 / 667 from 530 + 742
    named_late = dict(named_530_for_match)
    for orig, nc in named_742.items():
        named_late[f"742:{nc.new}"] = nc
    # rekey 742 by orig if unique
    named_late_list = {**named_530_for_match}
    i = 0
    for nc in named_742.values():
        named_late_list[nc.new] = nc

    for key in ("rs2-614", "rs2-667"):
        obf = load_jar_classes(builds[key])
        mapped = match_classes(obf, named_late_list)
        leftovers = name_leftovers(obf, mapped)
        mapped.update(leftovers)
        text = emit_remap(obf, mapped, None)
        write_out(f"{key}.txt", text)
        report(key, obf, mapped)
        stats[key] = {"mapped": len(mapped), "lines": text.count("\n")}

    (OUT_DIR / "stats.json").write_text(json.dumps(stats, indent=2) + "\n")
    print("Wrote", OUT_DIR)


if __name__ == "__main__":
    main()
