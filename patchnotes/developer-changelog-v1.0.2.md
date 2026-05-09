# Developer Changelog — v1.0.2
**Range:** `353ad60` (PR #230 merge) → `b1b4956` (HEAD)  
**Branch:** Patch-1.0.2  
**Date:** 2026-05-09  
**Commits in range:** 3 (excluding cosmetic/patchnote commits)  
**Files changed:** 29 | +1,111 / -242

---

## Table of Contents

1. [New Character Editor Command](#1-new-character-editor-command)
2. [Stat Cap Adjustment Commands](#2-stat-cap-adjustment-commands)
3. [Test Panel Expansion](#3-test-panel-expansion)
4. [Spawnpoint System — Group & Chest Defaults](#4-spawnpoint-system--group--chest-defaults)
5. [Soul Whisperer — New Boss NPC](#5-soul-whisperer--new-boss-npc)
6. [Areas Configuration — Fixes & Updates](#6-areas-configuration--fixes--updates)
7. [Combat System — GetMaxHP Removal](#7-combat-system--getmaxhp-removal)
8. [NPC Loot — Configuration Updates](#8-npc-loot--configuration-updates)

---

## 1. New Character Editor Command

### Files Changed
| File | Change |
|------|--------|
| `scripts/textcmd/test/editcharacter.src` | New command (222 lines) |

### Overview

A new staff command `.editcharacter` has been implemented to allow admins to modify existing player characters using an interactive gump interface. This replaces the need for manual database edits or multiple individual commands.

**Usage:** `.editcharacter` (then target a player character)

**Features:**
- Editable character name, gender, graphic, and color
- Direct stat modification (STR, DEX, INT)
- Real-time skill editing via gump interface
- Permission checks: Cannot edit staff at or above your level
- Full character persistence to data files

**File:** `scripts/textcmd/test/editcharacter.src` (new)

---

## 2. Stat Cap Adjustment Commands

### Files Changed
| File | Change |
|------|--------|
| `scripts/textcmd/test/lowerallchosenstatcaps.src` | New command (105 lines) |
| `scripts/textcmd/test/raiseallchosenstatcaps.src` | New command (105 lines) |
| `config/cmds.cfg` | Added 2 new command entries |

### Overview

Two new staff commands have been added to bulk-adjust stat caps for characters:

**`.lowerallchosenstatcaps`** - Lowers all selected stat caps on a targeted character
**`.raiseallchosenstatcaps`** - Raises all selected stat caps on a targeted character

These commands allow staff to quickly rebalance player stats without using `.editcharacter` for fine-tuning individual caps.

**Files:**
- `scripts/textcmd/test/lowerallchosenstatcaps.src` (new)
- `scripts/textcmd/test/raiseallchosenstatcaps.src` (new)
- `config/cmds.cfg` (updated)

---

## 3. Test Panel Expansion

### Files Changed
| File | Change |
|------|--------|
| `scripts/textcmd/test/testpanel.src` | Expanded (~126 additions) |
| `scripts/textcmd/test/gotonearestspawnpoint.src` | New navigation command (47 lines) |

### Overview

The developer `.testpanel` command has been significantly expanded with new debugging and testing utilities:

**New Features:**
- Go to nearest spawnpoint (`.gotonearestspawnpoint`)
- Character online list view
- Account grouping by IP and Account
- Multi-page account/player browsing interface
- Cleaner category-based GF gump layout

The test panel now serves as a central hub for staff world-building and debugging operations.

**Files:** `scripts/textcmd/test/testpanel.src` (updated)

---

## 4. Spawnpoint System — Group & Chest Defaults

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/spawnpoint/spawnpointmanager.src` | Spawn type default logic (4 lines) |
| `pkg/opt/test/createspawnpointchest.src` | New admin tool (60 lines) |
| `pkg/opt/test/createspawnpointgroup.src` | New admin tool (60 lines) |

### Overview

The spawnpoint system now includes default spawn types for newly created spawnpoints:

**Default Spawnpoint Types:**
- **Group Spawnpoint:** Spawns multiple related NPCs (default configuration added)
- **Chest Spawnpoint:** Spawns loot containers (default configuration added)

Two new commands assist staff in creating these spawnpoint types:
- `.createspawnpointchest` - Creates a new chest-type spawnpoint at target location
- `.createspawnpointgroup` - Creates a new group-type spawnpoint at target location

**Files:**
- `pkg/opt/spawnpoint/spawnpointmanager.src` (updated)
- `pkg/opt/test/createspawnpointchest.src` (new)
- `pkg/opt/test/createspawnpointgroup.src` (new)

---

## 5. Soul Whisperer — New Boss NPC

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/soulwhisperer.src` | New boss AI (472 lines) |
| `config/npcdesc.cfg` | New NPC configuration entry |

### Overview

A new superboss NPC, "Carrie the Soul Whisperer," has been implemented as a challenging encounter script. This AI is partially based on the existing `sum.src` (Summoner) and `chaosspellkillpcs.src` combat system.

**Boss Mechanics:**
- **Progressive Portal Summons:** At 75%, 50%, and 25% HP, opens a moongate-styled portal and summons a random superboss
- **Rare Champion Spawn:** 1-in-1000 chance to summon a champion-tier boss instead of standard superboss
- **Dynamic Hue Shifts:** Boss hue changes on each summon for visual variety
- **Portal Graphics:** Moongate portal appears with random color on each summon

**AI Framework:**
- Uses standard NPC combat event system (`spellcombatevent.src`)
- Integrates with spell setup and mod setup includes
- Supports anchors and NPC utility functions
- Sleep mode and loot system compatible

**Files:**
- `scripts/ai/soulwhisperer.src` (new, 472 lines)
- `config/npcdesc.cfg` (updated with Soul Whisperer entry)
- `config/nlootgroup.cfg` (updated with loot group for Soul Whisperer)

---

## 6. Areas Configuration — Fixes & Updates

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/areas/areas.cfg` | Significant rebalancing (123 updates) |
| `regions/regions.cfg` | Removed outdated entries (51 deletions) |

### Overview

The areas configuration has been significantly refactored to:
- Fix overlapping and conflicting area definitions
- Realign guarder/PK areas with current world layout
- Remove deprecated or test regions
- Improve area hierarchy and precedence

**Specific Changes:**
- Areas configuration: ~186 lines updated, 123 additions/removals
- Regions configuration: Simplified, 51 lines removed (deprecated entries)

**Impact:**
- More accurate area detection for guards and PK zones
- Reduced area conflicts and edge-case behavior
- Cleaner configuration for future additions

**Files:**
- `pkg/opt/areas/areas.cfg` (updated)
- `regions/regions.cfg` (updated)

---

## 7. Combat System — GetMaxHP Removal

### Files Changed
| File | Change |
|------|--------|
| `scripts/textcmd/admin/akill.src` | Removed GetMaxHP call (2 lines) |
| `scripts/textcmd/coun/kill.src` | Removed GetMaxHP call (2 lines) |
| `scripts/textcmd/player/suicide.src` | Removed GetMaxHP call (2 lines) |
| `scripts/bankwipe.src` | Removed GetMaxHP call (2 lines) |
| `scripts/storagewipe.src` | Removed GetMaxHP call (2 lines) |
| Plus 6 additional NPC AI scripts | Similar cleanup |

### Overview

The deprecated `GetMaxHP()` function has been removed from all kill/death-related commands and scripts. This function was previously used to validate maximum HP before applying death effects, but is no longer necessary with modern POL attribute handling.

**Affected Scripts:**
- Admin kill commands (`.akill`, `.kill`)
- Player suicide command
- Wipe operations (bank, storage)
- NPC AI scripts (animaltrainer, chaosmultikillpcs, humuc, instakillguard, newbiemultikillpcs, playermerchant, townguard, warrior)

**Technical Notes:**
- No functional change to gameplay: NPCs and players still die correctly
- Simplifies codebase by removing obsolete function calls
- Part of ongoing POL 099+ compatibility efforts

**Files:** 14 files total (see above listing)

---

## 8. NPC Loot — Configuration Updates

### Files Changed
| File | Change |
|------|--------|
| `config/nlootgroup.cfg` | New Soul Whisperer loot group (25 lines) |
| `config/itemdesc.cfg` | New summon-loot items (28 lines) |
| `pkg/systems/combat/config/itemdesc.cfg` | New boss loot items (28 lines) |

### Overview

New loot groups and item definitions have been added for the Soul Whisperer boss encounter:

**Soul Whisperer Loot Configuration:**
- Rare drops for defeating the boss
- Summon-related equipment drops
- Boss-specific rare items
- Integration with existing loot distribution system

**Files:**
- `config/nlootgroup.cfg` (new Soul Whisperer loot group)
- `config/itemdesc.cfg` (new items)
- `pkg/systems/combat/config/itemdesc.cfg` (new items)

---

## Summary of Changes

This patch introduces a comprehensive suite of administrative and gameplay features:
- **Admin Tools:** Character editor, stat cap commands, enhanced test panel
- **Spawnpoint System:** Improved defaults and creation commands
- **Boss Content:** Soul Whisperer superboss with progressive mechanics
- **Configuration:** Areas rebalancing and cleanup
- **Code Cleanup:** Removal of deprecated GetMaxHP() calls
- **Loot System:** New boss-specific loot groups

**Total Impact:**
- 29 files changed
- ~1,111 lines added
- ~242 lines removed
- 3 major commits
