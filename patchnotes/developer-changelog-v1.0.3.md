# Developer Changelog — v1.0.3
**Range:** `fd0ab98` (Patch 1.0.2 notes update) → `cf587a4` (HEAD)  
**Branch:** Patch-1.0.3  
**Date:** 2026-05-15 → 2026-05-17  
**Commits in range:** 11 (excluding merge commits)  
**Files changed:** 41 | +2,993 / -736

---

## Table of Contents

1. [Player Vendor System Overhaul](#1-player-vendor-system-overhaul)
2. [Banking Balance Update (Checks Counted)](#2-banking-balance-update-checks-counted)
3. [Admin Command and Spawnpoint Tooling Updates](#3-admin-command-and-spawnpoint-tooling-updates)
4. [Account Maintenance Command Updates](#4-account-maintenance-command-updates)
5. [Speech Hook Cleanup](#5-speech-hook-cleanup)
6. [Build/Config Integration Changes](#6-buildconfig-integration-changes)
7. [PvP Arena and Display Fixes](#7-pvp-arena-and-display-fixes)
8. [Commit Timeline](#8-commit-timeline)

---

## 1. Player Vendor System Overhaul

### Files Changed
| File | Change |
|------|--------|
| `pkg/systems/playervendor/playermerchant.src` | Core merchant script moved and heavily expanded |
| `pkg/systems/playervendor/include/escrow.inc` | New escrow data helper layer |
| `pkg/systems/playervendor/commands/player/escrow.src` | New player escrow claim command |
| `pkg/systems/playervendor/commands/admin/playermerchantstatus.src` | New admin vendor status command |
| `pkg/systems/playervendor/commands/test/migratevendorstorage.src` | New migration utility |
| `pkg/systems/playervendor/commands/test/pmescrowtest.src` | New escrow test utility |
| `pkg/systems/playervendor/commands/test/pmfireme.src` | New force-close test utility |
| `pkg/systems/playervendor/itemdesc.cfg` | Vendor deed config moved into package |
| `pkg/systems/playervendor/vendordeed.src` | Vendor deed script moved into package |
| `pkg/opt/zuluitems/vendordeed.src` | Removed (replaced by package-local vendor deed) |

### Overview

Patch 1.0.3 introduces a major rework of the player-vendor subsystem with escrow support, ownership tooling, and package-local vendor assets.

### Notable Functional Changes

- Added `.escrow` player command for claiming merchant escrow packages.
- Added escrow storage/datafile model (`merchantescrow_*`) for package persistence and retrieval.
- Added admin visibility command for player merchant status and debt tracking.
- Added migration and test tools to validate/transition vendor storage safely.
- Rehomed vendor deed/item definitions from `pkg/opt/zuluitems` to `pkg/systems/playervendor`.

### Wage Logic Changes

The vendor wage model now explicitly differentiates by region type:
- City regions: monthly wage basis `40,000`
- Non-city regions: monthly wage basis `10,000`

(Implemented through daily accrual logic in `playermerchant.src`.)

---

## 2. Banking Balance Update (Checks Counted)

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/banker.src` | Balance calculation updated |

### Overview

`Balance()` now includes bank checks (`objtype 0x14000`) in reported account totals, reading `Amount` and fallback `checkamount` properties.

### Impact

- Banker “balance” speech now reports a more complete bank value.
- Players no longer see coin-only totals when checks are present.

---

## 3. Admin Command and Spawnpoint Tooling Updates

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/spawnpoint/textcmd/admin/gotomobtype.src` | Updated mob-type navigation logic |
| `pkg/opt/spawnpoint/textcmd/admin/gotospawnpoint.src` | Expanded spawnpoint navigation workflow |
| `pkg/opt/spawnpoint/config/groups.cfg` | Spawnpoint grouping data updates |
| `pkg/opt/spawnpoint/textcmd/admin/createspawnpointchest.src` | Moved from test -> admin namespace |
| `pkg/opt/spawnpoint/textcmd/admin/createspawnpointgroup.src` | Moved from test -> admin namespace |
| `pkg/opt/spawnpoint/textcmd/admin/gotonearestspawnpoint.src` | Moved from test -> admin namespace |
| `pkg/opt/spawnpoint/textcmd/admin/forcespawnarea.src` | Moved from test -> admin; follow-up fix |
| `config/command_synopses.cfg` | Command documentation updated |

### Overview

Spawnpoint and movement/debug commands were promoted and refined for administrator workflows.

### Notable Changes

- Multiple spawnpoint tools moved from test command space to admin command space.
- `gotomobtype` and `gotospawnpoint` behaviors were revised and expanded.
- Command synopsis generation and output were refreshed for new/updated command set.

---

## 4. Account Maintenance Command Updates

### Files Changed
| File | Change |
|------|--------|
| `pkg/systems/accounts/commands/dev/eraseEmptyAccounts.src` | Removed legacy location |
| `pkg/systems/accounts/textcmd/test/eraseEmptyAccounts.src` | Added updated command in active test command path |
| `config/cmds.cfg` | CmdLevel DIR updates include systems accounts/playervendor paths |

### Overview

`eraseEmptyAccounts` was relocated/updated to align with active command directories and current command-level routing.

---

## 5. Speech Hook Cleanup

### Files Changed
| File | Change |
|------|--------|
| `pkg/packethooks/speech/receivespeechhook.src` | Debug print cleanup + flow adjustments |

### Overview

Speech packet hook logging/debug behavior was cleaned up to reduce noisy runtime output and tighten message handling.

---

## 6. Build/Config Integration Changes

### Files Changed
| File | Change |
|------|--------|
| `config/cmds.cfg` | Added command directory wiring for playervendor/accounts systems |
| `pythonscripts/_gen_command_synopses_cfg.py` | Generator update |
| `scripts/ecompile.cfg.example` | Compile inclusion updates |
| `scripts/include/utility.inc` | Helper updates supporting command flow |
| `scripts/include/constants/npcai.inc` | Minor constant-level update |
| `scripts/include/randname.inc` | Name/label support updates |
| `config/itemdesc.cfg` / `config/npcdesc.cfg` | Supporting config touch-ups |

### Overview

Supporting config/build plumbing was updated to ensure new commands and playervendor package content are discoverable and compilable.

---

## 7. PvP Arena and Display Fixes

During patch development, several PvP arena mechanics issues and a player display issue were identified and addressed.

### Files Changed
| File | Change |
|------|---------|
| `scripts/items/pvp.src` | Arena mechanics stabilization |
| `scripts/items/pvp2vs2.src` | 2v2 arena fixes and improvements |
| `scripts/ai/referee.src` | Referee NPC behavior additions |
| `scripts/ai/noble.src` | Minor cleanup for arena support |
| `scripts/textcmd/player/prots.src` | Display fix for equipped INT on mages |
| `config/npcdesc.cfg` | Referee NPC configuration update |

### 7.1 PvP Arena Mechanics Fixes

**Commits:** `216bf8b` (PVP Error), `9b9053e` (PVP Arena Fixes)  
**Changes:**
- Single-arena (1v1) PvP system: refined match flow and error handling.
- 2v2 Arena: significant improvements to team coordination, match logic, and recovery from edge cases.
- Noble arena participant initialization and tracking cleaned up.

### 7.2 Referee NPC Improvements

**Commit:** `3841a92` (More Referee Fixes)  
**Changes:**
- Expanded referee NPC functions to better manage arena events and support new mechanics.
- Referee configuration updated in NPC definitions.

### 7.3 Player Character Display — Prots Command

**Commit:** `cf587a4` (Prots fix to show equipped INT)  
**Changes:**
- `.prots` command now correctly displays **Equipped INT** for mage-class characters.
- Non-mage characters display "Not mage" instead of undefined values.
- Provides clarity on magic equipment bonuses in real time.

---

## 8. Commit Timeline

1. `1924540` — Update to gotomobtypes and eraseemptyaccounts command  
2. `cff900f` — Gotomobtypes and eraseemptyaccounts updates  
3. `c8795d2` — Updated Admin commands  
4. `873be49` — Speechhook print removed  
5. `5bb6706` — Checks are counted for balance  
6. `fcb88a0` — Player Vendor Update (escrow, fire-me path, wage model)  
7. `e49d63a` — Patch notes  
8. `216bf8b` — PVP Error  
9. `9b9053e` — PVP Arena Fixes  
10. `3841a92` — More Referee Fixes  
11. `cf587a4` — Prots fix to show equipped INT

---

## Summary of Changes

Patch 1.0.3 is primarily a systems-and-operations update centered on player vendors and staff tooling:

- **Player Vendor:** escrow architecture, new commands, package migration, wage model updates
- **Banking:** checks now count in banker balance output
- **Admin Tools:** spawnpoint command promotions and navigation updates
- **Maintenance:** account cleanup command relocation/update
- **Runtime Cleanup:** speech hook debug output refinement

**Total Impact:**
- 41 files changed
- ~2,993 lines added
- ~736 lines removed
- 11 non-merge commits
