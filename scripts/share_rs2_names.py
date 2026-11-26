#!/usr/bin/env python3
"""Share one identifier for the same RS2 class, method, and field.

RS2 only. RSC and RS3 are different engines and are not read or edited.
Obfuscated members of a multi-year cluster take one name already present in
that cluster. Readable class names that differ are never united. Nothing is
minted: no kindN, no new prefix. String and char literals are left untouched.
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
from dataclasses import dataclass, field
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import jdk  # noqa: E402

BUILDS = ROOT / "builds" / "rs2"
WORK = ROOT / "work" / "share-rs2"
BACKUP = WORK / "backup"
JAVA_HOME = jdk.java_home()

REVS = [
    254, 282, 317, 350, 377, 443, 468, 474, 481, 508, 525, 530, 578, 614, 634,
    667, 695, 742, 745,
]
ADJACENT = list(zip(REVS, REVS[1:]))

# 317 wins, then 254, then 377, then an RT4-style name already present in 530.
ANCHOR_PRIO = {317: 400, 254: 300, 377: 200}
RT4_PRIO = 100
OTHER_READABLE_PRIO = 10

KEYWORDS = {
    "abstract", "assert", "boolean", "break", "byte", "case", "catch", "char",
    "class", "const", "continue", "default", "do", "double", "else", "enum",
    "extends", "final", "finally", "float", "for", "goto", "if", "implements",
    "import", "instanceof", "int", "interface", "long", "native", "new",
    "package", "private", "protected", "public", "return", "short", "static",
    "strictfp", "super", "switch", "synchronized", "this", "throw", "throws",
    "transient", "try", "void", "volatile", "while", "true", "false", "null",
    "var", "yield", "record", "sealed", "permits",
}
PRIMITIVES = {"void", "boolean", "byte", "short", "int", "long", "char", "float", "double"}
JDK_SIMPLE = {
    "Object", "String", "Class", "Enum", "Throwable", "Exception", "Error",
    "RuntimeException", "Thread", "Runnable", "Applet", "Frame", "Canvas",
    "Panel", "Component", "Container", "Window", "Graphics", "Color", "Font",
    "Image", "Event", "AWTEvent", "Insets", "Dimension", "Point", "Rectangle",
    "Toolkit", "Cursor", "Buffer", "ByteBuffer", "IntBuffer", "Socket",
    "ServerSocket", "URL", "URLConnection", "HttpURLConnection", "InetAddress",
    "InputStream", "OutputStream", "FilterInputStream", "FilterOutputStream",
    "DataInputStream", "DataOutputStream", "BufferedReader", "BufferedInputStream",
    "ByteArrayInputStream", "ByteArrayOutputStream", "File", "RandomAccessFile",
    "IOException", "EOFException", "SocketException", "UnknownHostException",
    "BigInteger", "Random", "Arrays", "Collections", "List", "ArrayList",
    "Map", "HashMap", "Set", "HashSet", "Iterator", "Iterable", "Collection",
    "Enumeration", "Vector", "Hashtable", "Properties", "Calendar", "Date",
    "GregorianCalendar", "TimeZone", "Math", "System", "Integer", "Long",
    "Boolean", "Byte", "Short", "Float", "Double", "Character", "Number",
    "StringBuilder", "StringBuffer", "Comparable", "Comparator", "Cloneable",
    "Readable", "Appendable", "CharSequence", "AutoCloseable", "Closeable",
    "Flushable", "Serializable", "Override", "Deprecated", "SuppressWarnings",
    "SafeVarargs", "FunctionalInterface", "Throwable", "StackTraceElement",
    "Method", "Field", "Constructor", "Modifier", "Proxy", "InvocationHandler",
    "AccessController", "PrivilegedAction", "PrivilegedExceptionAction",
    "ZipInputStream", "GZIPInputStream", "Inflater", "Deflater", "CRC32",
    "Charset", "StandardCharsets", "Locale", "Optional", "Objects", "UUID",
    "Timer", "TimerTask", "DateFormat", "SimpleDateFormat", "Pattern",
    "Matcher", "Scanner", "StringTokenizer", "BitSet", "Observable",
    "Observer", "EventObject", "ActionEvent", "FocusEvent", "KeyEvent",
    "MouseEvent", "MouseWheelEvent", "WindowEvent", "ComponentEvent",
    "InputEvent", "AdjustmentEvent", "ItemEvent", "TextEvent",
    "ActionListener", "FocusListener", "KeyListener", "MouseListener",
    "MouseMotionListener", "MouseWheelListener", "WindowListener",
    "WindowFocusListener", "WindowStateListener", "ComponentListener",
    "ImageObserver", "LayoutManager", "LayoutManager2", "BorderLayout",
    "FlowLayout", "GridLayout", "CardLayout", "ImageProducer", "ImageConsumer",
    "MemoryImageSource", "PixelGrabber", "BufferedImage", "Graphics2D",
    "RenderingHints", "AffineTransform", "ImageIO", "AudioInputStream",
    "Clip", "DataLine", "SourceDataLine", "TargetDataLine", "Mixer", "Line",
    "AudioSystem", "AudioFormat", "Control", "FloatControl", "BooleanControl",
    "DatagramSocket", "DatagramPacket", "MulticastSocket", "NetworkInterface",
    "URI", "Proxy", "CookieHandler", "Authenticator", "PasswordAuthentication",
    "FileInputStream", "FileOutputStream", "PrintStream", "PrintWriter",
    "Writer", "Reader", "BufferedWriter", "OutputStreamWriter",
    "InputStreamReader", "PushbackInputStream", "SequenceInputStream",
    "ObjectInputStream", "ObjectOutputStream", "Externalizable",
    "IllegalArgumentException", "IllegalStateException", "NullPointerException",
    "IndexOutOfBoundsException", "ArrayIndexOutOfBoundsException",
    "StringIndexOutOfBoundsException", "NumberFormatException",
    "ClassCastException", "ClassNotFoundException", "NoSuchMethodException",
    "NoSuchFieldException", "InstantiationException", "InterruptedException",
    "SecurityException", "UnsupportedOperationException", "ConcurrentModificationException",
    "EmptyStackException", "NoSuchElementException", "MissingResourceException",
    "ArithmeticException", "NegativeArraySizeException",
}
# Superclass agreement ignores these. "Object" agreeing is not evidence.
JDK_SUPERS = JDK_SIMPLE | {
    "Object", "Applet", "Frame", "Canvas", "Panel", "Component", "Container",
    "Window", "Dialog", "JFrame", "JPanel", "Thread", "Exception", "Throwable",
    "Error", "RuntimeException", "InputStream", "OutputStream", "Socket",
    "Timer", "TimerTask", "Enum", "Buffer", "ByteBuffer",
}
# Never rename these onto a different name. Includes the AWT callbacks from the
# task plus JDK methods whose rename fails compilation when the class implements
# a platform interface.
JDK_CALLBACKS = {
    "run", "paint", "update", "init", "start", "stop", "destroy",
    "keyPressed", "keyReleased", "keyTyped",
    "mouseClicked", "mousePressed", "mouseReleased", "mouseDragged",
    "mouseMoved", "mouseEntered", "mouseExited", "mouseWheelMoved",
    "focusGained", "focusLost",
    "windowOpened", "windowClosing", "windowClosed", "windowIconified",
    "windowDeiconified", "windowActivated", "windowDeactivated",
    "windowGainedFocus", "windowLostFocus", "windowStateChanged",
    "componentResized", "componentMoved", "componentShown", "componentHidden",
    "actionPerformed", "imageUpdate", "adjustmentValueChanged",
    "itemStateChanged", "textValueChanged",
    "iterator", "hasNext", "next", "remove",
    "size", "isEmpty", "contains", "toArray", "containsAll", "addAll",
    "removeAll", "retainAll", "clear",
    "equals", "hashCode", "toString", "clone", "finalize", "compareTo",
    "hasMoreElements", "nextElement", "main",
}
IFACE_METHODS = {
    "Iterable": {"iterator"},
    "Iterator": {"hasNext", "next", "remove"},
    "Collection": {
        "size", "isEmpty", "contains", "toArray", "add", "remove", "containsAll",
        "addAll", "removeAll", "retainAll", "clear", "equals", "hashCode",
    },
    "List": {
        "size", "isEmpty", "contains", "toArray", "add", "remove", "containsAll",
        "addAll", "removeAll", "retainAll", "clear", "equals", "hashCode",
        "get", "set", "indexOf", "lastIndexOf", "listIterator", "subList",
    },
    "Set": {
        "size", "isEmpty", "contains", "toArray", "add", "remove", "containsAll",
        "addAll", "removeAll", "retainAll", "clear", "equals", "hashCode",
    },
    "Map": {
        "size", "isEmpty", "containsKey", "containsValue", "get", "put", "remove",
        "putAll", "clear", "keySet", "values", "entrySet", "equals", "hashCode",
    },
    "Enumeration": {"hasMoreElements", "nextElement"},
    "Comparable": {"compareTo"},
    "Comparator": {"compare"},
    "Runnable": {"run"},
    "ActionListener": {"actionPerformed"},
    "MouseListener": {"mouseClicked", "mousePressed", "mouseReleased", "mouseEntered", "mouseExited"},
    "MouseMotionListener": {"mouseDragged", "mouseMoved"},
    "MouseWheelListener": {"mouseWheelMoved"},
    "KeyListener": {"keyPressed", "keyReleased", "keyTyped"},
    "FocusListener": {"focusGained", "focusLost"},
    "WindowListener": {
        "windowOpened", "windowClosing", "windowClosed", "windowIconified",
        "windowDeiconified", "windowActivated", "windowDeactivated",
    },
    "WindowFocusListener": {"windowGainedFocus", "windowLostFocus"},
    "ComponentListener": {"componentResized", "componentMoved", "componentShown", "componentHidden"},
    "ImageObserver": {"imageUpdate"},
    "CharSequence": {"length", "charAt", "subSequence"},
    "Appendable": {"append"},
    "Closeable": {"close"},
    "AutoCloseable": {"close"},
    "Flushable": {"flush"},
    "Applet": {"init", "start", "stop", "destroy"},
}
SKIP_WORDS = KEYWORDS | {
    "if", "for", "while", "switch", "catch", "do", "try", "else", "return",
    "new", "throw", "synchronized",
}

HASH_CLASS_RE = re.compile(r"^C[0-9a-f]{7,8}\d*$")
CALL_HEX_RE = re.compile(r"^call[0-9a-fA-F]+$")
OBF_NUM_RE = re.compile(
    r"^(?:method|field|class|kind)\d+$|^(?:anInt|anLong|aBoolean|aString|aClass)\d+$"
)
IDENT_RE = re.compile(r"[A-Za-z_$][A-Za-z0-9_$]*")
CLASS_RE = re.compile(
    r"\b(class|interface|enum)\s+([A-Za-z_$][\w$]*)"
    r"(?:\s+extends\s+([\w.$]+))?"
    r"(?:\s+implements\s+([^{]+))?"
)
SIG_RE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|native|synchronized|abstract|default)[ \t]+)*"
    r"(?:(?P<ret>[\w.$]+(?:[ \t]*<[^>;{}]*>)?(?:[ \t]*\[[ \t]*\])*)[ \t]+)?"
    r"(?P<name>[A-Za-z_$][\w$]*)[ \t]*\((?P<params>[^)]*)\)"
    r"[ \t]*(?:throws[ \t]+[\w.$, \t]+)?(?P<end>\{|;)[ \t]*$"
)
FIELD_RE = re.compile(
    r"^[ \t]*(?:(?:public|protected|private|static|final|volatile|transient)[ \t]+)*"
    r"(?P<type>[\w.$]+(?:[ \t]*<[^>;{}]*>)?(?:[ \t]*\[[ \t]*\])*)[ \t]+"
    r"(?P<name>[A-Za-z_$][\w$]*)[ \t]*(?:=\s*(?P<init>[^;]+))?[ \t]*;[ \t]*$"
)
LITERAL_RE = re.compile(
    r"^(?:\"(?:\\.|[^\"\\])*\"|'(?:\\.|[^'\\])'|[-+]?\d+(?:[lL])?|true|false|null)"
)
PKG_RE = re.compile(r"^package\s+([\w.]+)\s*;")
TRIVIAL_LITERALS = {
    "null", "true", "false", "0", "1", "-1", "0L", "1L", "-1L", "0l", "1l", "-1l",
    '""', "''",
}
JACCARD_MIN = 0.62
STRING_MIN = 8


def log(msg: str) -> None:
    print(msg, flush=True)


def is_obfuscated(name: str) -> bool:
    if not name:
        return True
    if HASH_CLASS_RE.fullmatch(name):
        return True
    if CALL_HEX_RE.fullmatch(name):
        return True
    if OBF_NUM_RE.fullmatch(name):
        return True
    if name.startswith("op") and any(ch.isdigit() for ch in name):
        return True
    return False


def is_readable(name: str) -> bool:
    if not name or name in KEYWORDS or name in SKIP_WORDS:
        return False
    if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", name):
        return False
    return not is_obfuscated(name)


def year_dir(rev: int) -> Path:
    hits = sorted(BUILDS.glob(f"{rev}-*"))
    if not hits:
        raise SystemExit(f"no build directory for {rev}")
    return hits[0]


def source_root(folder: Path) -> Path:
    for rel in (Path("src/src/main/java"), Path("src/client/src/main/java")):
        candidate = folder / rel
        if candidate.is_dir():
            return candidate
    raise SystemExit(f"no Java sources under {folder}")


def java_files(src: Path) -> list[Path]:
    out = []
    for path in src.rglob("*.java"):
        if "build" in path.parts:
            continue
        out.append(path)
    return out


def strip_line(line: str, state: dict) -> tuple[str, list[str]]:
    """Blank comments, strings, and char literals. Return collected string bodies."""
    out: list[str] = []
    strings: list[str] = []
    i = 0
    n = len(line)
    in_block = state.get("block", False)
    while i < n:
        if in_block:
            end = line.find("*/", i)
            if end < 0:
                out.append(" " * (n - i))
                state["block"] = True
                return "".join(out), strings
            out.append(" " * (end + 2 - i))
            i = end + 2
            in_block = False
            continue
        ch = line[i]
        if ch == "/" and i + 1 < n and line[i + 1] == "/":
            out.append(" " * (n - i))
            break
        if ch == "/" and i + 1 < n and line[i + 1] == "*":
            in_block = True
            i += 2
            continue
        if ch == '"':
            j = i + 1
            while j < n:
                if line[j] == "\\":
                    j += 2
                    continue
                if line[j] == '"':
                    j += 1
                    break
                j += 1
            body = line[i + 1:j - 1] if j > i + 1 and j <= n and line[j - 1] == '"' else line[i + 1:j]
            if STRING_MIN <= len(body) <= 200:
                strings.append(body)
            out.append('""' + " " * max(0, j - i - 2))
            i = j
            continue
        if ch == "'" and i + 1 < n:
            j = i + 1
            if j < n and line[j] == "\\":
                j += 2
            elif j < n:
                j += 1
            if j < n and line[j] == "'":
                j += 1
            out.append(" " * (j - i))
            i = j
            continue
        out.append(ch)
        i += 1
    state["block"] = False
    return "".join(out), strings


def split_params(param_text: str) -> tuple[str, ...]:
    param_text = param_text.strip()
    if not param_text:
        return ()
    parts: list[str] = []
    depth = 0
    cur: list[str] = []
    for ch in param_text:
        if ch == "<":
            depth += 1
        elif ch == ">":
            depth = max(0, depth - 1)
        elif ch == "," and depth == 0:
            parts.append("".join(cur))
            cur = []
            continue
        cur.append(ch)
    if cur:
        parts.append("".join(cur))
    types: list[str] = []
    for part in parts:
        part = re.sub(r"@\w+(?:\([^)]*\))?\s*", "", part)
        part = re.sub(r"\bfinal\s+", "", part.strip())
        part = re.sub(r"\s+", " ", part).strip()
        if not part:
            continue
        if part.endswith("..."):
            part = part[:-3] + "[]"
        toks = part.split(" ")
        if len(toks) >= 2:
            types.append("".join(toks[:-1]))
        else:
            types.append(toks[0])
    return tuple(types)


def literal_of(init: str | None) -> str | None:
    if not init:
        return None
    text = init.strip()
    match = LITERAL_RE.match(text)
    if not match:
        return None
    token = match.group(0)
    if token in TRIVIAL_LITERALS:
        return None
    if token.startswith('"'):
        body = token[1:-1]
        if len(body) < STRING_MIN:
            return None
    if re.fullmatch(r"[-+]?\d+[lL]?", token):
        number = token[:-1] if token[-1] in "lL" else token
        try:
            value = int(number)
        except ValueError:
            return None
        # Small and round constants collide across unrelated fields.
        if abs(value) < 1000 or abs(value) in {
            1024, 2048, 4096, 8192, 16384, 32768, 65535, 65536, 10000, 100000,
        }:
            return None
    return token


@dataclass
class JMethod:
    name: str
    ret: str
    params: tuple[str, ...]
    strings: set[str] = field(default_factory=set)
    static: bool = False
    private: bool = False
    proposed: str = ""

    @property
    def arity(self) -> int:
        return len(self.params)

    @property
    def raw_desc(self) -> str:
        return f"({','.join(self.params)}){self.ret}"

    def shape(self) -> str:
        return "(" + ",".join(structural_type(p) for p in self.params) + ")" + structural_type(self.ret)


@dataclass
class JField:
    name: str
    jtype: str
    init: str | None
    proposed: str = ""


@dataclass
class JClass:
    rev: int
    path: Path
    package: str
    simple: str
    kind: str
    super_name: str | None
    ifaces: tuple[str, ...]
    strings: set[str]
    methods: list[JMethod]
    fields: list[JField]
    shapes: Counter = field(default_factory=Counter)
    proposed: str = ""

    @property
    def key(self) -> tuple:
        return (self.rev, self.package, self.simple, str(self.path))


def structural_type(jtype: str) -> str:
    dims = jtype.count("[")
    base = re.sub(r"<.*$", "", jtype.replace("[", "").replace("]", ""))
    base = base.split(".")[-1]
    if base in PRIMITIVES or base in JDK_SIMPLE:
        return base + ("[]" * dims)
    return "L" + ("[]" * dims)


def simple_type(jtype: str) -> tuple[str, int]:
    dims = jtype.count("[")
    base = re.sub(r"<.*$", "", jtype.replace("[", "").replace("]", ""))
    base = base.split(".")[-1]
    return base, dims


def parse_file(rev: int, path: Path) -> list[JClass]:
    text = path.read_text(errors="ignore")
    package = ""
    pkg = PKG_RE.search(text)
    if pkg:
        package = pkg.group(1)
    lines = text.splitlines()
    state: dict = {"block": False}
    depth = 0
    classes: list[JClass] = []
    stack: list[tuple[JClass, int]] = []
    current_method: JMethod | None = None
    method_base: int | None = None

    def current_class() -> JClass | None:
        return stack[-1][0] if stack else None

    for line in lines:
        bare, strings = strip_line(line, state)
        jc = current_class()
        if jc is not None and current_method is None and depth == stack[-1][1]:
            for body in strings:
                jc.strings.add(body)
        if current_method is not None:
            current_method.strings.update(strings)
            if jc is not None:
                jc.strings.update(strings)
        header = CLASS_RE.search(bare)
        if (
            current_method is None
            and header
            and "{" in bare
            and not bare.strip().endswith(";")
        ):
            simple = header.group(2)
            super_name = header.group(3).split(".")[-1] if header.group(3) else None
            ifaces: list[str] = []
            if header.group(4):
                for part in header.group(4).split(","):
                    part = part.strip()
                    if not part or part.startswith("{"):
                        continue
                    ifaces.append(part.split(".")[-1].split("<")[0].strip())
            born = JClass(
                rev, path, package, simple, header.group(1), super_name,
                tuple(ifaces), set(strings), [], [], Counter(), "",
            )
            # The class body starts after the brace on this line.
            body_depth = depth + 1
            stack.append((born, body_depth))
            classes.append(born)
            depth += bare.count("{") - bare.count("}")
            continue
        if jc is not None and current_method is None and depth == stack[-1][1]:
            sig = SIG_RE.match(bare)
            if sig and sig.group("name") not in SKIP_WORDS:
                name = sig.group("name")
                if name != jc.simple and name not in {jc.simple}:
                    ret = sig.group("ret") or "void"
                    ret = re.sub(r"\s+", "", ret)
                    params = split_params(sig.group("params") or "")
                    method = JMethod(
                        name, ret, params, set(strings),
                        static=("static" in bare.split("(")[0]),
                        private=bare.lstrip().startswith("private"),
                    )
                    # A one-line abstract/native declaration has no brace. SIG requires '{'.
                    jc.methods.append(method)
                    jc.shapes[method.shape()] += 1
                    depth += bare.count("{") - bare.count("}")
                    if sig.group("end") == "{" and depth > depth - bare.count("{") + bare.count("}"):
                        current_method = method
                        method_base = depth - bare.count("{") + bare.count("}")
                    continue
            field_match = FIELD_RE.match(line if "=" in line else bare)
            if field_match is None:
                field_match = FIELD_RE.match(bare)
            if field_match and field_match.group("name") not in SKIP_WORDS:
                fname = field_match.group("name")
                if fname[:1].islower() or fname.startswith("_") or is_obfuscated(fname) or is_readable(fname):
                    ftype = re.sub(r"\s+", "", field_match.group("type"))
                    init_src = field_match.group("init")
                    jc.fields.append(JField(fname, ftype, literal_of(init_src)))
        opens = bare.count("{")
        closes = bare.count("}")
        depth += opens - closes
        if depth < 0:
            depth = 0
        if current_method is not None and method_base is not None and depth <= method_base:
            current_method = None
            method_base = None
        while stack and depth < stack[-1][1]:
            stack.pop()
            current_method = None
            method_base = None
    for jc in classes:
        jc.proposed = jc.simple
        for method in jc.methods:
            method.proposed = method.name
        for fld in jc.fields:
            fld.proposed = fld.name
    return classes


def load_all() -> dict[int, list[JClass]]:
    out: dict[int, list[JClass]] = {}
    for rev in REVS:
        folder = year_dir(rev)
        src = source_root(folder)
        classes: list[JClass] = []
        for path in java_files(src):
            classes.extend(parse_file(rev, path))
        out[rev] = classes
        log(f"parsed {rev}: {len(classes)} classes from {src}")
    return out


class UnionFind:
    def __init__(self) -> None:
        self.p: dict[tuple, tuple] = {}

    def add(self, item: tuple) -> None:
        self.p.setdefault(item, item)

    def find(self, item: tuple) -> tuple:
        self.add(item)
        parent = self.p[item]
        if parent != item:
            self.p[item] = self.find(parent)
        return self.p[item]

    def union(self, a: tuple, b: tuple) -> None:
        ra, rb = self.find(a), self.find(b)
        if ra != rb:
            self.p[rb] = ra

    def groups(self) -> dict[tuple, list[tuple]]:
        out: dict[tuple, list[tuple]] = defaultdict(list)
        for item in list(self.p):
            out[self.find(item)].append(item)
        return out


def class_clusters(classes: dict[int, list[JClass]]) -> tuple[dict[tuple, list[JClass]], dict[tuple, tuple], int]:
    by_key: dict[tuple, JClass] = {}
    uf = UnionFind()
    for items in classes.values():
        for jc in items:
            by_key[jc.key] = jc
            uf.add(jc.key)
    readable_names: dict[tuple, set[str]] = {}
    years: dict[tuple, set[int]] = {}
    for key, jc in by_key.items():
        readable_names[key] = {jc.simple} if is_readable(jc.simple) else set()
        years[key] = {jc.rev}
    blocked = 0

    def safe_union(a: tuple, b: tuple) -> bool:
        nonlocal blocked
        ra, rb = uf.find(a), uf.find(b)
        if ra == rb:
            return False
        if years[ra] & years[rb]:
            blocked += 1
            return False
        na, nb = readable_names[ra], readable_names[rb]
        if na and nb and na != nb:
            blocked += 1
            return False
        uf.union(a, b)
        root = uf.find(ra)
        other = rb if root == ra else ra
        years[root] = years[ra] | years[rb]
        readable_names[root] = readable_names[ra] | readable_names[rb]
        years.pop(other, None)
        readable_names.pop(other, None)
        return True

    # Same already-readable simple name, one class per year.
    name_links = 0
    by_exact: dict[str, list[JClass]] = defaultdict(list)
    for jc in by_key.values():
        if is_readable(jc.simple):
            by_exact[jc.simple].append(jc)
    for group in by_exact.values():
        per_year: dict[int, list[JClass]] = defaultdict(list)
        for jc in group:
            per_year[jc.rev].append(jc)
        singles = [lst[0] for rev, lst in per_year.items() if len(lst) == 1]
        if len(singles) < 2:
            continue
        anchor = singles[0].key
        for other in singles[1:]:
            if safe_union(anchor, other.key):
                name_links += 1
    log(f"class name links={name_links} blocked={blocked}")

    # Distinctive strings: length >= 8 and unique to one class in that year.
    owners: dict[int, dict[str, list[JClass]]] = {}
    for rev, items in classes.items():
        bucket: dict[str, list[JClass]] = defaultdict(list)
        for jc in items:
            for text in jc.strings:
                if len(text) >= STRING_MIN:
                    bucket[text].append(jc)
        owners[rev] = bucket
    string_edges: list[tuple[int, tuple, tuple]] = []
    seen_pairs: set[tuple] = set()
    for rev_i, rev in enumerate(REVS):
        for other_rev in REVS[rev_i + 1:]:
            for text, left in owners[rev].items():
                if len(left) != 1 or len(text) < STRING_MIN:
                    continue
                right = owners[other_rev].get(text)
                if not right or len(right) != 1:
                    continue
                a, b = left[0].key, right[0].key
                pair = (a, b) if a < b else (b, a)
                if pair in seen_pairs:
                    continue
                seen_pairs.add(pair)
                string_edges.append((len(text), a, b))
    string_edges.sort(key=lambda item: -item[0])
    string_links = 0
    for _length, a, b in string_edges:
        if safe_union(a, b):
            string_links += 1
    log(f"class string links={string_links} blocked={blocked}")

    def resolve(rev: int, simple: str, package: str) -> JClass | None:
        found = [
            jc for jc in classes[rev]
            if jc.simple == simple
        ]
        if not found:
            return None
        same = [jc for jc in found if jc.package == package]
        if len(same) == 1:
            return same[0]
        if len(found) == 1:
            return found[0]
        return None

    def supers_agree(a: JClass, b: JClass) -> bool:
        if not a.super_name or not b.super_name:
            return False
        if a.super_name in JDK_SUPERS or b.super_name in JDK_SUPERS:
            return False
        a_ob = is_obfuscated(a.super_name)
        b_ob = is_obfuscated(b.super_name)
        if not a_ob and not b_ob:
            return a.super_name == b.super_name
        if a_ob and b_ob:
            sa = resolve(a.rev, a.super_name, a.package)
            sb = resolve(b.rev, b.super_name, b.package)
            if not sa or not sb:
                return False
            return uf.find(sa.key) == uf.find(sb.key)
        return False

    def jaccard(a: Counter, b: Counter) -> float:
        if not a and not b:
            return 0.0
        inter = sum((a & b).values())
        union = sum((a | b).values())
        return inter / union if union else 0.0

    adjacent_links = 0
    for _round in range(3):
        gained = 0
        for left_rev, right_rev in ADJACENT:
            right_df: Counter = Counter()
            for jc in classes[right_rev]:
                for text in jc.strings:
                    if len(text) >= STRING_MIN:
                        right_df[text] += 1
            left_df: Counter = Counter()
            for jc in classes[left_rev]:
                for text in jc.strings:
                    if len(text) >= STRING_MIN:
                        left_df[text] += 1
            index: dict[str, list[JClass]] = defaultdict(list)
            for jc in classes[right_rev]:
                for text in jc.strings:
                    if len(text) >= STRING_MIN and right_df[text] <= 3:
                        index[text].append(jc)
            best_left: dict[tuple, tuple] = {}
            for jc in classes[left_rev]:
                scores: dict[tuple, int] = defaultdict(int)
                for text in jc.strings:
                    if len(text) < STRING_MIN or left_df[text] > 3:
                        continue
                    for other in index.get(text, ()):
                        scores[other.key] += len(text)
                ranked = []
                for other_key, score in scores.items():
                    other = by_key[other_key]
                    if not supers_agree(jc, other):
                        continue
                    jac = jaccard(jc.shapes, other.shapes)
                    if jac < JACCARD_MIN or score < STRING_MIN:
                        continue
                    ranked.append((jac, score, other_key))
                if not ranked:
                    continue
                ranked.sort(reverse=True)
                if len(ranked) > 1:
                    jac1, score1, _ = ranked[0]
                    jac2, score2, _ = ranked[1]
                    if jac1 < jac2 + 0.08 and score1 < score2 * 1.25:
                        continue
                best_left[jc.key] = ranked[0]
            best_right: dict[tuple, tuple] = {}
            index_left: dict[str, list[JClass]] = defaultdict(list)
            for jc in classes[left_rev]:
                for text in jc.strings:
                    if len(text) >= STRING_MIN and left_df[text] <= 3:
                        index_left[text].append(jc)
            for jc in classes[right_rev]:
                scores = defaultdict(int)
                for text in jc.strings:
                    if len(text) < STRING_MIN or right_df[text] > 3:
                        continue
                    for other in index_left.get(text, ()):
                        scores[other.key] += len(text)
                ranked = []
                for other_key, score in scores.items():
                    other = by_key[other_key]
                    if not supers_agree(jc, other):
                        continue
                    jac = jaccard(jc.shapes, other.shapes)
                    if jac < JACCARD_MIN or score < STRING_MIN:
                        continue
                    ranked.append((jac, score, other_key))
                if not ranked:
                    continue
                ranked.sort(reverse=True)
                if len(ranked) > 1:
                    jac1, score1, _ = ranked[0]
                    jac2, score2, _ = ranked[1]
                    if jac1 < jac2 + 0.08 and score1 < score2 * 1.25:
                        continue
                best_right[jc.key] = ranked[0]
            for left_key, (jac, score, right_key) in best_left.items():
                back = best_right.get(right_key)
                if not back or back[2] != left_key:
                    continue
                if safe_union(left_key, right_key):
                    gained += 1
        adjacent_links += gained
        log(f"adjacent round gained={gained}")
        if not gained:
            break
    log(f"class adjacent links={adjacent_links} blocked={blocked}")

    groups: dict[tuple, list[JClass]] = defaultdict(list)
    root_of: dict[tuple, tuple] = {}
    for key, jc in by_key.items():
        root = uf.find(key)
        groups[root].append(jc)
        root_of[key] = root
    return groups, root_of, blocked


def rt4_vocab(classes: dict[int, list[JClass]]) -> set[str]:
    names: set[str] = set()
    for jc in classes.get(530, []):
        if jc.package != "rt4":
            continue
        if is_readable(jc.simple):
            names.add(jc.simple)
        for method in jc.methods:
            if is_readable(method.name):
                names.add(method.name)
        for fld in jc.fields:
            if is_readable(fld.name):
                names.add(fld.name)
    return names


def readable_prio(rev: int, package: str, name: str, rt4: set[str]) -> int:
    if rev in ANCHOR_PRIO:
        return ANCHOR_PRIO[rev]
    if rev == 530 and package == "rt4" and is_readable(name):
        return RT4_PRIO
    if name in rt4 and is_readable(name):
        return RT4_PRIO - 5
    if is_readable(name):
        return OTHER_READABLE_PRIO
    return 0


def choose_name(cands: list[tuple[int, str, str]], rt4: set[str]) -> str:
    """cands are (rev, package, name). Prefer a readable name by anchor order."""
    readable = [c for c in cands if is_readable(c[2])]
    if readable:
        freq = Counter(name for _rev, _pkg, name in readable)
        best = sorted(
            readable,
            key=lambda item: (
                -readable_prio(item[0], item[1], item[2], rt4),
                -freq[item[2]],
                item[2],
            ),
        )
        return best[0][2]
    freq_all = Counter(name for _rev, _pkg, name in cands)
    return sorted(freq_all, key=lambda name: (-freq_all[name], name))[0]


def assign_class_names(
    groups: dict[tuple, list[JClass]], rt4: set[str],
) -> None:
    multi = single = renamed = 0
    for members in groups.values():
        years = {jc.rev for jc in members}
        if len(years) < 2:
            single += 1
            continue
        multi += 1
        chosen = choose_name([(jc.rev, jc.package, jc.simple) for jc in members], rt4)
        readable_chosen = is_readable(chosen)
        for jc in members:
            if jc.simple == chosen:
                jc.proposed = chosen
                continue
            if is_readable(jc.simple):
                # Different readable class names are not in this cluster.
                jc.proposed = jc.simple
                continue
            if readable_chosen or not is_readable(jc.simple):
                jc.proposed = chosen
                renamed += 1
    log(f"class multi-year clusters={multi} single-year={single} proposed renames={renamed}")


def norm_type(rev: int, jtype: str, root_of: dict[tuple, tuple], by_simple: dict[tuple, JClass]) -> str:
    base, dims = simple_type(jtype)
    suffix = "[]" * dims
    if base in PRIMITIVES or base in JDK_SIMPLE:
        return base + suffix
    owner = by_simple.get((rev, base))
    if owner is None:
        return ("L" if is_obfuscated(base) else base) + suffix
    root = root_of.get(owner.key, owner.key)
    return "T" + str(root[0]) + "x" + str(abs(hash(root)) % 10**8) + suffix


def method_clusters(
    classes: dict[int, list[JClass]],
    groups: dict[tuple, list[JClass]],
    root_of: dict[tuple, tuple],
    rt4: set[str],
) -> tuple[int, int]:
    by_simple: dict[tuple, JClass] = {}
    for items in classes.values():
        for jc in items:
            # First class with this simple name wins when duplicates exist.
            by_simple.setdefault((jc.rev, jc.simple), jc)
    uf = UnionFind()
    method_of: dict[tuple, tuple[JClass, JMethod]] = {}

    def desc_of(jc: JClass, method: JMethod) -> str:
        params = ",".join(norm_type(jc.rev, p, root_of, by_simple) for p in method.params)
        ret = norm_type(jc.rev, method.ret, root_of, by_simple)
        return f"({params}){ret}"

    ident_of: dict[tuple, tuple] = {}
    for root, members in groups.items():
        if len({jc.rev for jc in members}) < 2:
            continue
        per_class_methods: list[tuple[JClass, JMethod, str, tuple]] = []
        for jc in members:
            seen_desc: dict[str, int] = defaultdict(int)
            for index, method in enumerate(jc.methods):
                if method.name in JDK_CALLBACKS:
                    continue
                desc = desc_of(jc, method)
                seen_desc[desc] += 1
                ident = (jc.rev, jc.key, index, method.name, desc)
                # Two overloads that normalize to one descriptor stay distinct
                # by index, and are not unioned with each other below.
                per_class_methods.append((jc, method, desc, ident))
                uf.add(ident)
                method_of[ident] = (jc, method)
                ident_of[ident] = ident
        # Same non-JDK name and same descriptor.
        by_name: dict[tuple[str, str], list[tuple]] = defaultdict(list)
        for jc, method, desc, ident in per_class_methods:
            if method.name in JDK_CALLBACKS:
                continue
            by_name[(desc, method.name)].append(ident)
        for idents in by_name.values():
            if len(idents) < 2:
                continue
            years = [method_of[item][0].rev for item in idents]
            if len(years) != len(set(years)):
                continue
            head = idents[0]
            for other in idents[1:]:
                uf.union(head, other)
        # Distinctive string inside the class, same descriptor.
        by_string: dict[tuple[str, str], list[tuple]] = defaultdict(list)
        for jc, method, desc, ident in per_class_methods:
            counts: Counter = Counter()
            for other in jc.methods:
                for text in other.strings:
                    if len(text) >= STRING_MIN:
                        counts[text] += 1
            for text in method.strings:
                if len(text) >= STRING_MIN and counts[text] == 1:
                    by_string[(desc, text)].append(ident)
        for idents in by_string.values():
            if len(idents) < 2:
                continue
            # One method per year.
            per_year: dict[int, list[tuple]] = defaultdict(list)
            for ident in idents:
                per_year[method_of[ident][0].rev].append(ident)
            singles = [lst[0] for lst in per_year.values() if len(lst) == 1]
            if len(singles) < 2:
                continue
            # Reject if a class would contribute two methods. Already one per year
            # and one class per year in the class cluster.
            head = singles[0]
            for other in singles[1:]:
                uf.union(head, other)

    multi = single = 0
    renamed = 0
    for root, idents in uf.groups().items():
        for ident in idents:
            method_of[ident][1].cluster = root
        years = {method_of[item][0].rev for item in idents}
        if len(years) < 2:
            single += len(idents)
            continue
        multi += 1
        cands = []
        for ident in idents:
            jc, method = method_of[ident]
            cands.append((jc.rev, jc.package, method.name))
        chosen = choose_name(cands, rt4)
        for ident in idents:
            _jc, method = method_of[ident]
            if method.name == chosen:
                continue
            if method.name in JDK_CALLBACKS:
                continue
            if is_readable(chosen) and is_obfuscated(method.name):
                method.proposed = chosen
                renamed += 1
            elif not is_readable(chosen) and method.name != chosen:
                method.proposed = chosen
                renamed += 1
            elif is_readable(chosen) and is_readable(method.name) and method.name != chosen:
                # Anchor order picks one shared spelling.
                method.proposed = chosen
                renamed += 1
    # Methods that were never inserted stay at their original name.
    log(f"method multi-year clusters={multi} single-year members={single} proposed renames={renamed}")
    return multi, single


def field_clusters(
    groups: dict[tuple, list[JClass]],
    root_of: dict[tuple, tuple],
    classes: dict[int, list[JClass]],
    rt4: set[str],
) -> tuple[int, int]:
    by_simple: dict[tuple, JClass] = {}
    for items in classes.values():
        for jc in items:
            by_simple.setdefault((jc.rev, jc.simple), jc)
    # Literals that show up everywhere are not evidence.
    literal_df: Counter = Counter()
    for items in classes.values():
        seen: set[str] = set()
        for jc in items:
            for fld in jc.fields:
                if fld.init and fld.init not in seen:
                    seen.add(fld.init)
                    literal_df[fld.init] += 1
    uf = UnionFind()
    field_of: dict[tuple, tuple[JClass, JField]] = {}
    for root, members in groups.items():
        if len({jc.rev for jc in members}) < 2:
            continue
        bag: list[tuple[JClass, JField, str, str | None, tuple]] = []
        for jc in members:
            desc_counts: dict[str, list[JField]] = defaultdict(list)
            typed: list[tuple[JField, str]] = []
            for index, fld in enumerate(jc.fields):
                desc = norm_type(jc.rev, fld.jtype, root_of, by_simple)
                typed.append((fld, desc))
                desc_counts[desc].append(fld)
            init_counts: dict[tuple[str, str], int] = Counter()
            for fld, desc in typed:
                if fld.init:
                    init_counts[(desc, fld.init)] += 1
            for index, (fld, desc) in enumerate(typed):
                ident = (jc.rev, jc.key, index, fld.name, desc)
                uf.add(ident)
                field_of[ident] = (jc, fld)
                init = fld.init
                if init and init_counts[(desc, init)] != 1:
                    init = None
                if init and literal_df[init] > 4:
                    init = None
                bag.append((jc, fld, desc, init, ident))
        by_name: dict[tuple[str, str], list[tuple]] = defaultdict(list)
        by_init: dict[tuple[str, str], list[tuple]] = defaultdict(list)
        for jc, fld, desc, init, ident in bag:
            by_name[(desc, fld.name)].append(ident)
            if init:
                by_init[(desc, init)].append(ident)
        for idents in by_name.values():
            years = [field_of[item][0].rev for item in idents]
            if len(set(years)) < 2 or len(years) != len(set(years)):
                continue
            head = idents[0]
            for other in idents[1:]:
                uf.union(head, other)
        for idents in by_init.values():
            per_year: dict[int, list[tuple]] = defaultdict(list)
            for ident in idents:
                per_year[field_of[ident][0].rev].append(ident)
            singles = [lst[0] for lst in per_year.values() if len(lst) == 1]
            if len(singles) < 2:
                continue
            head = singles[0]
            for other in singles[1:]:
                uf.union(head, other)
    multi = single = renamed = 0
    for root, idents in uf.groups().items():
        for ident in idents:
            field_of[ident][1].cluster = root
        years = {field_of[item][0].rev for item in idents}
        if len(years) < 2:
            single += len(idents)
            continue
        multi += 1
        cands = []
        for ident in idents:
            jc, fld = field_of[ident]
            cands.append((jc.rev, jc.package, fld.name))
        chosen = choose_name(cands, rt4)
        for ident in idents:
            _jc, fld = field_of[ident]
            if fld.name == chosen:
                continue
            if is_readable(chosen) and is_obfuscated(fld.name):
                fld.proposed = chosen
            elif not is_readable(chosen):
                fld.proposed = chosen
            elif is_readable(chosen) and is_readable(fld.name):
                fld.proposed = chosen
            else:
                continue
            renamed += 1
    log(f"field multi-year clusters={multi} single-year members={single} proposed renames={renamed}")
    return multi, single


def jdk_method(jc: JClass, method: JMethod) -> bool:
    if method.name in JDK_CALLBACKS:
        return True
    if method.name == "main" and method.params == ("String[]",):
        return True
    owners = [jc.super_name, *jc.ifaces]
    for owner in owners:
        if owner and method.name in IFACE_METHODS.get(owner, ()):
            return True
    if method.name in {"equals", "hashCode", "toString", "clone", "finalize"}:
        return True
    return False


def enforce_overrides(classes: dict[int, list[JClass]]) -> int:
    """If an override slot would split, keep every existing name in the slot."""
    reverted = 0
    for rev, items in classes.items():
        by_name: dict[str, list[JClass]] = defaultdict(list)
        for jc in items:
            by_name[jc.simple].append(jc)

        def parents_of(jc: JClass) -> list[JClass]:
            found: list[JClass] = []
            for simple in (jc.super_name, *jc.ifaces):
                if not simple or simple in JDK_SUPERS or simple in JDK_SIMPLE:
                    continue
                cands = by_name.get(simple, [])
                same = [c for c in cands if c.package == jc.package]
                if len(same) == 1:
                    found.append(same[0])
                elif len(cands) == 1:
                    found.append(cands[0])
            return found

        uf = UnionFind()
        lookup: dict[tuple, JMethod] = {}
        for jc in items:
            for method in jc.methods:
                if method.static or method.private:
                    continue
                ident = (jc.key, method.name, method.raw_desc)
                uf.add(ident)
                lookup[ident] = method
                for parent in parents_of(jc):
                    for other in parent.methods:
                        if other.static or other.private:
                            continue
                        if other.name == method.name and other.raw_desc == method.raw_desc:
                            other_ident = (parent.key, other.name, other.raw_desc)
                            uf.add(other_ident)
                            lookup[other_ident] = other
                            uf.union(ident, other_ident)
        for idents in uf.groups().values():
            methods = [lookup[item] for item in idents if item in lookup]
            proposed = {method.proposed for method in methods}
            if len(proposed) > 1:
                for method in methods:
                    if method.proposed != method.name:
                        reverted += 1
                    method.proposed = method.name
    log(f"override slots reverted members={reverted}")
    return reverted


def enforce_overloads(classes: dict[int, list[JClass]]) -> int:
    reverted = 0
    for items in classes.values():
        for jc in items:
            by_arity: dict[tuple[str, int], list[JMethod]] = defaultdict(list)
            for method in jc.methods:
                by_arity[(method.name, method.arity)].append(method)
            for group in by_arity.values():
                if len(group) < 2:
                    continue
                for method in group:
                    if method.proposed != method.name:
                        reverted += 1
                    method.proposed = method.name
            # Proposed name must not create a same-arity clash.
            proposed_groups: dict[tuple[str, int], list[JMethod]] = defaultdict(list)
            for method in jc.methods:
                proposed_groups[(method.proposed, method.arity)].append(method)
            for group in proposed_groups.values():
                if len(group) < 2:
                    continue
                originals = {method.name for method in group}
                if len(originals) == 1:
                    continue
                for method in group:
                    if method.proposed != method.name:
                        reverted += 1
                        method.proposed = method.name
    log(f"overload guards reverted members={reverted}")
    return reverted


def freeze_jdk(classes: dict[int, list[JClass]]) -> int:
    frozen = 0
    for items in classes.values():
        for jc in items:
            for method in jc.methods:
                if jdk_method(jc, method) and method.proposed != method.name:
                    method.proposed = method.name
                    frozen += 1
    log(f"jdk callbacks kept={frozen}")
    return frozen


def build_maps(classes: dict[int, list[JClass]]) -> dict[int, dict[str, str]]:
    """Year-wide identifier maps. A name is replaced only when every declaration agrees."""
    maps: dict[int, dict[str, str]] = {}
    for rev, items in classes.items():
        decls: dict[str, set[str]] = defaultdict(set)
        class_targets: dict[str, set[tuple[str, str]]] = defaultdict(set)
        for jc in items:
            decls[jc.simple].add(jc.proposed)
            class_targets[jc.proposed].add((jc.package, jc.simple))
            for method in jc.methods:
                decls[method.name].add(method.proposed)
            for fld in jc.fields:
                decls[fld.name].add(fld.proposed)
        mapping: dict[str, str] = {}
        for old, targets in decls.items():
            if len(targets) != 1:
                continue
            new = next(iter(targets))
            if new == old or new in KEYWORDS or not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", new):
                continue
            # The target token must not already be a different declaration we are keeping.
            if new in decls and old != new:
                staying = decls[new]
                if staying != {new} and new not in mapping:
                    # Some declaration still uses `new` as its source name and is not moving.
                    if any(target == new for target in staying):
                        continue
            mapping[old] = new
        # Class files in one package cannot collide.
        used_packages: dict[tuple[str, str], str] = {}
        drop: set[str] = set()
        for jc in items:
            target = mapping.get(jc.simple, jc.simple)
            slot = (jc.package, target)
            previous = used_packages.get(slot)
            if previous is not None and previous != jc.simple:
                drop.add(jc.simple)
            else:
                used_packages[slot] = jc.simple
        for old in drop:
            mapping.pop(old, None)
        # If we dropped a class rename, a method of the same token must not move either
        # when the map entry was the class. Methods use their own tokens.
        maps[rev] = mapping
        log(f"map {rev}: {len(mapping)} identifiers")
    return maps


def apply_idents(text: str, mapping: dict[str, str]) -> str:
    if not mapping:
        return text
    out: list[str] = []
    i = 0
    n = len(text)
    while i < n:
        ch = text[i]
        if ch == "/" and i + 1 < n and text[i + 1] == "/":
            end = text.find("\n", i)
            end = n if end < 0 else end
            out.append(text[i:end])
            i = end
            continue
        if ch == "/" and i + 1 < n and text[i + 1] == "*":
            end = text.find("*/", i + 2)
            end = n if end < 0 else end + 2
            out.append(text[i:end])
            i = end
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
            i = j
            continue
        if ch == "'":
            j = i + 1
            if j < n and text[j] == "\\":
                j += 2
            elif j < n:
                j += 1
            if j < n and text[j] == "'":
                j += 1
            out.append(text[i:j])
            i = j
            continue
        match = IDENT_RE.match(text, i)
        if match:
            word = match.group(0)
            out.append(mapping.get(word, word))
            i = match.end()
            continue
        out.append(ch)
        i += 1
    return "".join(out)


def public_class_name(text: str) -> str | None:
    match = re.search(
        r"(?m)^(?:public\s+(?:(?:abstract|final|static)\s+)*)?(?:class|interface|enum)\s+([A-Za-z_$][\w$]*)",
        text,
    )
    return match.group(1) if match else None


def backup_file(rev: int, src: Path, path: Path) -> None:
    dest = BACKUP / str(rev) / path.relative_to(src)
    if dest.exists():
        return
    dest.parent.mkdir(parents=True, exist_ok=True)
    shutil.copy2(path, dest)


def apply_maps(classes: dict[int, list[JClass]], maps: dict[int, dict[str, str]]) -> dict:
    """Rewrite sources. Returns records for restore and counts."""
    records: dict[str, list[dict]] = {}
    counts = Counter()
    for rev, items in classes.items():
        mapping = maps.get(rev, {})
        if not mapping:
            records[str(rev)] = []
            continue
        src = source_root(year_dir(rev))
        by_path: dict[Path, list[JClass]] = defaultdict(list)
        for jc in items:
            by_path[jc.path].append(jc)
        blocked_files: set[str] = set()
        for path in by_path:
            new_simple = mapping.get(path.stem)
            if not new_simple or new_simple == path.stem:
                continue
            dest = path.with_name(new_simple + ".java")
            if dest.exists():
                blocked_files.add(path.stem)
                log(f"skip file clobber {rev} {path.name} -> {dest.name}")
        if blocked_files:
            mapping = {old: new for old, new in mapping.items() if old not in blocked_files}
        rev_records = []
        renames: list[tuple[Path, Path]] = []
        for path, owners in by_path.items():
            original = path.read_text(errors="ignore")
            updated = apply_idents(original, mapping)
            if updated == original:
                continue
            backup_file(rev, src, path)
            dest = path
            if path.stem in mapping and mapping[path.stem] != path.stem:
                candidate = path.with_name(mapping[path.stem] + ".java")
                if not candidate.exists():
                    dest = candidate
            path.write_text(updated)
            if dest != path:
                renames.append((path, dest))
            rev_records.append({
                "old": str(path.relative_to(src)),
                "new": str(dest.relative_to(src)),
            })
            counts["files"] += 1
        # Two-step rename so two files can exchange names without clobbering.
        temps: list[tuple[Path, Path]] = []
        for src_path, dest in renames:
            tmp = src_path.with_name(f".__share_{src_path.stem}__.java")
            n = 0
            while tmp.exists():
                n += 1
                tmp = src_path.with_name(f".__share_{src_path.stem}_{n}__.java")
            src_path.rename(tmp)
            temps.append((tmp, dest))
        for tmp, dest in temps:
            if dest.exists():
                tmp.rename(dest.with_name(tmp.name + ".clash.java"))
                log(f"clash leaving {tmp.name} beside {dest.name}")
                continue
            tmp.rename(dest)
            counts["files_renamed"] += 1
        records[str(rev)] = rev_records
        for old, new in mapping.items():
            if HASH_CLASS_RE.fullmatch(old) or (is_obfuscated(old) and old[:1].isupper()):
                counts["class_idents"] += 1
            elif old.startswith("op") or CALL_HEX_RE.fullmatch(old) or old.startswith("method"):
                counts["method_idents"] += 1
            else:
                counts["other_idents"] += 1
    return {"records": records, "token_buckets": dict(counts)}


def member_counts(classes: dict[int, list[JClass]], maps: dict[int, dict[str, str]]) -> dict:
    class_renames = method_renames = field_renames = 0
    class_multi = class_multi_shared = class_single = 0
    # Recompute sharing from proposed names that survived the year map.
    def final_class(jc: JClass) -> str:
        return maps.get(jc.rev, {}).get(jc.simple, jc.simple)

    def final_method(jc: JClass, method: JMethod) -> str:
        return maps.get(jc.rev, {}).get(method.name, method.name)

    def final_field(jc: JClass, fld: JField) -> str:
        return maps.get(jc.rev, {}).get(fld.name, fld.name)

    # Clusters are recovered by proposed identity stored before maps dropped
    # conflicts. Sharing is "final names equal and the cluster spans years".
    # Use the object groups via proposed class name only when the map applied.
    for items in classes.values():
        for jc in items:
            if final_class(jc) != jc.simple:
                class_renames += 1
            for method in jc.methods:
                if final_method(jc, method) != method.name:
                    method_renames += 1
            for fld in jc.fields:
                if final_field(jc, fld) != fld.name:
                    field_renames += 1
    return {
        "class_renames": class_renames,
        "method_renames": method_renames,
        "field_renames": field_renames,
        "class_multi": class_multi,
        "class_multi_shared": class_multi_shared,
        "class_single": class_single,
    }


def shared_stats(classes: dict[int, list[JClass]], groups: dict[tuple, list[JClass]], root_of: dict[tuple, tuple], maps: dict[int, dict[str, str]]) -> dict:
    def fmap(rev: int, name: str) -> str:
        return maps.get(rev, {}).get(name, name)

    class_multi = class_shared = class_single = 0
    for members in groups.values():
        years = {jc.rev for jc in members}
        if len(years) < 2:
            class_single += len(members)
            continue
        class_multi += 1
        names = {fmap(jc.rev, jc.simple) for jc in members}
        if len(names) == 1:
            class_shared += 1

    method_groups: dict[tuple, list[tuple[JClass, JMethod]]] = defaultdict(list)
    method_single = 0
    for items in classes.values():
        for jc in items:
            for method in jc.methods:
                cluster = getattr(method, "cluster", None)
                if cluster is None:
                    method_single += 1
                else:
                    method_groups[cluster].append((jc, method))
    method_multi = method_shared = 0
    for members in method_groups.values():
        years = {jc.rev for jc, _method in members}
        if len(years) < 2:
            method_single += len(members)
            continue
        method_multi += 1
        names = {fmap(jc.rev, method.name) for jc, method in members}
        if len(names) == 1:
            method_shared += 1

    field_groups: dict[tuple, list[tuple[JClass, JField]]] = defaultdict(list)
    field_single = 0
    for items in classes.values():
        for jc in items:
            for fld in jc.fields:
                cluster = getattr(fld, "cluster", None)
                if cluster is None:
                    field_single += 1
                else:
                    field_groups[cluster].append((jc, fld))
    field_multi = field_shared = 0
    for members in field_groups.values():
        years = {jc.rev for jc, _fld in members}
        if len(years) < 2:
            field_single += len(members)
            continue
        field_multi += 1
        names = {fmap(jc.rev, fld.name) for jc, fld in members}
        if len(names) == 1:
            field_shared += 1
    return {
        "class_multi_clusters": class_multi,
        "class_multi_shared": class_shared,
        "class_single_members": class_single,
        "method_multi_clusters": method_multi,
        "method_multi_shared": method_shared,
        "method_single_members": method_single,
        "field_multi_clusters": field_multi,
        "field_multi_shared": field_shared,
        "field_single_members": field_single,
    }


GRADLE_DIST = Path("/tmp/gradle-rs3-831/wrapper/dists/gradle-9.3.0-bin")


def seed_gradle_home(rev: int) -> None:
    """Each revision gets its own Gradle home. Seed the wrapper dist so --offline works."""
    home = Path(f"/tmp/gradle-rs2-{rev}")
    dest = home / "wrapper" / "dists" / "gradle-9.3.0-bin"
    if dest.exists() or not GRADLE_DIST.is_dir():
        home.mkdir(parents=True, exist_ok=True)
        return
    dest.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(["cp", "-a", str(GRADLE_DIST), str(dest)], check=False)


def gradle_cmd(folder: Path) -> list[str]:
    if (folder / "src" / "build.gradle").exists():
        return ["./gradlew", "compileJava", "--offline"]
    return ["./gradlew", ":client:compileJava", "--offline"]


def compile_rev(rev: int) -> tuple[bool, str]:
    folder = year_dir(rev)
    src = folder / "src"
    seed_gradle_home(rev)
    env = os.environ.copy()
    env["JAVA_HOME"] = str(JAVA_HOME)
    env["PATH"] = str(JAVA_HOME / "bin") + os.pathsep + env.get("PATH", "")
    env["GRADLE_USER_HOME"] = f"/tmp/gradle-rs2-{rev}"
    log_path = WORK / f"compile-{rev}.log"
    WORK.mkdir(parents=True, exist_ok=True)
    with log_path.open("w") as handle:
        proc = subprocess.run(
            gradle_cmd(folder),
            cwd=src,
            env=env,
            stdout=handle,
            stderr=subprocess.STDOUT,
            check=False,
        )
    text = log_path.read_text(errors="ignore")
    tail = "\n".join(text.splitlines()[-30:])
    return proc.returncode == 0, tail


def restore_rev(rev: int, records: list[dict]) -> None:
    src = source_root(year_dir(rev))
    bdir = BACKUP / str(rev)
    for rec in records:
        old = src / rec["old"]
        new = src / rec["new"]
        backed = bdir / rec["old"]
        if rec["new"] != rec["old"] and new.exists() and new != old:
            new.unlink()
        if not backed.exists():
            log(f"missing backup for {rev} {rec['old']}")
            continue
        old.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(backed, old)


def update_status(rev: int, compiled: bool) -> None:
    path = year_dir(rev) / "status.json"
    if not path.exists():
        return
    text = path.read_text()
    updated, n = re.subn(
        r'"compiled"\s*:\s*(true|false)',
        f'"compiled": {str(compiled).lower()}',
        text,
        count=1,
    )
    if n:
        path.write_text(updated)


def write_report(payload: dict) -> None:
    WORK.mkdir(parents=True, exist_ok=True)
    path = WORK / "report.md"
    compiled = payload.get("compiled", {})
    reverted = payload.get("reverted", {})
    lines = [
        "# RS2 shared names",
        "",
        "One name per logical class, method, and field across the RS2 revisions.",
        "RSC and RS3 were not used. No new prefix was minted.",
        "",
        "## Revisions compiled",
        "",
    ]
    for rev in REVS:
        if str(rev) not in compiled and rev not in compiled:
            lines.append(f"- {rev}: not compiled")
            continue
        ok = compiled.get(str(rev), compiled.get(rev))
        lines.append(f"- {rev}: {'compiled' if ok else 'failed'}")
    lines.extend([
        "",
        "## Renames applied",
        "",
        f"- class renames: {payload.get('class_renames', 0)}",
        f"- method renames: {payload.get('method_renames', 0)}",
        f"- field renames: {payload.get('field_renames', 0)}",
        "",
        "## Multi-year clusters that share one name",
        "",
        f"- class clusters spanning 2+ years: {payload.get('class_multi_clusters', 0)}",
        f"- of those, one shared simple name: {payload.get('class_multi_shared', 0)}",
        f"- method clusters spanning 2+ years: {payload.get('method_multi_clusters', 0)}",
        f"- of those, one shared simple name: {payload.get('method_multi_shared', 0)}",
        f"- field clusters spanning 2+ years: {payload.get('field_multi_clusters', 0)}",
        f"- of those, one shared simple name: {payload.get('field_multi_shared', 0)}",
        "",
        "## Members that stayed single-year",
        "",
        f"- classes: {payload.get('class_single_members', 0)}",
        f"- methods: {payload.get('method_single_members', 0)}",
        f"- fields: {payload.get('field_single_members', 0)}",
        "",
        "## Reverted revisions",
        "",
    ])
    if not reverted:
        lines.append("None.")
    else:
        for rev, reason in reverted.items():
            lines.append(f"- {rev}: {reason}")
    lines.append("")
    path.write_text("\n".join(lines))
    log(f"wrote {path}")


def plan(classes: dict[int, list[JClass]]) -> tuple[dict[tuple, list[JClass]], dict[tuple, tuple], dict[int, dict[str, str]]]:
    groups, root_of, _blocked = class_clusters(classes)
    rt4 = rt4_vocab(classes)
    log(f"rt4 vocabulary={len(rt4)}")
    assign_class_names(groups, rt4)
    method_clusters(classes, groups, root_of, rt4)
    field_clusters(groups, root_of, classes, rt4)
    freeze_jdk(classes)
    enforce_overloads(classes)
    enforce_overrides(classes)
    enforce_overloads(classes)
    enforce_overrides(classes)
    # Class proposed names must agree with what the year map will do.
    # If a class rename loses the year-map vote, leave the object proposed
    # so counts can be recomputed from the map itself.
    maps = build_maps(classes)
    return groups, root_of, maps


def snapshot_counts(classes: dict[int, list[JClass]], groups, root_of, maps) -> dict:
    base = {
        "class_renames": 0,
        "method_renames": 0,
        "field_renames": 0,
    }
    for items in classes.values():
        for jc in items:
            final = maps.get(jc.rev, {}).get(jc.simple, jc.simple)
            if final != jc.simple:
                base["class_renames"] += 1
            for method in jc.methods:
                if maps.get(jc.rev, {}).get(method.name, method.name) != method.name:
                    base["method_renames"] += 1
            for fld in jc.fields:
                if maps.get(jc.rev, {}).get(fld.name, fld.name) != fld.name:
                    base["field_renames"] += 1
    base.update(shared_stats(classes, groups, root_of, maps))
    return base


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--apply", action="store_true", help="rewrite RS2 sources")
    parser.add_argument("--compile", action="store_true", help="compile every RS2 revision")
    args = parser.parse_args()
    WORK.mkdir(parents=True, exist_ok=True)
    classes = load_all()
    groups, root_of, maps = plan(classes)
    for label in ("Client", "GameShell", "Linkable", "Packet", "Node"):
        shown = 0
        for members in groups.values():
            names = sorted({jc.simple for jc in members})
            if label not in names:
                continue
            years = sorted(jc.rev for jc in members)
            if len(years) < 2:
                continue
            log(f"cluster {label}: years={years} names={names[:12]}")
            shown += 1
            if shown >= 3:
                break
    counts = snapshot_counts(classes, groups, root_of, maps)
    log(
        "dry-run class={class_renames} method={method_renames} field={field_renames} "
        "class_clusters={class_multi_clusters} shared={class_multi_shared} "
        "single_class={class_single_members} single_method={method_single_members} "
        "single_field={field_single_members}".format(**counts)
    )
    (WORK / "plan.json").write_text(json.dumps({"counts": counts, "maps": {str(k): v for k, v in maps.items()}}, indent=2))
    if not args.apply and not args.compile:
        return
    records: dict[str, list] = {}
    if args.apply:
        applied = apply_maps(classes, maps)
        records = applied["records"]
        (WORK / "applied.json").write_text(json.dumps({"counts": counts, "records": records}, indent=2))
        log(f"applied files={sum(len(v) for v in records.values())}")
    elif (WORK / "applied.json").exists():
        records = json.loads((WORK / "applied.json").read_text()).get("records", {})
    if not args.compile:
        return
    compiled: dict[str, bool] = {}
    reverted: dict[str, str] = {}
    # A few large clients at a time. Each revision has its own Gradle home.
    pending = list(REVS)
    running: list[tuple[int, subprocess.Popen, object]] = []

    def start(rev: int) -> None:
        folder = year_dir(rev)
        seed_gradle_home(rev)
        env = os.environ.copy()
        env["JAVA_HOME"] = str(JAVA_HOME)
        env["PATH"] = str(JAVA_HOME / "bin") + os.pathsep + env.get("PATH", "")
        env["GRADLE_USER_HOME"] = f"/tmp/gradle-rs2-{rev}"
        log_path = WORK / f"compile-{rev}.log"
        handle = log_path.open("w")
        proc = subprocess.Popen(
            gradle_cmd(folder),
            cwd=folder / "src",
            env=env,
            stdout=handle,
            stderr=subprocess.STDOUT,
        )
        running.append((rev, proc, handle))
        log(f"compile start {rev}")

    while pending or running:
        while pending and len(running) < 3:
            start(pending.pop(0))
        if not running:
            break
        rev, proc, handle = running.pop(0)
        code = proc.wait()
        handle.close()
        ok = code == 0
        compiled[str(rev)] = ok
        tail = "\n".join((WORK / f"compile-{rev}.log").read_text(errors="ignore").splitlines()[-20:])
        log(f"compile {rev} exit={code}")
        if not ok:
            reason = f"gradlew exit {code}: " + " ".join(tail.split())[-500:]
            restore_rev(rev, records.get(str(rev), []))
            reverted[str(rev)] = reason
            # Restored sources are the previous compiling tree. Recompile once
            # so status.json matches the tree now on disk.
            again, _tail2 = compile_rev(rev)
            compiled[str(rev)] = again
            if not again:
                reverted[str(rev)] = reason + " | restore also failed to compile"
            update_status(rev, again)
        else:
            update_status(rev, True)
    payload = dict(counts)
    payload["compiled"] = compiled
    payload["reverted"] = reverted
    (WORK / "compile.json").write_text(json.dumps(payload, indent=2))
    write_report(payload)


if __name__ == "__main__":
    main()
