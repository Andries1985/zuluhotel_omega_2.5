# Developer Changelog - v1.1.2
**Range:** `7107d6d` (HEAD of v1.1.1) -> `8bb55cb` (HEAD)  
**Branch:** Patch-1.1.2  
**Date:** 2026-08-08 -> 2026-08-11  
**Commits in range:** 6 non-boundary commits (`29962db`, `4ce28ea`, `25b0b3b`, `37cf238`, `bc79410`, `8bb55cb`); 2 additional merge commits (`8386eef`, `316c467`) carry no new content; `9e36189` (this release's own patchnotes commit) is a boundary commit, not counted  
**Files changed (committed):** 30 (+1514 / -111), including this release's own 3 `patchnotes/` files

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Vendor Training - Magic Resistance Now Teachable](#3-vendor-training---magic-resistance-now-teachable)
4. [Stability - Defensive Nil/Error Guards From Live Log Review](#4-stability---defensive-nilerror-guards-from-live-log-review)
5. [FileAccess - AllowRemote Enabled for Housing/PlayerVendor Escrow Logs](#5-fileaccess---allowremote-enabled-for-housingplayervendor-escrow-logs)
6. [Command-Level Corrections and Script Housekeeping](#6-command-level-corrections-and-script-housekeeping)
7. [New Staff Tool - .memdump Command and Standalone Memory-Usage Log Analyzers](#7-new-staff-tool---memdump-command-and-standalone-memory-usage-log-analyzers)
8. [Command Fix - .ph Now Reports Personal Powerhour Status](#8-command-fix---ph-now-reports-personal-powerhour-status)
9. [Exhaustive File-by-File Change List](#9-exhaustive-file-by-file-change-list)
10. [Risk and Regression Notes](#10-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.2 is a small maintenance patch, five commits over two days. `29962db` is nominally "Patchnotes for Patch 1.1.1" but also bundles a genuine fix - `AllowRemote 1` added to the housing/playervendor `.log` FileAccess grants - that postdates 1.1.1's own changelog range and is therefore covered here (section 5). `4ce28ea` ("Minor fixes from the logs") adds defensive nil/error guards across eleven files, all triggered by specific errors observed in the live server logs. `25b0b3b` ("Fixes for commands") corrects two migration commands' staff level, moves them out of the `admin` command folder, and strips leftover debug `Print()` calls from the 1.1.1 house-escrow system. `37cf238` ("Memory Dump Checks") adds a new staff diagnostic command plus two standalone (non-EScript) log-analysis scripts. `bc79410` ("Magic resistence training fix") is the fix requested by a player report: no vendor anywhere on the shard had Magic Resistance in its base skill list, so "vendor train" could never offer it regardless of which vendor type was asked. `8bb55cb` ("ph command fix"), landing after this release's own patch-notes commit (`9e36189`) and folded back into this document (section 8), ports a `.ph` improvement already shipped on ZH3.0 so the command also reports personal-powerhour status, not just server-wide.

---

## 2. Commit Timeline

| Hash | Date | Message |
|---|---|---|
| `7107d6d` | 2026-08-08 | (v1.1.1 HEAD - range start) |
| `8386eef` | 2026-08-08 | Merge pull request #317 from Andries1985/Patch-1.1.1 (boundary - no new content) |
| `316c467` | 2026-08-08 | Merge pull request #318 from Andries1985/Test-Shard (boundary - brings in already-released Patch-1.1.0 merge commits, no new content) |
| `29962db` | 2026-08-08 | Patchnotes for Patch 1.1.1 (adds 1.1.1's docs; also bundles the FileAccess `AllowRemote` fix, section 5) |
| `4ce28ea` | 2026-08-08 | Minor fixes from the logs |
| `25b0b3b` | 2026-08-08 | Fixes for commands |
| `37cf238` | 2026-08-10 | Memory Dump Checks |
| `bc79410` | 2026-08-10 | Magic resistence training fix |
| `9e36189` | 2026-08-10 | Patch Notes (boundary - this release's own v1.1.2 docs; superseded by this update) |
| `8bb55cb` | 2026-08-11 | ph command fix |

---

## 3. Vendor Training - Magic Resistance Now Teachable

### Files Changed

| File | Change |
|---|---|
| `config/npcdesc.cfg` | Adds `MagicResistance 100` to the `mage`, `alchemist`, and `scribe` NPC templates |

### Overview

A player reported that "vendor train" never offered Magic Resistance from any vendor - not mage, not scribe, not alchemist. `MerchantTrain()` in `scripts/ai/merchant.src` only lists a skill as trainable if the NPC's own base skill value for it (`GetBaseSkillBaseValue(me, i)`) is non-zero, which in turn only exists if the NPC's template defines that skill line at spawn. A repo-wide search of every `MagicResistance` line in `npcdesc.cfg` found it only on six combat-monster templates (`scourgegladiator`, `scourgecommander`, `titanwarmaster`, `plague`, `pestilence`, `scourgebattlemaster`) - none of which use `script merchant`. No vendor template anywhere in the file had it.

### Notable Functional Changes

- `mage` template: adds `MagicResistance 100` alongside its existing `evaluatingintelligence`, `magery`, `SpiritSpeak`, `meditation` lines (100 matches the value already used for those).
- `alchemist` template: adds `MagicResistance 100` alongside `alchemy`, `evaluatingintelligence`.
- `scribe` template: adds `MagicResistance 100` alongside `itemid`, `evaluatingintelligence`, `inscription`, `spiritspeak`, `meditation`.

### Expected Impact

Newly spawned Mage, Alchemist, and Scribe vendors now offer Magic Resistance training (up to 33.3 skill, per the `NPC_level/3` cap in `MerchantTrain()`) via "vendor train". Vendors already spawned before this patch keep their existing base skills until they respawn or the server reloads `npcdesc.cfg` and re-templates them - not retroactively updated in place.

---

## 4. Stability - Defensive Nil/Error Guards From Live Log Review

### Files Changed

| File | Change |
|---|---|
| `pkg/items/armor/include/armorZones.inc` | `CS_GetLayersInArmorZone()`, `CS_GetEquipmentInArmorZone()`, `CS_GetEffectiveArmor()` - guard against missing zone/item config entries |
| `pkg/opt/powerscrolls/textcmd/player/showcaps.src` | `skillwindow()` - guard against an empty `classe` before the `GetObjProperty()` lookup |
| `pkg/opt/spawnpoint/checkpoint.src` | `ValidatePointSpawns()` - treat an `error`-valued spawned-objects property the same as missing |
| `pkg/opt/vanityshop/mountstone.src` | `MountStone()`, `CreateMount()`, `StoreMount()` - guard `error`-valued `Owner`/`DMountSerial`/`DMountStoneSerial` properties and missing owner/mount objects |
| `pkg/opt/versebook/Bardic_Boulders.src` | `HitWithBoulder()` - guard against `CreateItemAtLocation()` returning nothing |
| `pkg/std/boat/boat.src` | `DoEncounter()` - bail out if `boat` or `boat.realm` is unset |
| `pkg/std/boat/plankutil.inc` | `IsPlankOccupied()` - bail out if `plank` or `plank.realm` is unset |
| `pkg/std/boat/plankwalk.src` | `plankwalk()` - pass `who.realm`/`plank.realm` explicitly to `MoveObjectToLocation()`/`ListMobilesNearLocation()` instead of relying on defaults |
| `pkg/std/traps/traps.src` | `open_trapped_item()` - skip the crafter lookup when `trapcrafter` is the literal string `"Spawnpoint"` |
| `scripts/ai/chaosmultikillpcs.src` | `MakeLord()`, `SplitLord()` - pass `"britannia"` explicitly to two `MoveObjectToLocation()` calls that previously omitted the realm argument |
| `scripts/control/firecontrol.src` | `field_control()` - break out of the damage loop if `item.realm` becomes unset |
| `scripts/misc/death.src` | `npcdeath()`, `RegisterNPC()` - guard `error`-valued `master`/`KilledBySerial` properties and missing master objects |

### Overview

Eleven files, one fix pattern: each was throwing (or silently misbehaving) on a code path that assumed an obj-property lookup, a config-file lookup, or a freshly-created/located object reference would always succeed. The commit message ties this directly to specific errors seen in the live server logs rather than a general audit - `death.src`'s `RegisterNPC()`/heart-creation path and `traps.src`'s spawnpoint-chest trap message were the most concretely motivated (see inline comment added in `traps.src`), the rest are the same class of defensive fix applied to nearby call sites that shared the same risk.

### Notable Functional Changes

- `armorZones.inc`: all three functions now check their `zone_cfg`/`itemdesc_cfg` lookup against `error` before using it, returning an empty array/`0` instead of erroring when an item or zone isn't in config.
- `showcaps.src`: `level` now defaults to `0` and the `GetObjProperty()` call is skipped entirely when `classe` is falsy, instead of calling `GetObjProperty(who, classe)` with an empty key.
- `checkpoint.src`: `serials` is now reset to `{}` when the stored property is either missing *or* `error`-valued (previously only checked falsy).
- `mountstone.src`: `Owner`, `DMountSerial`, and `DMountStoneSerial` obj-properties are each normalized to `0` if `error`-valued before use; `StoreMount()` now returns early if the resolved mount or original-stone item can't be found, instead of dereferencing a nil object.
- `Bardic_Boulders.src`: the entire boulder-placement/cleanup block is now skipped if `CreateItemAtLocation()` returns nothing, instead of immediately reading `.z` off it.
- `boat.src` / `plankutil.inc`: both bail out early if the boat/plank reference or its `.realm` is unset, before calling `ListMobilesNearLocation*()`/`GetMapInfo()`.
- `plankwalk.src`: two `MoveObjectToLocation()`/`ListMobilesNearLocation()` calls that previously omitted the realm argument (defaulting to the current/default realm) now pass `who.realm`/`plank.realm` explicitly, matching the third call in the same function that already did.
- `traps.src`: spawnpoint-placed trapped chests store the literal string `"Spawnpoint"` in the `trapped_by` obj-property (see `checkpoint.src`'s `PROPID_CHEST_TRAPPED_BY`) rather than a crafter serial. `open_trapped_item()` previously tried to `SystemFindObjectBySerial()` that string as if it were a serial; it's now explicitly excluded from the crafter-lookup/notify branch, with a comment explaining why.
- `chaosmultikillpcs.src`: `MoveObjectToLocation(mergewith, 5376, 1081, 0, 0)` and the equivalent call for `me` in `SplitLord()` previously passed `0` where a realm string is expected; both now pass `"britannia"` explicitly.
- `firecontrol.src`: `field_control()`'s damage-tick loop now breaks if `item.realm` becomes unset mid-loop (e.g. the field item was destroyed/moved off-realm), instead of calling `ListMobilesNearLocation()` with a stale/invalid realm every tick until `item.x` itself goes falsy.
- `death.src`: `npcdeath()` now resolves `mastersrl` and `masterobj` once up front, normalizing an `error`-valued `master` property to `0` and skipping the "Heart of the Beast" backpack placement/sysmessage and the disintegration message entirely if no master object can be resolved (previously called `.backpack` and `SendSysMessage()` directly on whatever `SystemFindObjectBySerial()` returned, including a nil result). The `IsInSafeArea(corpse)` item-recovery branch is now also gated on `masterobj` being resolved. `RegisterNPC()` similarly normalizes an `error`-valued `KilledBySerial` to `0` before checking it.

### Expected Impact

No new features and no intended change to normal-case behavior - every fix is guarding a path that would previously error out (visible as a server-log error, and in several cases as a script simply failing partway through, e.g. a tamed-pet death not producing a Heart, or a boat encounter/plank check throwing instead of running) when it hit one of these edge cases. Should reduce the frequency of the specific errors these were pulled from in the logs.

---

## 5. FileAccess - AllowRemote Enabled for Housing/PlayerVendor Escrow Logs

### Files Changed

| File | Change |
|---|---|
| `config/fileaccess.cfg` | Adds `AllowRemote 1` to the `housing` and `playervendor` package `.log` FileAccess blocks |

### Overview

1.1.1 added `HouseEscrowLog()` and `MerchantEscrowLog()` (sections 10-11 of the v1.1.1 changelog), which write to package-prefixed paths (`::log/houseescrow.log`, `::log/merchantescrow.log`) outside each package's own directory tree. Per `fileaccess.cfg`'s documented semantics, a package-prefixed/remote file path only resolves if the granting `FileAccess` block has `AllowRemote 1` set - the `.log` grants added alongside those two functions in 1.1.1 did not set it. This fix was bundled into the same commit as the 1.1.1 patch-notes files (`29962db`), after the 1.1.1 changelog's own commit range had already closed, so it's documented here instead.

### Notable Functional Changes

- Both the `housing` and `playervendor` `.log`-extension `FileAccess` blocks in `config/fileaccess.cfg` gain `AllowRemote 1`.

### Expected Impact

`HouseEscrowLog()` and `MerchantEscrowLog()` calls can now actually write to their target log files under `::log/`. Staff-facing only - no player-visible change, but the house-escrow and player-vendor-escrow audit logs referenced in 1.1.1's documentation should now actually be populated going forward (any 1.1.1-era calls between that release and this fix landing may not have written successfully).

---

## 6. Command-Level Corrections and Script Housekeeping

### Files Changed

| File | Change |
|---|---|
| `config/command_synopses.cfg` | `backfillteleporterserials`, `migrateclassfirsts` - `Level Administrator`/`CmdLevel 4` -> `Level Developer`/`CmdLevel 5` |
| `scripts/textcmd/admin/backfillteleporterserials.src` -> `scripts/textcmd/test/backfillteleporterserials.src` | Renamed, no content change |
| `scripts/textcmd/admin/migrateclassfirsts.src` -> `scripts/textcmd/test/migrateclassfirsts.src` | Renamed, no content change |
| `pkg/opt/classfirsts/pkg.cfg` | Comment updated to reference the new `test/migrateclassfirsts.src` path |
| `pkg/std/housing/houseescrow.inc` | Removes leftover `[houseescrow][debug]`/`[houseescrow][ERROR]` `Print()` calls (12 call sites) |
| `pkg/std/housing/sign.src` | Removes the `[houseescrow][audit]` `Print()` call before `DemolishAndEscrowHouse()` |
| `scripts/textcmd/player/houseescrow.src` | Removes 4 `[houseescrow][debug]` `Print()` calls |

### Overview

Two related cleanups bundled into one commit: (1) the two one-time migration commands added in 1.1.1 (`.backfillteleporterserials`, `.migrateclassfirsts`) were registered at `Administrator`/`CmdLevel 4` and living in the `admin` command folder despite being one-shot, rarely-rerun migration tools rather than routine admin commands - both are now `Developer`/`CmdLevel 5` and moved into `test`; and (2) the console-only `Print()` debug logging added throughout 1.1.1's new house-escrow system is removed now that `HouseEscrowLog()` (section 5) is confirmed to actually persist to its log file, leaving the persistent log as the sole record instead of duplicating everything to console.

### Notable Functional Changes

- `command_synopses.cfg` regenerated (`pythonscripts/_gen_command_synopses_cfg.py`) to reflect the two commands' new level/folder.
- File renames are pure moves (`git show` reports 100% similarity, no content diff) - existing references were already package-relative and don't need updating beyond `pkg.cfg`'s comment.
- `houseescrow.inc`/`sign.src`/`houseescrow.src`: every removed `Print()` call had a corresponding `HouseEscrowLog()` call carrying the same (or more detailed) information already in place - none of the removed lines were the only record of that event.

### Expected Impact

Both migration commands now require the shard's top staff level instead of Administrator, matching how they're actually used (one-time, developer-run). No player-visible change. House-escrow operations stop double-logging to console; the persistent `::log/houseescrow.log` (now reliably written per section 5) remains the record of truth.

---

## 7. New Staff Tool - .memdump Command and Standalone Memory-Usage Log Analyzers

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/alryc/textcmd/test/memdump.src` | New - `.memdump` |
| `config/command_synopses.cfg` | New `memdump` entry (Developer, CmdLevel 5) |
| `pythonscripts/analyze_memory_usage.py` | New (286 lines) - standalone log parser/report tool |
| `pythonscripts/Analyze-MemoryUsage.ps1` | New (199 lines) - PowerShell equivalent of the above |

### Overview

A new staff diagnostic command plus two standalone (run outside the game server) scripts for investigating script memory usage, in support of ongoing shard-stability work.

### Notable Functional Changes

- `.memdump` (Developer, CmdLevel 5): calls `POLCore().internal(2)` and `POLCore().internal(5)` - engine-internal diagnostic dumps (script memory usage, among other things) written to the server directory, per the command's own sysmessage/console output. No parameters.
- `analyze_memory_usage.py` / `Analyze-MemoryUsage.ps1`: both parse `memoryusagescripts.log` (the file `.memdump`'s `internal(2)`/`internal(5)` calls produce) into per-script memory entries and per-snapshot totals, and report top-N consumers; the Python version also supports CSV export. Not part of the EScript codebase - run directly via `python`/`pwsh` against a log file pulled from the server.

### Expected Impact

Staff/developer-facing only. No player-visible change. Gives the team a repeatable way to trigger and then analyze script memory-usage snapshots when investigating memory-related stability issues.

---

## 8. Command Fix - .ph Now Reports Personal Powerhour Status

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/powerhour/textcmd/player/ph.src` | Rewritten - adds personal-powerhour reporting; server-wide reporting unchanged in substance |

### Overview

`.ph` previously only reported server-wide powerhour status (active type, or countdown to the next chance at one); it had no visibility into a player's own personal powerhour (`.setph`), even though `.setph` itself already reported remaining time when a personal PH was active. Ported from the equivalent fix already shipped on ZH3.0. `checkPH()` now also reads the same `pph_use_time`/`pph_use_weekday`/`#PPHH`/`#PPHC`/`#PPHS` obj-properties `setph.src` uses, and mirrors `setph.src`'s eligibility check (`weekday_now < use_weekday || time_now > use_time_nextweek`) to report either the active personal PH's remaining time or the countdown until the player is next eligible to start one via `.setph`.

### Notable Functional Changes

- Adds `use math;` for `Floor()`, used in the weekday/reset-time math (previously only `use uo;`).
- New personal-status block, printed before the (retained) server-wide block: if any of `#PPHH`/`#PPHC`/`#PPHS` is set, reports which personal PH is active and its remaining minutes (same `(use_time + hour - time_now) / minute` calculation `setph.src` uses). Otherwise, reports the player has no personal PH active, then either "you can start one now" or a days/hours/minutes countdown to the next eligible day, computed by finding the next weekday-0 (Sunday) boundary on/after `use_time` - the same reset point implied by `setph.src`'s eligibility check.
- Server-wide messages reworded for clarity now that personal-PH messages exist alongside them ("Hunting powerhour is currently active!" -> "A server-wide Hunting powerhour is currently active!", and adds an explicit "No server-wide powerhour is currently active." message in the else branch instead of only printing the countdown).
- `POLCORE().systime` is now read once into `time_now` up front and reused for both the personal and server-wide countdown math, instead of being read a second time (as `curTime`) partway through the program.

### Expected Impact

Player-facing: `.ph` now tells you your own personal powerhour's remaining time (if active) or when you're next eligible to start one via `.setph`, in addition to the server-wide status it already reported. No change to `.setph` itself or to how either type of powerhour is granted/expires.

---

## 9. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `config/command_synopses.cfg` | 6, 7 | Migration commands re-leveled; new `memdump` entry |
| `config/fileaccess.cfg` | 5 | `AllowRemote 1` added for housing/playervendor `.log` |
| `config/npcdesc.cfg` | 3 | `MagicResistance 100` added to mage/alchemist/scribe |
| `pkg/items/armor/include/armorZones.inc` | 4 | Nil-guards for zone/item config lookups |
| `pkg/opt/alryc/textcmd/test/memdump.src` | 7 | New - `.memdump` |
| `pkg/opt/classfirsts/pkg.cfg` | 6 | Comment path update |
| `pkg/opt/powerhour/textcmd/player/ph.src` | 8 | Adds personal-powerhour status reporting |
| `pkg/opt/powerscrolls/textcmd/player/showcaps.src` | 4 | Nil-guard for empty `classe` |
| `pkg/opt/spawnpoint/checkpoint.src` | 4 | `error`-value guard for spawned-objects property |
| `pkg/opt/vanityshop/mountstone.src` | 4 | `error`-value/nil guards, owner/mount lookups |
| `pkg/opt/versebook/Bardic_Boulders.src` | 4 | Nil-guard for `CreateItemAtLocation()` result |
| `pkg/std/boat/boat.src` | 4 | Realm-unset guard in `DoEncounter()` |
| `pkg/std/boat/plankutil.inc` | 4 | Realm-unset guard in `IsPlankOccupied()` |
| `pkg/std/boat/plankwalk.src` | 4 | Explicit realm args to movement/lookup calls |
| `pkg/std/housing/houseescrow.inc` | 6 | Debug `Print()` calls removed |
| `pkg/std/housing/sign.src` | 6 | Debug `Print()` call removed |
| `pkg/std/traps/traps.src` | 4 | `"Spawnpoint"` trapcrafter guard |
| `pythonscripts/Analyze-MemoryUsage.ps1` | 7 | New - standalone log analyzer (PowerShell) |
| `pythonscripts/analyze_memory_usage.py` | 7 | New - standalone log analyzer (Python) |
| `scripts/ai/chaosmultikillpcs.src` | 4 | Explicit realm arg on two `MoveObjectToLocation()` calls |
| `scripts/control/firecontrol.src` | 4 | Realm-unset guard in damage-tick loop |
| `scripts/misc/death.src` | 4 | `error`-value/nil guards for master/killer lookups |
| `scripts/textcmd/player/houseescrow.src` | 6 | Debug `Print()` calls removed |
| `scripts/textcmd/test/backfillteleporterserials.src` | 6 | Moved from `admin/`, re-leveled |
| `scripts/textcmd/test/migrateclassfirsts.src` | 6 | Moved from `admin/`, re-leveled |
| `patchnotes/developer-changelog-v1.1.2.md` | - | This file |
| `patchnotes/patch-v1.1.2.md` | - | Player-facing notes |
| `patchnotes/launchernotes.md` | - | Replaced with this release's player-facing content |

---

## 10. Risk and Regression Notes

- **`.ph` personal-powerhour reporting (section 8):** read-only reporting change - it does not touch how personal or server-wide powerhours are granted, tracked, or expired (still entirely owned by `setph.src`/`activateph()`). Worth spot-checking the eligibility-countdown branch against a live `#PPHH`/`#PPHC`/`#PPHS` obj-property state once, since the reset-time math (next Sunday on/after `use_time`) is inferred from `setph.src`'s existing condition rather than a separate stored "next eligible" timestamp.
- **Magic Resistance vendor training (section 3):** only affects vendors spawned/re-templated after this patch deploys - existing live Mage/Alchemist/Scribe vendors keep their current base skills until they respawn or the server reloads and re-applies `npcdesc.cfg`.
- **`plankwalk.src` explicit realm arguments (section 4):** previously-implicit realm defaults are now explicit `who.realm`/`plank.realm` - should be behaviorally identical for any plank/player pair that was already on the same realm (the normal case), but worth watching for cross-realm boat/plank edge cases specifically, since that's the scenario this class of fix targets.
- **House-escrow log de-duplication (section 6):** console `Print()` output for house-escrow operations is gone; `::log/houseescrow.log` and `::log/merchantescrow.log` (via `HouseEscrowLog()`/`MerchantEscrowLog()`) are now the only record, and depend on the `AllowRemote 1` fix in section 5 actually being in effect - confirm those log files are being written to post-deploy before relying on them for any incident investigation.
- **`.memdump` (section 7):** triggers engine-internal diagnostic dumps on demand; no rate-limiting or size cap on the resulting `memoryusagescripts.log` file is implemented in this patch - fine for occasional developer use, worth keeping in mind if it's ever scripted/automated.
