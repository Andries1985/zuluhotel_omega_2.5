#!/usr/bin/env python3
"""
analyze_memory_usage.py
Analyzes memoryusagescripts.log for script memory usage statistics.
"""

import re
import csv
import argparse
from pathlib import Path
from datetime import datetime
from collections import defaultdict

# -- ANSI colors --------------------------------------------------------------
class C:
    CYAN   = "\033[96m"
    YELLOW = "\033[93m"
    GREEN  = "\033[92m"
    GRAY   = "\033[90m"
    RESET  = "\033[0m"

def c(color: str, text: str) -> str:
    return f"{color}{text}{C.RESET}"

# -- Patterns -------------------------------------------------------------------
RE_TIMESTAMP = re.compile(r"^\[(\d{2}/\d{2}\s+\d{2}:\d{2}:\d{2})\]")
RE_SECTION   = re.compile(r"^\s*(\w+):\s*$")
RE_ENTRY     = re.compile(r"^\s*(\S+\.ecl)\s+(\d+)\s*$")

# -- Parsing ----------------------------------------------------------------------
def parse_log(path: Path):
    entries   = []   # {timestamp, section, script, memory}
    snapshots = []   # {timestamp, count, total}

    current_ts      = None
    current_section = None
    snap_entries    = []

    def flush_snapshot():
        if current_ts and snap_entries:
            total = sum(e["memory"] for e in snap_entries)
            snapshots.append({"timestamp": current_ts, "count": len(snap_entries), "total": total})

    with open(path, "r", encoding="utf-8", errors="replace") as fh:
        for line in fh:
            line = line.rstrip()

            m = RE_TIMESTAMP.match(line)
            if m:
                flush_snapshot()
                current_ts      = m.group(1)
                current_section = None
                snap_entries    = []
                continue

            m = RE_SECTION.match(line)
            if m:
                current_section = m.group(1)
                continue

            m = RE_ENTRY.match(line)
            if m:
                obj = {
                    "timestamp": current_ts,
                    "section":   current_section,
                    "script":    m.group(1),
                    "memory":    int(m.group(2)),
                }
                entries.append(obj)
                snap_entries.append(obj)

    flush_snapshot()
    return entries, snapshots

# -- Aggregation ------------------------------------------------------------------
def aggregate_scripts(entries):
    grouped = defaultdict(list)
    for e in entries:
        grouped[e["script"]].append(e["memory"])

    stats = []
    for script, mems in grouped.items():
        total  = sum(mems)
        avg    = total / len(mems)
        mn, mx = min(mems), max(mems)
        stats.append({
            "script":   script,
            "instances": len(mems),
            "total":    total,
            "avg":      round(avg),
            "min":      mn,
            "max":      mx,
            "growth":   mx - mn,
        })
    return stats

def aggregate_sections(entries):
    grouped = defaultdict(list)
    for e in entries:
        if e["section"]:
            grouped[e["section"]].append(e["memory"])

    stats = []
    for section, mems in grouped.items():
        stats.append({
            "section":   section,
            "instances": len(mems),
            "total":     sum(mems),
        })
    return sorted(stats, key=lambda x: x["total"], reverse=True)

# -- Display helpers ----------------------------------------------------------------
def rule(label: str):
    print(c(C.YELLOW, f"-- {label} {'-' * max(0, 57 - len(label))}"))

def fmt_b(n: int) -> str:
    return f"{n:>12,}"

def fmt_kb(n: int) -> str:
    return f"{n/1024:>10.2f}"

def truncate(s: str, width: int) -> str:
    return s if len(s) <= width else "..." + s[-(width - 1):]

def print_table(rows, columns):
    """
    columns: list of (header, key, width, right_align)
    """
    headers = [h.ljust(w) if not r else h.rjust(w) for h, _, w, r in columns]
    sep     = "  ".join("-" * w for _, _, w, _ in columns)
    print("  " + "  ".join(headers))
    print("  " + sep)
    for row in rows:
        cells = []
        for _, key, w, right in columns:
            val = str(row[key])
            cells.append(val.rjust(w) if right else val.ljust(w))
        print("  " + "  ".join(cells))
    print()

