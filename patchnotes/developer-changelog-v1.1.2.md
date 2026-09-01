# Developer Changelog - v1.1.2
**Range:** `7107d6d` (HEAD of v1.1.1) -> `97a7dcd` (HEAD)  
**Branch:** Patch-1.1.2  
**Date:** 2026-08-08 -> 2026-09-01  
**Commits in range:** 16 non-boundary commits (`29962db`, `4ce28ea`, `25b0b3b`, `37cf238`, `bc79410`, `8bb55cb`, `a18c2e7`, `43261fe`, `88720ac`, `1ae01df`, `e3980f7`, `5aa5f81`, `e6a6be9`, `9e06736`, `9033a5f`, `97a7dcd`); `d0220a8` is folded into section 14 alongside `e6a6be9` (same file, same topic); 2 merge commits (`8386eef`, `316c467`) carry no new content; `9e36189`, `f6c4bd5`, `9c354ed` (this release's own prior patchnotes commits) are boundary commits, not counted  
**Files changed (committed):** 62 (+1969 / -199), including this release's own 3 `patchnotes/` files

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
9. [New GM Tool - .resetph Command](#9-new-gm-tool---resetph-command)
10. [New Item - Eon-Prism](#10-new-item---eon-prism)
11. [Housing - Sign Owner Name Resolved Live Instead of Cached](#11-housing---sign-owner-name-resolved-live-instead-of-cached)
12. [Character Death - Clearer Full-Backpack Message](#12-character-death---clearer-full-backpack-message)
13. [Healing & Veterinary - Dead-Patient Handling Now Stops Correctly](#13-healing--veterinary---dead-patient-handling-now-stops-correctly)
14. [Area-Policy System - Cache Simplification and Datastore-Save Crash Fix](#14-area-policy-system---cache-simplification-and-datastore-save-crash-fix)
15. [Corpse Decay - Existence-Check Reliability (Three-Pass Fix)](#15-corpse-decay---existence-check-reliability-three-pass-fix)
16. [Additional Defensive Guards From Live Shard Log Review, Round 2](#16-additional-defensive-guards-from-live-shard-log-review-round-2)
17. [Exhaustive File-by-File Change List](#17-exhaustive-file-by-file-change-list)
18. [Risk and Regression Notes](#18-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.2 grew from a small maintenance patch into a longer-running stability branch, sixteen non-boundary commits over three and a half weeks. `29962db` is nominally "Patchnotes for Patch 1.1.1" but also bundles a genuine fix - `AllowRemote 1` added to the housing/playervendor `.log` FileAccess grants - that postdates 1.1.1's own changelog range and is therefore covered here (section 5). `4ce28ea` ("Minor fixes from the logs") adds defensive nil/error guards across eleven files, all triggered by specific errors observed in the live server logs. `25b0b3b` ("Fixes for commands") corrects two migration commands' staff level, moves them out of the `admin` command folder, and strips leftover debug `Print()` calls from the 1.1.1 house-escrow system. `37cf238` ("Memory Dump Checks") adds a new staff diagnostic command plus two standalone (non-EScript) log-analysis scripts. `bc79410` ("Magic resistence training fix") is the fix requested by a player report: no vendor anywhere on the shard had Magic Resistance in its base skill list, so "vendor train" could never offer it regardless of which vendor type was asked. `8bb55cb` ("ph command fix"), landing after this release's own patch-notes commit (`9e36189`) and folded back into this document (section 8), ports a `.ph` improvement already shipped on ZH3.0 so the command also reports personal-powerhour status, not just server-wide.

After the first patch-notes pass (`f6c4bd5`, `9c354ed`), a second wave of fixes landed. `a18c2e7` ("sign name update", section 11) fixes house signs showing a stale cached owner name. `43261fe` ("Quick Eon Fix") and `88720ac` ("Eon Prism Update") are both folded into section 10's Eon-Prism description rather than given their own sections, since they refine a feature this same document already covers in full - `43261fe` added the decay/artifact-box integration section 10 describes, and `88720ac` changed the item's graphic from the originally-documented `0x2f57` to `0x1f1c` with `color 1182`. `1ae01df` ("memdump update") is similarly folded into section 7, expanding `.memdump` to also trigger an `internal(6)` killpcs AI-memory dump. `e3980f7` ("Corpse Decay Fix") and `5aa5f81` ("corpsedecay fix") are the first two passes of a three-pass fix to NPC corpse-decay reliability, completed by this session's own `97a7dcd` (section 15). `e6a6be9` ("Area policy memory update") simplifies the area-policy mask/line cache down to a single shared cache layer (section 14), and turned out to also fix a latent invalidation bug in the process.

A live-shard log review (this session, 2026-09-01) then found the area-policy datastore save failure actually crippling the running shard's periodic saves: a stale mobile reference's `.realm` access degrading to an `error` value, which `SanitizePolicyRealm()` didn't reject before it got stringified into a datastore filename. That fix (`d0220a8`) is folded into section 14 alongside `e6a6be9`, since both are the same function in the same file. The same log review turned up a much larger set of "typed native call fed an unguarded error/wrong-type value" bugs across the wider codebase; `9033a5f` fixes the ones directly visible in the crash-log data (death.src, guilds.src, boat.src, traps.src), and a follow-up code review of the whole branch (section 16, `97a7dcd`) found and fixed six more: `mountstone.src`'s new crash-guards not being checked by their caller, the corpse-decay guard's own raw-`.serial`-access risk, the `serials==error` fix from `4ce28ea` not being applied to five sibling files, the four Donator mount-stone forks never receiving `mountstone.src`'s guard, `healing.src`'s dead-patient fix (`9e06736`, section 13) not being ported to `vet.src`'s equivalent functions, and several more `"master"` obj-property call sites that fed an unguarded value into `SystemFindObjectBySerial`/`GetSkill`/`SendSysmessage`.

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
| `f6c4bd5` | 2026-08-11 | Patchnotes update (boundary) |
| `9c354ed` | 2026-08-11 | resetph command eon-prism artifact patchnotes update (boundary) |
| `a18c2e7` | 2026-08-11 | sign name update (section 11) |
| `43261fe` | 2026-08-11 | Quick Eon Fix (folded into section 10) |
| `88720ac` | 2026-08-11 | Eon Prism Update (folded into section 10) |
| `1ae01df` | 2026-08-12 | memdump update (folded into section 7) |
| `e3980f7` | 2026-08-12 | Corpse Decay Fix (pass 1 of 3, section 15) |
| `5aa5f81` | 2026-08-12 | corpsedecay fix (pass 2 of 3, section 15) |
| `e6a6be9` | 2026-08-12 | Area policy memory update (section 14) |
| `9e06736` | 2026-08-19 | Healing and Chrdeath update (sections 12, 13) |
| `d0220a8` | 2026-09-01 | Guard against stale who.realm crashing area-policy datastore saves (folded into section 14) |
| `9033a5f` | 2026-09-01 | Guard remaining unguarded serial/property lookups from the crash logs (section 16) |
| `97a7dcd` | 2026-09-01 | Fix correctness findings 1-6 from the Patch-1.1.2 code review (pass 3 of 3 for section 15; section 16) |

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
| `pkg/opt/alryc/textcmd/test/memdump.src` | New - `.memdump`; later updated (`1ae01df`) to add an `internal(6)` killpcs dump |
| `config/command_synopses.cfg` | New `memdump` entry (Developer, CmdLevel 5); synopsis text updated alongside the `1ae01df` change |
| `pythonscripts/analyze_memory_usage.py` | New (286 lines) - standalone log parser/report tool |
| `pythonscripts/Analyze-MemoryUsage.ps1` | New (199 lines) - PowerShell equivalent of the above |

### Overview

A new staff diagnostic command plus two standalone (run outside the game server) scripts for investigating script memory usage, in support of ongoing shard-stability work. `1ae01df` ("memdump update", 2026-08-12) later expanded the command to also dump killpcs AI memory usage.

### Notable Functional Changes

- `.memdump` (Developer, CmdLevel 5): calls `POLCore().internal(2)` and `POLCore().internal(5)` - engine-internal diagnostic dumps (script memory usage, among other things) written to the server directory, per the command's own sysmessage/console output. No parameters.
- `analyze_memory_usage.py` / `Analyze-MemoryUsage.ps1`: both parse `memoryusagescripts.log` (the file `.memdump`'s `internal(2)`/`internal(5)` calls produce) into per-script memory entries and per-snapshot totals, and report top-N consumers; the Python version also supports CSV export. Not part of the EScript codebase - run directly via `python`/`pwsh` against a log file pulled from the server.
- `1ae01df`: adds `POLCore().internal(6, "scripts/ai/killpcs.ecl")`, dumping memory usage specifically for the killpcs AI script - the shard's most heavily-instantiated AI script and therefore the most likely source of AI-related memory growth. Three more `internal(6)` calls for `spellkillpcs.ecl`/`barker.ecl`/`animal.ecl` are present in the source but commented out - left as ready-to-enable follow-ups rather than run unconditionally every call, since each additional dump adds to the diagnostic log's size and parse time. Command synopsis and sysmessage/console text updated to mention all three dump types.

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

## 9. New Developer Tool - .resetph Command

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/powerhour/textcmd/test/resetph.src` | New - `.resetph` |
| `config/command_synopses.cfg` | New `resetph` entry (Developer, CmdLevel 5), regenerated via `pythonscripts/_gen_command_synopses_cfg.py` |

### Overview

Investigating the report that prompted section 8 surfaced a related stuck-state bug: `setph.src` sets the obj-property `#SettingPH` to `1` while a personal powerhour gump is open, and only clears it once `activateph()` finishes its full one-hour `Sleep(3600)` and reaches its cleanup at the end of that function (or on the player's next logon/reconnect, both of which already erase it). If that backgrounded script instance dies before then - a server restart while a player's personal PH is active being the most likely case - `#SettingPH` is orphaned at `1` indefinitely, and every subsequent `.setph` hits the "You have the gump window open already!" branch with no way out for the player. `.resetph`, a targeted staff command, was ported from the equivalent tool already shipped on ZH3.0 (`pkg/opt/powerhour/textcmd/test/resetph.src` there) to give staff a way to clear a player out of this state on request.

### Notable Functional Changes

- `.resetph` (Developer, CmdLevel 5): targets a mobile, erases `pph_use_time`, `pph_use_weekday`, `#PPHH`, `#PPHC`, `#PPHS`, and `#SettingPH` from it, and reports the reset to the caller. Refuses to run on NPCs.
- Placed under `pkg/opt/powerhour/textcmd/test/`, matching the level/folder ZH3.0's own `resetph.src` uses there (Developer-level, `textcmd/test/`).
- Adapted from the ZH3.0 source rather than copied verbatim: adds the `#SettingPH` erase (not present on ZH3.0, whose `setph.src` doesn't use that property at all - see below), drops `SetObjProperty(mobile, "usedpph", 0)` (a ZH3.0-only property this repo's powerhour system never reads or writes), and uses this repo's `mobile.isa(POLCLASS_NPC)` NPC-check idiom instead of ZH3.0's `mobile == POLCLASS_NPC` object/constant equality comparison.

### Expected Impact

Staff-facing only. Gives Developer-level staff a one-command fix for a player stuck unable to `.setph` after a personal powerhour's backing script is lost (server restart mid-PH being the main trigger), instead of requiring a manual obj-property edit. No change to normal `.setph`/`.ph` behavior.

---

## 10. New Item - Eon-Prism (Artifact System)

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/ArtifactSystem/eonprism.src` | New - `eonprism` double-click handler |
| `pkg/opt/ArtifactSystem/itemdesc.cfg` | New - `item 0x792F` (`eonprism`, graphic `0x2f57` "runed prism") |
| `pkg/opt/ArtifactSystem/artifactbox.src` | Added `UOBJ_EON_PRISM` (`0x792F`) to the decay-on-pull objtype check alongside the champion relics/world gem |

### Overview

A player-facing self-service version of `.resetph` (section 9): an item that lets a player clear their own stuck/used-up personal-powerhour properties without needing staff. Built as an Artifact System item (`pkg/opt/ArtifactSystem/` - see `pkg/opt/ArtifactSystem/artifact-system-summary.txt`) rather than a plain consumable, matching `championrelic.src`/`worldgem.src`'s pattern: `CProp Artifact i1` means the item recycles back into the global artifact pool via `maindestroy.src` instead of being permanently deleted when "used up," so it can circulate through `.makeartifact`/`.openartifact`/artifactbox pulls like the system's other rare rewards.

### Notable Functional Changes

- `item 0x792F` ("Eon-Prism", internal name `eonprism`): objtype picked from the documented-unused `0x792F-0x798F` gap in `objtypes.txt`, immediately after the `sysbook` package's `0x7910-0x792E` block - no collisions with any other `itemdesc.cfg` in the repo. Originally shipped with graphic `0x2f57` (the stock UO "runed prism" tile); `88720ac` ("Eon Prism Update", 2026-08-11) changed this to graphic `0x1f1c` (the "magical crystal" tile, also used by the unrelated `dundee` package's `lifecrystal` item) with `color 1182`, giving the Eon-Prism a distinct tinted-crystal appearance instead of sharing its look with the plain runed-prism tile.
- On use: refuses (with a sysmessage, releasing the item instead of consuming it) if the player has an active personal powerhour (`#PPHH`/`#PPHC`/`#PPHS`) - so the item can't silently cut a live personal powerhour short (the same hazard noted for `.resetph` in section 18's risk notes) - or if a server-wide powerhour is currently active (`GetGlobalProperty("PHH"/"PHC"/"PHS")`), so it can't be used to stack a fresh personal powerhour on top of/immediately after a global one - or if the player is already eligible to start a fresh personal powerhour right now (mirrors the `weekday_now < use_weekday || time_now > use_time_nextweek` eligibility check in `textcmd/player/setph.src`/`textcmd/player/ph.src`), so it can't be wasted resetting a cooldown that isn't actually blocking them.
- Otherwise erases `pph_use_time`, `pph_use_weekday`, `#PPHH`, `#PPHC`, `#PPHS`, and `#SettingPH` (same property set `.resetph` clears), then `PrintTextAbove()`s a random line from a 5-entry flavor-text array via `.randomentry()` (public overhead text, like `scripts/textcmd/coun/sayabove.src` - not a sysmessage, so nearby players see it too) and plays `SFX_SPELL_RESURRECTION` (the same sound `Resurrect()`/`highpriest.src` use), then `ReleaseItem()`/`DestroyItem()`s itself - matching `championrelic.src`/`worldgem.src`'s exact release-then-destroy order, needed because `maindestroy.src` intercepts the destroy for `Artifact`-flagged items and reroutes them into the global pool rather than freeing them; skipping the explicit `ReleaseItem()` would leave the recycled item stuck reserved (undoubleclickable) once it lands back in the artifact box.
- Not yet added to the live artifact pool via `.makeartifact`/`.openartifact`, and not on any loot table/vendor - this patch only adds the item and its objtype; getting copies into circulation is a separate staff step.
- Decays like the other artifact-box pulls: `dblclick_artifactbox()` in `artifactbox.src` now also matches `UOBJ_EON_PRISM` (`0x792F`) in its objtype check, so pulling one from the box sets `item.decayat` and `#ArtifactExpireAt` to `RELIC_DECAY_SECONDS` (1209600s / 2 weeks) out, and stamps a `RelicUID`. If it sits unused (not double-clicked) past that window, the engine's native `decayat` and/or the `artifact_daily_sweeper.src`/`artifact_startup_sweeper.src` sweep (`SweepExpiredArtifacts()` in `artifact.inc`) will `DestroyItem()` it, which `maindestroy.src` reroutes back into the artifact box (`toArtifactBox()`, which also clears the expiry/`RelicUID` so it decays fresh on its next pull) rather than deleting it outright - same lifecycle as `championrelic.src`/`worldgem.src`.

### Expected Impact

Player-facing: gives players a way to self-recover from a stuck personal-powerhour state (or reset their weekly eligibility) without staff intervention, once copies exist in the artifact pool. No effect until staff adds it via `.makeartifact`/`.openartifact`.

---

## 11. Housing - Sign Owner Name Resolved Live Instead of Cached

### Files Changed

| File | Change |
|---|---|
| `pkg/std/housing/sign.src` | `textcmd_sign()` - resolve the house owner's current name live instead of trusting a cached property |

### Overview

`textcmd_sign()` displayed the owner's name from a `lastownername` obj-property on the sign, which is only refreshed when the owner personally opens their own sign. Any other name change - most notably a town-citizenship change, which renames the character - left every sign showing the stale pre-change name to everyone else who opened it, indefinitely.

### Notable Functional Changes

- When the person opening the sign isn't the owner, `textcmd_sign()` now first tries `SystemFindObjectBySerial(CInt(oserial), SYSFIND_SEARCH_OFFLINE_MOBILES)` to resolve the current owner (works whether they're online or not) and uses `.name` from that live lookup, refreshing `lastownername` in the process. Only falls back to the old cached `lastownername` value if the owner object can no longer be resolved at all (e.g. deleted character).

### Expected Impact

Player-facing: house signs now show the owner's current name in all cases, not just after the owner personally reopens their own sign. No change to sign behavior for the owner themselves (their own name was always resolved correctly, live, in the `if` branch this only affects the `else`).

---

## 12. Character Death - Clearer Full-Backpack Message

### Files Changed

| File | Change |
|---|---|
| `scripts/misc/chrdeath.src` | Three identical `SendSysMessage()` calls reworded |

### Overview

Part of the `9e06736` ("Healing and Chrdeath update") commit. When a resurrecting player's backpack doesn't have room for everything on their corpse, `chrdeath.src` leaves the remaining items on the corpse and previously told the player "There are still items left on your corpse - please page staff." - worded as if this were an error requiring staff intervention, when it's actually expected behavior (the corpse and its contents are not deleted; the player can go back and loot them, or let the corpse decay and the items drop to the ground).

### Notable Functional Changes

- All three occurrences of the message (covering the three code paths in `chrdeath.src` that can leave items behind: normal resurrection, safe-area resurrection, and the reportables-gump path) now read: "Your backpack is too full to hold everything - some items are still on your corpse. They won't be lost; loot them now or they'll drop to the ground when the corpse decays."

### Expected Impact

Player-facing text-only change - no behavior change to what happens to the items themselves. Should reduce staff pages from players who previously read the old message as an error state rather than informational.

---

## 13. Healing & Veterinary - Dead-Patient Handling Now Stops Correctly

### Files Changed

| File | Change |
|---|---|
| `pkg/std/healing/healing.src` | `TryToCure()`, `TryToHeal()` - add missing `return;` after the "patient is dead" message |
| `pkg/std/veterinary/vet.src` | `ResAnimal()`'s cure path and `TryToHeal()` - same fix, ported from `healing.src` |

### Overview

`9e06736` ("Healing and Chrdeath update", 2026-08-19) fixed `healing.src`: both `TryToCure()` and `TryToHeal()` sent "Your patient is dead." but had no `return` afterward, so execution fell through into the normal cure/heal skill check and applied `CurePoison()`/healing math to a dead patient anyway. A follow-up code review of the whole Patch-1.1.2 branch (`97a7dcd`, 2026-09-01) found `vet.src` had the identical bug in its own cure and heal functions, never having received the same fix.

### Notable Functional Changes

- `healing.src`: `TryToCure()` and `TryToHeal()` both now `return;` immediately after the "Your patient is dead." message, instead of continuing into `CheckSkill()`/`CurePoison()`/the heal-amount calculation.
- `vet.src`'s cure function: same fix - now sends the message, calls `EraseObjProperty(who,"DoingVet")` (matching this function's other early-return paths, which all clear that property before returning), and returns, instead of falling through into `PoisonLevel()`/`CheckSkill()`/`CurePoison()`.
- `vet.src`'s `TryToHeal()`: had a more nuanced pre-existing branch - healing yourself already returned, but healing *someone else's* dead pet sent the message and fell through anyway. Simplified so both branches always `return 0;`, only the message differs (shown when healing someone else's pet, suppressed for the self-heal case, matching the original intent).

### Expected Impact

Player-facing: healing/curing an already-dead patient (yours or someone else's) with either the Healing or Veterinary skill now correctly does nothing beyond the message, instead of still rolling the skill check and applying a (wasted, and previously silently-succeeding-looking) cure/heal attempt to a corpse.

---

## 14. Area-Policy System - Cache Simplification and Datastore-Save Crash Fix

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/areas/include/areapolicy.inc` | Removed the per-program in-RAM cache layer (`e6a6be9`); added an `error`-value guard to `SanitizePolicyRealm()` (`d0220a8`) |
| `pkg/opt/areas/EnterAreaDelay.src` | Added a stale-`who` guard before resolving area policy (`d0220a8`) |

### Overview

Two related but separately-motivated changes to the same file. `e6a6be9` ("Area policy memory update", 2026-08-12) removed the parsed-area-lines and policy-mask caches' per-program in-RAM dictionary layer, leaving only the shared `GetGlobalProperty`-backed cache - a memory-usage cleanup that also happened to fix a latent bug: `InvalidatePolicyMaskCache()`'s old per-program-dict branch invalidated the *caller's own*, possibly-never-populated local dict instead of the real shared cache, so a `.areas` edit's cache invalidation could silently miss the global cache other programs were actually reading from.

Separately, this session's live-shard log review (2026-09-01) traced a recurring `[ERROR]`-level server log entry - `failed to store datastore datafile! failed to open data/ds/areas/area_policies_error{ errortext = "object does not support members" }.N.txt`, repeating on every periodic save - back to `EnterAreaDelay.src`. That program is scheduled via `Start_Script()` from `EnterArea.src`, so by the time it actually runs, the mobile (`who`) that triggered it may have already logged out or been destroyed. Accessing a member on a stale `who` returns an `error{}` value rather than throwing, and `SanitizePolicyRealm(realm)` only rejected a falsy value or the literal string `"<uninitialized object>"` - an `error` value is truthy, so it sailed through, got `CStr()`'d, and was concatenated directly into the area-policy datastore's filename. The resulting invalid path (containing `"`, illegal on Windows) failed to open on every save attempt from then on, since the bad datastore entry stays registered in the running server's in-memory datastore table once created.

### Notable Functional Changes

- `SanitizePolicyRealm(realm)`: now checks `realm == error` and falls back to `AREA_POLICY_DEFAULT_REALM` *before* calling `CStr(realm)`, so an error-valued realm can never reach the datastore-descriptor string. This is the load-bearing fix - every caller of `ResolvePolicyAtLocation()`/`GetPolicyMask()`/etc. routes through this one function, so no other call site needed touching.
- `EnterAreaDelay.src`: additionally captures `who.x`/`who.y`/`who.realm` into locals up front and returns early if any of them is `error`, as defense in depth at the specific call site that triggered this - redundant with the `SanitizePolicyRealm()` fix for the realm case, but also covers `who.x`/`who.y` (not sanitized downstream) going stale.
- `_parsed_area_lines_cache` and `_policy_mask_cache` (per-program dictionaries) are removed entirely; `GetParsedRealmAreaLines()` and `GetCachedRealmMaskDict()`/`StoreCachedPolicyMask()`/`InvalidatePolicyMaskCache()` now read/write only the shared `GetGlobalProperty`/`SetGlobalProperty` cache.

### Expected Impact

The datastore-save error should stop recurring going forward for *newly*-triggered instances of this bug - but the already-corrupted in-memory datastore entry from before this fix is deployed will keep failing on every save until the server process is restarted (recompiling scripts alone does not clear it, since it's live server state, not script state). **A server restart after deploying this fix is required to fully clear the existing error spam.** Separately, removing the per-program cache layer means `ResolvePolicyAtLocation()` - the shard's documented `sysload=100` hot path, called every AI tick for every mobile - now does up to 5 real `GetGlobalProperty()` round-trips per call instead of 1 real fetch plus 4 free in-process dictionary hits. This is a real efficiency regression flagged by this session's code review but was left as-is per this session's scope (fixing correctness, not re-litigating an already-shipped simplification) - worth revisiting if AI-tick sysload increases are observed.

---

## 15. Corpse Decay - Existence-Check Reliability (Three-Pass Fix)

### Files Changed

| File | Change |
|---|---|
| `scripts/control/corpsedecay.src` | Three successive passes at making NPC corpse decay tolerate the corpse being removed early by another system |

### Overview

`corpse_decay()`'s NPC-corpse path (`ProcessNpcCorpseDecaying()`) originally slept for the full 30-minute decay window in one `Sleep()` call, then unconditionally destroyed the corpse - if something else (most notably `restartspawnpointarea`) had already destroyed that corpse first, this call would fail. Fixing this took three passes:

1. `e3980f7` ("Corpse Decay Fix", 2026-08-12): introduced a 30-second polling loop with an `if(!corpse) return; endif` check each iteration - but this checks the local *variable* `corpse` for falsiness, which stays truthy even after the underlying game object has been destroyed elsewhere. The check was a no-op; the underlying failure mode was unchanged.
2. `5aa5f81` ("corpsedecay fix", 2026-08-12): replaced the no-op check with `if(!SystemFindObjectBySerial(corpse.serial)) return; endif` - a real existence check - and added the same check to `corpse_decay()`'s own top-level dispatch, before routing to the NPC or human decay path. This fixed the original bug but introduced a new, narrower risk: `corpse.serial` is a raw member access on `corpse`, and per the documented hazard in `houseescrow.inc`'s `MoveItemToHouseEscrowPacks()` (a stale/destroyed *item* reference throwing on raw member access, unlike a stale mobile reference which degrades gracefully to `error`), this could itself throw exactly when the corpse has already been destroyed - the one case this guard exists to catch.
3. `97a7dcd` (this session, 2026-09-01): captures `corpse.serial` into a local `corpse_serial` variable once, at the very top of `corpse_decay()`, while `corpse` is guaranteed fresh (it was just handed to the program as a live parameter). Every subsequent existence check - in `corpse_decay()` itself and throughout `ProcessNpcCorpseDecaying()`'s polling loop and final check - reuses `corpse_serial` instead of touching `corpse.serial` again, sidestepping the throw risk entirely regardless of which theory of item-staleness semantics is correct.

### Notable Functional Changes

- `corpse_decay(corpse)`: captures `corpse_serial := corpse.serial` as its first statement, before `corpse.decayat := 0` and `Sleep(5)`. The existence check after the sleep, and the `npctemplate` branch, are unchanged in structure but now pass `corpse_serial` through to `ProcessNpcCorpseDecaying(corpse, corpse_serial)`.
- `ProcessNpcCorpseDecaying(corpse, corpse_serial)`: signature gains the `corpse_serial` parameter; both the per-iteration existence check in the 30-second polling loop and the final pre-`DestroyItem()` check use it instead of `corpse.serial`.
- `ProcessHumanCorpseDecaying()` is untouched by all three passes and still uses a single two-stage `Sleep()` with no existence check at all - flagged by this session's code review as the same class of risk (a player corpse destroyed mid-sleep would hit the same "operate on a stale reference" hazard on `corpse.color`/`.graphic`/`.name`/`EnumerateItemsInContainer(corpse)`/`DestroyItem(corpse)`), but left out of scope for this patch since it wasn't the path any log data showed failing.

### Expected Impact

NPC corpse decay should no longer error out when a corpse is destroyed early by another system (e.g. a spawnpoint restart) while still in its decay wait. No player-visible behavior change for the normal case (corpse decays after 30 minutes as before).

---

## 16. Additional Defensive Guards From Live Shard Log Review, Round 2

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/vanityshop/mountstone.src` | `MountStone()` now checks `StoreMount()`'s return value before reporting success |
| `pkg/opt/spawnpoint/checkpoint.src` | Applied the `4ce28ea` `serials==error` guard to the 6 remaining unguarded reads of the same property in this file |
| `pkg/opt/spawnpoint/spawndeath.src`, `despawner.src`, `destroypoint.src`, `include/restartspawnpoint.inc` | Same `serials==error` guard applied to these siblings' reads of `PROPID_SPAWNPOINT_SPAWNED_OBJECTS` |
| `pkg/opt/Donator/donatorbearstone.src`, `donatorhorsestone.src`, `donatorllamastone.src`, `donatorostardstone.src` | `killmounts()` - ported `mountstone.src`'s `DMountSerial`-error guard and added a missing mount-nil check |
| `pkg/packethooks/megacliloc/mobiledata.src` | Widened a truthy-only check to also reject `error` before calling `SystemFindObjectBySerial()` |
| `pkg/opt/areas/callguards.src` | Added an `error`-value guard before resolving a tamed pet's master by serial |
| `pkg/std/housing/utility.inc` | `ConfiscateBossPet()` - same `error`-value guard before the master-serial resolution |
| `pkg/opt/necro/release.src` | Resolves the pet's master to an object once (guarded) instead of passing the raw `master` property directly to `GetSkill()`/`SendSysmessage()` |

### Overview

A follow-up, whole-branch code review (this session, 2026-09-01) surfacing two categories of gap: fixes from earlier in this same patch that were applied to one call site but left unaddressed in near-identical sibling code, and a handful of previously-unreviewed `"master"` obj-property reads that fed an unguarded value into a function expecting a real Integer serial or mobile object. (Most other `"master"`-property reads found in the same sweep - `warrior.src`, `playermerchant.src`, `animaltrainer.src`, `WearItem.src`, `DoubleClick.src`, `highpriest.src` - only ever compare the value with `==`/`!=` or wrap it in `CInt()`, both of which are safe against an `error` value; those were left alone.)

### Notable Functional Changes

- `mountstone.src`: `StoreMount()` already returns `0` (added in `4ce28ea`) when the tracked mount or its original stone item can't be resolved, but `MountStone()`'s caller never checked that return value, so it sent "Your mount has been stored!" regardless - leaving `DonatorMounted` still set to `1` on a failed store, soft-locking the stone (every future use re-enters the same failing path). Now sends that message only if `StoreMount()` returns truthy, otherwise "Your mount could not be found and may already be lost."
- `checkpoint.src`: the `if(!serials or serials==error)` guard `4ce28ea` added to `ValidatePointSpawns()` is now applied identically to the other 6 `GetObjProperty(point, PROPID_SPAWNPOINT_SPAWNED_OBJECTS)` reads in the same file (spawn-chest/NPC/group creation and the two loot-drop branches).
- `spawndeath.src`, `include/restartspawnpoint.inc`: same guard added to their own reads of the same property.
- `destroypoint.src`: `if(spawned)` widened to `if(spawned and spawned != error)` before the `foreach` over it.
- `despawner.src`: previously iterated `GetObjProperty(point, PROPID_SPAWNPOINT_SPAWNED_OBJECTS)` directly in a `foreach` with no guard at all; now captured into a local, normalized to `{}` if missing/`error`, then iterated.
- Donator mount stones (bear/horse/llama/ostard): each `killmounts()` normalizes an `error`-valued `DMountSerial` to `0` (matching `mountstone.src`) and now returns early if the resolved mount can't be found, instead of calling `ApplyRawDamage()` on a nil object.
- `mobiledata.src`: `if(masterserial)` widened to `if(masterserial and masterserial != error)` before calling `SystemFindObjectBySerial(masterserial, SYSFIND_SEARCH_OFFLINE_MOBILES)` for the pet-ownership tooltip property.
- `callguards.src`: `LookAround()`'s tamed-pet criminal-master check now normalizes the master serial to `0` if `error` before the `SystemFindObjectBySerial()` calls, instead of passing `GetObjProperty(mobile, "master")` directly into them.
- `housing/utility.inc`: `ConfiscateBossPet()` now normalizes an `error`-valued master serial to `0` immediately after reading it, before the existing `if(!master_serial or ...)` wild-boss check - previously an `error` value (truthy) would skip that check and reach `SystemFindObjectBySerial(master_serial)` with a bad argument.
- `release.src`: previously called `GetSkill(GetObjProperty(victim,"master"), ...)` and `SendSysmessage(GetObjProperty(victim,"master"), ...)` directly - passing either an `error` value or (even in the normal case) a raw Integer serial to functions that expect a resolved mobile object, unlike every other `"master"`-property consumer in the codebase, which resolves via `SystemFindObjectBySerial()` first. Now resolves the owner once (guarded against `error`) into a real object before calling `GetSkill()` on it or messaging it, only doing either when the resolution succeeds.

### Expected Impact

Further reduction in the same class of live-shard script errors this whole patch has been chipping away at (`4ce28ea` in section 4, the area-policy fix in section 14, and this section) - none of these are believed to have fired as frequently as the area-policy crash (section 14) or the original `4ce28ea` batch, since none showed up directly in the specific crash-log excerpt reviewed this session; they were found by proactively checking for the same bug shape in nearby/sibling code rather than from direct log evidence. `release.src`'s change is the most behaviorally significant of this batch: previously, checking whether a pet's owner has more Taming/Magery skill than the caster would have compared skill values off a nil/wrong-type object, meaning the "the owner's control is stronger than your spell" branch was silently broken in the common case.

---

## 17. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `config/command_synopses.cfg` | 6, 7, 9 | Migration commands re-leveled; new/updated `memdump` entry; new `resetph` entry |
| `config/fileaccess.cfg` | 5 | `AllowRemote 1` added for housing/playervendor `.log` |
| `config/npcdesc.cfg` | 3 | `MagicResistance 100` added to mage/alchemist/scribe |
| `pkg/items/armor/include/armorZones.inc` | 4 | Nil-guards for zone/item config lookups |
| `pkg/opt/alryc/textcmd/test/memdump.src` | 7 | New - `.memdump`; later updated to add killpcs `internal(6)` dump |
| `pkg/opt/areas/callguards.src` | 16 | `error`-value guard before resolving tamed pet's master |
| `pkg/opt/areas/EnterAreaDelay.src` | 14 | Stale-`who` guard before resolving area policy |
| `pkg/opt/areas/include/areapolicy.inc` | 14 | Per-program cache removed; `error`-value guard in `SanitizePolicyRealm()` |
| `pkg/opt/ArtifactSystem/eonprism.src` | 10 | New - `eonprism` double-click handler |
| `pkg/opt/ArtifactSystem/itemdesc.cfg` | 10 | New - `item 0x792F` (`eonprism`); graphic later changed `0x2f57` -> `0x1f1c` + `color 1182` |
| `pkg/opt/classfirsts/pkg.cfg` | 6 | Comment path update |
| `pkg/opt/Donator/donatorbearstone.src` | 16 | `killmounts()` - `DMountSerial`-error guard, mount-nil check |
| `pkg/opt/Donator/donatorhorsestone.src` | 16 | Same as above |
| `pkg/opt/Donator/donatorllamastone.src` | 16 | Same as above |
| `pkg/opt/Donator/donatorostardstone.src` | 16 | Same as above |
| `pkg/opt/necro/release.src` | 16 | Resolves pet's master to a guarded object before `GetSkill()`/`SendSysmessage()` |
| `pkg/opt/powerhour/textcmd/player/ph.src` | 8 | Adds personal-powerhour status reporting |
| `pkg/opt/powerhour/textcmd/test/resetph.src` | 9 | New - `.resetph` |
| `pkg/opt/powerscrolls/textcmd/player/showcaps.src` | 4 | Nil-guard for empty `classe` |
| `pkg/opt/spawnpoint/checkpoint.src` | 4, 16 | `error`-value guard for spawned-objects property, extended to all 7 read sites |
| `pkg/opt/spawnpoint/despawner.src` | 16 | `error`-value guard for spawned-objects property |
| `pkg/opt/spawnpoint/destroypoint.src` | 16 | `error`-value guard for spawned-objects property |
| `pkg/opt/spawnpoint/include/restartspawnpoint.inc` | 16 | `error`-value guard for spawned-objects property |
| `pkg/opt/spawnpoint/spawndeath.src` | 16 | `error`-value guard for spawned-objects property |
| `pkg/opt/vanityshop/mountstone.src` | 4, 16 | `error`-value/nil guards, owner/mount lookups; caller now checks `StoreMount()`'s return |
| `pkg/opt/versebook/Bardic_Boulders.src` | 4 | Nil-guard for `CreateItemAtLocation()` result |
| `pkg/packethooks/megacliloc/mobiledata.src` | 16 | Widened truthy check to also reject `error` |
| `pkg/std/boat/boat.src` | 4 | Realm-unset guard in `DoEncounter()` |
| `pkg/std/boat/plankutil.inc` | 4 | Realm-unset guard in `IsPlankOccupied()` |
| `pkg/std/boat/plankwalk.src` | 4 | Explicit realm args to movement/lookup calls |
| `pkg/std/healing/healing.src` | 13 | Missing `return;` after dead-patient message |
| `pkg/std/housing/houseescrow.inc` | 6 | Debug `Print()` calls removed |
| `pkg/std/housing/sign.src` | 6, 11 | Debug `Print()` call removed; owner name now resolved live |
| `pkg/std/housing/utility.inc` | 16 | `error`-value guard before master-serial resolution |
| `pkg/std/traps/traps.src` | 4 | `"Spawnpoint"` trapcrafter guard |
| `pkg/std/veterinary/vet.src` | 16 | Missing `return;`/early-return after dead-patient message (ported from `healing.src`, section 13) |
| `pythonscripts/Analyze-MemoryUsage.ps1` | 7 | New - standalone log analyzer (PowerShell) |
| `pythonscripts/analyze_memory_usage.py` | 7 | New - standalone log analyzer (Python) |
| `scripts/ai/chaosmultikillpcs.src` | 4 | Explicit realm arg on two `MoveObjectToLocation()` calls |
| `scripts/control/corpsedecay.src` | 15 | Three-pass fix for NPC corpse existence-checking (see section 15) |
| `scripts/control/firecontrol.src` | 4 | Realm-unset guard in damage-tick loop |
| `scripts/misc/chrdeath.src` | 12 | Full-backpack message reworded (3 sites) |
| `scripts/misc/death.src` | 4, 16 | `error`-value/nil guards for master/killer/spawnpoint lookups |
| `scripts/textcmd/player/guilds.src` | 16 | (See note below - guarded `GuildMaster` lookup, extracted `GetGuildMasterName()` helper) |
| `scripts/textcmd/player/houseescrow.src` | 6 | Debug `Print()` calls removed |
| `scripts/textcmd/test/backfillteleporterserials.src` | 6 | Moved from `admin/`, re-leveled |
| `scripts/textcmd/test/migrateclassfirsts.src` | 6 | Moved from `admin/`, re-leveled |
| `patchnotes/developer-changelog-v1.1.2.md` | - | This file |
| `patchnotes/patch-v1.1.2.md` | - | Player-facing notes |
| `patchnotes/launchernotes.md` | - | Replaced with this release's player-facing content |

Note: `scripts/textcmd/player/guilds.src` was fixed by `9033a5f` as part of the initial live-log-driven pass (guarding `GuildMaster` the same way `4ce28ea` guarded other properties) rather than as part of section 16's follow-up review, but is grouped with section 16 above since that section documents the rest of that same commit's file list.

---

## 18. Risk and Regression Notes

- **Eon-Prism (section 10):** guarded against use during an active personal powerhour, but otherwise unconditional and irreversible like `.resetph` - a player who uses it while ineligible anyway (e.g. just to reset the weekly countdown) gets no confirmation prompt. As an `Artifact`-flagged item it recycles into the global artifact pool on "destroy" rather than being deleted, so the `ReleaseItem()` call before `DestroyItem()` is load-bearing - worth confirming in-game that a used prism actually turns up recoverable in the artifact box rather than stuck reserved, before relying on it in production. Not yet added to the live pool via `.makeartifact`, so has zero live impact until staff does that.
- **`.resetph` (section 9):** unconditionally erases the targeted player's `pph_use_time`/`pph_use_weekday`/`#PPHH`/`#PPHC`/`#PPHS`/`#SettingPH` - if used on a player with a legitimately active personal powerhour rather than a stuck one, it ends that powerhour early without the normal "has ended" sysmessage `activateph()` sends. GM-level tool, used on request; no automatic trigger.
- **`.ph` personal-powerhour reporting (section 8):** read-only reporting change - it does not touch how personal or server-wide powerhours are granted, tracked, or expired (still entirely owned by `setph.src`/`activateph()`). Worth spot-checking the eligibility-countdown branch against a live `#PPHH`/`#PPHC`/`#PPHS` obj-property state once, since the reset-time math (next Sunday on/after `use_time`) is inferred from `setph.src`'s existing condition rather than a separate stored "next eligible" timestamp.
- **Magic Resistance vendor training (section 3):** only affects vendors spawned/re-templated after this patch deploys - existing live Mage/Alchemist/Scribe vendors keep their current base skills until they respawn or the server reloads and re-applies `npcdesc.cfg`.
- **`plankwalk.src` explicit realm arguments (section 4):** previously-implicit realm defaults are now explicit `who.realm`/`plank.realm` - should be behaviorally identical for any plank/player pair that was already on the same realm (the normal case), but worth watching for cross-realm boat/plank edge cases specifically, since that's the scenario this class of fix targets.
- **House-escrow log de-duplication (section 6):** console `Print()` output for house-escrow operations is gone; `::log/houseescrow.log` and `::log/merchantescrow.log` (via `HouseEscrowLog()`/`MerchantEscrowLog()`) are now the only record, and depend on the `AllowRemote 1` fix in section 5 actually being in effect - confirm those log files are being written to post-deploy before relying on them for any incident investigation.
- **`.memdump` (section 7):** triggers engine-internal diagnostic dumps on demand; no rate-limiting or size cap on the resulting `memoryusagescripts.log` file is implemented in this patch - fine for occasional developer use, worth keeping in mind if it's ever scripted/automated.
- **Area-policy datastore crash fix (section 14) - restart required:** the `SanitizePolicyRealm()` fix prevents *new* bad datastore entries, but it cannot retroactively fix one that's already registered in a running server's in-memory datastore table. If this patch is deployed onto a shard that's already hit the bug, the specific corrupted `area_policies_error{...}` entry will keep failing every save cycle until the server process is restarted. **Restart the server after deploying this patch**, not just after recompiling scripts.
- **Area-policy cache simplification (section 14) - hot-path efficiency regression:** removing the per-program in-RAM cache means `ResolvePolicyAtLocation()` (called every AI tick, every mobile) now does up to 5 real `GetGlobalProperty()` calls per resolve instead of 1 real fetch + 4 free dictionary hits. Flagged by this session's code review; not fixed in this patch since it was an intentional, already-shipped (`e6a6be9`) simplification and re-architecting it was out of scope for a correctness-focused session. Worth monitoring `sysload` if AI-tick load becomes a concern, and worth revisiting with a same-call memoization (rather than a persistent per-program cache) if it does.
- **Corpse decay (section 15):** `ProcessHumanCorpseDecaying()` was not given the same existence-check hardening as `ProcessNpcCorpseDecaying()` - it still assumes the corpse reference stays valid across two long uninterrupted `Sleep()` calls. Not believed to be actively failing (no log evidence, unlike the NPC path), but the same class of hazard exists in principle if a player corpse is destroyed by another system mid-decay.
- **`release.src` master-skill comparison (section 16):** previously passed a raw obj-property value where a resolved mobile object was expected, meaning `GetSkill()`/`SendSysmessage()` were almost certainly always operating on a nil or wrong-type value - this fix changes that comparison's actual behavior (the "owner's control is stronger than your spell" branch may now trigger in cases it silently couldn't before), rather than purely guarding against a crash. Worth a quick in-game check of the Release spell against a well-trained pet's owner to confirm the new behavior matches intent.
- **Donator mount stones (section 16):** `killmounts()` on all four stones now returns early (skipping `EraseObjProperty(who, "DonatorMounted")`) if the tracked mount can't be resolved, matching `mountstone.src`'s `StoreMount()` convention. This means a player whose mount reference is already gone keeps `DonatorMounted` set to `1` rather than having it cleared - intentional (mirrors the existing convention elsewhere in this same patch) but means such a player may need a mount-stone interaction that goes through `StoreMount()` (which does clear it) to fully recover, rather than `killmounts()` alone.
