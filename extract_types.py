#!/usr/bin/env python3
"""Extract NPC Type properties from npcdesc.cfg and cross-reference with tracking.cfg"""
import re

def parse_tracking_cfg(path):
    graphic_to_tracking = {}
    current_graphic = None
    with open(path, 'r') as f:
        for line in f:
            line = line.strip()
            m = re.match(r'trackingitem\s+(0x[0-9a-fA-F]+|\d+)', line)
            if m:
                val = m.group(1)
                current_graphic = int(val, 16) if val.lower().startswith('0x') else int(val)
                continue
            m = re.match(r'type\s+(\S+)', line)
            if m and current_graphic is not None:
                graphic_to_tracking[current_graphic] = m.group(1)
                current_graphic = None
    return graphic_to_tracking

def parse_npcdesc_cfg(path):
    npcs = []
    current_template = None
    current_name = None
    current_objtype = None
    current_type = None
    with open(path, 'r') as f:
        for line in f:
            stripped = line.strip()
            if stripped.startswith('//'):
                continue
            m = re.match(r'NpcTemplate\s+(\S+)', stripped)
            if m:
                if current_template is not None:
                    npcs.append({'template': current_template, 'name': current_name or '', 'objtype': current_objtype, 'type': current_type or ''})
                current_template = m.group(1)
                current_name = None
                current_objtype = None
                current_type = None
                continue
            if stripped == '}':
                continue
            m = re.match(r'Name\s+(.+)', stripped, re.IGNORECASE)
            if m and current_template and current_name is None:
                current_name = m.group(1).strip()
                continue
            m = re.match(r'objtype\s+(0x[0-9a-fA-F]+|\d+)', stripped, re.IGNORECASE)
            if m and current_template:
                val = m.group(1)
                current_objtype = int(val, 16) if val.lower().startswith('0x') else int(val)
                continue
            m = re.match(r'CProp\s+Type\s+s(\S+)', stripped)
            if m and current_template:
                current_type = m.group(1)
                continue
        if current_template is not None:
            npcs.append({'template': current_template, 'name': current_name or '', 'objtype': current_objtype, 'type': current_type or ''})
    return npcs

