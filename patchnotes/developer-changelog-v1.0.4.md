# Developer Changelog — v1.0.4
**Range:** `decbf45` (Patch 1.0.3 notes update) → `1103845` (HEAD)  
**Branch:** Patch-1.0.4  
**Date:** 2026-05-22 → 2026-05-23  
**Commits in range:** 5 (excluding merge commits)  
**Files changed:** 11 | +266 / -76

---

## Table of Contents

1. [Player Vendor Escrow Filename/Resolution Fixes](#1-player-vendor-escrow-filenameresolution-fixes)
2. [Skill Gain and InfoVault Adjustments](#2-skill-gain-and-infovault-adjustments)
3. [Soul Whisperer Portal Flow Rework](#3-soul-whisperer-portal-flow-rework)
4. [Test Command Additions](#4-test-command-additions)
5. [Commit Timeline](#5-commit-timeline)

---

## 1. Player Vendor Escrow Filename/Resolution Fixes

### Files Changed
| File | Change |
|------|--------|
| `pkg/systems/playervendor/include/escrow.inc` | Introduced canonical/legacy escrow filename resolution + optional migration path |
| `pkg/systems/playervendor/playermerchant.src` | Escrow append path now resolves serial-targeted datafile name with migration enabled |
| `pkg/systems/playervendor/commands/player/escrow.src` | Escrow gump loading now serial-aware across merchant keys; robust key enumeration |
| `pkg/systems/playervendor/commands/test/pmescrowtest.src` | Test command adjusted to serial-aware escrow datafile resolution |
| `pkg/systems/playervendor/vendordeed.src` | Removed legacy vendor pay key write (`p`) |

### Overview

This patch stabilizes escrow persistence and retrieval by standardizing canonical filename usage (`merchantescrow_<serial>`) while preserving compatibility with legacy name-based files.

### Notable Functional Changes

- Added `BuildLegacyEscrowDatafileName()` and retained canonical `BuildEscrowDatafileName()` behavior.
- Added `ResolveEscrowDatafileNameForSerial(player_obj, merchant_serial, migrate_to_canonical)`:
  - Prefers canonical file when matching vendor key exists.
  - Falls back to legacy file when needed.
  - Can migrate legacy entries to canonical format on write paths.
- Added helper migration routine `CopyLegacyEscrowEntriesForVendorKey()`.
- Player escrow gump collection now enumerates merchant serial keys first, then loads entries per serial.

### Expected Impact

- Reduces false-empty escrow views caused by filename drift/mismatch.
- Preserves previously stored escrow entries without forced manual migration.
- Improves reliability of escrow claims and future writes.

---

## 2. Skill Gain and InfoVault Adjustments

### Files Changed
| File | Change |
|------|--------|
| `scripts/include/skillpoints.inc` | Power Hour multiplier moved to raw-point stage to avoid double scaling path |
| `scripts/textcmd/player/infovault.src` | Corrected message text + URL path (`/infovault`) |
| `pkg/opt/shilitems/infinitegems.src` | Added 8 gem objtypes and matching placement coordinates |

### Overview

Miscellaneous gameplay and player-command fixes:

- Power Hour gain modifier application was moved lower in the calculation flow to correct double-gain behavior.
- `.infovault` command now opens the corrected site endpoint.
- Infinite gem generator now includes additional gem variants.

---

## 3. Soul Whisperer Portal Flow Rework

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/soulwhisperer.src` | Portal summon flow refactored to offload delayed spawn/cleanup |
| `scripts/misc/soulwhispererportal.src` | New detached worker script for delayed summon + portal cleanup |

### Overview

Soul Whisperer summoning now delegates delayed boss spawn and portal lifetime handling to a dedicated script, reducing timing coupling in the AI script.

### Notable Functional Changes

- Champion selection chance changed to `11/1000` (`Random(1000) <= 10`).
- Main AI now tags spawned portal and starts detached cleanup/summon scripts.
- New helper script:
  - waits 5s,
  - spawns selected template at computed location,
  - applies summon color,
  - waits 10s,
  - destroys portal.
- Additional fallback portal cleanup is scheduled via existing deleter script.

### Expected Impact

- More deterministic portal lifecycle during HP-threshold summon events.
- Cleaner separation of AI decision logic vs delayed world actions.

---

## 4. Test Command Additions

### Files Changed
| File | Change |
|------|--------|
| `scripts/textcmd/test/pmescrowtest.src` | New test-scope command to transfer merchant ownership and escrow packs |

### Overview

Added a test command workflow to support merchant takeover and escrow transfer verification in staff/test environments.

---

## 5. Commit Timeline

1. `a10acd1` — Fix player-vendor escrow filename stability and remove legacy vendordeed pay key  
2. `142e73d` — Fixed double skill gain power hour added new gems to infinitegems infovault fix  
3. `f87379b` — Take over merchant test command  
4. `daf6e36` — Escrow Fix  
5. `1103845` — Soulwhisper portal fix

---

## Summary of Changes

Patch 1.0.4 is a reliability-focused follow-up with targeted fixes:

- **Playervendor Escrow:** canonical filename resolution, legacy compatibility, key-level migration support
- **Gameplay Math:** corrected Power Hour gain application stage
- **Utility/QoL:** Infinite Gems expansion and `.infovault` URL correction
- **Event Logic:** Soul Whisperer portal flow offloaded to dedicated worker script
- **Test Tooling:** merchant takeover escrow test command

**Total Impact:**
- 11 files changed
- ~266 lines added
- ~76 lines removed
- 5 non-merge commits
