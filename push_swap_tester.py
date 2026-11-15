#!/usr/bin/env python3
"""push_swap tester (self-contained: simulates the operations, no checker needed).

Usage:
    python3 push_swap_tester.py [./push_swap] [--full] [--runs N] [--seed S]
                                [--leaks] [--stress] [--timeout SEC]

  --full     also test all permutations of 7 and random sizes up to 1000
  --runs N   random runs per size/strategy in the benchmark section (default 20)
  --leaks    leak checks (macOS: `leaks`, Linux: `valgrind`)
  --stress   2000/5000 numbers (timing and correctness)
"""
import argparse
import itertools
import os
import random
import re
import shlex
import shutil
import subprocess
import sys
import time
from collections import Counter, deque

INT_MIN, INT_MAX = -2**31, 2**31 - 1
OPS = ("sa", "sb", "ss", "pa", "pb", "ra", "rb", "rr", "rra", "rrb", "rrr")
FLAGS = ("", "--simple", "--medium", "--complex", "--adaptive")
CFG = {}
TOTALS = {"pass": 0, "fail": 0, "warn": 0}
FAILDIR = "tester_failures"
_case = [0]
USE_COLOR = sys.stdout.isatty()


def col(text, code):
    return f"\033[{code}m{text}\033[0m" if USE_COLOR else text


def label(flag):
    return flag or "(default)"


def show(args):
    """Printable repro command; long argument lists are saved to a file."""
    s = " ".join(shlex.quote(str(a)) for a in args)
    if len(s) <= 160:
        return CFG["bin"] + " " + s
    os.makedirs(FAILDIR, exist_ok=True)
    _case[0] += 1
    path = f"{FAILDIR}/case_{_case[0]}.txt"
    with open(path, "w") as f:
        f.write(" ".join(str(a) for a in args))
    return f"{CFG['bin']} $(cat {path})"


def run(args, timeout=None):
    cmd = [CFG["bin"]] + [str(a) for a in args]
    try:
        p = subprocess.run(cmd, capture_output=True,
                           timeout=timeout or CFG["timeout"])
    except subprocess.TimeoutExpired:
        return None, b"", b""
    return p.returncode, p.stdout, p.stderr


# --------------------------------------------------------------------------
# simulator
# --------------------------------------------------------------------------
def simulate(values, ops):
    a, b, noops = deque(values), deque(), 0
    for op in ops:
        if op in ("sa", "ss"):
            if len(a) > 1:
                a[0], a[1] = a[1], a[0]
            elif op == "sa":
                noops += 1
        if op in ("sb", "ss"):
            if len(b) > 1:
                b[0], b[1] = b[1], b[0]
            elif op == "sb":
                noops += 1
        if op == "ss" and len(a) < 2 and len(b) < 2:
            noops += 1
        if op == "pa":
            if b:
                a.appendleft(b.popleft())
            else:
                noops += 1
        if op == "pb":
            if a:
                b.appendleft(a.popleft())
            else:
                noops += 1
        if op in ("ra", "rr") and len(a) > 1:
            a.rotate(-1)
        if op in ("rb", "rr") and len(b) > 1:
            b.rotate(-1)
        if op in ("rra", "rrr") and len(a) > 1:
            a.rotate(1)
        if op in ("rrb", "rrr") and len(b) > 1:
            b.rotate(1)
    return a, b, noops


def parse_ops(out):
    text = out.decode("latin-1")
    if text == "":
        return [], None
    if not text.endswith("\n"):
        return None, "output does not end with a newline"
    lines = text[:-1].split("\n")
    for i, line in enumerate(lines):
        if line not in OPS:
            return None, f"invalid output line {i + 1}: {line!r}"
    return lines, None


def verify(args, bench=False):
    """Run push_swap and check it sorts. Returns (ok, reason, ops)."""
    rc, out, err = run(args)
    if rc is None:
        return False, "timeout", None
    if rc < 0:
        return False, f"crashed (signal {-rc})", None
    if rc != 0:
        return False, f"exit code {rc}, stderr={err[:60]!r}", None
    if err and not bench:
        return False, f"unexpected stderr {err[:60]!r}", None
    ops, e = parse_ops(out)
    if e:
        return False, e, None
    values = [int(x) for x in args if not str(x).startswith("--")]
    a, b, _ = simulate(values, ops)
    if b:
        return False, f"stack b not empty ({len(b)} left)", ops
    if list(a) != sorted(values):
        return False, "stack a is not sorted", ops
    return True, "", ops


