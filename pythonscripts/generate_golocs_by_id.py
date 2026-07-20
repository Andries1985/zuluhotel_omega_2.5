from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
AREAS_PATH = ROOT / "pkg" / "opt" / "areas" / "areas.cfg"
REGIONS_PATH = ROOT / "regions" / "regions.cfg"
OUTPUT_PATH = ROOT / "config" / "golocs_by_id.cfg"


AREA_SECTION_RE = re.compile(r"^\s*Areas\s+(\S+)\s*$", re.IGNORECASE)
AREA_LINE_RE = re.compile(
    r"^\s*Area\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)\s+(id=\S+\s+)?(.+?)\s*$",
    re.IGNORECASE,
)
REGION_HEADER_RE = re.compile(r"^\s*Region\s+(.+?)\s*$", re.IGNORECASE)


def normalize_spaces(value: str) -> str:
    return " ".join(value.strip().split())


def normalize_name(value: str) -> str:
    return normalize_spaces(value).lower()


def make_slug(value: str) -> str:
    slug = re.sub(r"[^a-z0-9]+", "_", normalize_name(value)).strip("_")
    return slug or "area"


def parse_ints(value: str) -> tuple[int, int, int, int] | None:
    parts = value.split()
    if len(parts) < 4:
        return None
    try:
        return int(parts[0]), int(parts[1]), int(parts[2]), int(parts[3])
    except ValueError:
        return None


def parse_areas(path: Path) -> list[dict[str, str | int]]:
    entries: list[dict[str, str | int]] = []
    current_realm: str | None = None

    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        line = raw_line.strip()
        if not line or line.startswith("//"):
            continue

        section_match = AREA_SECTION_RE.match(line)
        if section_match:
            current_realm = section_match.group(1)
            continue

        area_match = AREA_LINE_RE.match(raw_line)
        if not area_match or current_realm is None:
            continue

        min_x, max_x, min_y, max_y, _, name = area_match.groups()
        entries.append(
            {
                "realm": current_realm,
                "name": normalize_spaces(name),
                "name_norm": normalize_name(name),
                "min_x": int(min_x),
                "max_x": int(max_x),
                "min_y": int(min_y),
                "max_y": int(max_y),
                "range": f"{min_x} {min_y} {max_x} {max_y}",
            }
        )

    return entries


def parse_regions(path: Path) -> list[dict[str, str | int]]:
    regions: list[dict[str, str | int]] = []
    pending_region_name: str | None = None
    current_region: dict[str, str] | None = None

    for raw_line in path.read_text(encoding="utf-8", errors="ignore").splitlines():
        stripped = raw_line.strip()
        if not stripped or stripped.startswith("#") or stripped.startswith("//"):
            continue

        header_match = REGION_HEADER_RE.match(raw_line)
        if header_match and current_region is None:
            pending_region_name = header_match.group(1).strip()
            continue

        if stripped == "{":
            current_region = {}
            if pending_region_name:
                current_region["HeaderName"] = pending_region_name
            pending_region_name = None
            continue

        if stripped == "}":
            if current_region:
                header_name = normalize_spaces(current_region.get("HeaderName", ""))
                region_name = normalize_spaces(current_region.get("Name", "") or header_name)
                region_id = normalize_spaces(current_region.get("Id", ""))
                range_value = normalize_spaces(current_region.get("Range", ""))
                parsed_range = parse_ints(range_value)

                region_entry: dict[str, str | int] = {
                    "HeaderName": header_name,
                    "Name": region_name,
                    "NameNorm": normalize_name(region_name),
                    "Id": region_id,
                    "Type": normalize_spaces(current_region.get("Type", "")),
                    "Realm": normalize_spaces(current_region.get("Realm", "")),
                    "GoLoc": normalize_spaces(current_region.get("GoLoc", "")),
                    "Range": range_value,
                }

                if parsed_range:
                    region_entry["MinX"] = parsed_range[0]
                    region_entry["MinY"] = parsed_range[1]
                    region_entry["MaxX"] = parsed_range[2]
                    region_entry["MaxY"] = parsed_range[3]

                regions.append(region_entry)
            current_region = None
            pending_region_name = None
            continue

        if current_region is None:
            continue

        parts = stripped.split(None, 1)
        key = parts[0]
        value = parts[1].strip() if len(parts) > 1 else ""
        current_region[key] = value

    return regions