# -- Main ---------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="Analyze a memoryusagescripts.log file."
    )
    parser.add_argument(
        "logfile",
        nargs="?",
        default="memoryusagescripts.log",
        help="Path to the log file (default: memoryusagescripts.log)",
    )
    parser.add_argument(
        "-n", "--top",
        type=int,
        default=10,
        metavar="N",
        help="How many top entries to show (default: 10)",
    )
    parser.add_argument(
        "--csv",
        action="store_true",
        help="Export per-script stats to a CSV file next to the log",
    )
    args = parser.parse_args()

    log_path = Path(args.logfile)
    if not log_path.exists():
        print(c(C.YELLOW, f"Error: file not found: {log_path}"))
        raise SystemExit(1)

    print()
    print(c(C.CYAN, "=== Memory Usage Log Analyzer ==="))
    print(c(C.GRAY, f"File   : {log_path.resolve()}"))
    print(c(C.GRAY, f"Parsed : {datetime.now():%Y-%m-%d %H:%M:%S}"))
    print()

    entries, snapshots = parse_log(log_path)

    if not entries:
        print(c(C.YELLOW, "No script entries found in the log file."))
        raise SystemExit(0)

    script_stats  = aggregate_scripts(entries)
    section_stats = aggregate_sections(entries)

    total_memory    = sum(e["memory"] for e in entries)
    total_instances = len(entries)
    unique_scripts  = len(set(e["script"] for e in entries))

    # -- Overall summary -----------------------------------------------------------
    rule("OVERALL SUMMARY")
    print(f"  Snapshots (timestamps) : {len(snapshots)}")
    print(f"  Total script instances : {total_instances:,}")
    print(f"  Unique scripts         : {unique_scripts:,}")
    print(f"  Total memory (all)     : {total_memory:,} bytes  ({total_memory/1024:.2f} KB)")
    print()

    N = args.top

    # -- Top N by total memory -------------------------------------------------------
    rule(f"TOP {N} SCRIPTS BY TOTAL MEMORY")
    top_mem = sorted(script_stats, key=lambda x: x["total"], reverse=True)[:N]
    rows = [{**r, "script": truncate(r["script"], 58),
             "total_f": f"{r['total']:,}", "avg_f": f"{r['avg']:,}",
             "min_f": f"{r['min']:,}", "max_f": f"{r['max']:,}"} for r in top_mem]
    print_table(rows, [
        ("Script",     "script",  58, False),
        ("Instances",  "instances", 9, True),
        ("Total (B)",  "total_f", 12, True),
        ("Avg (B)",    "avg_f",    9, True),
        ("Min (B)",    "min_f",    9, True),
        ("Max (B)",    "max_f",    9, True),
    ])

    # -- Top N by instance count -------------------------------------------------------
    rule(f"TOP {N} SCRIPTS BY INSTANCE COUNT")
    top_inst = sorted(script_stats, key=lambda x: x["instances"], reverse=True)[:N]
    rows = [{**r, "script": truncate(r["script"], 58),
             "total_f": f"{r['total']:,}", "avg_f": f"{r['avg']:,}"} for r in top_inst]
    print_table(rows, [
        ("Script",    "script",   58, False),
        ("Instances", "instances", 9, True),
        ("Total (B)", "total_f",  12, True),
        ("Avg (B)",   "avg_f",    9, True),
    ])

    # -- Memory growth suspects -----------------------------------------------------
    rule("MEMORY GROWTH SUSPECTS (Max - Min > 0)")
    suspects = sorted(
        [s for s in script_stats if s["growth"] > 0],
        key=lambda x: x["growth"], reverse=True
    )[:N]
    if suspects:
        rows = [{**r, "script": truncate(r["script"], 58),
                 "min_f": f"{r['min']:,}", "max_f": f"{r['max']:,}",
                 "growth_f": f"{r['growth']:,}"} for r in suspects]
        print_table(rows, [
            ("Script",     "script",   58, False),
            ("Instances",  "instances", 9, True),
            ("Min (B)",    "min_f",     9, True),
            ("Max (B)",    "max_f",     9, True),
            ("Growth (B)", "growth_f", 11, True),
        ])
    else:
        print(c(C.GREEN, "  No memory growth detected across instances.\n"))

    # -- Snapshot timeline -----------------------------------------------------------
    if len(snapshots) > 1:
        rule("SNAPSHOT TIMELINE")
        rows = [{**s, "total_f": f"{s['total']:,}",
                 "kb_f": f"{s['total']/1024:.2f}"} for s in snapshots]
        print_table(rows, [
            ("Timestamp",  "timestamp", 18, False),
            ("Instances",  "count",      9, True),
            ("Total (B)",  "total_f",   12, True),
            ("Total (KB)", "kb_f",      10, True),
        ])

    # -- Section breakdown -------------------------------------------------------------
    if section_stats:
        rule("MEMORY BY SECTION")
        rows = [{**s, "total_f": f"{s['total']:,}",
                 "kb_f": f"{s['total']/1024:.2f}"} for s in section_stats]
        print_table(rows, [
            ("Section",    "section",   24, False),
            ("Instances",  "instances",  9, True),
            ("Total (B)",  "total_f",   12, True),
            ("Total (KB)", "kb_f",      10, True),
        ])

    # -- CSV export ---------------------------------------------------------------------
    if args.csv:
        csv_path = log_path.with_suffix(".csv")
        with open(csv_path, "w", newline="", encoding="utf-8") as fh:
            writer = csv.DictWriter(fh, fieldnames=[
                "script","instances","total","avg","min","max","growth"
            ])
            writer.writeheader()
            for row in sorted(script_stats, key=lambda x: x["total"], reverse=True):
                writer.writerow(row)
        print(c(C.GREEN, f"  CSV exported to: {csv_path}\n"))

    print(c(C.CYAN, "=== Analysis complete ===\n"))

if __name__ == "__main__":
    main()