def disorder(v):
    n = len(v)
    pairs = n * (n - 1) // 2
    if pairs == 0:
        return 0.0
    bad = sum(1 for i in range(n) for j in range(i + 1, n) if v[i] > v[j])
    return bad / pairs


# --------------------------------------------------------------------------
# reporting
# --------------------------------------------------------------------------
class Sec:
    def __init__(self, name):
        self.name, self.n = name, 0
        self.fails, self.warns, self.info = [], [], []
        self.t0 = time.time()
        print(f"... {name}", end="\r", flush=True)

    def res(self, ok, msg):
        self.n += 1
        if not ok:
            self.fails.append(msg() if callable(msg) else msg)

    def warn(self, msg):
        self.warns.append(msg)

    def done(self):
        bad = len(self.fails)
        tag = col("FAIL", "31") if bad else col("PASS", "32")
        print(f"[{tag}] {self.name}: {self.n - bad}/{self.n} "
              f"({time.time() - self.t0:.1f}s)" + " " * 10)
        for f in self.fails[:5]:
            print("    - " + f)
        if bad > 5:
            print(f"    ... and {bad - 5} more")
        for line in self.info:
            print("    " + line)
        for w in self.warns[:4]:
            print(col("    ~ " + w, "33"))
        if len(self.warns) > 4:
            print(col(f"    ~ ... and {len(self.warns) - 4} more warnings", "33"))
        TOTALS["pass"] += self.n - bad
        TOTALS["fail"] += bad
        TOTALS["warn"] += len(self.warns)


def with_flag(flag, values):
    return ([flag] if flag else []) + list(values)


# --------------------------------------------------------------------------
# generators
# --------------------------------------------------------------------------
def gen(kind, n):
    if kind == 0:
        v = list(range(n))
        random.shuffle(v)
        return v
    if kind == 1:
        return random.sample(range(-10**9, 10**9), n)
    if kind == 2:
        return random.sample(range(-10000, 1), n)
    k = max(n - 2, 0)
    v = random.sample(range(INT_MIN + 1, INT_MAX), k) + [INT_MIN, INT_MAX][:n - k]
    random.shuffle(v)
    return v


