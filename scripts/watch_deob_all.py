#!/usr/bin/env python3
"""Watch scripts/deob_all_clients.py. Prints DONE/FAILED/ACTION_REQUIRED only."""
import json
import os
import sys
import time

pid = int(sys.argv[1])
summary = sys.argv[2]
start = time.time()
last_n = -1
stall = 0


def alive() -> bool:
    try:
        os.kill(pid, 0)
        return True
    except OSError:
        return False


def load():
    try:
        return json.loads(open(summary).read())
    except Exception:
        return None


while alive():
    time.sleep(45)
    data = load()
    n = (data or {}).get("total") or 0
    ok = (data or {}).get("ok") or 0
    if n == last_n:
        stall += 45
    else:
        stall = 0
        last_n = n
    if time.time() - start > 900 and n == 0:
        print("FAILED: no progress in 15 minutes", flush=True)
        sys.exit(1)
    if stall > 2400:
        print(f"ACTION_REQUIRED: stalled at {n} clients for 40 minutes", flush=True)
        stall = 0

# process exited
for _ in range(20):
    try:
        os.waitpid(pid, os.WNOHANG)
    except ChildProcessError:
        break
    time.sleep(0.1)
data = load() or {}
ok = data.get("ok", 0)
total = data.get("total", 0)
compiled = data.get("compiled", 0)
failed = data.get("failed", 0)
if total == 0:
    print("FAILED: deob-all exited with empty summary", flush=True)
    sys.exit(1)
print(
    f"DONE: deob-all finished ok={ok}/{total} compiled={compiled} failed_hard={failed}",
    flush=True,
)
sys.exit(0)