def build_indexes(
    regions: list[dict[str, str | int]],
) -> tuple[
    dict[tuple[str, str], list[dict[str, str | int]]],
    dict[str, list[dict[str, str | int]]],
    dict[str, list[dict[str, str | int]]],
]:
    by_name_and_range: dict[tuple[str, str], list[dict[str, str | int]]] = {}
    by_range: dict[str, list[dict[str, str | int]]] = {}
    by_name: dict[str, list[dict[str, str | int]]] = {}

    for region in regions:
        name_norm = str(region.get("NameNorm", ""))
        range_value = str(region.get("Range", ""))
        if name_norm and range_value:
            by_name_and_range.setdefault((name_norm, range_value), []).append(region)
        if range_value:
            by_range.setdefault(range_value, []).append(region)
        if name_norm:
            by_name.setdefault(name_norm, []).append(region)

    return by_name_and_range, by_range, by_name


def pick_region(
    area: dict[str, str | int],
    by_name_and_range: dict[tuple[str, str], list[dict[str, str | int]]],
    by_range: dict[str, list[dict[str, str | int]]],
    by_name: dict[str, list[dict[str, str | int]]],
    used_region_ids: set[int],
) -> dict[str, str | int]:
    area_name_norm = str(area["name_norm"])
    area_range = str(area["range"])
    candidates: list[dict[str, str | int]] = []

    candidates.extend(by_name_and_range.get((area_name_norm, area_range), []))
    if not candidates:
        candidates.extend(by_range.get(area_range, []))
    if not candidates:
        candidates.extend(by_name.get(area_name_norm, []))

    for candidate in candidates:
        candidate_id = id(candidate)
        if candidate_id not in used_region_ids:
            used_region_ids.add(candidate_id)
            return candidate

    return {}


def make_fallback_id(area: dict[str, str | int], used_ids: set[str]) -> str:
    fallback = (
        f"{make_slug(str(area['name']))}_"
        f"{area['min_x']}_{area['max_x']}_{area['min_y']}_{area['max_y']}"
    )
    candidate = fallback
    suffix = 2
    while candidate.lower() in used_ids:
        candidate = f"{fallback}_{suffix}"
        suffix += 1

    used_ids.add(candidate.lower())
    return candidate


def build_output(areas: list[dict[str, str | int]], regions: list[dict[str, str | int]]) -> str:
    lines = [
        "# Generated by pythonscripts/generate_golocs_by_id.py",
        "# Source: pkg/opt/areas/areas.cfg + regions/regions.cfg",
        "",
    ]

    by_name_and_range, by_range, by_name = build_indexes(regions)
    used_region_refs: set[int] = set()
    used_ids: set[str] = set()

    for area in areas:
        region = pick_region(area, by_name_and_range, by_range, by_name, used_region_refs)

        name = str(region.get("Name", "")).strip() or str(area["name"])
        region_id = str(region.get("Id", "")).strip()
        if region_id:
            used_ids.add(region_id.lower())
        else:
            region_id = make_fallback_id(area, used_ids)

        region_type = str(region.get("Type", "")).strip() or "None"
        realm = str(region.get("Realm", "")).strip() or str(area["realm"])

        lines.append(f"GoLocEntry {region_id}")
        lines.append("{")
        lines.append(f"\tRealm\t{realm}")
        lines.append(f"\tName\t{name}")
        lines.append(f"\tId\t{region_id}")
        lines.append(f"\tType\t{region_type}")
        lines.append(f"\tRange\t{str(area['range'])}")

        goloc = str(region.get("GoLoc", "")).strip()
        if goloc:
            lines.append(f"\tGoLoc\t{goloc}")

        lines.append("}")
        lines.append("")

    return "\n".join(lines)


def main() -> None:
    areas = parse_areas(AREAS_PATH)
    regions = parse_regions(REGIONS_PATH)
    output = build_output(areas, regions)
    OUTPUT_PATH.write_text(output, encoding="utf-8", newline="\n")
    print(f"Wrote {len(areas)} entries to {OUTPUT_PATH}")


if __name__ == "__main__":
    main()