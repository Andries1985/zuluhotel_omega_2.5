# Developer Changelog - v1.0.7
**Range:** `27eebc3` (origin/Patch-1.0.6) -> `f844d53` (HEAD)  
**Branch:** Patch-1.0.7  
**Date:** 2026-07-02 -> 2026-07-04  
**Commits in range:** 3 (excluding merge commits)  
**Files changed:** 25 | +169248 / -124

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Townstones - Upgrade System Expansion and Player-Run Town Controls](#3-townstones---upgrade-system-expansion-and-player-run-town-controls)
4. [Globe of Sosaria - Destination Logic and Cooldown Changes](#4-globe-of-sosaria---destination-logic-and-cooldown-changes)
5. [Soul Whisperer - Threshold Summon Persistence Fix](#5-soul-whisperer---threshold-summon-persistence-fix)
6. [Artifact Tooltip Follow-Up](#6-artifact-tooltip-follow-up)
7. [Alryc Package and Animated Graphics Tooling](#7-alryc-package-and-animated-graphics-tooling)
8. [Tooling and Data Authoring Automation](#8-tooling-and-data-authoring-automation)
9. [Exhaustive File-by-File Change List](#9-exhaustive-file-by-file-change-list)
10. [Risk and Regression Notes](#10-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.0.7 is dominated by a large Townstone upgrade-data migration and UI/controller rewrite for upgrade browsing and player-town state management. It also includes a Globe of Sosaria travel logic update (own-town preference, randorin exclusion, cooldown reduction), Soul Whisperer summon-threshold state persistence hardening, and a new developer/staff package (`alryc`) for animated graphics and hue testing.

Large insertion volume is primarily from generated/configurable data assets:

- `pkg/opt/townstones/upgrades.cfg` expanded from 55 entries to 16,440 entries and now carries `Type` + `Animation` metadata.
- `pkg/opt/alryc/config/animatedgraphics.cfg` and `config/animated_tiles*.txt` added as generated animated tile datasets.
- `pkg/opt/townstones/upgrades.xlsx` added as an editable source mirror for cfg synchronization.

---

## 2. Commit Timeline

| Commit | Date | Message |
|--------|------|---------|
| `c52d0f7` | 2026-07-02 | animated graphics command; new alryc package; moved color/animation test commands; Globe search update; inscription test comment |
| `5954aba` | 2026-07-02 | exclude Randorin for players (Globe filtering) |
| `f844d53` | 2026-07-04 | Townstone updates; large upgrades import; Globe update; Soulwhisperer fix |

---

## 3. Townstones - Upgrade System Expansion and Player-Run Town Controls

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/townstones/upgrades.cfg` | Massive data expansion and schema extension (`Type`, `Animation`) |
| `pkg/opt/townstones/upgrades.xlsx` | New spreadsheet source for large-scale upgrade authoring |
| `pkg/opt/townstones/tstone.src` | Upgrade loading/filtering rewrite, category gump flow, page-jump UX, inline preview tile, 7-field tuple support |
| `pkg/opt/townstones/tstone.inc` | `ELECTION_DURATION` changed from test value (2 min) to 1 week |
| `pkg/opt/townstones/playertowns.cfg` | New purchase-price config map for player-run town regions |
| `pkg/opt/townstones/textcmd/player/playerruntowns.src` | New player command + paged status gump for player-run town state |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | Expanded admin gump with toggles for `available`/`purchased`, plus runtime persistence helpers |
| `config/cmds.cfg` | Registered `pkg/opt/townstones/textcmd/player` path under Player cmd level |

### Notable Functional Changes

- Upgrade tuple format changed from 5 fields to 7 fields:
  - old: key, name, desc, cost, graphic
  - new: key, name, desc, cost, graphic, type, animation
- New normalization helpers in `tstone.src`:
  - `NormalizeUpgradeType(...)`
  - `NormalizeUpgradeAnimation(...)`
- Added type filtering:
  - `FilterTownUpgradesByType(...)`
  - category-select entry gump (`OpenTownUpgradeTypeGump`) with Staff/Areas/Vendors/Fiddler/Items buckets.
- Upgrades purchase gump (`OpenTownUpgradesGump`) redesign:
  - wider canvas and row spacing,
  - added animation flag display (`A`, Y/N),
  - inline preview tile instead of separate preview gump,
  - explicit Back button,
  - direct page jump via text entry and Go button,
  - repositioned paging/close controls.
- Removed legacy preview gump function path (`ShowTownUpgradePictureGump`) from interaction flow.
- Election timing updated for live cadence:
  - `ELECTION_DURATION := (7*24*3600)`.
- New `playertowns.cfg` baseline:
  - region list with `PurchasePrice` entries (10,000,000 each by default in this import),
  - includes `Randorin` among configured player-town candidates.
- New player command `.playerruntowns`:
  - displays town, purchased, price, availability, treasury, population, mayor, upgrades enabled, donations enabled.
  - merges sources from regions config and townstone datafile, deduplicated by region key.
- Admin `.townbankstatus` enhancements:
  - new columns/toggles: `Available`, `Purchased`, `Price`.
  - runtime state stored in townstone datafile props:
    - `playertown_available`
    - `playertown_purchased`
  - added helper functions to read/write these runtime toggles and parse safe enabled/int states.

### Data Scale Notes

- `upgrades.cfg` counts in 1.0.7:
  - `Upgrade` entries: 16,440
  - `Type` fields: 16,440
  - `Animation` fields: 16,383
  - animation `Yes`: 1,458
  - animation `No`: 14,925
- `upgrades.cfg` counts in 1.0.6:
  - `Upgrade` entries: 55
  - `Type`: 0
  - `Animation`: 0

---

## 4. Globe of Sosaria - Destination Logic and Cooldown Changes

### Files Changed
| File | Change |
|------|--------|
| `pkg/std/dundee/globeofsosaria.src` | Cooldown reduction, own-town preference, random-filter exclusion, bounds increase, updated messaging |

### Notable Functional Changes

- Cooldown reduced:
  - `GLOBE_COOLDOWN_SECONDS` from `24*60*60` to `8*60*60`.
- World bounds updated:
  - `SOSARIA_MAX_X` from `6151` to `7167`.
- New destination strategy:
  - first attempts `FindTownStoneForCharacter(who)` using player `town` property and `townlist` name fallback.
  - if not found, falls back to filtered random selection via `FindRandomTownStone(who)`.
- New exclusion filter:
  - `EXCLUDED_REGION_NAME := "randorin"`.
  - `IsExcludedTownStone(...)` blocks excluded region for non-staff (`!who.cmdlevel`).
  - staff bypass exclusion.
- Success message now distinguishes own-town return vs random-town send.

---

## 5. Soul Whisperer - Threshold Summon Persistence Fix

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/soulwhisperer.src` | Replaced local threshold flags with object-property gates + global count cap |

### Notable Functional Changes

- Removed local runtime vars:
  - `najs75`, `najs50`, `najs25`.
- `CheckSummonThresholds()` now persists state on NPC object:
  - `SoulWhispererSummon75`
  - `SoulWhispererSummon50`
  - `SoulWhispererSummon25`
  - `SoulWhispererSummonCount`
- Added hard stop:
  - if `SoulWhispererSummonCount >= 3`, threshold summon checks exit early.
- Each threshold trigger increments count and returns immediately after summon.

### Expected Impact

- Prevents repeated threshold summons due to local-variable reset behavior.
- Makes threshold summon state resilient across script loop/state churn.

---

## 6. Artifact Tooltip Follow-Up

### Files Changed
| File | Change |
|------|--------|
| `pkg/packethooks/megacliloc/itemdata.src` | Added artifact label and expiry text append in tooltip property flow |

### Notable Functional Changes

- Added `Artifact` property check and tooltip line:
  - `"<BASEFONT COLOR=#ff9900>Artifact</BASEFONT>"`.
- Added expiry tooltip from `#ArtifactExpireAt`:
  - if elapsed: `"Artifact expired, pending sweep"`
  - else: `"Artifact expires in ..."` via `FormatDuration(...)`.

---

## 7. Alryc Package and Animated Graphics Tooling

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/alryc/pkg.cfg` | New package manifest |
| `pkg/opt/alryc/README.txt` | Package intent and migration note |
| `pkg/opt/alryc/config/animatedgraphics.cfg` | Generated grouped animated graphic IDs (30 groups / 1457 tiles) |
| `pkg/opt/alryc/textcmd/test/animatedgraphics.src` | New command to spawn configured animated tile groups |
| `pkg/opt/alryc/textcmd/test/animationtest.src` | Migrated command to spawn graphic ranges |
| `pkg/opt/alryc/textcmd/test/colortest.src` | Migrated command to spawn color ranges |
| `config/cmds.cfg` | Added `pkg/opt/alryc/textcmd/test` to Test cmd-level dirs |
| `config/command_synopses.cfg` | Added synopses for `animatedgraphics`, `animationtest`, `colortest` |
| `config/animated_tiles.txt` | Added raw animated tile ID list |
| `config/animated_tiles_groups_50.txt` | Added grouped list view (group size 50) |
| `config/animated_tiles_ranges.txt` | Added contiguous range list for animated tiles |

### Notable Functional Changes

- `.animatedgraphics` command:
  - reads `:alryc:animatedgraphics` cfg,
  - supports per-row control,
  - caps maximum spawn batch,
  - annotates spawned items with staff metadata.
- `.animationtest` and `.colortest` moved into package-local test command path with bounds validation and spawn limits.
- New synopsis coverage added in generated command synopsis config.

---

## 8. Tooling and Data Authoring Automation

### Files Changed
| File | Change |
|------|--------|
| `pythonscripts/_gen_alryc_animatedgraphics_cfg.py` | New generator: parse `config/tiles.cfg` animated flag and emit grouped cfg |
| `pythonscripts/_sync_townstone_upgrades.py` | New bidirectional sync utility for `upgrades.cfg` <-> `upgrades.xlsx` |
| `batchFiles/SyncTownstoneUpgrades.bat` | Wrapper batch to run sync script with optional direction |

### Notable Functional Changes

- `--direction cfg-to-xlsx | xlsx-to-cfg | auto` support for townstone upgrades sync.
- Preserves header/meta context in sheet metadata and supports dry-run validation.
- Uses `openpyxl` for workbook generation with formatted headers/metadata sheet.

---

## 9. Exhaustive File-by-File Change List

All files changed in `Patch-1.0.6..Patch-1.0.7`:

| File | +/- | Notes |
|------|-----|-------|
| `batchFiles/SyncTownstoneUpgrades.bat` | +27 / -0 | Batch wrapper for upgrades cfg/xlsx sync script |
| `config/animated_tiles.txt` | +1457 / -0 | Animated tile ID list data artifact |
| `config/animated_tiles_groups_50.txt` | +90 / -0 | Animated tile groups list (size 50) |
| `config/animated_tiles_ranges.txt` | +338 / -0 | Animated tile contiguous range data |
| `config/cmds.cfg` | +2 / -0 | Added townstones player commands dir + alryc test commands dir |
| `config/command_synopses.cfg` | +21 / -0 | Added synopses for new alryc test commands |
| `pkg/opt/alryc/README.txt` | +6 / -0 | Package documentation stub |
| `pkg/opt/alryc/config/animatedgraphics.cfg` | +1579 / -0 | Generated grouped animated graphics config |
| `pkg/opt/alryc/pkg.cfg` | +4 / -0 | New package manifest |
| `pkg/opt/alryc/textcmd/test/animatedgraphics.src` | +104 / -0 | New test command: spawn configured animated groups |
| `pkg/opt/alryc/textcmd/test/animationtest.src` | +67 / -0 | Migrated graphic range test command |
| `pkg/opt/alryc/textcmd/test/colortest.src` | +74 / -0 | Migrated hue range test command |
| `pkg/opt/townstones/playertowns.cfg` | +135 / -0 | New player-town pricing config |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | +247 / -22 | Admin gump expansion; player-town available/purchased runtime toggles |
| `pkg/opt/townstones/textcmd/player/playerruntowns.src` | +426 / -0 | New player command + status gump for player-run towns |
| `pkg/opt/townstones/tstone.inc` | +1 / -1 | Election duration set to one week |
| `pkg/opt/townstones/tstone.src` | +183 / -67 | Upgrade loader schema update, type filter gumps, page jump, preview inline |
| `pkg/opt/townstones/upgrades.cfg` | +163911 / -18 | Massive upgrade dataset expansion + `Type`/`Animation` metadata |
| `pkg/opt/townstones/upgrades.xlsx` | binary add | Spreadsheet source for upgrades dataset |
| `pkg/packethooks/megacliloc/itemdata.src` | +19 / -0 | Artifact tooltip/expiry property text additions |
| `pkg/std/dundee/globeofsosaria.src` | +84 / -7 | Cooldown, own-town preference, randorin exclusion, bounds, messaging |
| `pkg/std/inscription/inscription.src` | +6 / -0 | Commented-out test branch placeholders for future item enchant/recharge paths |
| `pythonscripts/_gen_alryc_animatedgraphics_cfg.py` | +148 / -0 | Generator for animated graphics cfg |
| `pythonscripts/_sync_townstone_upgrades.py` | +303 / -0 | Bidirectional cfg/xlsx sync utility |
| `scripts/ai/soulwhisperer.src` | +16 / -9 | Threshold summon gating moved to object properties |

---

## 10. Risk and Regression Notes

1. **Townstone upgrades data size/performance**
- Upgrade dataset size jump (55 -> 16,440 rows) materially increases config parse and gump rendering work; runtime performance should be observed under live loads.

2. **Animation metadata completeness**
- `Animation` field count is 57 entries short of total upgrades (16,383 vs 16,440). Current normalization safely defaults to `No`, but this indicates partial metadata coverage.

3. **Globe destination filtering edge cases**
- Randorin exclusion is string-based (`regionname` or townlist fallback). Misnamed region metadata could bypass intended filter behavior.

4. **Soul Whisperer persistent props**
- Existing spawned Soul Whisperers with pre-existing state props may need validation/reset policy if behavior differs after script update.

5. **Tooling dependency**
- `_sync_townstone_upgrades.py` requires `openpyxl`; build/deploy environments that use the script need this dependency available.
