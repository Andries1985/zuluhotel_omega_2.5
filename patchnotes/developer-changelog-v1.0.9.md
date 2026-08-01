# Developer Changelog - v1.0.9
**Range:** `ed5a584` (origin/Patch-1.0.8) -> `cc3be3d` (HEAD)  
**Branch:** Patch-1.0.9  
**Date:** 2026-07-28 -> 2026-07-30  
**Commits in range:** 4 (excluding merge commits)  
**Files changed:** 23 (+4491 / -218)

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [NPC Templates - Skill Keyword Typo Fixes](#3-npc-templates---skill-keyword-typo-fixes)
4. [NPC Templates - Rainbow Ostard HP Scaling Correction](#4-npc-templates---rainbow-ostard-hp-scaling-correction)
5. [NPC Templates - Cleanup and Misc Corrections](#5-npc-templates---cleanup-and-misc-corrections)
6. [Loot and Crafting - Corrupt Angel Feathers and Hat Fortifying](#6-loot-and-crafting---corrupt-angel-feathers-and-hat-fortifying)
7. [Bard Songs - Cloaking Party Restriction and Boost-Match Fix](#7-bard-songs---cloaking-party-restriction-and-boost-match-fix)
8. [Guild Command - Guild Tag Display](#8-guild-command---guild-tag-display)
9. [Spawnpoints - Despawn-on-Destroy Default Changed](#9-spawnpoints---despawn-on-destroy-default-changed)
10. [Death Handling - KilledBy Property Crash Fix](#10-death-handling---killedby-property-crash-fix)
11. [Power Scrolls - Alchemy Cap-Check Fix](#11-power-scrolls---alchemy-cap-check-fix)
12. [Staff Tools - Test Admin Panel Expansion](#12-staff-tools---test-admin-panel-expansion)
13. [Staff Tools - Login IP History](#13-staff-tools---login-ip-history)
14. [NPC Creation Tooling - New HTML Builder](#14-npc-creation-tooling---new-html-builder)
15. [Areas - No Damage Zone Removed](#15-areas---no-damage-zone-removed)
16. [Exhaustive File-by-File Change List](#16-exhaustive-file-by-file-change-list)
17. [Risk and Regression Notes](#17-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.0.9 is a small, focused patch on top of 1.0.8. Four substantive commits: a bundle of NPC-template and gameplay fixes (`6a8c143`), a staff test-admin-panel expansion adding login-IP cross-referencing (`57b5466`), a large `npcdesc.cfg` skill-keyword typo sweep plus a new offline NPC-creation HTML tool and a `KilledBy` crash fix (`907367c`), and the power scroll (tome) Alchemy cap-check fix from earlier in this session (`cc3be3d`). The merge commit `05a5314` in this range only pulled in 1.0.8's own patch-notes tail (`patchnotes/developer-changelog-v1.0.8.md`, `patch-v1.0.8.md`, `launchernotes.md`, and a `classfirsts/pkg.cfg` addition) and carries no new 1.0.9 content.

---

## 2. Commit Timeline

| Hash | Date | Message |
|---|---|---|
| `6a8c143` | 2026-07-28 | Fortify elven glasses and dragonhelm / corrupt angel drops feathers / guild relations shows Guild tag / default spawnpoint behaviour is kill npcs on destroy / Bard invisibility fix for other groups |
| `05a5314` | 2026-07-28 | Merge pull request #298 from Andries1985/Patch-1.0.8 (Package update for class firsts) |
| `57b5466` | 2026-07-29 | Test Admin Panel Updates |
| `907367c` | 2026-07-29 | NPC Creation website made / npcdesc fixes with typos / test admin panel updated |
| `cc3be3d` | 2026-07-30 | Power scroll fix for alchemy |

---

## 3. NPC Templates - Skill Keyword Typo Fixes

### Files Changed

| File | Change |
|---|---|
| `config/npcdesc.cfg` | Skill keyword corrections across multiple templates |

### Overview

`npcdesc.cfg` skill lines are matched against a fixed set of recognized keywords. Several templates used keywords that don't match any recognized skill name, so those lines were silently ignored by the loader - the NPC spawned with that skill at its default (unset) value instead of the listed number.

### Notable Functional Changes

- `treasurehunter`, `thief`, `koboldmercenary`: `detecthidden` / `DetectHidden` -> `DetectingHidden` (the recognized keyword, already used 144 other places in the file). These templates now actually receive their listed Detect Hidden skill (100 / 100 / 80) instead of none.
- `jeweler`, `mage`, `listener`: `itemidentification` / `Itemidentification` -> `ItemId`. Now actually receive their listed Item Identification skill (100 / 100 / 90).
- `mage`: `spiritspeaking` -> `SpiritSpeak`. Now actually receives 100 Spirit Speak.
- `shadowgolem`: `PoisonProtection i4` -> `PermPoisonImmunity i4`. This was a valid keyword either way (both are recognized CProps used elsewhere), but changes the golem from a small percentage poison resist to full poison immunity - a deliberate strengthening, not a typo fix.

### Expected Impact

Treasure hunters, thieves, kobold mercenaries, jewelers, mage vendors, and the `listener` NPC now behave with the skills their template always claimed to give them (detecting hidden players, identifying items, spirit speak). No spawn data migration needed - this takes effect on next spawn/respawn of the affected templates.

---

## 4. NPC Templates - Rainbow Ostard HP Scaling Correction

### Files Changed

| File | Change |
|---|---|
| `config/npcdesc.cfg` | `rainbowostard`, `rainbowfrenziedostard` HP field |

### Overview

Both templates previously used a flat `HITS` value of 175. `907367c` commented out `HITS` and replaced it with `CProp CustomHitsLevel i3000` / `i7000` respectively. `CustomHitsLevel` is stored in hundredths (displayed HP = value / 100, see engine constraint notes), so as committed this set actual max HP to 30 and 70 - far *below* the old 175, not the intended increase. Caught during patch-notes review and corrected before release to `i300000` / `i700000` (3000 / 7000 HP), confirmed against the existing convention elsewhere in the same file (e.g. other `CustomHitsLevel i300000`/`i700000` entries already present for comparable NPCs).

### Notable Functional Changes

- `rainbowostard`: max HP corrected to 3000 (`CustomHitsLevel i300000`).
- `rainbowfrenziedostard`: max HP corrected to 7000 (`CustomHitsLevel i700000`).

### Expected Impact

Both NPCs now have significantly higher max HP than the old flat 175, as intended - not the accidental 30/70 HP that would have shipped from the raw commit.

---

## 5. NPC Templates - Cleanup and Misc Corrections

### Files Changed

| File | Change |
|---|---|
| `config/npcdesc.cfg` | Duplicate template removal, dead-CProp cleanup |

### Notable Functional Changes

- Removed two unused, fully-duplicated `NpcTemplate` blocks: `peacekeeper` and `peacekeeper2` (98 lines). Neither is referenced by any spawn config or script - confirmed no other file references `peacekeeper2`.
- `kappa` template: removed `CProp kappa i1`; `packhorse`/`packllama` templates: removed `CProp pack i1`. Confirmed via repo-wide search that no script reads `GetObjProperty(*, "kappa")` or `GetObjProperty(*, "pack")` - these were unused flags, so removal has no functional effect. Note the edit also merged the following `CProp` line onto the same physical line in two spots (`PermPoisonImmunity i8	CProp rise skapparat` and `BaseDexmod i10}`); `npcdesc.cfg` is token-parsed rather than line-parsed elsewhere in this file so this is not expected to break loading, but it's flagged here since it wasn't verified against a live load in this session.
- `beckon`, `mapmaker` (already-commented block): `HITS` lines commented out rather than removed, consistent with the `CustomHitsLevel` migration pattern used for the rainbow ostards - `mapmaker`'s block was already entirely commented out so this has no live effect.

### Expected Impact

No player-visible change from this section other than the two rainbow-ostard corrections covered in section 4.

---

## 6. Loot and Crafting - Corrupt Angel Feathers and Hat Fortifying

### Files Changed

| File | Change |
|---|---|
| `config/corpses.cfg` | Added `feather 250` drop to `corruptangel` |
| `pkg/std/tailoring/make_cloth_items.src` | Added item IDs to `IsHat()` / `IsHelm()` |

### Overview

`IsHat()` gates the tailoring "Fortify Hat" flow (`FortifyHat()`), and `IsHelm()` gates which helm can be combined with it. Elven Glasses (`0x3172`) and Dragon Helm (`0x2645`) were not present in either list, so the Fortify Hat feature silently didn't recognize them as valid inputs.

### Notable Functional Changes

- `corruptangel` corpse now includes `feather 250` alongside its existing daemon bone / brimstone / angel hide drops.
- `IsHat()` now returns true for `0x3172` (Elven Glasses).
- `IsHelm()` now returns true for `0x2645` (Dragon Helm).

### Expected Impact

Corrupt Angels drop feathers on death. Elven Glasses can now be used as the hat input to Fortify Hat, and Dragon Helm can now be used as the helm combined into it.

---

## 7. Bard Songs - Cloaking Party Restriction and Boost-Match Fix

### Files Changed

| File | Change |
|---|---|
| `scripts/include/bard.inc` | `ValidSongBoost()` |
| `pkg/opt/versebook/include/versefunctions.inc` | `SmartSongBoost()` |

### Overview

Two related but distinct gates control which nearby characters a bard's song affects. `ValidSongBoost()` (used directly by Song of Cloaking, `pkg/opt/songbook/songofcloaking.src`) previously required a non-NPC target to share the caster's party to be affected. `SmartSongBoost()` (used by several other songbook/versebook spells - defense, haste, glory, life, light, remedy, salvation, etc.) had a latent bug: when neither the caster nor the target had a party, `person.party == who.party` compared two null values as equal, incorrectly treating two unrelated unpartied players as a party match.

### Notable Functional Changes

- `ValidSongBoost(player, cast_near)`: the non-NPC branch no longer checks `cast_near.party == player.party` - any non-NPC target in range is now valid, regardless of party.
- `SmartSongBoost(who, person)`: the non-NPC branch now requires `who.party` to be truthy before comparing `person.party == who.party`, so two unpartied non-NPCs are never treated as matching.

### Expected Impact

Song of Cloaking (bard invisibility) now hides everyone in range regardless of party membership, not just party members. Other `SmartSongBoost`-gated songs no longer incorrectly extend their boost to a random unpartied bystander just because neither side has a party.

---

## 8. Guild Command - Guild Tag Display

### Files Changed

| File | Change |
|---|---|
| `scripts/textcmd/player/guilds.src` | `DisplayGuildMembers()` |

### Notable Functional Changes

- The guild member list gump now shows a `Guild Tag: {abbreviation}` line above the member count, reading the existing `GuildAbv` guild property.

### Expected Impact

Players viewing their guild's member list can now see the guild's abbreviation/tag alongside the member count.

---

## 9. Spawnpoints - Despawn-on-Destroy Default Changed

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/spawnpoint/checkpoint.src` | `pt_data[9]` default |
| `pkg/opt/spawnpoint/defaultdelay.src` | `pt_data[9]` default |
| `pkg/opt/spawnpoint/despawner.src` | `pt_data[9]` default |
| `pkg/opt/spawnpoint/destroypoint.src` | `pt_data[9]` default |
| `pkg/opt/spawnpoint/include/restartspawnpoint.inc` | `pt_data[9]` default |
| `pkg/opt/spawnpoint/spawndeath.src` | `pt_data[9]` default |
| `pkg/opt/spawnpoint/spawnpoint.src` | `pt_data[9]` default |
| `pkg/opt/spawnpoint/textcmd/admin/gotospawnpoint.src` | `pt_data[9]` default |

### Overview

`pt_data[9]` ("Despawn on destroy") governs whether a spawnpoint's currently-spawned NPCs are killed/removed when that spawnpoint is destroyed or reset. All eight places that initialize a fresh `pt_data` array changed the default from `0` to `1`.

### Notable Functional Changes

- Newly created spawnpoints (and any spawnpoint whose settings get re-initialized through these helpers) now default to killing their spawned NPCs on destroy, instead of leaving them behind.

### Expected Impact

Staff-facing default behavior change for spawnpoint management - existing spawnpoints with already-saved settings are unaffected; this only changes the default applied to new/reset spawnpoints.

---

## 10. Death Handling - KilledBy Property Crash Fix

### Files Changed

| File | Change |
|---|---|
| `scripts/misc/chrdeath.src` | Guard `KilledBy` / `KilledBySerial` reads |
| `pkg/opt/admin/include/adminpanel.inc` | Tolerate old unresolved-error records |

### Overview

`chrdeath.src` read `GetObjProperty(ghost, "KilledBy")` and `GetObjProperty(ghost, "KilledBySerial")` without checking for a missing property, which returns an `error` struct rather than `""`/`0`. That unguarded error struct then flowed into `RecordCharacterDeath()` and got persisted. This was previously noted as a known gap (see repo notes on the `KilledBy` error-struct bug) and is fixed here at the source, plus the admin panel's death-history display was hardened to tolerate any already-persisted bad records from before this fix.

### Notable Functional Changes

- `chrdeath.src`: `killer` now defaults to `""` and `killer_serial` now defaults to `0` when the corresponding `GetObjProperty()` call returns `error` (e.g. death with no recorded killer).
- `adminpanel.inc` `ResolveAccountNameFromSerial()`: guards `obj.acctname` against `error` (e.g. NPC objects, which have no account) before `CStr()`-ing it.
- `adminpanel.inc` `NormalizeDeathHistoryEntry()` / `BuildDeathHistorySummary()`: now detect a stringified error struct (`Find(..., "errortext", 1)`) in old persisted records and display "Unknown" instead of the raw error text.
- `BuildDeathHistorySummary()` also now appends "(Suicide)" to a death-history line when the recorded killer serial matches the victim's own serial.

### Expected Impact

No more error-struct values leaking into death records going forward. Staff viewing account death history no longer see raw error text for older, already-bad records, and self-inflicted deaths are now labeled.

---

## 11. Power Scrolls - Alchemy Cap-Check Fix

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/powerscrolls/powerscroll.src` | Cap-check gate at the top of `program powerscroll()` |

### Overview

`SKILLID_ALCHEMY` is `0`. `pScrollMatrix` (the per-player skill-cap-increase tracker) is a 1-based array; every other place in this file that indexes it for Alchemy remaps skill id `0` to slot `49` first (`chances()`, and the increment logic in the success branch). The initial "are you already capped?" gate near the top of `program powerscroll()` read `matrix[skill]` directly with the raw, un-remapped skill id, so for Alchemy it read `matrix[0]` - an out-of-range/empty read for a 1-based array - which never satisfied the `> 15` cap check. Reported as: Alchemy tomes could be used past a maxed-out cap while every other skill correctly blocked further use.

### Notable Functional Changes

- Added a `matrixIndex` local, remapped `0 -> 49` before the cap check, mirroring the existing pattern in `chances()`. The raw `skill` variable is left untouched for the rest of the function since `GetAttributeIdBySkillId(skill)` (used for the success message) expects the raw skill id, not the remapped slot.

### Expected Impact

Alchemy power scrolls (tomes) now correctly refuse to raise the Alchemy skill cap further once it's already maxed, matching every other skill's behavior.

---

## 12. Staff Tools - Test Admin Panel Expansion

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/admin/textcmd/test/testadminpanel.src` | New lookup/goto functions |
| `pkg/opt/admin/include/adminpanel.inc` | Supporting helpers |

### Overview

Internal `.testadminpanel` staff tool. Added character-owned-asset lookups with in-gump "go to" navigation.

### Notable Functional Changes

New functions added: `GetRealmNameFromMapId`, `ResolveStandableZ`, `GetHouseTypeDisplayName`, `GetBoatTypeDisplayName`, `ResolveHouseAccessLevel`, `FindMultisForCharacter`, `ShowMultisForCharacter`, `ShowMultiGotoListGump`, `FindBoatsForCharacter`, `ShowBoatsForCharacter`, `ShowBoatGotoListGump`, `ResolveCorpseKillerName`, `FindCorpsesForCharacter`, `ShowCorpsesForCharacter`, `ShowCorpseGotoListGump`.

### Expected Impact

Staff can look up a character's houses, boats, and corpses from the test admin panel and jump directly to them. No player-facing effect.

---

## 13. Staff Tools - Login IP History

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/admin/include/adminpanel.inc` | `MakeIPHistoryEntry()`, `RecordAccountLoginIP()`, IP display helpers |
| `pkg/opt/admin/textcmd/test/testadminpanel.src` | `BuildIPDisplayEntries()`, `ShowAccountsForIP()` |
| `scripts/playermanager.src` | Hook into `logon()` |

### Overview

New account-wide (not character-specific) login IP history, stored alongside the existing notes/death history on the reserved admin-panel datafile element. Unlike per-login histories, this one de-duplicates to one entry per unique IP with a hit count and first/last-seen timestamps.

### Notable Functional Changes

- `scripts/playermanager.src` now includes `:admin:include/adminpanel` and calls `RecordAccountLoginIP(acc, who.ip)` on every login.
- `RecordAccountLoginIP()` no-ops safely if the account is invalid/errored or the IP is empty.
- `ShowAccountsForIP()` lets staff pull every account that has ever logged in from a given IP - the basis for alt/multi-account detection from the test admin panel.
- `GetLatestNoteInfo()` also hardened: `account.GetProp("Notes")` guarded against `error` before `CStr()`, same class of fix as section 10.

### Expected Impact

Staff investigative tooling only - no player-facing effect. Every login now writes one extra datafile record.

---

## 14. NPC Creation Tooling - New HTML Builder

### Files Changed

| File | Change |
|---|---|
| `ainotes/npc-creator/npc-creator.html` | New (3253 lines) |
| `ainotes/npc_creation_builder.xlsx` | New binary, moved from `pythonscripts/` |
| `pythonscripts/npc_creation_builder.xlsx` | Removed (superseded) |
| `ainotes/random-npc-generator-design.md` | New design notes (113 lines), unrelated paused-feature notes carried in alongside this commit |

### Overview

A local, offline HTML tool for building `npcdesc.cfg` templates, replacing the old Excel-based `npc_creation_builder.xlsx` workflow. Purely an authoring tool - not loaded by the server.

### Expected Impact

No player or server-runtime effect. Internal content-authoring workflow only.

---

## 15. Areas - No Damage Zone Removed

### Files Changed

| File | Change |
|---|---|
| `pkg/opt/areas/include/areapolicy.inc` | Removed `AREA_POLICY_NO_DAMAGE_ZONE` bitflag |
| `pkg/opt/areas/include/areafunctions.inc` | Removed `SetNoDamageZoneProperties()`, `RemoveNoDamageZoneProperties()`, `StoreDonatorMountForNoDamageZone()`, `ConfiscateTamedPetForNoDamageZone()`; removed the now-unused `include ":housing:utility";` |
| `pkg/opt/areas/EnterAreaDelay.src` | Removed the No Damage Zone entry message/flag-set block |
| `pkg/opt/areas/LeaveArea.src` | Removed the `InNoDamageZone` clear-on-exit block |
| `pkg/opt/areas/callguards.src` | Reverted the two `!IsInNoDamageZone(mobile)` guard-spawn conditions |
| `pkg/systems/combat/include/hitscriptinc.inc` | Removed the No Damage Zone backstop from `RecalcDmg()` and `DealDamage()` |
| `pkg/systems/combat/banishonhit.src` | Removed the No Damage Zone bail-out check |
| `pkg/systems/combat/banishscript.src` | Removed the No Damage Zone bail-out check |
| `pkg/systems/combat/blackrockscript.src` | Removed the No Damage Zone bail-out check |
| `pkg/packethooks/packethook/packethook.src` | Removed the `InNoDamageZone` rejection blocks from `OnTarget()` and `checkAttack()` |
| `scripts/ai/combat/fight.inc` | Removed the shared `Fight()` No Damage Zone chokepoint and the two includes (`include/areas`, `:areas:include/areafunctions`) added solely for it |
| `scripts/ai/tamed.src` | Removed the tamed-pet `Fight()` No Damage Zone check and the now-unused `include ":areas:include/areafunctions";` (kept `include "include/areas";` - `ConfiscatePetForRestrictedRelease()` in this file still needs `IsInNOPKArea()`/`IsInGuardedArea()`/`IsInSafeArea()`, and predates the No Damage Zone commit) |
| `scripts/include/areas.inc` | Removed `IsInNoDamageZone(who)`; `IsInAntiLootingArea()`/`IsInAntiMagicArea()` no longer also match on the removed flag |
| `scripts/include/skillpoints.inc` | Reverted skill-gain block back to `IsInSafeArea(who)` only |
| `pkg/opt/areas/textcmd/admin/areas.src` | Removed the "No Damage Zone" checkbox column, its `nodamagezone` array and all load/save/apply/refresh wiring |

### Overview

The No Damage Zone area policy (added in 1.0.8, see that release's changelog section 15) was reported as not working at all, and per 1.0.8's own risk notes it had never actually been assigned to a live area (`areas.cfg`-backed policy store had no area with the bit set). Rather than debug infrastructure with zero live usage, it was removed outright: the policy bitflag, every enforcement backstop (packethook targeting/attacking, `Fight()` chokepoints, the three proc scripts, `RecalcDmg()`/`DealDamage()`), the tamed-pet confiscation/mount-storage helpers, the admin gump column, and the `IsInNoDamageZone()` lookup are all gone.

### Notable Functional Changes

- No behavior change for any currently-live area, since no area had this flag set.
- The admin `.areas` gump loses its 11th ("No Damage Zone") checkbox column; the `retval` encoding (`area_index * 100 + column`) was left as-is rather than reverted to `* 10`, since it's harmless headroom and touching it risks unrelated regressions in the column-click math.
- `pkg/opt/areas/include/areafunctions.inc` had its `include ":housing:utility";` removed - it was added solely to reach `PopulateConfiscatedPetTicket()`/`KillConfiscatedPet()` for the now-deleted `ConfiscateTamedPetForNoDamageZone()`; nothing else in the file used it.

### Expected Impact

No player-facing change - the feature never reached players. Removes dead-but-load-bearing-looking infrastructure (functions and an admin gump column staff might otherwise have assumed were live) that was reported broken.

---

## 16. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `ainotes/npc-creator/npc-creator.html` | 14 | New offline NPC template builder tool |
| `ainotes/npc_creation_builder.xlsx` | 14 | Moved from `pythonscripts/` |
| `ainotes/random-npc-generator-design.md` | - | Paused-feature design notes, carried in with this commit |
| `config/corpses.cfg` | 6 | `corruptangel` now drops `feather 250` |
| `config/npcdesc.cfg` | 3, 4, 5 | Skill keyword fixes, rainbow ostard HP correction, dead-template/CProp cleanup |
| `pkg/opt/admin/include/adminpanel.inc` | 10, 12, 13 | Error-struct guards, suicide labeling, login IP history, asset-lookup helpers |
| `pkg/opt/admin/textcmd/test/testadminpanel.src` | 12, 13 | Multi/boat/corpse lookup + goto gumps, IP cross-reference |
| `pkg/opt/areas/EnterAreaDelay.src` | 15 | No Damage Zone entry block removed |
| `pkg/opt/areas/LeaveArea.src` | 15 | No Damage Zone exit block removed |
| `pkg/opt/areas/callguards.src` | 15 | No Damage Zone guard-spawn exemption removed |
| `pkg/opt/areas/include/areafunctions.inc` | 15 | No Damage Zone helper functions + `:housing:utility` include removed |
| `pkg/opt/areas/include/areapolicy.inc` | 15 | `AREA_POLICY_NO_DAMAGE_ZONE` bitflag removed |
| `pkg/opt/areas/textcmd/admin/areas.src` | 15 | No Damage Zone gump column and all supporting arrays/wiring removed |
| `pkg/opt/powerscrolls/powerscroll.src` | 11 | Alchemy cap-check index fix |
| `pkg/opt/spawnpoint/checkpoint.src` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/spawnpoint/defaultdelay.src` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/spawnpoint/despawner.src` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/spawnpoint/destroypoint.src` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/spawnpoint/include/restartspawnpoint.inc` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/spawnpoint/spawndeath.src` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/spawnpoint/spawnpoint.src` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/spawnpoint/textcmd/admin/gotospawnpoint.src` | 9 | Despawn-on-destroy default -> 1 |
| `pkg/opt/versebook/include/versefunctions.inc` | 7 | `SmartSongBoost()` unpartied-match fix |
| `pkg/packethooks/packethook/packethook.src` | 15 | No Damage Zone target/attack rejection removed |
| `pkg/std/tailoring/make_cloth_items.src` | 6 | Elven Glasses / Dragon Helm added to Fortify Hat |
| `pkg/systems/combat/banishonhit.src` | 15 | No Damage Zone bail-out removed |
| `pkg/systems/combat/banishscript.src` | 15 | No Damage Zone bail-out removed |
| `pkg/systems/combat/blackrockscript.src` | 15 | No Damage Zone bail-out removed |
| `pkg/systems/combat/include/hitscriptinc.inc` | 15 | No Damage Zone backstop removed from `RecalcDmg()`/`DealDamage()` |
| `pythonscripts/npc_creation_builder.xlsx` | 14 | Removed, superseded |
| `scripts/ai/combat/fight.inc` | 15 | No Damage Zone chokepoint + its two dedicated includes removed |
| `scripts/ai/tamed.src` | 15 | No Damage Zone `Fight()` check removed; unused `areafunctions` include removed |
| `scripts/include/areas.inc` | 15 | `IsInNoDamageZone()` removed; anti-looting/anti-magic checks reverted |
| `scripts/include/bard.inc` | 7 | `ValidSongBoost()` party restriction removed |
| `scripts/include/skillpoints.inc` | 15 | Skill-gain block reverted to `IsInSafeArea(who)` only |
| `scripts/misc/chrdeath.src` | 10 | `KilledBy`/`KilledBySerial` error-struct guard |
| `scripts/playermanager.src` | 13 | Hooks `RecordAccountLoginIP()` into `logon()` |
| `scripts/textcmd/player/guilds.src` | 8 | Guild tag shown in member list |
| `patchnotes/*` (via merge `05a5314`) | - | 1.0.8's own patch-notes tail, no new 1.0.9 content |

---

## 17. Risk and Regression Notes

- **`config/npcdesc.cfg` line-merge formatting** (section 5): the `kappa` and `packhorse`/`packllama` edits merged two statements onto one physical line in two spots. Confirmed no script reads the removed `kappa`/`pack` CProps, and the file's format elsewhere doesn't appear to be strictly line-oriented, but this wasn't verified against an actual server load in this session - worth a sanity-check load/parse before wide release.
- **Rainbow ostard HP** (section 4): shipped-then-caught-and-corrected before release; verify the corrected `i300000`/`i700000` values against design intent (3000/7000 HP) if these NPCs are meant to be tuned differently.
- **Spawnpoint despawn-on-destroy default** (section 9): changes behavior for *newly created or reset* spawnpoints only; does not retroactively touch already-saved spawnpoint settings, but staff should be aware the default flipped.
- **`ValidSongBoost()` party removal** (section 7): Song of Cloaking now affects any nearby non-NPC, not just party members - confirm this matches intended design (e.g. it will also hide nearby hostile players if they're in range when the song is cast).
- **No Damage Zone removal** (section 15): confirmed via the 1.0.8 changelog's own risk notes that no live area had `AREA_POLICY_NO_DAMAGE_ZONE` set on its stored policy mask before removing the flag and all its enforcement code, so this should be a pure no-op for any currently-configured area. Not independently re-verified against the live policy datastore in this session - worth a quick `.areas` gump spot-check on a couple of facets before release to confirm no stray mask has bit `1024` set (it would silently stop doing anything, not error).
