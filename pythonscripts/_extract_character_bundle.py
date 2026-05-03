#!/usr/bin/env python3
"""
Extract a single player character into portable fragments for fresh test shards.

What this exports:
1) Character block + directly attached PCS item blocks (hair/beard/backpack/shroud/etc)
2) Recursive PCEQUIP items rooted at character serial + backpack serial(s)
3) Account block for the character account

Usage examples:
  python _extract_character_bundle.py --data-dir data --serial 0x2adf1f
  python _extract_character_bundle.py --data-dir data --name Kokain --account jhonas
"""

from __future__ import annotations

import argparse
import datetime as _dt
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Dict, List, Optional, Sequence, Tuple


@dataclass
class Block:
    kind: str
    raw: str
    fields: Dict[str, str]
    start_line: int


def _norm(s: str) -> str:
    return s.strip().lower()


def _slug(s: str) -> str:
    s = s.strip().lower()
    out = []
    for ch in s:
        if ch.isalnum() or ch in ("-", "_"):
            out.append(ch)
        else:
            out.append("_")
    # Collapse repeated underscores for cleaner folder names.
    joined = "".join(out)
    while "__" in joined:
        joined = joined.replace("__", "_")
    return joined.strip("_") or "unknown"


def _read_blocks(path: Path, allowed_kinds: Sequence[str]) -> List[Block]:
    lines = path.read_text(encoding="utf-8", errors="replace").splitlines(keepends=True)
    blocks: List[Block] = []
    i = 0
    allowed = set(allowed_kinds)

    while i < len(lines):
        kind = lines[i].strip()
        if kind not in allowed:
            i += 1
            continue

        start = i
        i += 1
        # Tolerate comments/blank lines between type and opening brace.
        while i < len(lines) and lines[i].strip() == "":
            i += 1
        if i >= len(lines) or lines[i].strip() != "{":
            continue

        depth = 0
        raw_lines: List[str] = [lines[start]]
        fields: Dict[str, str] = {}

        while i < len(lines):
            line = lines[i]
            raw_lines.append(line)
            stripped = line.strip()
            if stripped == "{":
                depth += 1
            elif stripped == "}":
                depth -= 1
                if depth == 0:
                    i += 1
                    # Capture trailing blank lines to preserve spacing.
                    while i < len(lines) and lines[i].strip() == "":
                        raw_lines.append(lines[i])
                        i += 1
                    break
            else:
                parts = stripped.split(None, 1)
                if len(parts) == 2:
                    key, value = parts
                    fields[key] = value.strip()

            i += 1

        blocks.append(
            Block(kind=kind, raw="".join(raw_lines), fields=fields, start_line=start + 1)
        )

    return blocks


def _find_character(
    chars: Sequence[Block], serial: Optional[str], name: Optional[str], account: Optional[str]
) -> Block:
    matches: List[Block] = []
    serial_n = _norm(serial) if serial else None
    name_n = _norm(name) if name else None
    account_n = _norm(account) if account else None

    for c in chars:
        c_serial = _norm(c.fields.get("Serial", ""))
        c_name = _norm(c.fields.get("Name", ""))
        c_account = _norm(c.fields.get("Account", ""))

        if serial_n and c_serial != serial_n:
            continue
        if name_n and c_name != name_n:
            continue
        if account_n and c_account != account_n:
            continue
        matches.append(c)

    if not matches:
        bits = []
        if serial:
            bits.append(f"serial={serial}")
        if name:
            bits.append(f"name={name}")
        if account:
            bits.append(f"account={account}")
        raise ValueError(f"No character matched ({', '.join(bits)}).")

    if len(matches) > 1:
        preview = []
        for m in matches[:8]:
            preview.append(
                f"line {m.start_line}: {m.fields.get('Name', '?')} / {m.fields.get('Account', '?')} / {m.fields.get('Serial', '?')}"
            )
        raise ValueError(
            "Multiple characters matched. Narrow with --serial or --account.\n" + "\n".join(preview)
        )

    return matches[0]


def _find_account(accounts: Sequence[Block], account_name: str) -> Optional[Block]:
    target = _norm(account_name)
    for a in accounts:
        if _norm(a.fields.get("Name", "")) == target:
            return a
    return None


def _filter_pcs_items_for_character(pcs_items: Sequence[Block], char_serial: str) -> Tuple[List[Block], List[str]]:
    char_serial_n = _norm(char_serial)
    attached: List[Block] = []
    backpack_serials: List[str] = []

    for item in pcs_items:
        if _norm(item.fields.get("Container", "")) != char_serial_n:
            continue
        attached.append(item)

        layer = item.fields.get("Layer", "").strip()
        objtype = _norm(item.fields.get("ObjType", ""))
        if layer == "21" or objtype == "0xe75":
            ser = item.fields.get("Serial", "").strip()
            if ser:
                backpack_serials.append(ser)

    # Keep order, remove duplicates.
    deduped: List[str] = []
    seen = set()
    for s in backpack_serials:
        n = _norm(s)
        if n in seen:
            continue
        seen.add(n)
        deduped.append(s)

    return attached, deduped


def _collect_recursive_pcequip_items(
    pcequip_items: Sequence[Block], seed_containers: Sequence[str]
) -> List[Block]:
    selected: List[Block] = []
    selected_serials = set()
    containers = {_norm(c) for c in seed_containers if c}

    changed = True
    while changed:
        changed = False
        for item in pcequip_items:
            ser = item.fields.get("Serial", "")
            ser_n = _norm(ser)
            cont_n = _norm(item.fields.get("Container", ""))

            if ser_n in selected_serials:
                continue
            if cont_n not in containers:
                continue

            selected.append(item)
            selected_serials.add(ser_n)
            if ser_n:
                containers.add(ser_n)
            changed = True

    return selected