def patterns(n):
    base = sorted(random.sample(range(-10**6, 10**6), n))
    p = {"reversed": base[::-1]}
    sw = base[:]
    for _ in range(3):
        i = random.randrange(n - 1)
        sw[i], sw[i + 1] = sw[i + 1], sw[i]
    p["3 adjacent swaps"] = sw
    p["first moved to end"] = base[1:] + base[:1]
    p["last moved to front"] = base[-1:] + base[:-1]
    p["rotated by n/2"] = base[n // 2:] + base[:n // 2]
    p["organ pipe"] = base[0::2] + base[1::2][::-1]
    lo, hi = base[:n // 2], base[n // 2:]
    z = []
    for x, y in zip(lo, hi):
        z += [y, x]
    p["zigzag"] = z + hi[len(lo):]
    p["negatives reversed"] = [-(i + 1) for i in range(n)]
    return p


# --------------------------------------------------------------------------
# sections
# --------------------------------------------------------------------------
ERR_CASES = [
    ["one"], ["1a"], ["a1"], [""], ["", "1"], ["1", ""], ["+"], ["-"], ["--"],
    ["---"], ["+-1"], ["-+1"], ["--1"], ["1-"], ["1+"], ["1.5"], ["1,5"],
    ["0x10"], ["1e3"], [" "], ["1", " "], ["\u22121"],
    ["2147483648"], ["-2147483649"], ["99999999999999999999"],
    ["-99999999999999999999"], ["2147483647", "2147483648"],
    ["-2147483648", "-2147483649"],
    ["00000000000000000000000000000000000002147483648"],
    ["1", "1"], ["3", "2", "3"], ["0", "-0"], ["1", "+1"], ["5", "3", "5", "1"],
    ["-1", "-1"], ["2147483647", "1", "2147483647"],
    ["--unknown", "1", "2"], ["--SIMPLE", "3", "2", "1"], ["-simple", "3", "2"],
    ["--simple", "--medium", "3", "2", "1"], ["--simple", "--simple", "1", "2"],
    ["--bench", "--bench", "1", "2"], ["--adaptive", "--complex", "2", "1"],
    ["--simple", "a"], ["3", "2", "one", "0"], ["0", "one", "2", "3"],
    ["--adaptive", "0", "one", "2", "3"], ["--simple", "3", "2", "3"],
    ["--complex", "--bench", "1", "1"],
]


def sec_errors():
    s = Sec("error handling (stdout empty, stderr == 'Error\\n')")
    for args in ERR_CASES:
        rc, out, err = run(args)
        ok = rc is not None and rc >= 0 and out == b"" and err == b"Error\n"
        s.res(ok, lambda a=args, r=rc, o=out, e=err:
              f"{show(a)} -> rc={r} stdout={o[:30]!r} stderr={e[:30]!r}")
        if ok and rc == 0:
            s.warn(f"exit status 0 on error: {show(args)}")
    s.done()


def sec_silent():
    s = Sec("no output (no args / single / already sorted / flags only)")
    big = list(range(-250, 250))
    cases = [[], ["5"], ["-5"], ["+5"], ["0"], ["-0"], ["00"], ["2147483647"],
             ["-2147483648"], ["1", "2"], ["1", "2", "3"],
             ["-2147483648", "2147483647"], ["-3", "-2", "-1", "0", "1"],
             ["--simple"], ["--medium"], ["--complex"], ["--adaptive"],
             ["--simple", "1", "2", "3"], ["--medium", "1"],
             ["--complex", "1", "2"], ["--adaptive", "1"]]
    cases += [with_flag(f, big) for f in FLAGS]
    for args in cases:
        rc, out, err = run(args)
        s.res(rc == 0 and out == b"" and err == b"",
              lambda a=args, r=rc, o=out, e=err:
              f"{show(a)} -> rc={r} stdout={o[:30]!r} stderr={e[:30]!r}")
    s.done()


def sec_valid_forms():
    s = Sec("valid number forms and extreme values (all strategies)")
    cases = [["+2", "+1"], ["007", "3", "-0000"], ["-0", "1"], ["+0", "-1"],
             ["2147483647", "-2147483648", "0"],
             ["2147483647", "-2147483648"],
             ["0000000000000000000000000000001", "0"],
             ["-2147483648", "-2147483647", "2147483647", "2147483646", "0"]]
    for base in cases:
        for f in FLAGS:
            args = with_flag(f, base)
            ok, why, _ = verify(args)
            s.res(ok, lambda a=args, w=why: f"{show(a)} -> {w}")
    s.done()


def sec_perms():
    pool = [-2147483648, -77, 0, 5, 1000, 2147483647, 31]
    for n in range(1, (7 if CFG["full"] else 6) + 1):
        s = Sec(f"all {n}! permutations x 5 strategies")
        worst = {}
        for perm in itertools.permutations(sorted(pool[:n])):
            for f in FLAGS:
                args = with_flag(f, perm)
                ok, why, ops = verify(args)
                s.res(ok, lambda a=args, w=why: f"{show(a)} -> {w}")
                if ok:
                    worst[f] = max(worst.get(f, 0), len(ops))
        s.info.append("max ops: " + ", ".join(
            f"{label(f)}={worst.get(f, 0)}" for f in FLAGS))
        if n == 3 and worst.get("", 0) > 3:
            s.warn(f"n=3 default needs up to {worst['']} ops (2-3 is typical)")
        if n == 5 and worst.get("", 0) > 12:
            s.warn(f"n=5 default needs up to {worst['']} ops (<=12 is typical)")
        s.done()


def sec_random():
    sizes = [2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 15, 20, 30, 50, 100, 200, 500]
    if CFG["full"]:
        sizes.append(1000)
    s = Sec("random inputs (4 value ranges) x 5 strategies")
    for n in sizes:
        for kind in range(4):
            vals = gen(kind, n)
            for f in FLAGS:
                args = with_flag(f, vals)
                ok, why, _ = verify(args)
                s.res(ok, lambda a=args, w=why, n=n: f"n={n}: {show(a)} -> {w}")
    s.done()


def sec_patterns():
    s = Sec("special patterns (n=10, 100, 500) x 5 strategies")
    for n in (10, 100, 500):
        s.res(True, "")
        for name, vals in patterns(n).items():
            for f in FLAGS:
                args = with_flag(f, vals)
                ok, why, _ = verify(args)
                s.res(ok, lambda a=args, w=why, nm=name, n=n:
                      f"n={n} {nm}: {show(a)} -> {w}")
    s.done()


CX = {"n2": ("O(n^2)", "O(n²)"),
      "nsqrt": ("O(n√n)", "O(nsqrtn)", "O(nsqrt(n))", "O(n*sqrt(n))"),
      "nlogn": ("O(nlogn)", "O(nlog(n))", "O(n*logn)")}


def cx_class(text):
    t = text.replace(" ", "")
    for k, forms in CX.items():
        if t in forms:
            return k
    return None


def parse_bench(err):
    text = err.decode("utf-8", "replace")
    if not text.endswith("\n"):
        return None, "stderr missing or has no trailing newline"
    info = {"ops": {}}
    for line in text[:-1].split("\n"):
        if not line.startswith("[bench]"):
            return None, f"stderr line without [bench] prefix: {line!r}"
        m = re.match(r"\[bench\] disorder:\s*(\d+\.\d\d)%\s*$", line)
        if m:
            info["disorder"] = float(m.group(1))
            continue
        m = re.match(r"\[bench\] strategy:\s*(\S+)\s*/\s*(.+?)\s*$", line)
        if m:
            info["name"], info["cx"] = m.group(1), m.group(2)
            continue
        m = re.match(r"\[bench\] total_ops:\s*(\d+)\s*$", line)
        if m:
            info["total"] = int(m.group(1))
            continue
        pairs = re.findall(r"(\w+):\s*(\d+)", line[len("[bench]"):])
        if not pairs:
            return None, f"unparseable bench line {line!r}"
        for k, v in pairs:
            info["ops"][k] = int(v)
    for k in ("disorder", "name", "cx", "total"):
        if k not in info:
            return None, f"missing bench field '{k}'"
    return info, None


def check_bench(s, values, flag, flags_args, tag):
    args = list(flags_args) + [str(v) for v in values]
    rc, out, err = run(args)
    base_rc, base_out, _ = run([a for a in args if a != "--bench"])
    show_ = lambda: show(args)
    if rc != 0 or rc is None:
        s.res(False, lambda: f"{tag}: {show_()} -> rc={rc}")
        return
    s.res(out == base_out, lambda: f"{tag}: --bench changes stdout: {show_()}")
    ops, e = parse_ops(out)
    s.res(e is None, lambda: f"{tag}: {e}")
    if e:
        return
    info, e = parse_bench(err)
    s.res(info is not None, lambda: f"{tag}: {show_()} -> {e}")
    if not info:
        return
    s.res(info["total"] == len(ops),
          lambda: f"{tag}: total_ops={info['total']} but stdout has {len(ops)}")
    counts = Counter(ops)
    for op in OPS:
        s.res(info["ops"].get(op) == counts.get(op, 0),
              lambda op=op: f"{tag}: bench {op}={info['ops'].get(op)} "
                            f"but stdout has {counts.get(op, 0)}")
    d = disorder(list(values))
    s.res(abs(info["disorder"] - 100 * d) <= 0.006,
          lambda: f"{tag}: disorder {info['disorder']}% but real {100 * d:.4f}%")
    want = (flag or "--adaptive")[2:]
    s.res(info["name"].lower() == want,
          lambda: f"{tag}: strategy name {info['name']!r}, expected {want}")
    cls = cx_class(info["cx"])
    s.res(cls is not None, lambda: f"{tag}: unknown complexity {info['cx']!r}")
    fixed = {"simple": "n2", "medium": "nsqrt", "complex": "nlogn"}
    if want in fixed:
        s.res(cls == fixed[want],
              lambda: f"{tag}: {want} reports {info['cx']!r}")
    elif len(values) > 5 and abs(d - 0.2) > 0.001 and abs(d - 0.5) > 0.001:
        exp = "n2" if d < 0.2 else "nsqrt" if d < 0.5 else "nlogn"
        s.res(cls == exp, lambda: f"{tag}: adaptive at disorder {d:.3f} "
                                  f"reports {info['cx']!r}, expected class {exp}")


def sec_bench_format():
    s = Sec("--bench output (stderr only, values match stdout)")
    inputs = {"n=3": [3, 2, 1], "n=5": gen(0, 5), "n=6": gen(0, 6),
              "n=100": gen(0, 100), "n=500": gen(0, 500),
              "reversed 100": list(range(100, 0, -1)),
              "sorted 100": list(range(100)),
              "nearly sorted 100": patterns(100)["3 adjacent swaps"]}
    for name, vals in inputs.items():
        for f in FLAGS:
            check_bench(s, vals, f, with_flag(f, ["--bench"]), f"{name} {label(f)}")
    for order in (["--bench", "--simple"], ["--simple", "--bench"]):
        check_bench(s, gen(0, 20), "--simple", order, "flag order " + " ".join(order))
    s.done()


LIM = {100: (2000, 1500, 700), 500: (12000, 8000, 5500)}


def sec_benchmarks():
    s = Sec("operation-count benchmarks")
    for n, (limit, good, great) in LIM.items():
        for f in ("", "--simple", "--medium", "--complex"):
            counts = []
            t0 = time.time()
            for _ in range(CFG["runs"]):
                vals = gen(0, n)
                args = with_flag(f, vals)
                ok, why, ops = verify(args)
                s.res(ok, lambda a=args, w=why: f"{show(a)} -> {w}")
                if ok:
                    counts.append(len(ops))
            if not counts:
                continue
            mx, mn, avg = max(counts), min(counts), sum(counts) / len(counts)
            s.res(mx < limit, lambda: f"n={n} {label(f)}: max {mx} >= {limit}")
            tier = ("excellent" if mx < great else "good" if mx < good
                    else "pass" if mx < limit else "FAIL")
            s.info.append(f"n={n:<3} {label(f):<11} min={mn:<5} avg={avg:<7.0f} "
                          f"max={mx:<5} worst-case tier: {tier:<9} "
                          f"({(time.time() - t0) / len(counts):.2f}s/run)")
    s.done()


CANCEL = {("pa", "pb"), ("pb", "pa"), ("ra", "rra"), ("rra", "ra"),
          ("rb", "rrb"), ("rrb", "rb"), ("rr", "rrr"), ("rrr", "rr"),
          ("sa", "sa"), ("sb", "sb"), ("ss", "ss")}
MERGE = {("sa", "sb"), ("sb", "sa"), ("ra", "rb"), ("rb", "ra"),
         ("rra", "rrb"), ("rrb", "rra")}


def sec_waste():
    s = Sec("wasted operations (warnings only)")
    for n in (100, 500):
        for f in FLAGS[:4]:
            vals = gen(0, n)
            ok, why, ops = verify(with_flag(f, vals))
            s.res(ok, lambda w=why: w)
            if not ok:
                continue
            pairs = list(zip(ops, ops[1:]))
            can = sum(1 for p in pairs if p in CANCEL)
            mer = sum(1 for p in pairs if p in MERGE)
            noop = simulate(vals, ops)[2]
            if can or mer or noop:
                s.warn(f"n={n} {label(f)}: {can} cancelling pairs, "
                       f"{mer} mergeable pairs, {noop} no-op moves")
    s.done()


def sec_determinism():
    s = Sec("determinism (same input -> same output)")
    for n in (5, 50, 300):
        vals = gen(0, n)
        for f in FLAGS:
            args = with_flag(f, vals)
            r1, r2 = run(args), run(args)
            s.res(r1 == r2, lambda a=args: f"{show(a)} differs between runs")
    s.done()


def sec_flag_position():
    s = Sec("flag positions")
    for args in (["--bench", "3", "2", "1"], ["--simple", "--bench", "3", "2", "1"],
                 ["--bench", "--complex", "3", "2", "1"]):
        ok, why, _ = verify(args, bench=True)
        s.res(ok, lambda a=args, w=why: f"{show(a)} -> {w}")
    for args in (["3", "2", "1", "--bench"], ["3", "--simple", "2", "1"],
                 ["3", "2", "1", "--medium"]):
        ok, why, _ = verify(args, bench=True)
        if not ok:
            s.warn(f"flag after numbers not accepted: {show(args)} ({why}) "
                   "- fine if you require flags first")
    s.done()


def sec_stress():
    s = Sec("stress (large inputs)")
    old = CFG["timeout"]
    CFG["timeout"] = max(old, 60)
    for n in (2000, 5000):
        for f in ("", "--complex", "--medium"):
            vals = gen(1, n)
            args = with_flag(f, vals)
            t0 = time.time()
            ok, why, ops = verify(args)
            dt = time.time() - t0
            s.res(ok, lambda a=args, w=why: f"{show(a)} -> {w}")
            if ok:
                s.info.append(f"n={n} {label(f):<10} {len(ops)} ops, {dt:.2f}s")
    CFG["timeout"] = old
    s.done()


def leak_tool():
    if sys.platform == "darwin" and shutil.which("leaks"):
        return "leaks"
    if shutil.which("valgrind"):
        return "valgrind"
    return None


def sec_leaks():
    tool = leak_tool()
    s = Sec(f"memory leaks ({tool})")
    if not tool:
        s.warn("neither `leaks` (macOS) nor `valgrind` found; skipped")
        return s.done()
    cases = [[], ["1"], ["2", "1"], ["3", "2", "1"], ["1", "1"], ["one"],
             ["--simple", "--medium", "1"], ["--bench", "5", "4", "3", "2", "1"]]
    for n in (6, 50, 100):
        for f in FLAGS:
            cases.append(with_flag(f, ["--bench"] + gen(0, n)))
    for args in cases:
        cmd_args = [str(a) for a in args]
        if tool == "leaks":
            cmd = ["leaks", "--atExit", "--", CFG["bin"]] + cmd_args
        else:
            cmd = ["valgrind", "--leak-check=full", "--error-exitcode=42", "-q",
                   CFG["bin"]] + cmd_args
        try:
            r = subprocess.run(cmd, capture_output=True, timeout=120)
        except subprocess.TimeoutExpired:
            s.res(False, lambda a=args: f"{show(a)} -> timeout")
            continue
        text = r.stdout.decode("utf-8", "replace")
        if tool == "leaks":
            if "leaks for" not in text:
                s.warn(f"`leaks` could not inspect: {show(args)}")
                continue
            clean = "0 leaks for 0 total leaked bytes" in text
        else:
            clean = r.returncode != 42
        s.res(clean, lambda a=args: f"leak/memory error: {show(a)}")
    s.done()


# --------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("binary", nargs="?", default="./push_swap")
    ap.add_argument("--full", action="store_true")
    ap.add_argument("--runs", type=int, default=20)
    ap.add_argument("--seed", type=int, default=int(time.time()))
    ap.add_argument("--leaks", action="store_true")
    ap.add_argument("--stress", action="store_true")
    ap.add_argument("--timeout", type=float, default=20)
    o = ap.parse_args()
    CFG.update(bin=o.binary, full=o.full, runs=o.runs, timeout=o.timeout)
    if not (os.path.isfile(o.binary) and os.access(o.binary, os.X_OK)):
        sys.exit(f"cannot execute {o.binary} (run `make` first?)")
    random.seed(o.seed)
    print(f"push_swap tester: {o.binary}, seed={o.seed} "
          f"(rerun with --seed {o.seed} to reproduce)\n")
    secs = [sec_errors, sec_silent, sec_valid_forms, sec_perms, sec_random,
            sec_patterns, sec_bench_format, sec_benchmarks, sec_waste,
            sec_determinism, sec_flag_position]
    if o.stress:
        secs.append(sec_stress)
    if o.leaks:
        secs.append(sec_leaks)
    try:
        for fn in secs:
            fn()
    except KeyboardInterrupt:
        print("\ninterrupted")
    print(f"\nchecks passed: {TOTALS['pass']}   failed: {TOTALS['fail']}   "
          f"warnings: {TOTALS['warn']}")
    sys.exit(1 if TOTALS["fail"] else 0)


if __name__ == "__main__":
    main()
