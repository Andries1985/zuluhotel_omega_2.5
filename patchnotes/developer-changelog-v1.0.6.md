# Developer Changelog - v1.0.6
**Range:** `9b695eb` (origin/Patch-1.0.5) -> `9b8a144` (HEAD)  
**Branch:** Patch-1.0.6  
**Date:** 2026-06-03 -> 2026-07-01  
**Commits in range:** 17 (excluding merge commits)  
**Files changed:** 39 | +1445 / -181

---

## Table of Contents

1. [Talisman Compile Fix](#1-talisman-compile-fix)
2. [Tamed Following Fix - Multi Overhang Cleanup](#2-tamed-following-fix---multi-overhang-cleanup)
3. [Artifact System - Champion Relics and Expiry Sweeper](#3-artifact-system---champion-relics-and-expiry-sweeper)
4. [Bank Resource Usage Fix - Cartography, Camping, Blacksmithy](#4-bank-resource-usage-fix---cartography-camping-blacksmithy)
5. [Player Merchant - Hidden/Concealed Announce Guard](#5-player-merchant---hiddenconcealed-announce-guard)
6. [Commit Timeline](#6-commit-timeline)
7. [Runebook Overflow Fix and MoveObject Migration](#7-runebook-overflow-fix-and-moveobject-migration)
8. [High Priest Relationship Prompt, Dual Planar Gate Change, and Life Crystal Loot Expansion](#8-high-priest-relationship-prompt-dual-planar-gate-change-and-life-crystal-loot-expansion)
13. [Patchnotes and Launcher Copy Maintenance](#13-patchnotes-and-launcher-copy-maintenance)
10. [Omega Cache and Areas Refresh Fixes](#10-omega-cache-and-areas-refresh-fixes)
11. [INT Skill Advancement Updates](#11-int-skill-advancement-updates)
12. [Vanity Shop Bulk Options and Akill Staff Safety](#12-vanity-shop-bulk-options-and-akill-staff-safety)

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
| `pkg/opt/ArtifactSystem/artifactbox.src` | Added Champion Relic and World Gem objtype consts, decay timer assignment on pull, null guard |
| `pkg/opt/ArtifactSystem/championrelic.src` | New script — use handler for Champion Relics; spawns a champion boss and destroys relic |
| `pkg/opt/ArtifactSystem/itemdesc.cfg` | Added item entries for Champion Relic Tier 1 (`0x7991`), Tier 2 (`0x7992`), and World Gem (`0x7993`) |
| `pkg/opt/ArtifactSystem/artifact_daily_sweeper.src` | New script — daily daemon that calls `SweepExpiredArtifacts` |
| `pkg/opt/ArtifactSystem/artifact_startup_sweeper.src` | New script — one-shot startup sweep to catch any expired artifacts from before the feature existed |
| `pkg/opt/ArtifactSystem/worldgem.src` | New script — blesses targeted items and consumes the World Gem |
| `pkg/packethooks/megacliloc/itemdata.src` | Added artifact expiry countdown display, artifact color styling update, and `FormatDuration()` helper |
| `scripts/start.src` | Registered artifact sweeper daemons at startup |
| `.gitignore` | Minor update |
| `pkg/opt/ArtifactSystem/artifact-system-summary.txt` | Internal system documentation |

### Overview

Three artifact items have been added or updated in the Artifact System: Champion Relics (Tier 1 and Tier 2) and the World Gem. Champion Relics are obtained from the artifact box and allow players to summon a champion-level boss when activated. The World Gem is a targeted bless-use item. All three use the same 14-day artifact decay flow. A sweep system was also added to automatically remove expired relics from all storage areas.

### Notable Functional Changes

**Champion Relic Items:**
- `0x7991` — Champion Relic (Tier 1): `MagicItemLevel i10`, activates barracoon or erebuschaosgod.
- `0x7992` — Champion Relic (Tier 2): `MagicItemLevel i11`, activates rikktor or nyxseductress.
- Both use graphic `0x3CC1` and are tagged with `Artifact i1` CProp.
- `0x7993` — World Gem: `graphic 0x3DDA`, blesses a targeted item and is tagged with `Artifact i1` CProp.
- Artifact box itself is now movable (`Weight 5`, `Movable i1`).

**Artifact Box Distribution:**
- When a Champion Relic is pulled from the artifact box, it receives:
  - `decayat` set to `ReadGameClock() + 1209600` (14 days).
  - `#ArtifactExpireAt` property (same value) for sweeper reference.
  - `RelicUID` set to a random integer for uniqueness tracking.
- When a relic is recycled back into the artifact box, all expiry/uid props are erased so it restarts fresh on next pull.
- Added null guard: if artifact box is empty, `getItemArtifactBox()` returns `0` (no crash).
- World Gem also receives the 14-day decay timer when pulled from the artifact box.

**Champion Relic Use Script (`championrelic.src`):**
- Blocks activation in safe areas, guarded areas, and NOPK areas.
- Spawns a randomly chosen boss from the tier's template list near the activating player.
- Plays visual effect (`0x3728`) and sound (`0x01FE`) on boss spawn location.
- Destroys the relic on successful boss spawn.
- Returns graceful failure message if `CreateNpcFromTemplate` fails.

**World Gem Use Script (`worldgem.src`):**
- Prompts the user to target an item.
- Refuses invalid targets and already-blessed items.
- Sets the target's `newbie` flag to bless it.
- Consumes the gem after a successful bless.

**Expiry Sweeper:**
- `SweepExpiredArtifacts(scope)` iterates all storage areas and recursively checks containers for expired artifact items (where `expire_at <= now`). Destroys expired items and logs a summary.
- `artifact_daily_sweeper.src` runs this sweep on a daily loop.
- `artifact_startup_sweeper.src` runs a one-shot sweep on shard startup.
- Both sweepers registered in `scripts/start.src`.

**MegaCliloc Expiry Display:**
- Artifact items tagged with `#ArtifactExpireAt` now show a human-readable countdown in item tooltips.
- Artifact label color was changed to a highlighted orange style.
- Display: `"Artifact expires in Xd Xh Xm Xs"` (or `"Artifact expired, pending sweep"` if timer elapsed).
- `FormatDuration(total_seconds)` helper added to handle days/hours/minutes/seconds formatting.

### Expected Impact

- Champion Relics are now obtainable from artifact boxes and can be activated in the open world to summon a champion-class boss.
- World Gem can be used from the artifact box to bless a target item.
- Expired relics are swept from player storage/banks automatically.
- Players can inspect relic time-remaining from item tooltip.
- Artifact items now present with updated orange artifact styling in the tooltip.

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
| `1e993f0` | 2026-06-25 | Patchnotes update |
| `fd01eae` | 2026-06-25 | Fix for runebooks with full backpack; updated deprecated code |
| `d1f52d9` | 2026-06-30 | Added more life crystals to junk; High Priest relationship cost prompt; Dual Planar protection check removal |
| `5d7836f` | 2026-06-30 | Patch Notes Update |
| `b597f9b` | 2026-06-30 | Max Cache error |
| `fa8fa68` | 2026-06-30 | Artifact Fix |
| `e643e1c` | 2026-07-01 | Areas change; artifact box made movable |
| `369ee35` | 2026-07-01 | Try and move recall scrolls to feet when bag is full |
| `aa8d527` | 2026-07-01 | Scrolls kept on runebook insert if backpack is full; drop to ground |
| `490bdf6` | 2026-07-01 | INT skill gain updates |
| `3e3a029` | 2026-07-01 | Added relationship advice if in good standing |
| `34ef696` | 2026-07-01 | Added in world gem along with 2 week timer; changed color of artifact cliloc |
| `852ba1c` | 2026-07-01 | Updated patchnotes |
| `9b8a144` | 2026-07-01 | Added bulk options for transcription items; fixed `.akill` so it does not hit staff |

---

## 7. Runebook Overflow Fix and MoveObject Migration

### Files Changed
| File | Change |
|------|--------|
| `pkg/std/runebook/runeoninsert.src` | Reworked recall-scroll recharge overflow path: partial consume to free-charge amount; overflow return to backpack or ground; fallback destroys only excess scrolls with user message while recharge still succeeds |
| `pkg/std/runebook/runecaninsert.src` | Kept insertion gate compatible with overflow recharge behavior (does not block oversized stack insert when runebook has free charges) |
| `scripts/util/loot.inc` | Migrated deprecated `MoveItemToLocation` call to `MoveObjectToLocation` |
| `scripts/include/std.inc` | Updated movement helper wrappers to use `MoveObjectToLocation` instead of deprecated item-move API |
| `scripts/include/rituals.inc` | Migrated `MoveItemToLocation` calls to `MoveObjectToLocation` |
| `scripts/include/possess.inc` | Migrated `MoveItemToLocation` calls to `MoveObjectToLocation` |
| `scripts/include/chaoseffects.inc` | Migrated `MoveItemToLocation` call to `MoveObjectToLocation` |
| `pkg/std/housing/utility.inc` | Migrated fallback item placement from `MoveItemToLocation` to `MoveObjectToLocation` |

### Overview

Within the v1.0.6 range, a runebook edge case was fixed where inserting a recall-scroll stack larger than remaining book charges could fail in overflow-handling branches and leave bad outcomes when backpack/ground return paths were constrained. The recharge flow now guarantees successful charge application when at least one charge can be consumed, while handling overflow scrolls safely.

In the same hotfix pass, deprecated `MoveItemToLocation` script calls were migrated to `MoveObjectToLocation` in all touched files to align with modern POL API guidance and avoid reliance on removed/deprecated movement functions.

### Notable Functional Changes

**Runebook (`runeoninsert.src`):**
- `recharge_amount` is capped to `maxcharges - charges`.
- Script subtracts only `recharge_amount` from the inserted stack.
- Leftover scrolls (`scrolls.amount` after subtraction) are handled in this order:
  - `MoveItemToContainer(..., who.backpack)`
  - `MoveObjectToLocation(..., who.x, who.y, GetWorldHeight(...), who.realm, MOVEOBJECT_NORMAL)`
  - if both fail: `DestroyItem(scrolls)` and user-facing warning about destroyed excess scrolls.
- Recharge does not fail when overflow return fails; only excess scrolls are discarded.
- Charge state is still clamped to `maxcharges`, and revision increment is preserved.

**Move API migration:**
- All modified files now call `MoveObjectToLocation` for world-placement behavior.
- Helper code in `scripts/include/std.inc` remains backward-compatible at call site level (`MoveItem(...)` wrapper still exists), but internally routes through `MoveObjectToLocation`.

### Expected Impact

- Dragging an oversized recall-scroll stack onto a partially charged runebook now reliably recharges the runebook by available capacity.
- Overflow scroll behavior is deterministic under constrained inventory/terrain conditions:
  - return to backpack if possible;
  - else drop to ground if possible;
  - else destroy overflow with explicit player notification.
- No expected gameplay behavior changes from the movement API migration itself; changes are compatibility/maintenance-focused.

---

## 8. High Priest Relationship Prompt, Dual Planar Gate Change, and Life Crystal Loot Expansion

### Files Changed
| File | Change |
|------|--------|
| `scripts/ai/highpriest.src` | Added `relationship` speech-response path while priest is upset; reports exact donation requirement based on class level |
| `pkg/systems/combat/dualplanaronhit.src` | Removed (commented out) protection/immunity gate in Dual Planar on-hit flow |
| `config/nlootgroup.cfg` | Added `lifecrystal` to additional `Group Junk` entries |

### Overview

This pass added three gameplay-facing adjustments: better discoverability for High Priest relationship repair cost, a behavior change in Dual Planar on-hit immunity handling, and broader loot-table coverage for life crystals in junk-group rolls.

### Notable Functional Changes

**High Priest (`highpriest.src`):**
- When player has `PriestUpset`, priest now checks speech text for `relationship`/`Relationship`.
- Calculates donation requirement as `GetClasseLevel(player) * 2500`.
- If class level is unavailable/zero, priest provides fallback line: any donation can begin repair.
- If the relationship is already in good standing, priest now says so directly.
- Existing refusal lines remain for other upset-state speech.

**Dual Planar (`dualplanaronhit.src`):**
- `IsProtected(...)` immunity/cursed handling block has been disabled (commented).
- Effect path no longer early-exits on `IMMUNED` in this script block.
- Circle scaling from `CURSED` branch is no longer applied from that gate.

**Loot Group (`nlootgroup.cfg`):**
- `Item lifecrystal` added in four additional places under `Group Junk`.
- Increases junk-table opportunities to roll life crystals.

### Expected Impact

- Players can explicitly ask High Priest how much gold is needed to repair relationship status.
- Players now get a direct answer when the relationship is already in good standing.
- Dual Planar on-hit behavior is now more permissive against previously gated protected targets.
- Life crystals should appear more frequently from junk-category loot sources.

---

## 10. Omega Cache and Areas Refresh Fixes

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/omegacache/placecache.src` | Fixed lazy-init cache slot variable name so house Omega Cache counts initialize cleanly |
| `pkg/opt/areas/textcmd/admin/areas.src` | Added immediate online area-status refresh after `.areas` edits |

### Overview

These are maintenance/staff-facing fixes. The Omega Cache path now initializes cache counters without the previous variable naming issue, and the areas admin command now refreshes online characters immediately so safe/NOPK state changes apply without needing to leave and re-enter the region.

### Notable Functional Changes

- `placecache.src` now stores the initial cache count in `maxcache` instead of reusing a generic `max` local.
- `areas.src` now includes `:areas:include/areafunctions` and calls `RefreshOnlineAreaStatus()` after saving edited area data.
- `RefreshOnlineAreaStatus()` re-evaluates safe-area and NOPK state for online characters after area edits.

### Expected Impact

- Omega Cache values initialize correctly on houses/signs that had not yet been opened.
- Staff editing area data see the resulting status changes applied immediately to players already online.

---

## 11. INT Skill Advancement Updates

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/shilhook/skillsdef.cfg` | Rebalanced several `IntAdv` and a few related advancement values across skill definitions |

### Overview

The skill definition file received a broad tuning pass on advancement values, especially for intelligence-based skills.

### Notable Functional Changes

- Several INT-linked skill advancement profiles were increased or adjusted, including skills such as Alchemy, Item Identification, Peacemaking, Inscription, Evaluating Intelligence, Tracking, Veterinary, Magery-related skills, and others.
- A few non-INT advancement lines were also cleaned up or retuned alongside the intelligence pass.

### Expected Impact

- Skill gain pacing for affected skills is now different than before.
- This is a tuning pass, not a rules/system rewrite.

---

## 12. Vanity Shop Bulk Options and Akill Staff Safety

### Files Changed
| File | Change |
|------|--------|
| `pkg/opt/vanityshop/vanityshop.src` | Added bundle-capable vanity listings and purchase flow (single and multi-amount options) |
| `pkg/opt/powerscrolls/itemdesc.cfg` | Added `VanityCost` to item `0x9A89` to enable vanity shop listing |
| `pkg/opt/vanityshop/customitemdye.src` | Restricted dye target scope to items in player backpack |
| `pkg/opt/vanityshop/customitemname.src` | Restricted rename target scope to items in player backpack |
| `scripts/textcmd/admin/akill.src` | Added command-level guard so `.akill` does not kill staff mobiles |

### Overview

This update expands vanity shop purchasing behavior with bulk options for transcription-related items and tightens target-scope safety for custom vanity tools. It also hardens the `.akill` admin command to avoid affecting staff characters.

### Notable Functional Changes

**Vanity Shop (`vanityshop.src`):**
- Introduced `AddVanityItem(...)` helper with explicit `Amount` support.
- Added multi-quantity listing/purchase paths (including bundled amounts) for selected entries.
- Bundle purchases create items in a temporary container then move into backpack, with explicit failure handling when space is insufficient.
- Expanded debug prints around purchase flow and token validation.

**Transcription-related vanity availability:**
- Added `VanityCost 2` to `0x9A89` in powerscroll itemdesc so it participates in vanity item config parsing.

**Custom vanity tool scope:**
- Dye and rename scripts now require target item to be in `who.backpack`.
- Invalid scope exits cleanly with a user message and item releases.

**Admin command safety (`akill.src`):**
- Added `!mob.cmdlevel` condition to all nearby-mobile kill loops.
- Prevents `.akill` from killing staff accounts while preserving intended non-staff kill behavior.

### Expected Impact

- Players can buy selected transcription vanity items in bundles instead of only single-item purchases.
- Vanity rename/dye interactions are now constrained to backpack-contained items for safer targeting.
- Staff are protected from accidental `.akill` collateral in operational use.

---

## 13. Patchnotes and Launcher Copy Maintenance

### Files Changed
| File | Change |
|------|--------|
| `patchnotes/developer-changelog-v1.0.6.md` | Expanded dev changelog content during patchnote update commit sequence |
| `patchnotes/patch-v1.0.6.md` | Player-facing patch notes updated with new gameplay changes |
| `patchnotes/launchernotes.md` | Launcher copy synchronized with player-impact notes |

### Overview

Patchnotes were updated in-range to keep player-facing and developer-facing notes aligned with all delivered v1.0.6 work, including later hotfixes and follow-up gameplay adjustments.

### Notable Functional Changes

- Added/expanded v1.0.6 sections for runebook overflow handling and compatibility migration notes.
- Synchronized launcher copy to the same player-impact bullets used in patch notes.
- Preserved the standard launcher structure: `Latest Changes` header, Discord disclaimer line, `## What Changed`, summary, closing thanks line.

### Expected Impact

- No gameplay behavior change directly from this section.
- Reduces drift between in-game changes and published communication artifacts.