def _write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Extract one player character + linked inventory/account data into a drop-in data folder."
    )
    parser.add_argument("--data-dir", default="data", help="Folder containing pcs.txt/pcequip.txt/accounts.txt")
    parser.add_argument("--serial", help="Character serial, e.g. 0x2adf1f")
    parser.add_argument("--name", help="Character name, e.g. Kokain")
    parser.add_argument("--account", help="Account name (optional unless disambiguating by name)")
    parser.add_argument(
        "--out-dir",
        help="Output folder (default: exports/character_<serial-or-name>_<timestamp>)",
    )
    parser.add_argument(
        "--allow-missing-account",
        action="store_true",
        help="Do not fail if account block is missing in accounts.txt",
    )
    parser.add_argument(
        "--auto-name-output",
        action="store_true",
        help="When --out-dir is not provided, use exports/<account>_<character>_<timestamp>",
    )

    args = parser.parse_args()

    if not args.serial and not args.name:
        print("ERROR: provide --serial or --name (or both).", file=sys.stderr)
        return 2

    data_dir = Path(args.data_dir)
    pcs_path = data_dir / "pcs.txt"
    pcequip_path = data_dir / "pcequip.txt"
    accounts_path = data_dir / "accounts.txt"

    for req in (pcs_path, pcequip_path, accounts_path):
        if not req.exists():
            print(f"ERROR: missing required file: {req}", file=sys.stderr)
            return 2

    pcs_blocks = _read_blocks(pcs_path, allowed_kinds=["Character", "Item"])
    pcs_chars = [b for b in pcs_blocks if b.kind == "Character"]
    pcs_items = [b for b in pcs_blocks if b.kind == "Item"]
    pcequip_items = _read_blocks(pcequip_path, allowed_kinds=["Item"])
    account_blocks = _read_blocks(accounts_path, allowed_kinds=["Account"])

    try:
        char = _find_character(pcs_chars, args.serial, args.name, args.account)
    except ValueError as exc:
        print(f"ERROR: {exc}", file=sys.stderr)
        return 2

    char_serial = char.fields.get("Serial", "").strip()
    char_name = char.fields.get("Name", "UNKNOWN")
    char_account = char.fields.get("Account", "UNKNOWN")

    attached_pcs_items, backpack_serials = _filter_pcs_items_for_character(pcs_items, char_serial)
    seeds = [char_serial] + backpack_serials
    selected_pcequip = _collect_recursive_pcequip_items(pcequip_items, seeds)

    account_block = _find_account(account_blocks, char_account)
    if account_block is None and not args.allow_missing_account:
        print(
            f"ERROR: account '{char_account}' not found in {accounts_path}. Use --allow-missing-account to continue.",
            file=sys.stderr,
        )
        return 2

    stamp = _dt.datetime.now().strftime("%Y%m%d_%H%M%S")
    base_name = _norm(char_serial or char_name).replace("0x", "") or "character"
    if args.out_dir:
        out_dir = Path(args.out_dir)
    elif args.auto_name_output:
        out_dir = Path("exports") / f"{_slug(char_account)}_{_slug(char_name)}_{stamp}"
    else:
        out_dir = Path("exports") / f"character_{base_name}_{stamp}"
    out_dir.mkdir(parents=True, exist_ok=True)

    data_out_dir = out_dir / "data"
    pcs_out = data_out_dir / "pcs.txt"
    pcequip_out = data_out_dir / "pcequip.txt"
    accounts_out = data_out_dir / "accounts.txt"
    manifest_out = out_dir / "manifest.txt"
    readme_out = out_dir / "README_IMPORT.txt"

    pcs_text = "".join([char.raw] + [b.raw for b in attached_pcs_items])
    pcequip_text = "".join([b.raw for b in selected_pcequip])
    accounts_text = account_block.raw if account_block is not None else ""

    _write(pcs_out, pcs_text)
    _write(pcequip_out, pcequip_text)
    _write(accounts_out, accounts_text)

    readme_lines = [
        "This export mirrors the shard data layout.",
        "",
        "Included files:",
        "- data/pcs.txt",
        "- data/pcequip.txt",
        "- data/accounts.txt",
        "",
        "Recommended import workflow:",
        "1) Stop the target POL server.",
        "2) Use a fresh test shard data folder, or back up existing files first.",
        "3) Copy this export folder's data/* files into the target data folder.",
        "",
        "Note: these files contain only the extracted character/account subset.",
    ]
    _write(readme_out, "\n".join(readme_lines) + "\n")

    manifest_lines = [
        f"CharacterName: {char_name}",
        f"CharacterSerial: {char_serial}",
        f"Account: {char_account}",
        f"PCS attached items: {len(attached_pcs_items)}",
        f"Detected backpacks: {', '.join(backpack_serials) if backpack_serials else '(none)'}",
        f"PCEQUIP extracted items: {len(selected_pcequip)}",
        f"Account block included: {'yes' if account_block else 'no'}",
        f"Source data dir: {data_dir.resolve()}",
        "",
        "Output files:",
        f"- {pcs_out}",
        f"- {pcequip_out}",
        f"- {accounts_out}",
        f"- {readme_out}",
        "",
        "Import note: export is structured as data/*.txt for direct drop-in on a fresh test shard.",
    ]
    _write(manifest_out, "\n".join(manifest_lines) + "\n")

    print("Extraction complete")
    print(f"Character: {char_name} ({char_serial})")
    print(f"Account: {char_account}")
    print(f"PCS attached items: {len(attached_pcs_items)}")
    print(f"Backpacks found: {len(backpack_serials)}")
    print(f"PCEQUIP items: {len(selected_pcequip)}")
    print(f"Output dir: {out_dir}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
