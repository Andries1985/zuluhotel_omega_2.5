# Developer Changelog - v1.0.6
**Range:** `9b695eb` (origin/Patch-1.0.5) -> `1d47820` (HEAD)  
**Branch:** Patch-1.0.6  
**Date:** 2026-06-03 -> 2026-06-25  
**Commits in range:** 3 (excluding merge commits)  
**Files changed:** 16 | +407 / -29

---

## Table of Contents

1. [Talisman Compile Fix](#1-talisman-compile-fix)
2. [Tamed Following Fix - Multi Overhang Cleanup](#2-tamed-following-fix---multi-overhang-cleanup)
3. [Artifact System - Champion Relics and Expiry Sweeper](#3-artifact-system---champion-relics-and-expiry-sweeper)
4. [Bank Resource Usage Fix - Cartography, Camping, Blacksmithy](#4-bank-resource-usage-fix---cartography-camping-blacksmithy)
5. [Player Merchant - Hidden/Concealed Announce Guard](#5-player-merchant---hiddenconcealed-announce-guard)
6. [Commit Timeline](#6-commit-timeline)

---

## 1. Talisman Compile Fix

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/talisman/include/talismanid.inc` | Moved `var result` declaration out of inner conditional scope to fix compile error |

### Overview

The batch-ID container path in `TalismanID(...)` declared `var result` inside an inner `else` block, which caused a compile error due to the variable being referenced prior to declaration in the outer scope. This was a direct follow-up to the 1.0.5 talisman logic rewrite.

### Notable Functional Changes

- `var result` is now declared at the start of the container-ID branch, before the itemlist size check.
- `result :=` is used for the actual assignment inside `IDCore_IdentifyContainerItems(...)`.
- No behavioral change; compile-only fix.

### Expected Impact

- Talisman batch-ID compiles and runs correctly.

---

## 2. Tamed Following Fix - Multi Overhang Cleanup

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/tamed.src` | Removed `following := 0` assignments from all multi-mismatch early-return branches |

### Overview

The `Follow()` function had a pattern where every early-return branch (multi mismatch, line-of-sight fail) both zeroed `following` and returned. Since `following` is a local variable and the function returns immediately after, zeroing it had no observable effect — but more critically it was causing the pet to stall in overhang/tower situations by invalidating the follow target before returning, which could interact with outer loop state. Removing the redundant assignments fixes following behavior beneath multi overhangs.

### Notable Functional Changes

- Removed `following := 0;` from all 21 early-return branches across three multi-owner check blocks.
- `return;` remains in each branch — behavior is otherwise unchanged for non-overhang cases.
- Pets now follow correctly through tower/multi overhang geometry.

### Expected Impact

- Pets no longer stall or lose follow state when the path crosses tower overhangs.
- No change to multi-boundary enforcement logic.

---

## 3. Artifact System - Champion Relics and Expiry Sweeper

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/ArtifactSystem/artifact.inc` | Added `ARTIFACT_EXPIRE_PROP` const, null-safe `getItemArtifactBox`, recycle prop cleanup, `SweepExpiredArtifacts` and `SweepExpiredArtifactsInContainer` functions |
| `pkg/opt/ArtifactSystem/artifactbox.src` | Added Champion Relic objtype consts, decay timer assignment on pull, null guard |
| `pkg/opt/ArtifactSystem/championrelic.src` | New script — use handler for Champion Relics; spawns a champion boss and destroys relic |
| `pkg/opt/ArtifactSystem/itemdesc.cfg` | Added item entries for Champion Relic Tier 1 (`0x7991`) and Tier 2 (`0x7992`) |
| `pkg/opt/ArtifactSystem/artifact_daily_sweeper.src` | New script — daily daemon that calls `SweepExpiredArtifacts` |
| `pkg/opt/ArtifactSystem/artifact_startup_sweeper.src` | New script — one-shot startup sweep to catch any expired artifacts from before the feature existed |
| `pkg/packethooks/megacliloc/itemdata.src` | Added artifact expiry countdown display and `FormatDuration()` helper |
| `scripts/start.src` | Registered artifact sweeper daemons at startup |
| `.gitignore` | Minor update |
| `pkg/opt/ArtifactSystem/artifact-system-summary.txt` | Internal system documentation |

### Overview

Two new artifact items have been added to the Artifact System: Champion Relics (Tier 1 and Tier 2). These are obtained from the artifact box and allow players to summon a champion-level boss when activated. Relics have a 14-day decay timer and are destroyed on use. A sweep system was also added to automatically remove expired relics from all storage areas.

### Notable Functional Changes

**Champion Relic Items:**
- `0x7991` — Champion Relic (Tier 1): `MagicItemLevel i10`, activates barracoon or erebuschaosgod.
- `0x7992` — Champion Relic (Tier 2): `MagicItemLevel i11`, activates rikktor or nyxseductress.
- Both use graphic `0x3CC1` and are tagged with `Artifact i1` CProp.

**Artifact Box Distribution:**
- When a Champion Relic is pulled from the artifact box, it receives:
  - `decayat` set to `ReadGameClock() + 1209600` (14 days).
  - `#ArtifactExpireAt` property (same value) for sweeper reference.
  - `RelicUID` set to a random integer for uniqueness tracking.
- When a relic is recycled back into the artifact box, all expiry/uid props are erased so it restarts fresh on next pull.
- Added null guard: if artifact box is empty, `getItemArtifactBox()` returns `0` (no crash).

**Champion Relic Use Script (`championrelic.src`):**
- Blocks activation in safe areas, guarded areas, and NOPK areas.
- Spawns a randomly chosen boss from the tier's template list near the activating player.
- Plays visual effect (`0x3728`) and sound (`0x01FE`) on boss spawn location.
- Destroys the relic on successful boss spawn.
- Returns graceful failure message if `CreateNpcFromTemplate` fails.

**Expiry Sweeper:**
- `SweepExpiredArtifacts(scope)` iterates all storage areas and recursively checks containers for expired artifact items (where `expire_at <= now`). Destroys expired items and logs a summary.
- `artifact_daily_sweeper.src` runs this sweep on a daily loop.
- `artifact_startup_sweeper.src` runs a one-shot sweep on shard startup.
- Both sweepers registered in `scripts/start.src`.

**MegaCliloc Expiry Display:**
- Artifact items tagged with `#ArtifactExpireAt` now show a human-readable countdown in item tooltips.
- Display: `"Artifact expires in Xd Xh Xm Xs"` (or `"Artifact expired, pending sweep"` if timer elapsed).
- `FormatDuration(total_seconds)` helper added to handle days/hours/minutes/seconds formatting.

### Expected Impact

- Champion Relics are now obtainable from artifact boxes and can be activated in the open world to summon a champion-class boss.
- Expired relics are swept from player storage/banks automatically.
- Players can inspect relic time-remaining from item tooltip.

---

## 4. Bank Resource Usage Fix - Cartography, Camping, Blacksmithy

### Files Changed
| File | Change |
|------|--------|
| `pkg/std/cartography/cartography.src` | Added `EnsureMapResources`, `ItemIsInBackpackScope`, `GetNewMapMaterialCost` helpers; resource pre-check before each map tier; blank map backpack scope guard |
| `pkg/std/camping/camping.src` | Added `ItemIsInBackpackScope` check for kindling wood |
| `pkg/std/blacksmithy/blacksmithy.src` | Added missing `return` after insufficient gold check in `MakeJewelry` |

### Overview

Several crafting scripts had bugs where resources could be consumed from or validated against a bank, or flow would fall through after a validation failure. This commit fixes all three independently.

### Notable Functional Changes

**Cartography:**
- Added `GetNewMapMaterialCost(who)` — returns material cost (10, or 5 during crafting PH). Replaces inline duplicated logic in `makeNewmap`.
- Added `EnsureMapResources(who, amount)` — checks `GetAvailableResource(who, mapRequest).total` before proceeding. Returns `0` on failure with a "You don't have enough resources." message.
- All five map tiers (local/regional/world/newmap-1/newmap-2) now call `EnsureMapResources` before skill check and map creation, preventing erroneous skill gain or map creation with no resources.
- Added `ItemIsInBackpackScope(who, item)` — returns `1` if item is the backpack itself, directly in backpack, or nested inside backpack. Guards blank maps from being used out of a bank or container.
- If blank map is not in backpack scope, player sees: "That blank map must be in your backpack."
- Refactored `makeNewmap` to call `GetNewMapMaterialCost` instead of repeating the inline PH check.

**Camping:**
- Added `ItemIsInBackpackScope(character, item)` (same pattern as cartography).
- If kindling wood is inside a container that is not the backpack or ground, player sees: "That kindling must be in your backpack or on the ground."

**Blacksmithy:**
- `MakeJewelry`: the gold amount check (`ore.amount < 100`) previously sent the error message but did not return. Added `return;` so flow exits instead of proceeding to `CheckSkill`.

### Expected Impact

- Cartography no longer grants skill gain or uses a blank map when the player lacks map materials, regardless of where materials are stored.
- Camping kindling cannot be sourced from banked containers.
- Jewelry making no longer falls through to skill check when gold is insufficient.

---

## 5. Player Merchant - Hidden/Concealed Announce Guard

### Files Changed
| File | Change |
|------|--------|
| `pkg/systems/playervendor/playermerchant.src` | Added `who.hidden or who.concealed` guard in `AnnounceRecentStock` |

### Overview

`AnnounceRecentStock(who)` would previously announce new stock events even for players who are currently hidden or concealed. This could leak presence information.

### Notable Functional Changes

- Added `who.hidden or who.concealed` checks to the early-return guard in `AnnounceRecentStock`.
- If the controlling player is hidden or concealed, the recent stock announcement is suppressed.

### Expected Impact

- Hidden/concealed player merchants no longer announce stock updates, avoiding unintended position reveals.

---

## 6. Commit Timeline

| Commit | Date | Message |
|--------|------|---------|
| `8497a62` | 2026-06-03 | Talisman compile fix |
| `f30136a` | 2026-06-04 | Tamed following fix for under tower overhangs |
| `1d47820` | 2026-06-25 | Artifact updates — added 2 new artifact items, fixed using resources from banks, fix cartography errors on use and skill gain while in bank |