def main():
    tracking_map = parse_tracking_cfg('pkg/std/tracking/tracking.cfg')
    npcs = parse_npcdesc_cfg('config/npcdesc.cfg')
    with_type = sum(1 for n in npcs if n['type'])
    without_type = sum(1 for n in npcs if not n['type'])
    with_tracking = sum(1 for n in npcs if n['objtype'] is not None and n['objtype'] in tracking_map)
    type_counts = {}
    for n in npcs:
        t = n['type'] or '(none)'
        type_counts[t] = type_counts.get(t, 0) + 1
    lines = []
    lines.append('# NPC Type Mapping Report')
    lines.append('')
    lines.append(f'Total NPC templates: {len(npcs)}')
    lines.append(f'With slayer Type: {with_type}')
    lines.append(f'Without slayer Type: {without_type}')
    lines.append(f'With tracking entry: {with_tracking}')
    lines.append('')
    lines.append('## Slayer Type Summary')
    lines.append('')
    lines.append('| Slayer Type | Count |')
    lines.append('|-------------|-------|')
    for t in sorted(type_counts.keys()):
        lines.append(f'| {t} | {type_counts[t]} |')
    lines.append('')
    lines.append('## Full NPC Mapping')
    lines.append('')
    lines.append('| Template | Name | Graphic | Slayer Type | Tracking Type |')
    lines.append('|----------|------|---------|-------------|---------------|')
    for n in npcs:
        objtype = n['objtype']
        slayer_type = n['type'] or '(none)'
        if objtype is not None:
            graphic_str = f'0x{objtype:02X}'
            tracking_type = tracking_map.get(objtype, 'N/A')
        else:
            graphic_str = 'N/A'
            tracking_type = 'N/A'
        name = n['name'].replace('|', '\\|')
        lines.append(f'| {n["template"]} | {name} | {graphic_str} | {slayer_type} | {tracking_type} |')
    lines.append('')
    lines.append('## NPCs Without Slayer Type')
    lines.append('')
    lines.append('| Template | Name | Graphic | Tracking Type |')
    lines.append('|----------|------|---------|---------------|')
    for n in npcs:
        if not n['type']:
            objtype = n['objtype']
            if objtype is not None:
                graphic_str = f'0x{objtype:02X}'
                tracking_type = tracking_map.get(objtype, 'N/A')
            else:
                graphic_str = 'N/A'
                tracking_type = 'N/A'
            name = n['name'].replace('|', '\\|')
            lines.append(f'| {n["template"]} | {name} | {graphic_str} | {tracking_type} |')
    # Build reverse map: graphic -> list of NPC slayer types
    graphic_to_npc_types = {}
    for n in npcs:
        if n['objtype'] is not None:
            g = n['objtype']
            t = n['type'] or '(none)'
            if g not in graphic_to_npc_types:
                graphic_to_npc_types[g] = set()
            graphic_to_npc_types[g].add(t)

    # Also parse tracking.cfg for name and tracking type per graphic
    tracking_entries = []
    current_graphic = None
    current_name = None
    current_type = None
    with open('pkg/std/tracking/tracking.cfg', 'r') as f:
        for line in f:
            stripped = line.strip()
            m = re.match(r'trackingitem\s+(0x[0-9a-fA-F]+|\d+)', stripped)
            if m:
                if current_graphic is not None:
                    tracking_entries.append({'graphic': current_graphic, 'name': current_name or '', 'type': current_type or ''})
                val = m.group(1)
                current_graphic = int(val, 16) if val.lower().startswith('0x') else int(val)
                current_name = None
                current_type = None
                continue
            m = re.match(r'name\s+(.+)', stripped, re.IGNORECASE)
            if m and current_graphic is not None and current_name is None:
                current_name = m.group(1).strip()
            m = re.match(r'type\s+(\S+)', stripped)
            if m and current_graphic is not None:
                current_type = m.group(1)
        if current_graphic is not None:
            tracking_entries.append({'graphic': current_graphic, 'name': current_name or '', 'type': current_type or ''})

    # Deduplicate by graphic (keep first entry name/type)
    seen_graphics = {}
    unique_tracking = []
    for entry in tracking_entries:
        if entry['graphic'] not in seen_graphics:
            seen_graphics[entry['graphic']] = entry
            unique_tracking.append(entry)

    lines.append('')
    lines.append('## Tracking Graphics to NPC Mapping')
    lines.append('')
    lines.append('| Graphic | Tracking Name | Tracking Type | Has NPC | NPC Slayer Type(s) |')
    lines.append('|---------|---------------|---------------|---------|-------------------|')
    for entry in sorted(unique_tracking, key=lambda e: e['graphic']):
        g = entry['graphic']
        graphic_str = f'0x{g:02X}'
        npc_types = graphic_to_npc_types.get(g)
        has_npc = 'Yes' if npc_types else 'No'
        if npc_types:
            slayer_str = ', '.join(sorted(npc_types))
        else:
            slayer_str = 'N/A'
        lines.append(f'| {graphic_str} | {entry["name"]} | {entry["type"]} | {has_npc} | {slayer_str} |')

    lines.append('')
    with open('npc_type_mapping.md', 'w') as f:
        f.write('\n'.join(lines))
    print(f'Written npc_type_mapping.md')
    print(f'  {len(npcs)} NPC templates')
    print(f'  {with_type} with slayer Type, {without_type} without')
    print(f'  {with_tracking} with tracking.cfg entries')
    print(f'  {len(unique_tracking)} unique tracking graphics')
    no_npc = sum(1 for e in unique_tracking if e['graphic'] not in graphic_to_npc_types)
    print(f'  {no_npc} tracking graphics with no NPC match')

if __name__ == '__main__':
    main()
