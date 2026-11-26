#!/usr/bin/env python3
from pathlib import Path
import re
import sys

root = Path(sys.argv[1])
pat = re.compile(r"[ \t]*@\.?ObfuscatedName\(\"[^\"]*\"\)[ \t]*\n")
n = 0
for p in root.rglob("*.java"):
    t = p.read_text(errors="ignore")
    nt, c = pat.subn("", t)
    nt = nt.replace("@.ObfuscatedName", "@ObfuscatedName")
    if nt != t:
        p.write_text(nt)
        n += c
print("stripped", n)

muds = list(root.rglob("mudclient.java"))
if not muds:
    print("no mudclient.java")
    sys.exit(0)
mud = muds[0]
t = mud.read_text(errors="ignore")
if re.search(r"public\s+static\s+void\s+main\s*\(", t):
    print("has main")
    sys.exit(0)
m = re.search(r"public static void (\w+)\(String\[]", t)
if m and m.group(1) != "main":
    t = t.replace(f"public static void {m.group(1)}(String[]", "public static void main(String[]", 1)
    print("renamed", m.group(1), "to main")
    mud.write_text(t)
    sys.exit(0)
idx = t.rfind("}")
t = t[:idx] + "\n    public static void main(String[] args) {}\n" + t[idx:]
mud.write_text(t)
print("appended stub main")
