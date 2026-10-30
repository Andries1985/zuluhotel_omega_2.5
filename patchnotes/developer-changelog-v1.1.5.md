# Developer Changelog - v1.1.5
**Range:** `4abd02c` (last patchnotes commit of v1.1.4) -> `b3fa183` -> `b5bcf55` (sections 30-35, 2026-10-27 to 29) -> `e950d4d` (section 36) -> `3fe7a49` (port review fixes, 32.6, 37, 2026-10-29) + working tree (section 38, 2026-10-30)  
**Branch:** Patch-1.1.5  
**Date:** 2026-10-18 to 2026-10-29 (mayor resignation 2026-10-18; ZH3.0 3.1.4 review-fix port, keyrings and test-tool cache restore 2026-10-19/20; spell protections, Reactive Armor and Beastal Bond 2026-10-21/22; buff icons, `.newmobedit` and the NPC world-save fix 2026-10-23; class-level balance pass, NPC songs and spell lists, NPC area targeting, `.newmobedit` NPC pages, the Townsfolk type, `.npclist`, `.newiteminfo` and the use-script config 2026-10-25, `a5c79d1`; Warrior for Hire targeting, `.arrows`, vendor wages and the Vortex Beast 2026-10-27, `b3fa183`; the live log review fixes, kill credit, champion reward bags and loot, and powerhour gold 2026-10-27/28, champfire and the PvP cuts 2026-10-28/29, `b5bcf55`; the treasure map bard chance 2026-10-29, working tree)  
**Commits in range:** 6 with content (`d11f181`, `ea4722a`, `2587db3`, `07e7bc6`, `ccc8d27`, `a5c79d1`), the merge `fd57000` of #345, plus merge commits that carry only 1.1.4 work (`git diff 4abd02c 469834b` is empty); see section 2.  
**Files changed:** 43 code files modified, 1 renamed (`newmobedit.src`, admin -> gm) and 3 new includes (`scripts/include/spellprots.inc`, `scripts/include/buffs.inc`, `pkg/opt/versebook/include/beastalbond.inc`) in `4abd02c..fd57000`. In the working tree: 4 files for section 10.6 (`sanctuary.src`, `antidote.src`, `Dragon_Skin.src`, `spellprots.inc`), 32 files and 1 new include (`scripts/include/npcspellnames.inc`) for sections 18-25, plus this release's 3 `patchnotes/` files; those all landed in `a5c79d1`. In the working tree after `a5c79d1`: 5 files modified (`warrior.src`, `areas.inc`, `playermerchant.src`, `npcdesc.cfg`, `command_synopses.cfg`) and 1 new command (`scripts/textcmd/player/arrows.src`) for sections 26-29, plus the 3 `patchnotes/` files; those landed in `b3fa183`. After `b3fa183`: 52 files modified for sections 30-35, plus the 3 `patchnotes/` files, in `b5bcf55`; `digtreasure.src` (section 36) and the 3 `patchnotes/` files in `e950d4d`. In the working tree: 5 files for the ZH3.0 port review fixes (sections 31.3, 32.2, 32.3, 32.5, 35.4), 3 files modified (`death.inc`, `rewards.inc`, `scripts/textcmd/admin/akill.src`), `command_synopses.cfg` and 1 new command (`scripts/textcmd/player/champkills.src`) for section 32.6, 5 files for section 37, and the 3 `patchnotes/` files; those landed in `3fe7a49`. In the working tree: 6 files modified and 1 new include (`pkg/opt/townstones/citizens.inc`) for section 38, plus the 3 `patchnotes/` files. The full list is section 40. `d11f181` also touched `patchnotes/patch-v1.1.4.md` (release date and one heading); that is a 1.1.4 document fix, not 1.1.5 content.

> Scope was computed against `4abd02c`, the last commit that edited the 1.1.4 notes, not `master`.
> `archcure.src`, `archprot.src`, `nightsight.src`, `owlsight.src` and `angelicaura.src` show as modified in `git status` but differ from HEAD only in line endings: their per-file NPC branches were folded into `SmartBoost()` (section 20.4).

---

## Table of Contents

1. [Scope Summary](#1-scope-summary)
2. [Commit Timeline](#2-commit-timeline)
3. [Townstones - The Mayor Can Resign and Leave Town](#3-townstones---the-mayor-can-resign-and-leave-town)
4. [Personal Powerhour - One Reset Time for the Weekly Lock](#4-personal-powerhour---one-reset-time-for-the-weekly-lock)
5. [Warrior for Hire - Hearts and the High Priest](#5-warrior-for-hire---hearts-and-the-high-priest)
6. [Warrior for Hire - Damage Compare and AI Lookup Caching](#6-warrior-for-hire---damage-compare-and-ai-lookup-caching)
7. [Eon-Prism - Backpack Only](#7-eon-prism---backpack-only)
8. [Merchants - Empty Keyrings Sell for 1 Gold](#8-merchants---empty-keyrings-sell-for-1-gold)
9. [Area Policy Test Tools Restore the Mask Cache](#9-area-policy-test-tools-restore-the-mask-cache)
10. [Timed Spell Protections - Sanctuary, Antidote, Dragon Skin](#10-timed-spell-protections---sanctuary-antidote-dragon-skin)
11. [WipeMods - Dispel Removes the Timed Buffs](#11-wipemods---dispel-removes-the-timed-buffs)
12. [Login - Clear Old Protection Values](#12-login---clear-old-protection-values)
13. [Reactive Armor - Dispellable, Buff Icon](#13-reactive-armor---dispellable-buff-icon)
14. [Beastal Bond - Fumbles, Logoff and Return Position](#14-beastal-bond---fumbles-logoff-and-return-position)
15. [More Buff Icons - Night Sight, Poison, Magic Reflection](#15-more-buff-icons---night-sight-poison-magic-reflection)
16. [`.newmobedit` Moves to GM](#16-newmobedit-moves-to-gm)
17. [NPC Class Fix - NPCs No Longer Break World Saves](#17-npc-class-fix---npcs-no-longer-break-world-saves)
18. [NPCs Sing Bard Songs](#18-npcs-sing-bard-songs)
19. [NPC Spell Lists - Custom Spells, Self Buffs, the Random Index](#19-npc-spell-lists---custom-spells-self-buffs-the-random-index)
20. [NPC Area Targeting - One Rule Set in SmartAoE / SmartBoost](#20-npc-area-targeting---one-rule-set-in-smartaoe--smartboost)
21. [`.newmobedit` - NPC Pages, Staged Edits, Tame and Provoke](#21-newmobedit---npc-pages-staged-edits-tame-and-provoke)
22. [Townsfolk Creature Type](#22-townsfolk-creature-type)
23. [`.npclist` Rewrite](#23-npclist-rewrite)
24. [`.newiteminfo` Fixes](#24-newiteminfo-fixes)
25. [Use-Script Config and the Life Stream Recipe](#25-use-script-config-and-the-life-stream-recipe)
26. [Warrior for Hire - Fights the Warrior, Not Its Owner](#26-warrior-for-hire---fights-the-warrior-not-its-owner)
27. [`.arrows` Ammunition Counter](#27-arrows-ammunition-counter)
28. [Player Vendors - The City Wage Tier Now Applies](#28-player-vendors---the-city-wage-tier-now-applies)
29. [Vortex Beast Is a Beholder](#29-vortex-beast-is-a-beholder)
30. [Live Log Review - Script Errors and Runaways](#30-live-log-review---script-errors-and-runaways)
31. [Kill Credit - Every Kill Records Its Killer](#31-kill-credit---every-kill-records-its-killer)
32. [Champion Spawns - Kill Tally, Reward Bags, Relic Champions](#32-champion-spawns---kill-tally-reward-bags-relic-champions)
33. [Champion Loot Rebalance](#33-champion-loot-rebalance)
34. [Powerhour - Gold Piles and Overlapping Powerhours](#34-powerhour---gold-piles-and-overlapping-powerhours)
35. [Champfire - Fixed Damage, Elemental Protection Only](#35-champfire---fixed-damage-elemental-protection-only)
36. [Treasure Maps - Level 6 Bards Find Level 7 Chests More Often](#36-treasure-maps---level-6-bards-find-level-7-chests-more-often)
37. [Rangers - Taming, Pet Resurrection, Full Heals, Magic Fish](#37-rangers---taming-pet-resurrection-full-heals-magic-fish)
38. [Townstones - Every Character Can Be a Citizen](#38-townstones---every-character-can-be-a-citizen)
39. [ZH3.0 3.1.4 - Reviewed, Not Ported](#39-zh30-314---reviewed-not-ported)
40. [Exhaustive File-by-File Change List](#40-exhaustive-file-by-file-change-list)
41. [Risk and Regression Notes](#41-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.5 is a follow-up to 1.1.4.

Section 3 gives the mayor a way out of a town. The townstone gump's "give up your citizenship" button (3000) was only drawn on the citizen screen, and the mayor branch is tested before the citizen branch, so a mayor had no in-game way to leave short of losing an election or a staff `.removetownmember`. ZH3.0 ported this commit to its own 3.1.4 as theme 11.

Sections 4-7 port the fixes from ZH3.0 commit `9fbf224` (Patch 3.1.4, "theme 13", a `/code-review` round over 3.1.4) that apply to 2.5. ZH3.0's changelog itself notes that "ZH2.5 has the same code for the powerhour lock, both heart findings, the Eon-Prism, the damage compare and the memory-revival stats". Each one was checked against 2.5's current code first, and all were still present unchanged. The heart and memory-revival rules follow the choices made for ZH3.0: refuse unstamped hearts, destroy a heart handed in by anyone but the master, and apply stat loss on a memory revival. The warrior AI's itemdesc lookup caching (section 6) came with them. What was looked at and left out is in section 39.

Section 8 makes keyrings sellable. They already had `VendorBuysFor 1`, but they are `newbie 1` and the merchant AI refuses newbied items. They stay newbie, and the merchant's own sell paths accept empty ones.

Section 9 fixes two developer test tools that erased the shared area-policy mask cache and never rebuilt it.

Sections 10-14 rework the timed spell protections. It started from a player report: Sanctuary's poison immunity, when it ran out, also switched off poison immunity jewelry put on during the spell. Sanctuary, Antidote and Dragon Skin wrote straight into the CProps that equipment maintains. That caused the jewelry bug, immunity and Dragon Skin protection stuck on after restarts, and Dragon Skin stacking without limit. The spells now keep their own temporary levels, and the higher of gear and spell counts. Dragon Skin becomes a timed buff. After test-shard feedback (section 10.6), all three take their level and a 2-10 minute duration from the caster's class level alone. Dispel removes all of them plus Reactive Armor. Beastal Bond gets ZH3.0's fixes plus a saved start position.

Sections 15-17 are smaller follow-ups: three more buff icons, `.newmobedit` opened to GMs, and a fix for `.setclass` on an NPC breaking every world save.

Sections 18-21 are an NPC casting round. The 16 songbook songs could not run for an NPC at all: `castspell()` already started them with `{ "#MOB", caster, target }`, but each song took that array as its singer. They now accept an NPC singer (Song of Beckon stays players-only). NPCs can carry their own spell list (`CustomSpells`), cast helpful spells on themselves, and no longer lose part of their casts to an off-by-one `Random()` index. Every area spell or song an NPC casts now goes through one set of target rules inside `SmartAoE()` / `SmartBoost()`, shared with the boss mass specials. `.newmobedit` gains NPC pages to set all of this per NPC. A `/code-review` of the round found the fixes folded into sections 18-21 (Song of Sirens' stamina fall-through, the spells NPCs can't cast, Tame Diff 0 not stopping Spell Bind, newmobedit edits surviving Cancel, the duplicated targeting rules).

Sections 22-25 are smaller: town NPCs become the `Townsfolk` creature type, `.npclist` is rewritten, `.newiteminfo` gets a full bug pass, and the use-script config and the Life Stream engraving recipe are cleaned up.

Sections 26-29 come from player reports and suggestions (2026-10-27). A Warrior for Hire told to attack another player's warrior turned on that warrior's owner, because pet damage is credited to the master. A new `.arrows` command counts ammunition the way `.pagans` counts reagents. `IsInCityRegion()` read a `City` key that no region has, so the player vendor city wage tier never applied; it now reads `Type City`, and the city tier is 50,000 gold a month. The Vortex Beast becomes a Beholder-slayer target.

Section 30 comes from a review of the live server's logs and save (2026-10-27): custom spawnpoint NPCs that were never dressed, fields that read a destroyed item's realm or passed none, config files loaded by a bare name from package scripts (1,540 "stacking.cfg does not exist" lines alone), monster AIs recursing on ghosts until the core killed them, staff demotion failing, and an Omega Cache item list that ran away on a big cache.

Sections 31-34 started from 53 champion spawn errors in the same log. A kill is credited through `KilledBySerial`, which only `ApplyTheDamage()` writes. Poison, Mind Blast and a few items dealt raw damage and recorded nobody, and a missing attacker was saved as Uninit. Every damage path that has an attacker now goes through `ApplyTheDamage()`, which also gives Mind Blast and Vengeance the PvP reduction; poison skips it, and pets, summons and Warriors for Hire now take their master's PvP cuts (section 31). The champion altar's per-player tally had nothing reading it since the initial import; it now pays Champion Reward Bags by kill count, with a two-hour activity window, and relic champions are recognised as having no altar (section 32). The bags take over the presents, vanity tokens and part of the tomes, scrolls and stat potions from the champion corpses (section 33). Section 34 fixes gold piles doubling only when both Hunting powerhours overlap, and stops `.setph` starting a personal powerhour that the server-wide one already covers.

Section 35 retunes `champfire` (Rikktor, Nyx, The Summoner, the Soul Whisperers). Its fields had no `Creator`, so they hit players with the players' own skills. The bolt and fields now roll fixed dice that skip `Resisted()`: about 200 and 50 a tick for Rikktor and Nyx against no protection, cut only by protection in the rolled element, for every class alike.

Section 36 raises a level 6 Bard's chance of digging a level 7 chest from a level 6 treasure map to 8%.

## 2. Commit Timeline

| Commit | Date | Subject | Sections |
|---|---|---|---|
| `01bb007` ... `f93dfbb`, `469834b` | 2026-10-06 to 2026-10-12 | Merge pull requests #331, #333, #335-#342 (Patch-1.1.3, Patch-1.1.4, Test-Shard) | - (no new content; tree at `469834b` equals `4abd02c`) |
| `d11f181` | 2026-10-18 | Mayor can leave town | 3 |
| `ea4722a` | 2026-10-20 | Fixes: ZH3.0 3.1.4 review fixes (powerhour lock, warrior hearts, priest, damage compare, AI caching, Eon-Prism), keyrings, test tools restore the area mask cache | 4-9 |
| `2587db3` | 2026-10-22 | Bard fixes, antidote fixes: spell protections, Dragon Skin rework, Reactive Armor, Beastal Bond, patch notes | 10.1-10.5, 11-14 |
| `07e7bc6` | 2026-10-23 | Buff fixes for poison and other things.. newmobedit fix | 15, 16 |
| `ccc8d27` | 2026-10-23 | NPC class fixes: stop NPCs from breaking world saves | 17 |
| `fd57000` | 2026-10-23 | Merge pull request #345 (NPC-Class-Fixes) | - |
| `a5c79d1` | 2026-10-25 | Npc fixes, iteminfo fix, townsfolk type fix, npc bard groundwork, npc area of effect fixes sanc, antidote, and dragonskin patch | 10.6, 18-25 |
| `b3fa183` | 2026-10-27 | WFH attack target fix, .arrows, vendor city wage, Vortex Beast slayer type, patch notes | 26-29 |
| `b5bcf55` | 2026-10-29 | Kill credit, champion reward bags, champfire, PvP cuts for pets, log fixes, patch notes | 30-35 |
| `e950d4d` | 2026-10-29 | Treasure map: level 6 bards get 8% at a level 7 chest, patch notes | 36 |
| `3fe7a49` | 2026-10-29 | ZH3.0 port review fixes, `.champkills` and kill milestones, ranger taming, pet healing and magic fish, patch notes | 31.3, 32.2, 32.3, 32.5, 32.6, 35.4, 37 |
| *(next commit)* | 2026-10-30 | Townstones: one citizen per character, patch notes | 38 |

---

## 3. Townstones - The Mayor Can Resign and Leave Town

**File:** `pkg/opt/townstones/tstone.src` (+41 / -1)

### 3.1 The gap

`BuildTownStoneGump()` draws one of four screens: not a citizen (join), citizen of another town (`user == 99`), mayor (`mayorserial == who.serial`), or citizen. Only the citizen screen has button 3000 ("Do you want to give up your citizenship?" -> `CanselCityzenship()`). The mayor screen has "Call new poll" (2001), "Purchase upgrades" (2100) and "Start elections" (2002) / "View candidates" (5001). The only ways out were to call an election and lose it (the mayor cannot stand, `candidato()` refuses `who.serial == mayorserial`, and the election needs at least 3 votes and no tie), or for staff to run `.removetownmember`.

### 3.2 The change

- `include ":gumps:yesno"` added.
- Mayor screen: new button 3001 "Resign and leave town" at (260, 250 + `yshift`), the free right-hand slot under "Start elections" / "View candidates".
- Dispatcher: `3001: ResignMayorship(who, item);`.
- New `ResignMayorship(who, item)`:
  1. Re-reads `mayorserial` from the stone and refuses a non-mayor ("You are not the mayor of <city>.").
  2. `YesNo(who, "Resign as Mayor of <city> and give up your citizenship?", "Resign", "Cancel")`. Cancel says "Cancelled."
  3. Re-reads `mayorserial` after the prompt, since an election can finish while the gump is open ("You are no longer the mayor of <city>.").
  4. Remembers the stored `mayor` name, erases `mayor` and `mayorserial`, sets `nextcitizenelection` to 0, and resets the script globals `mayor` / `mayorserial` / `nextcitizenelection`.
  5. Calls the existing `CanselCityzenship()`. That strips " of <city>" from the name (or runs the duplicate-name changer), removes the account from `citizen`, decrements `populacao`, erases the character's `town` CProp, and removes any candidacy or vote rune while an election or poll is running.
  6. `BroadcastSystemLine("<name> has resigned as Mayor of <city>.")`. The stored mayor name still carries the " of <city>" suffix from candidacy time.
- The `TouchTownstoneRegionEntry()` call at the end of the program syncs the cleared mayor fields to `:townstone:townstone_data`, as for every other button.

### 3.3 Decisions

- **A running election or poll keeps going.** An election will seat the next mayor; a poll the mayor called simply runs out. `.removetownmember` does the opposite: it closes both when it removes a mayor. Self-resignation leaves them open on purpose.
- **Citizens can call an election straight away.** `nextcitizenelection := 0`, and `CanCitizenCallElection()` already allows a call when there is no mayor (subject to the 3-member minimum).

---

## 4. Personal Powerhour - One Reset Time for the Weekly Lock

**File:** `pkg/opt/powerhour/include/powerhour.inc` (+16 / -21). Ported from ZH3.0 3.1.4 theme 13.

### 4.1 The defect

`PPH_IsEligible()` was `(weekday_now < use_weekday) || (time_now > use_time + PH_WEEK)`. `PPH_SecondsUntilEligible()` computed a separate reset time: the Sunday 00:00 GMT after the use, or use + one week + 1 s for a Sunday use. The two disagreed for a week. Worked example, used Wednesday 15:00 GMT:

- From Sunday 00:00 to the end of Tuesday: weekday 0-2 < 3, so eligible. That's correct.
- Wednesday 00:00-15:00: `3 < 3` is false and a full week has not passed, so the player is **locked again**. `.setph` refused, and the Eon-Prism told them they needed it. Meanwhile `PPH_SecondsUntilEligible()` returned 0, so `.ph` said "You can start a new personal powerhour in less than a minute."
- From Wednesday 15:00: eligible again.

So a player who waited a full week, to the day, before using the next one was locked out until the hour of their previous use.

### 4.2 The fix

New `PPH_ResetTime(who)`: 0 with no `pph_use_time`. Otherwise it takes the weekday from the use time itself (`PH_WeekdayOf(use_time)`), returns use + `PH_WEEK` + 1 for a Sunday use, else `(CInt(use_time / PH_DAY) + (7 - use_weekday)) * PH_DAY`, the Sunday 00:00 after the use. `PPH_IsEligible()` is now `CInt(time_now) >= PPH_ResetTime(who)` and `PPH_SecondsUntilEligible()` is `PPH_ResetTime(who) - time_now`, floored at 0. `.setph`, `.ph` and the Eon-Prism all go through these two functions.

The stored `pph_use_weekday` is no longer read by the rule. `.setph` still writes it, `PPH_Clear()` still erases it, and `PPH_UseWeekday()` stays.

---

## 5. Warrior for Hire - Hearts and the High Priest

**Files:** `pkg/opt/warriorforhire/include/wfhcommon.inc` (+9 / -9), `scripts/ai/highpriest.src` (+11 / -4). Ported from ZH3.0 3.1.4 theme 13.

### 5.1 Unstamped hearts are refused

1.1.4 (section 10) stamped every heart and the matching `WFHBackup` with `WFHDeathId` (the corpse serial). `IsCurrentWfhHeart()` accepted a heart when the stamps matched, or when **neither** the heart nor a backup carried one, which was meant for hearts from before 1.1.4. Every revival erases `WFHBackup`, so "no backup" is the normal state after any revival, and an old unstamped heart kept in a chest passed every time: a free extra life.

`IsCurrentWfhHeart()` now returns 0 for a heart without `CInt(WFHDeathId)` and 0 when the owner has no backup. Otherwise it compares `CInt` heart id with `CInt(backup["WFHDeathId"])`. Stamped hearts behave exactly as before. A stamped heart with no backup already failed under the old code.

### 5.2 Only the master may hand in a heart

The heart branch of the priest's item-given handler judged the heart against the giver: it checked the giver's living henchman, then `IsCurrentWfhHeart(heart, giver)`. A stamped heart from someone else already crumbled, because the giver's backup cannot carry that death's id. An unstamped one could pass with a giver who had no backup, and be revived as the giver's warrior (`master := event.source.serial`). New first test: `CInt(heart "master") != event.source.serial` destroys the heart with "This heart is bound to another soul. It crumbles to dust." The rest of the branch is unchanged. Note that `master` is a character serial, so an alt on the same account counts as "another soul".

### 5.3 Memory revival applies the stat loss

"resurrect warrior" (10,000 gold) restored the backup's Str/Int/Dex unchanged, while a heart revival takes one point per death so far, floored at 10. The memory path now uses the same `Max(CInt(stat) - resnum, 10)` with `resnum` from the backup. A revival costs the same whichever way it is done.

---

## 6. Warrior for Hire - Damage Compare and AI Lookup Caching

**Files:** `pkg/systems/combat/include/hitscriptinc.inc` (+13 / -2), `pkg/opt/warriorforhire/warrior.src` (+47 / -8). Ported from ZH3.0 3.1.4 theme 13.

### 6.1 `WFHModifyDamage()`

The sparring halving compared `GetObjProperty(attacker, "master") == defender.serial` and the reverse. An absent CProp and an errored serial compare equal in eScript, so the halving could fire for a masterless warrior. Each side's master is now read once as `CInt` (only for the `warriorforhire` template) and compared only when non-zero. Real sparring between a warrior and its own master is unchanged. 1.1.4 already made masterless warriors inert, so this is defensive more than visible.

### 6.2 Per-script caches in the warrior AI

- `HasBow()` called `FindConfigElem(ReadConfigFile(":*:itemdesc"), weapon.objtype)` on every fight-loop step. New `WeaponAmmoType(objtype)` caches objtype -> ammo objtype (0 = fires nothing) in the script global `gWeaponAmmoTypes`. 2.5 keeps its own rule that any non-zero `ProjectileType` is ranged (ZH3.0 also excludes `ProjectileType 1`, its throwing weapons, which 2.5 doesn't have). The ammo-in-pack check stays live.
- `HealPotionPower()` did one `:alchemy:itemdesc` lookup per pack item on every drink search. New `LoadHealPotionPowers()` builds `gHealPotionPowers` (objtype -> `Strength`, `Script yellowpotion` only) once per script from `GetConfigIntKeys()`. The `ByTrueMage` CProp is still added per potion.
- Not ported because 2.5 already had it: moving `WFH_SKILL_CAP_TENTHS` and the tracked-skill list into `wfhcommon.inc`, and the status gump's use of `WFH_MAX_DEATHS`.

No behaviour change. Combat with a warrior is a little cheaper on the server.

---

## 7. Eon-Prism - Backpack Only

**File:** `pkg/opt/ArtifactSystem/eonprism.src` (+8). Ported from ZH3.0 3.1.4 theme 13.

The use script only reserved the item, so anyone in reach could double-click a prism lying on the floor or in an open chest and spend it on themselves. It now includes `include/itemutil` and refuses unless `IsOnPlayer(character.backpack, prism)` (a recursive backpack walk, so sub-bags count): "The Eon-Prism must be in your backpack to use it." The item is released first.

---

## 8. Merchants - Empty Keyrings Sell for 1 Gold

**File:** `scripts/ai/merchant.src` (+40 / -5)

### 8.1 Why keyrings couldn't be sold

`config/itemdesc.cfg` `Item 0x1011` (keyring) has `VendorBuysFor 1`, `VendorSellsFor 20` and `newbie 1`. All three merchant sell paths refuse newbied items:

- The engine sell window (`SendSellWindow`, "sell") never lists newbie items. A script cannot override that.
- "sell all" (`SellAll()`) answered "I don't buy newbied items" and skipped newbie items when grouping.
- "sell bag" (`SellBag()`) skipped newbie items.

None of the about 2,500 keyrings in the local test save carries its own `Newbie` line. All inherit the itemdesc default, so dropping `newbie 1` would have made every keyring in the world drop on death along with the keys on it. Keeping them newbie and changing the script was chosen instead.

### 8.2 Why a loaded keyring must be refused

A keyring's keys are not in a container. `key.src` keeps them in a "Tamed Storage" root box named `"Bankbox  " + hex(keyring.serial)` (two spaces), and `keyringdestroy.src` deletes that box when the keyring is destroyed. The merchant destroys what it buys, so selling a keyring with keys on it would delete the keys. `keyringdestroy.src` also refuses to destroy a keyring with `ownerserial` (sends it back through `:shilitems:itemreturner`) or `undestructible`, so the merchant would pay for one and the player would get it back.

### 8.3 The change

- New const `MERCHANT_KEYRING_OBJTYPE := 0x1011`.
- New `CanMerchantBuy(item)`: for anything but a keyring it returns `!item.newbie`, the old rule. For a keyring it returns 0 if it has `ownerserial` or `undestructible`, 0 if its Tamed Storage box exists and holds anything, else 1.
- `SellAll()`: the targeted item check is `!CanMerchantBuy(item)`, answering "Take the keys off that keyring first" for a keyring and "I don't buy newbied items" otherwise. The grouping loop uses `CanMerchantBuy(thing)`.
- `SellBag()`: the root-item loop uses `CanMerchantBuy(thing)`.

Price: `item.buyprice` from `VendorBuysFor 1`, so 1 gold each (halved for stolen items as usual). Any merchant running `merchant.src` buys them; "sell all" has no merchant-type check. The engine "sell" window still won't list keyrings.

---

## 9. Area Policy Test Tools Restore the Mask Cache

**Files:** `pkg/opt/alryc/textcmd/test/aimovementsim.src` (+5), `pkg/opt/alryc/stresstest/areastressworker.src` (+6)

`.aimovementsim` and the `.shardstress` area workers call `InvalidatePolicyMaskCache("britannia")` during their runs on purpose, to exercise the uncached datafile path. Nothing rebuilt the cache afterwards. The 1.1.4 single-writer rule (section 18.2) means readers never write it back, and only the boot rebuild in `start.src` or an `.areas` Save does. So after either tool ran, every policy check on the shard fell back to `ReadPolicyMaskUncached()`: a datafile open and unload for the matched area plus the four world-catchall ids, up to five per check. That is the hot path 1.1.4 removed.

Both now call `RebuildPolicyMaskCache("britannia")` when they finish. Each stress worker rebuilds as it exits. Workers only invalidate inside their loop, so the last rebuild always lands after the last invalidation. Nothing on the live play path changed.

Considered and rejected: rebuilding a missing cache from the read path. It would break the single-writer rule that 1.1.4 introduced to stop stale write-backs, would need `set_critical`, a re-check and a retry throttle to survive a datafile hiccup, and in production the cache only goes missing through these two tools or a failed rebuild.

---

## 10. Timed Spell Protections - Sanctuary, Antidote, Dragon Skin

**Files:** `scripts/include/spellprots.inc` (new, 102 lines), `scripts/include/dotempmods.inc` (+53 / -4), `pkg/opt/holybook/sanctuary.src` (+20 / -21), `pkg/opt/earth/antidote.src` (+11 / -11), `pkg/opt/versebook/Dragon_Skin.src` (rewrite, +47 / -107), `pkg/systems/combat/include/hitscriptinc.inc` (`ModByProt`), `pkg/opt/versebook/include/versefunctions.inc` (`AdjustForPhysicalDamage`, include), `scripts/control/skilladvancerequip.src` (+2 / -3), `scripts/textcmd/player/prots.src` (+3 / -2), `pkg/packethooks/megacliloc/mobiledata.src` (+5 / -4)

### 10.1 The defects

All three spells wrote straight into the permanent CProps that equipment maintains, then slept and undid the write:

- **Sanctuary / Antidote** set `PermPoisonImmunity` (only if the caster had none), slept `magery * 2` seconds and erased it. Jewelry equipped during the spell set the same CProp through `skilladvancerequip.src` and was wiped by the erase until re-equipped. A restart killed the sleeping script and left the immunity on until an immunity item was equipped or unequipped.
- **Dragon Skin** added the bard level to `PhysicalProtection` and subtracted it when the song ended. Its fumble path (`PlayBardicMusic` failing every 7 s) did `return 0` before the cleanup loop, so everyone protected kept the bonus permanently. The temporary `#DragonSkin` marker was lost on restart while the bonus persisted, so the next song added it again on top: unbounded stacking. Because `DragonSkin()` returned 0 for anyone already marked, the per-pulse `protected` list only held newcomers, so the range / death / hidden check stopped applying after the first pulse. Equip and unequip both rewrote `PhysicalProtection` to the best equipped item, dropping the bonus, and the song end then subtracted it from the equipment value.
- **Antidote** cured `cast_on` but gave the immunity to `caster`. Sanctuary's script was copied from Antidote (its program is still named `cast_antidote`), and "caster" is only right for the self-only Sanctuary.

### 10.2 Design: spell levels live beside the permanent CProps

The fix had to cover NPCs as well as players. `config/npcdesc.cfg` gives templates `PermPoisonImmunity` and `PhysicalProtection` CProps, and Antidote and Dragon Skin can target pets. So "recalculate from equipment when the spell ends" was not safe: it would wipe a pet's template protection.

`spellprots.inc` keeps each spell's level in its own temporary CProp and never touches the permanent ones:

- `SPELLPROT_POISON := "#SpellPoisonImmunity"` (Sanctuary, Antidote) and `SPELLPROT_DRAGONSKIN := "#DragonSkin"`, each holding `{ level, owner pid }`.
- `SetSpellProt()` writes `{ level, GetPid() }` and calls `IncRevision()`. `EndSpellProt()` erases it only if the stored pid is the calling script's, so a cast that was dispelled or replaced by a recast does nothing when its timer ends.
- `GetPoisonImmunityLevel()` / `GetPhysicalProtection()` return the higher of the permanent CProp and the spell level, except that a negative (cursed) permanent value always wins. The spells don't lift curses.
- Readers switched to them: `PoisonProtected()` in `dotempmods.inc`, `ModByProt()` (weapon hits), `AdjustForPhysicalDamage()` (physical verse damage), `.prots`, and both protection blocks in the megacliloc mobile tooltip.
- Being `#` CProps, the spell levels cannot survive a restart.

`skilladvancerequip.src` no longer recalculates Physical Protection when `#DragonSkin` is set, and its `dskin` variable is gone.

### 10.3 Sanctuary and Antidote

Power and duration formulas were left unchanged in `2587db3` (replaced in section 10.6). The cast is applied when `power >= GetPoisonImmunityLevel(target)`: a target with higher immunity from jewelry or a stronger cast is skipped, and an equal one is refreshed. The scripts `SetSpellProt()`, `BuffOn("poisonimmunity")`, `Detach()`, sleep, then `EndSpellProt()` / `BuffOff()`. Antidote now applies to `cast_on`, including NPC targets (no buff icon for NPCs; `BuffOn` returns 0 for them).

### 10.4 Dragon Skin

The channelled loop is gone. After `CanPlay()` and `SucceedPlayingVerse()`:

- In `2587db3`, level `IsBard(who)` (1-6) picked the protection from `{ 1, 2, 3, 5, 7, 9 }` and the duration `(15 + 3 * (level - 1)) * 60` seconds. Section 10.6 replaces both.
- Targets are `ListMobilesInLineOfSight(who, range) + who` that pass `SmartSongBoost()` (party, guild rules, pets) and `CheckLineOfSight()`, with range from `GetVerseRange()` as before.
- A target whose current Dragon Skin level is higher is skipped; otherwise it is replaced. Each target costs 20 stamina via `ConsumeBardStamina(who, stamina, 20)`; the loop stops with a message when the bard has less than 20.
- `EndVerse()` clears `#Casting` straight away, so the bard is free. The script then `Detach()`es, sleeps the duration, and ends each skin it still owns.
- The `Stamina 5` / `Difficulty 90` entry in `verses.cfg` is unchanged; `CanPlay()` still requires that much stamina to start.

### 10.5 Buff icons

`BuffOn()` / `BuffOff()` gained two entries on icons nothing else in 2.5 uses:

| Buff | Icon | Title cliloc | Text cliloc |
|---|---|---|---|
| `poisonimmunity` | 1020 (client's Gift of Renewal icon) | 1061161 "Poison Resistance" | 1063505 "Poison Immunity: ~1_VAL~" |
| `dragonskin` | 1021 (client's Attune Weapon icon) | 1061158 "Physical Resistance" | 1063504 "Physical Protection: ~1_VAL~" |

The clilocs were read from the `D:/UO` client's `Cliloc.enu`. `SendBuffPacket()` passes the amount as the text argument. No existing buff uses a text cliloc with a value placeholder, so whether the level shows is untested.

### 10.6 Balance pass: everything from the caster's class level

Test-shard feedback after `2587db3`: Sanctuary and Antidote durations were "all over the place" (about 3 minutes for a Paladin in plate, 6 for a Powerplayer, 15 for a Mage), Sanctuary gave a level 6 Powerplayer level 1, and a Mage's Antidote showed level 20. The causes, at Magery 150:

- Antidote's level was `ModifyWithMagicEfficiency(magery / 25)`: 6 x the Mage / Mystic Archer class multiplier (x1.25 to x2.5), so 7-15, and more with Intelligence gear. For a mage, `GetMagicEfficiencyPenalty()` subtracts equipped Int / 10 from the armor penalty with no floor, so it turns into an uncapped bonus.
- Sanctuary's level was Mage or Paladin level + 1, else 1 (Powerplayers included).
- Both durations were `ModifyWithMagicEfficiency(magery * 2)` seconds: class multiplier, armor penalty and the mage Int bonus all applied.
- Levels above 6 did nothing: `processpoisonmod.src` caps every poison at 6 before `PoisonProtected()`, so immunity 6 already blocks everything except thief poison (which ignores immunity).

New in `spellprots.inc`:

- `SpellProtCasterLevel(level)` clamps to 1-6; 0 (unclassed, staff, NPC casters) becomes 1.
- `SpellProtDuration(level)` returns `120 + 480 * (level - 1) / 5` seconds: 120, 216, 312, 408, 504, 600 (2:00 to 10:00). Constants `SPELLPROT_MIN_SECONDS` / `SPELLPROT_MAX_SECONDS`.

Uses:

- **Sanctuary / Antidote:** `power := SpellProtCasterLevel(GetClasseLevel(caster))`, `duration := SpellProtDuration(power)`. Magery, magic efficiency, armor and Int gear no longer matter. Antidote still uses the caster's level for the target's immunity. The cures are unchanged: Sanctuary always cures, Antidote's cure keeps its Magery check.
- **Dragon Skin:** `level := SpellProtCasterLevel(IsBard(who))`; protection = level (1-6, was 1/2/3/5/7/9) and duration `SpellProtDuration(level)` (was 15-30 minutes). The `DRAGONSKIN_PROT` table and minute constants are gone.

Everything else from 10.2-10.4 stands: higher of gear and spell, skip-if-weaker / refresh-if-equal, ownership by pid, dispellable through `WipeMods()`.

---

## 11. WipeMods - Dispel Removes the Timed Buffs

**File:** `scripts/include/dotempmods.inc`

`WipeMods()` only undid the `#mods` / `mods` arrays. It now also, before leaving its critical section:

- erases `#SpellPoisonImmunity` and `#DragonSkin` and turns their icons off;
- erases `ReactiveArmor` and `#ReactiveArmorPid` and turns the Reactive Armor icon off;
- restores the name from `realname` (Incognito) and turns its icon off.

Each one sets `had_mods`. The casting scripts are still asleep and find on waking that they no longer own the buff.

Callers that now remove these buffs: login (`playermanager.src`), Dispel, Mass Dispel, Song of Dismissal, Blackrock, the `newpotions.src` caller, Color Wars, `help.src` and the Warrior for Hire AI's periodic wipe. Dispel was the goal; the others come with it.

**Incognito is disabled.** `pkg/std/spells/incognito.src` returns "Disabled." before casting, so its script was not changed. The `realname` restore is only a safety net.

---

## 12. Login - Clear Old Protection Values

**File:** `scripts/playermanager.src` (+12)

After `WipeMods()` / `CurePoison()`, for players with `cmdlevel` 0, `ResyncEquippedProt(who, "PermPoisonImmunity")` and `ResyncEquippedProt(who, "PhysicalProtection")` reset both CProps to the best value on equipped items (or erase them). This uses the same rule as the equip scripts: highest positive item level. It clears values left by the old spell code, including Dragon Skin stacks already on live. Staff are skipped so hand-set values survive. NPCs (pets with stacked Dragon Skin from before this patch) are not cleaned.

---

## 13. Reactive Armor - Dispellable, Buff Icon

**Files:** `pkg/std/spells/reactivearmor.src` (+9 / -2), `pkg/systems/combat/include/hitscriptinc.inc`

- The cast stores `#ReactiveArmorPid := GetPid()` with `ReactiveArmor` and shows the existing `reactivearmor` buff icon. When it wakes, it only erases the buff if the pid still matches, so a dispelled or recast buff isn't ended early.
- `include/dotempmods` added for `BuffOn` / `BuffOff`.
- When the armor triggers in combat (`hitscriptinc.inc`), `#ReactiveArmorPid` is erased and the icon turned off as well.
- Removed by `WipeMods()` (section 11). A restart can no longer leave it on: login runs `WipeMods()`.

---

## 14. Beastal Bond - Fumbles, Logoff and Return Position

**Files:** `pkg/opt/versebook/Beastal_Bond.src` (+15 / -13), `pkg/opt/versebook/Beast_Bond_View.src` (+6), `pkg/opt/versebook/include/beastalbond.inc` (new, 55 lines), `pkg/opt/versebook/include/versefunctions.inc` (`EndVerse()`), `scripts/misc/logoff.src`, `scripts/misc/reconnect.src`, `scripts/playermanager.src`

### 14.1 The defects

While viewing the bonded creature, `Beast_Bond_View.src` freezes and conceals the bard and moves them to the creature every second. When the bard fumbled (`PlayBardicMusic` failing), `Beastal_Bond.src` did `return 0` and skipped `BringBackToPosition()`. The view loop kept dragging the bard along until the creature died or the bond timed out, "Leave Mind" did nothing (only the dead main loop read `#StopPlaying`), and when the view loop ended it did not unfreeze or unconceal. The bard was left frozen and invisible at the creature until logoff. Logoff only cleared the view state, so the bard came back at the creature's location. `BringBackToPosition()` also used `&&` and skipped the move when the creature shared the bard's original x or y.

### 14.2 Ported from ZH3.0 (2026-09-22)

- `Beast_Bond_View.src` always unfreezes, unconceals and clears `#ViewingBeast` when its loop ends.
- `BringBackToPosition()` moves the bard when x, y or realm differs, using the saved realm.
- `EndVerse(who)` (new in `versefunctions.inc`) clears `#Casting` at the end of the verse.
- `ListMobilesNearLocation()` is given `who.realm`.
- The `Start_Script(":versebook:Beastal_Bond_Reporting")` call is removed; that script never existed, and the wisp AI does the reporting.
- Logoff and reconnect end the bond (ZH3.0 behaviour): clear `BeastalBond`, `BeastalBondTimer`, `#StopPlaying`, `#ViewingBeast`, unfreeze, unconceal. The wisp AI exits on its own once `BeastalBond` is gone.

ZH3.0's wisp spawn location (`DEFAULT_LOCATION_NPC_RELOCATION_*`) was not ported; 2.5 keeps its coordinates.

### 14.3 Beyond ZH3.0

- **A fumble now ends the loop** (`breakplease := 1; break;`) instead of returning, so `BringBackToPosition()` and `EndVerse()` always run.
- **The loop stops if the bard goes offline** (`!who.connected`). Otherwise it would restart the view on a logged-off character after logoff cleared it.
- **The start position is saved persistently.** `beastalbond.inc` stores `BeastalBondOrigin := { x, y, z, realm }` after a successful play. `ReturnToBeastalBondOrigin()` moves the bard back and erases it. `EndBeastalBond()` (logoff, reconnect and, for restarts, login) clears the bond state and returns the bard. It only acts when some bond state is present, so concealed staff are left alone.

---

## 15. More Buff Icons - Night Sight, Poison, Magic Reflection

**Commit:** `07e7bc6`. **Files:** `scripts/include/buffs.inc` (new), `scripts/include/dotempmods.inc`, `scripts/include/spelldata.inc`, `pkg/std/spells/nightsight.src`, `pkg/opt/holybook/lightofday.src`, `pkg/opt/earth/owlsight.src`, `pkg/opt/songbook/songoflight.src`, `pkg/std/alchemy/blackpotion.src`, `pkg/opt/necro/darkcloak.src`, `pkg/systems/combat/blindingonhit.src`, `pkg/systems/combat/blindingscript.src`, `pkg/opt/summoning/processpoisonmod.src`, `pkg/std/spells/reflect.src`, `scripts/playermanager.src`

### 15.1 `buffs.inc`

`BuffOn()`, `SendBuffPacket()` and `BuffOff()` moved unchanged from `dotempmods.inc` into the new `scripts/include/buffs.inc`, which both `dotempmods.inc` and `spelldata.inc` now include. Spell scripts that only include `spelldata.inc` can show icons without pulling in `dotempmods.inc` (Reactive Armor had to add that include in section 13).

The `nightsight`, `poison` and `magicreflection` entries already existed in the icon list but nothing called them.

### 15.2 Night sight

`BuffOn(target, "nightsight", 0, duration)` right after each `setlightlevel(0, duration)` in Night Sight, Light of Day, Owl Sight, Song of Light and the night sight (black) potion. The light level ends on its own, and the client ends the icon with its countdown. Dark Cloak and both blinding on-hit scripts call `BuffOff(target, "nightsight")` when they blind, since blindness overrides night sight.

### 15.3 Poison

`processpoisonmod.src` calls `BuffOn(who, "poison", newPsnDmg, duration)` on a fresh poisoning (`!current`), with the real duration (30 s, or 30 + 5 x thief level). `CurePoison()` calls `BuffOff(who, "poison")`. Expiry, cure spells and potions, Sanctuary, death and logout all end there. A re-poison while poisoned doesn't extend the poison, so the icon isn't re-sent.

The `poison` entry's text cliloc was changed from 0x1069B1 ("~1_DMG~ poison damage every ~2_SEC~ seconds.") to the blank 1062495 used by the other icons. `SendBuffPacket()` sends a single argument, so the second placeholder would have shown raw. The icon reads "Poisoned".

### 15.4 Magic Reflection

`reflect.src` calls `BuffOn(caster, "magicreflection", amount, 0)` (no countdown: the `mr` CProp lasts until used). `Reflected()` in `spelldata.inc` calls `BuffOff(targ, "magicreflection")` where it erases `mr`. `mr` is a persistent CProp, but the client forgets buff icons on logout, so `playermanager.src` re-sends the icon at login when `mr` is set. Dispel doesn't remove `mr`, so the icon stays too, as before.

---

## 16. `.newmobedit` Moves to GM

**Commit:** `07e7bc6`. **Files:** `pkg/opt/spawnpoint/textcmd/admin/newmobedit.src` -> `pkg/opt/spawnpoint/textcmd/gm/newmobedit.src` (`git mv`, no content change), `config/cmds.cfg`, `config/command_synopses.cfg`

- `DIR pkg/opt/spawnpoint/textcmd/gm` added under `CmdLevel GM`. Package `textcmd/<level>` folders are only found through `cmds.cfg`.
- `command_synopses.cfg` regenerated: `newmobedit` is Game Master, CmdLevel 3 (was Administrator, 4). No other entry changed.
- The old compiled files in the admin folder were deleted, so a stale `.ecl` can't keep answering at admin level. Deploys should delete `pkg/opt/spawnpoint/textcmd/admin/newmobedit.ecl` too.
- GMs can now edit a mobile's stats, skills and protections, including `PermPoisonImmunity`, `PhysicalProtection` and the other prot CProps.

---

## 17. NPC Class Fix - NPCs No Longer Break World Saves

**Commit:** `ccc8d27` (merged as `fd57000`, #345). **Files:** `scripts/include/classes.inc`, `scripts/textcmd/admin/setclass.src`, `pkg/opt/Staff/include/staff.inc`, `pkg/opt/Staff/logon.src`, `pkg/opt/Staff/logoff.src`, `pkg/packethooks/speech/receivespeechhook.src`

`.setclass` on an NPC ran `AssignClasse()` -> `unequipRestrictedItems()`, which logged the NPC through `LogCommand()`. NPCs have no account, so the `:staff:` datafile was named after `CStr()` of the `acctname` error (`error{ errortext = "Not attached to an account" }`). Windows can't create that file, so every world save failed until a restart.

- `unequipRestrictedItems()` returns straight away for NPCs.
- `.setclass` refuses a target without an account ("You must target a player.").
- `LogCommand()`, `LogAction()`, the staff logon / logoff loggers and the speech logger skip anything without an account.

---

## 18. NPCs Sing Bard Songs

**Files:** `scripts/include/bard.inc` (+43), the 16 `pkg/opt/songbook/songof*.src` songs (about +17 / -7 each)

### 18.1 The gap

`GetSpellID()` in `npccast.inc` already mapped the song names to spell ids 182-197, so an NPC with a song in its spell list started the song script with `{ "#MOB", caster, target }`. Every song's program took one argument and treated it as the singer, so the array went into `GetHp()`, `findinstrument()` and the rest, and the song did nothing.

### 18.2 The change

New in `bard.inc`:

- `IsNpcSong(parms)` / `GetSongSinger(parms)`: an NPC call is recognised by `parms[1] == "#MOB"` and the singer is `parms[2]`; a songbook call passes the player as before.
- `GetSongInstrument(singer, npc_cast)`: an NPC without an instrument gets a struct with `graphic := UOBJ_HARP`, which only picks the success and failure sounds.
- `GetNpcBardLevel(musicianship)`: the class-level scale from `classes.inc` (`AVERAGE_SKILL` 75 = level 1, +15 per level, capped at 6). Below 75 it returns 0 and the song fails.
- `ValidSongBoost()` returns 1 for an NPC singer. Every caller runs `SmartBoost()` first, and that has already narrowed an NPC's list to its own side (section 20).

Each song now takes `parms`. For an NPC singer `spec := GetNpcBardLevel(mus)`, and the difficulty roll, the stamina cost and the "You were interrupted!" HP check are skipped. Ranges, effects and durations use `spec` and the NPC's skills as they would for a player.

### 18.3 Song of Fright and Song of Sirens

- **Fright** runs its target list through `NpcSmartAoE()` only for an NPC singer; a player's Fright still affects every NPC in range as before. It still uses the template's `provoke` (section 21.4).
- **Sirens** had its paralyze loop after the if / elseif / else, so the "Out of Stamina!" branch fell through and paralyzed everyone with no stamina paid. That branch now clears `#Casting` and returns. This affects players.

### 18.4 Song of Beckon stays players-only

An NPC Beckon went through `:summoning:npcsummoning`, which has no per-caster cap (the player path stops at 2 magic pets), sets the fairy's lifetime to the singer's Magery x 2 seconds (0 Magery killed it at once), and creates it without a realm, so in Britannia whatever realm the NPC was in. Beckon was taken out instead: `"songofbeckon"` is commented out of `GetSpellID()`, it is not in `npcspellnames.inc`, and the song returns at once on a `"#MOB"` call. The rest of `songofbeckon.src` is the committed player version. The `npcsummoning` issues apply to every NPC summon spell and are not fixed here.

---

## 19. NPC Spell Lists - Custom Spells, Self Buffs, the Random Index

**Files:** `scripts/include/npccast.inc` (+43 / -5), `scripts/ai/tamed.src` (+4 / -1), `scripts/include/npcspellnames.inc` (new, 36 lines)

- **Per-NPC spell list.** `cast_offensive_spell()` reads the `CustomSpells` CProp first and falls back to the template's `spell` lines only when the CProp is missing (`== error`). An empty array means no spells; the function then returns 0. `tamed.src` reads the same CProp, so a tamed NPC keeps its custom list.
- **Helpful spells go on the caster.** New `IsNpcSelfSpell()`: heal, gheal, cure, archcure, agility, cunning, strength, bless, protection, archprot, nightsight, reflect, invisibility, owlsight, earthblessing, naturestouch and antidote are cast on the caster. Before, only gheal was, and the rest landed on the player being fought.
- **The `Random()` index.** `spells[Random( len(spells) + 1 )]` picked 0..len, and `spells[0]` is an error, so `GetSpellID()` returned 0 and the attempt did nothing: 1 in n+1 casts (half for a one-spell NPC). Now `spells[Random( len(spells) ) + 1]`. This affects every spellcasting NPC on the shard, not only edited ones.
- **`npcspellnames.inc`:** `GetNpcSpellNames()` returns every name `castspell()` accepts, for `.newmobedit`'s spell picker. Left out: the aliases `meteorswarm` and `gustoair`; `reactivearmor`, `earthportal` and `shapeshift`, whose scripts have no `"#MOB"` branch (an audit of every `GetSpellID()` script against the package `spells.cfg` files found only these three); and `songofbeckon` (section 18.4). A comment in `castspell()` points at the file.

---

## 20. NPC Area Targeting - One Rule Set in SmartAoE / SmartBoost

**Files:** `scripts/include/spelldata.inc` (+126), `scripts/include/smarttamedaoe.inc` (+4 / -66), `scripts/include/bard.inc`, the songbook songs

### 20.1 Before

`SmartAoE()` and `SmartBoost()` were written for player casters. For an NPC caster `SmartAoE()` only removed the caster's own pets and party, so a monster's area spell hit other monsters; `SmartBoost()` removed nothing, so a monster's Arch Protection buffed the players around it. The boss mass specials in `npccast.inc` used a separate filter, `SmartFilterTamedTargets()`: players only for a wild NPC, and the master's rules for a tamed one. A first pass in this round added `NpcSmartAoE()` / `NpcSmartBoost()` with a third rule (monster side vs player side) and called them through an `if caster.isa(POLCLASS_NPC)` branch pasted into 19 spell and song files. Meteor Swarm, Chain Lightning, Earthquake, Explosion, Mass Curse and others kept the old behaviour.

### 20.2 One rule set

`SmartAoE()` and `SmartBoost()` now hand any NPC caster to `NpcSmartAoE()` / `NpcSmartBoost()` on their first line, so every caller is covered. `SmartFilterTamedTargets()` is now a one-line wrapper for `NpcSmartAoE()`, kept so the mass-spell callers need no change.

- `IsBystanderNpc(mobile)`: a masterless NPC that is `invul`, or whose template in `::npcdesc` has `alignment good`. That covers townsfolk, guards, merchants, healers, shrines and player vendors, and also good creatures such as pixies, wisps, unicorns and the Beckon fairy. Package templates (`:pkg:name`) are never bystanders.
- `IsMonsterSide(mobile)`: a masterless NPC that is not a bystander.
- `NpcPetCanHitPlayer(master, victim)`: the player half of `SmartAoE()` with the master as reference: not the master's party; staff masters hit anyone; otherwise not across a no-PK area, a faction (`IsRPer`) difference or the same clan war colour.

`NpcSmartAoE(caster, victims)` drops the caster and every bystander, then:

- **Wild caster:** keeps players and pets; drops other monsters.
- **Tamed caster:** keeps monsters and other players' pets, but not the master's or the master's party's pets; keeps players that pass `NpcPetCanHitPlayer()` and are not the master.

`NpcSmartBoost(caster, affected)`:

- **Wild caster:** itself and other monsters.
- **Tamed caster:** itself and any pet; the master, the master's party, everyone if the master is staff, and players with the master's faction and (in clan war) colour, the same rule as `SmartBoost()`.

### 20.3 Behaviour changes

- **Boss mass specials hit pets.** Massgust, Massvatten, Masswyvern, Masskill, Masscalllightning, Massicestrike, Massshiftingearth, MassHolyBolt, Masseld and ChampFire went from players only to players and pets for a wild caster. A tamed caster's specials now also spare bystanders.
- **A monster's area spells spare other monsters** and its area buffs only reach monsters, for every spell and song.
- **NPC area spells never touch bystanders.** Player casters are unchanged.

### 20.4 Per-file branches removed

The pasted branches are back to a plain `SmartAoE()` / `SmartBoost()` call in the 14 songs that had them, `archcure.src`, `archprot.src`, `nightsight.src`, `owlsight.src` and `angelicaura.src`. The five spell files are identical to HEAD apart from line endings. `songoffright.src` keeps its NPC-only `NpcSmartAoE()` call (section 18.3). `ValidSongBoost()`'s own copy of the side rule became `return 1` (section 18.2).

---

## 21. `.newmobedit` - NPC Pages, Staged Edits, Tame and Provoke

**Files:** `pkg/opt/spawnpoint/textcmd/gm/newmobedit.src` (+704 / -141), `pkg/std/provocation/provocation.src` (+10 / -1), `pkg/std/peacemaking/peacemaking.src` (+8 / -1), `pkg/opt/shilhook/regen.src` (unchanged, reads `CustomHitsLevel`)

### 21.1 New NPC pages

Stats 77-92 exist only for NPCs and are read by `LoadNpcSettings()` (CProps first, then the template) and written by `ApplyNpcSettings()` on Apply:

- **AI / Loot (page 9):** Script (restarts the AI; blank refused), title prefix and suffix, lootgroup, Magic Item Level and Magic Item Chance (written as `CustomLoot { chance, level, lootgroup }`, the customnpc spawnpoint format; admin+ only; one lootgroup kept), "worn items drop" and "backpack items drop" (set or erase `noloot` on the items; hair, beard, mount and pack skipped).
- **Flags (page 10):** LesserBoss, Boss, SuperBoss (a forced move resends the mobile so the name colour updates), Champion, Tame, Snoop (`snoopme`) and Steal (`stealme`) difficulty, and Provoke/Peace difficulty.

### 21.2 Custom Hits Level and Spells are staged

- **Custom Hits Level** (button 500, 1-100000, 0 removes): `ApplyStagedNpcEdits()` writes `CustomHitsLevel` x 100 on Apply, runs `RecalcVitals()` and sets HP to the new maximum. It runs before the HP field, so an HP typed into the gump still lands on top.
- **Spells** (button 501): a checkbox gump of `GetNpcSpellNames()` plus whatever the NPC already casts, three columns of 15 per page, with Save, Cancel and "Template". A list that matches the template's (as sets) erases `CustomSpells` instead of writing it. It warns when the AI script is not in `CASTER_SCRIPTS`, since other AI scripts ignore spell lists.
- Both used to write to the NPC as soon as their own gump closed, so Cancel on the main gump didn't undo them. Now they set `custom_hits` / `npc_spells` plus `hits_changed` / `spells_changed`, and only Apply writes them. "No change detected" accounts for both. The save-to-cfg button writes the mob's applied `CustomHitsLevel`, not a staged one.

### 21.3 Tame difficulty

A value above 0 writes `tamableAt` and erases `notame` and `untamable`. 0 erases `tamableAt` and writes `untamable`, the same prop `.untamable` sets. Taming checks both `untamable` and `notame`, but Spell Bind, Control Undead, Beastal Bond and herding check only `untamable`. With `notame` (the first pass of this round), those four still took control.

### 21.4 Provoke / peace difficulty

`ApplyNpcSettings()` writes a `provoke` CProp, kept even at 0. `provocation.src` and `peacemaking.src` read it before the template: 0 refuses ("You cannot provoke that!", or the NPC is skipped by peacemaking), anything else is the difficulty. Song of Fright, Taunt, the Grark Shepherd GM item and the megacliloc tooltip keep reading the template's `provoke` by design: the setting covers provoke and peace only.

### 21.5 Other fixes

- A right-click close returns 0, which fell into the "Warrior level 0" skill button; it now cancels.
- Text fields go through `GetGumpText()`.
- Dead fields 22, 25, 26, 27 (`magicefficiency`, `tempmagicimmunity`, `tempmagicreflection`, `temppoisonimmunity`) are removed; nothing read them.

---

## 22. Townsfolk Creature Type

**Files:** `config/npcdesc.cfg` (+65 / -64), `pkg/std/tracking/tracking.src` (+3 / -1), `pkg/std/healing/healing.src` (+2 / -1)

- Every template under the Townsfolk header in `npcdesc.cfg` went from `CProp Type sHuman` to `sTownsfolk`: 64 templates, including merchants, guards (`townguard`, `guard`, `CastleGuard`, `CastleGuard2`), `Banker`, `healer`, `HighPriest`, nobles, `questie` / `quest_target`, `playervendor` and `warriorforhire`. A comment line in the slayer-type header lists the new type.
- `tracking.src` gains the Townsfolk category (22 categories now) and its `NormalizeTrackingType()` case.
- `healing.src` adds Townsfolk to the `withheal` list, so bandages still use Healing on them.
- Side effect: the Human slayer (`slayerscript`, `SlayType Human`) and Human protection (`raceresistonhit`) compare the `Type` CProp, so they no longer match these NPCs. Both still match human monsters and, through their `!npctemplate` branch, players.

---

## 23. `.npclist` Rewrite

**Files:** `scripts/textcmd/seer/npclist.src` (+203 / -184), `config/command_synopses.cfg` (+1 / -1)

The old command split every template into merchants, monsters and misc text blocks. The rewrite builds groups from each template's `Type` CProp (templates without one go under "Missing Type", listed last), shows a menu of slayer types alphabetically, and for the chosen type lists each template with its script, lootgroup, magic item chance and magic item level, 20 rows per page, with a Back button. Synopsis: "Browse every NPC template in npcdesc.cfg grouped by slayer type". Still Seer.

---

## 24. `.newiteminfo` Fixes

**File:** `scripts/textcmd/gm/newiteminfo.src` (+174 / -216). Not ported from ZH3.0: ZH3.0 replaced this tool with a rebuilt `.iteminfo` that depends on its 200-1200 regen port, which 2.5 doesn't have.

- **Set Spell Hitscript:** `SetSpellFromList()` had its `return` commented out, so the button never switched the hitscript or asked for chance and circle. It is now a pure picker that returns the spell id. New `SetSpellHitscriptProps()` writes `HitWithSpell` and the matching `HitscriptNumber` / `OnHitscriptNumber` row from `hitscriptdesc` / `onhitscriptdesc` (as `ApplySpellHitscript()` in `starteqp.inc` does), so the item gets its name on ID. "Change Casted Spell" uses it too.
- **Effect / Greater / Special:** new shared `ApplyHitscriptRow()`. It clears the old props first (the old order would have erased the new value), sets the (on)hitscript and its row number, and writes the cfg's `CProp` under its own name. The old effect picker wrote `PoisonLvl`, but every reader uses `Poisonlvl`, so poison weapons made with the tool never poisoned. The Effect and Greater pickers also returned nothing, so cleanup and logging never ran.
- **Set a Special Hitscript:** neither cfg defines `NbSpecial`, so the list was always empty. It now says so.
- **Reset Hitscript** erases both row numbers and resets a weapon to its itemdesc `Hitscript` (`:combat:mainhit` if none).
- **Use scripts:** the picker's button id never incremented (always the first entry) and it read `.Script` off a string key; it now sets `usecfg[key].Script`. The spell-casting use script's spell goes to `SpellId` without leaving a stray `HitWithSpell`. The gump showed nothing because it read `:magic:usescriptdesc`, which doesn't exist; it reads `:shilitems:usescriptdesc`, matches on the `Script` line and shows `Desc`.
- **X / Y / Z edits** passed `MOVEOBJECT_FORCELOCATION` as the realm; `item.realm` added.
- **The gump stays open.** A redisplay loop restores `layout` / `data` from a copy each pass (eScript copies arrays on assignment), so stale rows don't linger. Destroy logs before `DestroyItem()` and ends the loop; Create Stack and Set Owner no longer end the command on cancel.
- **Regen page is view-only** ("Regen Mods (inactive)"): `DoRegenMods()` is commented out in `skilladvancerequip.src`, so regen props do nothing on 2.5.
- Smaller: an armor row with no CProp no longer shows an edit button; `layout[39] := 0` is `""`; the four-line "mark un-IDed" block, repeated 8 times, is `FlagForRenameOnID()`. Edited items are still renamed on ID rather than at once.

---

## 25. Use-Script Config and the Life Stream Recipe

**Files:** `pkg/opt/shilitems/usescriptdesc.cfg` (+9 / -16), `pkg/std/inscription/inscription.cfg` (+11 / -9)

- `usescriptdesc.cfg`: the `saferecall` entry was a copy of `regeneration`; it is now "Safe Recall", `:shilitems:saferecall`. The blank `Name` lines are removed; nothing reads them now.
- `:shilitems:lifestream` doesn't exist. `heaventouch.src` holds a program named `item_lifestream`, but it is the mass Bless the "of Heaven Touch" recipe already uses. The "of LifeStream" engraving recipe (`Inscription 0x1828`, skill 130, 50 diamonds) and the `lifestream` use-script entry are commented out with a dated note. Engraving itself is already off (`EnchantItem()` is commented out in `inscription.src`), so no player can reach it today. A missing recipe is handled: `EnchantItem()` answers "You must target an alchemical symbol." and `RechargeEngravedItem()` returns.

---

## 26. Warrior for Hire - Fights the Warrior, Not Its Owner

**File:** `pkg/opt/warriorforhire/warrior.src` (+23 / -5)

### 26.1 The defect

Reported while a player trained their warrior against another warrior: "attack" on another Warrior for Hire made the warrior chase that warrior's owner indefinitely.

- `ApplyTheDamage()` (`scripts/include/damages.inc`) swaps an NPC attacker for its master (`FindMaster()`, the `master` CProp) and calls `SetScriptController(master)` before `ApplyRawDamage()`. The hitlist, loot credit and criminal flags depend on that, so it stays. The side effect: the mobile that was hit gets `SYSEVENT_DAMAGED` / `SYSEVENT_ENGAGED` with the **owner** as `ev.source`, not the warrior that hit it.
- In `fight()`, a `DAMAGED` / `ENGAGED` event from anyone other than the current opponent switches targets on `Random(6)==1` or when the source is closer. The other warrior's hits all name its owner, so within a few hits the warrior switched to the owner. The owner never deals damage, so nothing switched it back. If both warriors belong to the same player, it attacked its own master.
- The reverse happened too: the target warrior was told the first warrior's owner hit it, and went after that player through `ProcessMasterEvents()`.

### 26.2 The fix

- New `ResolveAttacker(source)`. If `source` is a player, it looks for an NPC within 15 tiles whose `master` CProp is that player and whose `.opponent` is this warrior, and returns that NPC. Otherwise it returns `source` unchanged.
- `fight()`'s retarget uses the resolved attacker, and never retargets onto the warrior's own master from a damage event (for example the master's own area spell). Sparring on purpose still works, because a direct attack goes through `fight()`'s `opponent == master` path.
- `ProcessMasterEvents()`' `SYSEVENT_ENGAGED` and `SYSEVENT_DAMAGED` handlers call `Fight(ResolveAttacker(ev.source))`.
- `AddToHitList()` is unchanged: it still records the player, who is the one responsible.
- Kept to the warrior AI on purpose. Other pet AIs have the same retarget pattern but were not changed.

---

## 27. `.arrows` Ammunition Counter

**Files:** `scripts/textcmd/player/arrows.src` (new, 55 lines), `config/command_synopses.cfg` (+7)

A player suggestion: a command like `.pagans` that shows your ammunition. `.arrows` totals every stack in the backpack, including nested containers (`EnumerateItemsInContainer()`), and shows a small gump with an icon (`tilepichue`, matching the itemdesc graphic and hue) and the count per type:

| Type | Objtype | Graphic / hue |
|---|---|---|
| Arrows | `0x0F3F` | `0x0F3F` |
| Fire Arrows | `0x6051` | `0x0F42` / `0x0494` |
| Ice Arrows | `0xB200` | `0x0F42` / `0x0492` |
| Bolts | `0x1BFB` | `0x1BFB` |
| Thunder Bolts | `0x824E` | `0x1BFE` / `0x0502` |

- With none of them it answers "You have no arrows or bolts!".
- Silver arrows (`0x5001`, `pkg/opt/sunshine`) are left out; nothing on the shard uses them.
- `scripts/textcmd/player` is already a Player `DIR` in `config/cmds.cfg`, so nothing needed registering. `command_synopses.cfg` was regenerated with `pythonscripts/_gen_command_synopses_cfg.py`: "Count arrows and bolts in your backpack by type".

---

## 28. Player Vendors - The City Wage Tier Now Applies

**Files:** `scripts/include/areas.inc` (+2 / -1), `pkg/systems/playervendor/playermerchant.src` (+1 / -1)

- The question was whether player vendors really lose gold every day. They do: every 24 hours of game clock (`ReadGameClock()`, which doesn't advance while the server is down) `GetPay()` adds `monthly / 30` to `wagedebt`, with the remainder carried in `wagecarry`, and takes it from the vendor's gold pool (`g`: deposits plus sales). Unpaid wages stay as debt, and after two months of debt the vendor closes (`out_of_gold`).
- **The defect:** `CalculateDailyWage()` charges the city tier when `IsInCityRegion(me)`, but `IsInCityRegion()` read `CInt(GetRegionString("regions", ..., "City", ...)) == 1`. No region in `regions/regions.cfg` has a `City` key; towns are marked `Type City` (32 regions). So every vendor paid the rural 10,000 a month.
- **The fix:** `IsInCityRegion()` compares `lower(GetRegionString("regions", x, y, "Type", realm)) == "city"`.
- **New city tier:** `CITY_MONTHLY_WAGE_GOLD` 40,000 -> **50,000** (1,666 gold a day). Outside cities it stays `MONTHLY_WAGE_GOLD` 10,000 (333 a day). Vendor Market 1 and 2 are `Type POI`, so they stay on the 10,000 tier.
- **Also fixed by the same function:** the champion relic (`pkg/opt/ArtifactSystem/championrelic.src`) refuses to activate "in a city". That check has never worked for the same reason, and now does.

---

## 29. Vortex Beast Is a Beholder

**File:** `config/npcdesc.cfg` (+49 / -51, a move)

- `NpcTemplate vortexbeast`: `CProp Type sGiantkin` -> `sBeholder`. Beholder slayers now double their damage against it; Giantkin slayers no longer do (`GetSlayMultiplier()` compares the weapon's `SlayType` with the `Type` CProp).
- The template moved from "Giantkin Boss's" into "Beholder Boss's", replacing its "None Yet" line, so the file stays sorted by slayer type. Nothing else in the block changed.
- Its summons (`klock`, `klock2`, "a Vortex Minion", under "//Vortex Beast Summons") stay Giantkin.

---

## 30. Live Log Review - Script Errors and Runaways

From the live `debug.log`, `script.log` and `pol-*.log` (2026-10-13 to 10-27). Each count below is from those logs.

### 30.1 Custom spawnpoint NPCs are dressed

**Files:** `pkg/opt/spawnpoint/include/customnpc.inc`, `pkg/opt/spawnpoint/checkpoint.src` (155 errors, `checkpoint.ecl PC=12795`)

`DressNPCCustom(me)` found its spawnpoint through the critter's `SpawnPoint` CProp. `CreateCustomNPC()` wipes the critter's CProps before dressing it, and `SpawnPoint` is only set once the critter is placed, so the lookup was `SystemFindObjectBySerial(error)` and custom NPCs came out undressed. The point is now passed in (`DressNPCCustom(critter, point)`), and a missing point returns.

### 30.2 Field realms

**Files:** `scripts/control/firecontrol.src`, `poisoncontrol.src`, `paracontrol.src`, `energycontrol.src` (915 errors, `firecontrol.ecl PC=125`)

- Fire and poison fields read `item.realm` and `item.x/y/z` once per tick. The field could be destroyed between the `!item.realm` check and the `ListMobilesNearLocation()` call, which then got an Uninit realm.
- Paralyze and energy fields never passed a realm at all, so outside Britannia they searched Britannia's coordinates and never caught anyone. Both now pass the realm, read the same way.

### 30.3 Config files loaded from package scripts

**Files:** `scripts/ai/main/` `animalsetup`, `criersetup`, `questiesetup`, `setup`; `scripts/ai/setup/` `animalsetup`, `archersetup`, `criersetup`, `killpcssetup`, `modsetup`, `questiesetup`, `setup`, `spellsetup`; `scripts/include/anchors.inc`, `canstack.inc`, `npccast.inc` (2), `speech.inc`

- A bare `ReadConfigFile("npcdesc")` from a package script looks inside that package. These includes are used by package AIs (the Warrior for Hire) and package scripts (the packet hooks), so the files were not found: `stacking.cfg` 1,540 times, `npcdesc.cfg` 90, `speechgroup.cfg` 45.
- All of them now read `::npcdesc`, `::speechgroup` and `::stacking`.
- Effects: `IgnoreCprops` in `stacking.cfg` was never applied on those paths, so some items didn't stack; Warriors for Hire had no speech group; and `modsetup.inc`'s equip-from-template lookup silently did nothing for them.
- **`modsetup.inc`:** the equip block was `if(1)`. With the path fixed it would re-equip a Warrior for Hire from its template on every AI start and hand out free items, so it now skips NPCs with the `WarriorForHire` CProp. Other NPCs behave as before (a non-package AI already found `npcdesc`).

### 30.4 Monster AIs recursing on ghosts

**Files:** `scripts/ai/combat/fight.inc`, `chaosfight.inc`, `raiderfight.inc` ("exceeded maximum call depth": `killpcsclock.ecl` 6, `wolf.ecl` 1)

`Fight()` on a dead, hidden or concealed opponent went straight to `post_combat()`, whose `look_around()` picked the same ghost a second later and called `Fight()` again, one call deeper each time, until the core killed the AI. `Fight()` now returns at once for such an opponent, leaving war mode and clearing its opponent as `post_combat()` would (the events aren't switched yet; `prepare_for_fight()` comes after), but without `post_combat()`'s `look_around()`.

### 30.5 Staff demotion

**File:** `scripts/include/privs.inc` (18 errors, `makestaff.ecl`)

`who.disable(who, priv)` passed the mobile as the privilege name, so a demotion revoked the privileges but never disabled them. It is `who.disable(priv)`.

### 30.6 Omega Cache item list runaway

**Files:** `pkg/opt/omegacache/omegacache.inc`, `scripts/include/resourcemanager.inc` (1 runaway, `omegacache.ecl`)

`GetLeasedAmount()` scans the whole house DataFile for one item key, and the item list, `GetStoredAmountByObjtype()` and the crafting material pickers called it once per key. New `GetLeaseTotals()` sums every live lease per item key in one pass (cleaning up expired leases as `GetLeasedAmount()` does), and `LeaseItemKey()` takes the item key out of `RL#<item_key>|<lease_id>`. `GetStoredAmount()` takes the totals as an optional fourth parameter; without it, it behaves as before.

---

## 31. Kill Credit - Every Kill Records Its Killer

**Files:** `scripts/include/damages.inc`, `scripts/include/dotempmods.inc`, `pkg/opt/summoning/processpoisonmod.src`, `pkg/systems/combat/poisonhit.src`, `pkg/systems/combat/poisononhit.src`, `scripts/control/poisoncontrol.src`, `scripts/items/walkon_pfield.src`, `scripts/ai/tamed.src`, `scripts/misc/chrdeath.src`, `pkg/std/spells/mindblast.src`, `pkg/opt/shilitems/vengeance.src`, `pkg/opt/necro/sunderingsword.src`, `pkg/opt/farming/swarmofbees.src`

### 31.1 The defect

The live log had 53 `death.ecl` errors. The local `death.lst` (same build) places them:

- **50 at PC 14534:** `RegisterChampionSpawnKill()`'s `SystemFindObjectBySerial(GetObjProperty(corpse, "KilledBySerial"))`. 43 corpses had no `KilledBySerial`, 7 had it saved as Uninit.
- **3 at PC 12191:** `GetAltarByCorpse()` on a champion with no `altar` CProp, i.e. a relic champion (section 32.4).

`KilledBySerial` is written only by `ApplyTheDamage()`, on the killing blow. The corpse copies it from the mobile, and `RegisterNPC()` (KilledNPCS), the loot code (personal powerhour double drops, `#PPHH`) and the champion altar read it.

- **Poison** ticks used `ApplyRawDamage()` and recorded nobody. Its fallback `SetObjProperty(who, "KilledBy", ...)` ran after the lethal tick, when the corpse already existed, and wrote a name, not a serial.
- **Dropped poison weapons** (`poisonhit.src`), poison fields (`poisoncontrol.src`, `walkon_pfield.src`) and pet poison (`tamed.src`) didn't even pass the poisoner to `SetPoison()`.
- **Mind Blast, Vengeance and Sundering Sword** dealt raw damage.
- **`ApplyTheDamage()` saved a missing attacker** (`by_who` 0 or an error) as `by_who.serial`, which is Uninit, before its `!by_who` check.

### 31.2 `RecordKiller()`

New in `damages.inc`. It writes `KilledBy` and `KilledBySerial` only when `by_who` is a mobile, and `ApplyTheDamage()` uses it in both places. Pets and Warriors for Hire are already swapped for their master before it runs. A killing blow with no attacker erases both, so the death isn't credited to an earlier killer (the champion altar then falls back to the hitlist).

`chrdeath.src` now also erases the ghost's `KilledBySerial` after `RecordCharacterDeath()`, as it already did `KilledBy`. It was never erased, so a player's next death with no recorded killer (raw damage) was logged as the previous killer's. The corpse keeps its own copy for `death.src`.

### 31.3 Poison goes through `ApplyTheDamage()`

- `SetPoison(who, plvl, msg, caster, shuriken, credit)`. The new `credit` defaults to `caster`. `caster` still picks the damage table, duration and protection rule in `processpoisonmod`; `credit` only decides who gets the kill. `SetPoison()` passes it on, and `processpoisonmod`'s new `SetPoisonCredit()` writes `PoisonedBy` (name) and the new `PoisonedBySerial` only when the poisoning lands: a fresh one, or one stronger than the current `psnDmg`, which it replaces. A blocked one (safe area, immunity) or an equal or weaker one on a poisoned victim leaves the credit with whoever's poison is ticking. A landed poisoning with no poisoner, or a self-poisoning, erases `PoisonedBySerial` and `PoisonedBy`, so a trap doesn't credit whoever poisoned the mobile last time (`death.src`'s `RegisterNPC()` falls back to the `PoisonedBy` name for a poison death with no `KilledBySerial`).
- New `PoisonTick()` in `processpoisonmod.src` reads `PoisonedBySerial` each tick. With a poisoner it calls `ApplyTheDamage(who, poisoner, dmg, DMGID_POISON, 0)`: credit, hitlist, repsys and lotto damage. The new last parameter of `ApplyTheDamage()`, `pvp_cut` (default 1), set to 0 skips the global PvP 0.6, so poison on a player does what it did as raw damage. With none, or a self-poisoning, it stays `ApplyRawDamage()`, after the same `IsImmunedFromThisDamageType()` check `ApplyTheDamage()` makes, so a poison-immune mobile resists poison whether or not anyone gets the kill (Thief ticks, `DMGID_NO_RESIST`, still pierce it). The tick's own `StoreDamage()` call is removed (`ApplyTheDamage()` stores it), and so is the dead `KilledBy` line.
- A Thief caster's ticks add `DMGID_NO_RESIST`, so Thief poison still pierces immunity as `PoisonProtected(..., 1)` already makes it pierce poison protection.
- Callers: `poisonhit.src` passes the wielder as credit only, so a dropped poison weapon keeps the flat damage table in a Thief's hands; `poisoncontrol.src` and `walkon_pfield.src` credit the field's caster (`poisoncontrol` keeps a `poisoner` apart from `creator`, which falls back to the first victim, and reads `Creator` and `ByTrueMage` every tick, as `firecontrol` does in 35.4; `walkon_pfield` only runs when someone steps on the field, after the props are set); `tamed.src` credits the pet, which `ApplyTheDamage()` turns into its master; `poisononhit.src` (the Venomous/Envenomed armor enchantment, which poisons the attacker) credits the wearer, and cursed armor, which poisons the wearer, credits nobody.

### 31.4 Mind Blast, Vengeance, Sundering Sword

- **Mind Blast:** `ApplyTheDamage(cast_on, caster, ..., DMGID_MAGIC)`. A blast turned back by a superior intellect stays raw, since `ApplyTheDamage()` would apply the PvP cut to a player's own blast.
- **Vengeance:** `DMGID_MAGIC + DMGID_NO_RESIST`, so only the PvP cut and the credit change. Now includes `include/damages`.
- **Sundering Sword:** `DMGID_MAGIC + DMGID_NO_RESIST`, so the summon always dies, credited to the wielder.

### 31.5 Left alone

- **Song of Beckon** only kills the bard's own summon when the song ends; crediting it would give the bard karma for it.
- **Swarm of Bees** is a beehive hazard: `gethoney.src` never sets its `creator`. Only the missing realm was added to its mobile scan.
- **`thiefpoisonhit.src` and `chanceperm()`** are dead code: nothing calls `chanceperm()`, and no item on live carries `thiefpoisonhit`.
- **Monster poison AIs** (`poisoncombatevent.inc`, `spidercombatevent.inc`) still pass no poisoner; their ticks stay raw.

### 31.6 Pets take the PvP slowdown

**Files:** `scripts/include/damages.inc`, `pkg/systems/combat/include/hitscriptinc.inc`

A player's pet hitting a player took neither PvP cut. `ApplyTheDamage()` tested for PvP before it swapped a pet for its master, so it saw an NPC attacker and skipped the 0.6, and `CalcPhysicalDamage()`'s 0.4 melee cut only tested `attacker`. Both now treat anything with a `master` CProp (`FindMaster()`: tamed pets, summons, Warriors for Hire) as its master:

- `ApplyTheDamage()` does the master swap first and the PvP test after it. Pet melee and pet spells on a player take the 0.6; pet poison doesn't (`pvp_cut` 0, section 31.3).
- `CalcPhysicalDamage()` looks up the attacker's master for the 0.4 test. Pet melee on a player now takes 0.4 x 0.6 = 0.24, the same as a player's melee; pet spells take the ÷3 every spell on a player takes, then the 0.6.
- A player hitting someone else's pet is unchanged (the target is an NPC), and so is a pet whose master is an NPC.

---

## 32. Champion Spawns - Kill Tally, Reward Bags, Relic Champions

**Files:** `pkg/opt/champspawns/include/death.inc`, `rewards.inc`, `skulls.inc`, `scripts/control.src`, `pkg/opt/ArtifactSystem/championrelic.src`; 32.6 adds `scripts/textcmd/player/champkills.src` and `config/command_synopses.cfg`

### 32.1 Before

- `RegisterChampionSpawnKill()` kept a per-player kill count on the altar (`players`), but nothing read it: `GiveRewardsToPlayers()` in `rewards.inc` was commented out, and never called, in the initial import (`f8450b9`). The reward was the gold rain and the corpse loot.
- Every altar in the live save held `players` as an error ("Property not found"): ten minutes after a champion died, `ProcessChampionCorpse()` erased it, and the next leftover spawn kill wrote `GetObjProperty()`'s error back.
- Credit never expired, so a player who left an altar after 500 kills was still owed for them when someone else finished it a day later, though the altar had decayed back to wave 0 in the meantime.

### 32.2 The tally

- `players[serial] := {kills, systime of the last kill}`. An entry from before the patch (a bare count) is read as a count, and when the champion dies it counts as a kill at the newest time on the tally, so an altar that was mid-run at the deploy still pays for it.
- Only kills while the altar is `active` count. The killer is the corpse's `KilledBySerial`, or failing that the most recent hit on its hitlist; only players count (`IsChampionCreditable()`: no `npctemplate`, no `cmdlevel`). The read-modify-write runs under `set_critical`, because an area spell can kill many spawn creatures at once.
- A tally that isn't a dictionary is rebuilt. A missing altar or `sAltarSerial` returns.

### 32.3 Reward bags

`ProcessChampionCorpse()` reads the altar first, before the gold rain and the ten-minute sleep, calls `GiveRewardsToPlayers(altar)` and then erases the tally.

- **Eligible:** every character with at least `CHAMP_BAG_MIN_KILLS` (150) kills whose last kill was at most `CHAMP_CREDIT_WINDOW` (7200 seconds, two hours) before the newest kill on the tally. The newest kill is the reference, not the champion's death, because the champion fight adds no kills: measured from the death, a long champion fight paid nobody. Per character; there is no per-account limit, because credit needs killing blows. A player with kills in the window but fewer than 150 is told their count.
- **Contents (cumulative):**

| Kills | PowerTome | TranscendScroll | StatPotion | vanitytoken | Present (gift1-3) |
|---|---|---|---|---|---|
| 150 | - | 1 | - | 1 | 1 |
| 300 | - | +1 | - | +1 | - |
| 500 | - | - | +1 | - | - |
| 750 | +1 | - | - | - | - |
| 1000 | +1 | - | +1 | +2 | +1 |
| **1000+ total** | **2** | **2** | **2** | **4** | **2** |

- **Delivery:** the bag (`0x0E76`, "A Champion Reward Bag", random hue) goes in the backpack of an online player, or into the bank box (`util/bank` `FindBankBox()`) if the player is offline or the backpack is full. The script sets `SCRIPTOPT_CAN_ACCESS_OFFLINE_MOBILES` for that. Items are created one at a time. If the backpack takes the empty bag but not all of its contents (weight or item limit), the bag moves to the bank box and the missing items are created there. Whatever still can't be created, or a bag neither container takes, is logged with `SysLog()`.
- The `:Events:bag` include (the old random decoration bag) is no longer used and was dropped; `EligibleForLoot()` was removed.

### 32.4 Relic and staff champions

The four champion templates carry `CProp Champion`, so a champion from a relic or from staff went through `ProcessChampionCorpse()` too and logged `GetAltarByCorpse()`'s error. `GetAltarByCorpse()` returns 0 when there is no `altar` CProp, and such a champion only rains gold. `championrelic.src` marks its boss with `SummonedByRelic` (the summoner's serial). Relic champions get no bag and the reduced corpse loot of section 33.

### 32.5 The last wave no longer completes early

Every 10 minutes, if the current wave is under 90%, `control.src` resets it. Above 20% it called `createNextRedSkull()` and then `destroyLastRedSkull()`, meaning to keep the red skulls. On wave 16, `createNextRedSkull()` instead marks the altar `#completed` and wipes the red skulls, so the champion spawned with the last wave only about a fifth done. New `clearWhiteSkulls()` in `skulls.inc`: above 20% only the white skulls go; at 20% or less one red skull goes, as before. `createNextWhiteSkull()` and `createNextRedSkull()` now call it instead of repeating its three lines.

### 32.6 Seeing your count: `.champkills` and milestone messages

**Files:** `scripts/textcmd/player/champkills.src` (new), `pkg/opt/champspawns/include/death.inc`, `rewards.inc`, `scripts/textcmd/admin/akill.src`, `config/command_synopses.cfg`

Players had no way to see their tally. Two additions:

- **Milestone messages.** `RegisterChampionSpawnKill()` messages an online killer when their count reaches a tier: 150 "you have earned a Champion Reward Bag", 300/500/750 "your Champion Reward Bag grows", 1000 "is as full as it gets". The check runs after `set_critical(0)`, one `in` test per kill.
- **`.champkills` (Player).** Finds the nearest active altar (`champaltarnocreate`, 0x9101, `active` set, the champion fight included) within 50 tiles; minions spawn up to 37 tiles from the altar on each axis. It reads the caller's tally entry and says the count, how many more to 150 or to the next tier, and how long ago the last kill was. If that kill is more than `CHAMP_CREDIT_WINDOW` before the newest kill on the tally, it warns that the kills won't pay without another kill. An old bare-count entry shows the count with no time, and pays as in 32.2. No altar in range: "There is no active champion altar near you."
- `rewards.inc`: new `ChampionBagTiers()` (`{CHAMP_BAG_MIN_KILLS, 300, 500, 750, 1000}`, the thresholds in `GiveChampionBag()`) and `ChampionTallyNewest(players)`, which `GiveRewardsToPlayers()` now uses instead of its own loop.
- `scripts/textcmd/player` is already a Player `DIR` in `config/cmds.cfg`. `command_synopses.cfg` was regenerated: "Show your kills toward a Champion Reward Bag at the nearest active champion altar".
- **`.akill <range> credit` (Administrator), for testing altars.** Staff kills never counted: `mob.kill()` leaves no `KilledBySerial` or hitlist, and `IsChampionCreditable()` refuses a killer with a cmdlevel. With `credit`, `.akill` stamps each champion minion (`iChampSpawn`) with `KilledBySerial` and `StaffChampCredit` set to the caller before killing it, and `FindChampionSpawnKiller()` accepts a staff killer only when the corpse's `StaffChampCredit` matches. The tally, milestones, `.champkills` and the reward bag then work for the Administrator as for a player. One `LogCommand` line per use gives the range and the number of minions credited. Plain `.akill <range>` is unchanged and credits nobody. The three kill loops now share `AKillMobile()`. `.kill` is not changed.

---

## 33. Champion Loot Rebalance

**Files:** `config/nlootgroup.cfg` (lootgroups 150, 151), `config/npcdesc.cfg` (4 templates)

| | Lootgroup 150 (Barracoon, Erebus) | Lootgroup 151 (Rikktor, Nyx) |
|---|---|---|
| PowerTome | 3 -> **2** | 4 -> **3** |
| TranscendScroll | 7 -> **4** | 10 -> **4** |
| StatPotion | 5 -> **4** | 6 -> **5** |
| vanitytoken | 7 -> **0** | 10 -> **0** |
| ChampionPresents | 1d2 -> **0** | 2d2 -> **0** |
| `magicitemchance` | 99 -> **85** | 99 -> **95** |

Gold in the corpse (60,000 / 120,000), gear, GM items, Wonder Items, magic item levels (10 / 11), the gold rain and the champion skull are unchanged. The `ChampionPresents` group is now unused.

**Per spawn, corpse plus bags, lootgroup 150** (about 2,250 kills, split evenly):

| Spawn | Tomes | Scrolls | Stats | Tokens | Presents |
|---|---|---|---|---|---|
| Before | 3 | 7 | 5 | 7 | 1-2 |
| Solo | 4 | 6 | 6 | 4 | 2 |
| 2,000 / 225 | 4 | 7 | 6 | 5 | 3 |
| 2 even | 6 | 8 | 8 | 8 | 4 |
| 3 even | 5 | 10 | 7 | 6 | 3 |
| 5 even | 2 | 14 | 4 | 10 | 5 |
| 10 even | 2 | 14 | 4 | 10 | 10 |

Lootgroup 151 adds one tome and one stat potion to each row. Duos and trios that do most of the killing get more tomes than before; groups of four or more get fewer. Scrolls, tokens and presents scale with the number of players over 150 kills.

---

## 34. Powerhour - Gold Piles and Overlapping Powerhours

**Files:** `scripts/include/starteqp.inc`, `pkg/opt/powerhour/textcmd/player/setph.src`

- **Gold piles:** `CreateFromStackString()` (the `Stack` lines: 91 of them are GoldCoin, the rest gems, ash, arrows and fertile dirt) doubles the drop chance under either Hunting powerhour (`PHH || #PPHH`), but doubled the pile only under `!PHH || !#PPHH` being false, i.e. with both running. It is now `PHH || #PPHH` too, so a single Hunting powerhour doubles gold piles, including the champions' corpse gold. Both lines date from the initial import.
- **`.setph`:** nothing stopped a personal powerhour of the same type as the server-wide one, which then added nothing (the only thing overlap did was the gold pile bug above). After the type is chosen, `.setph` refuses when `PH_ActiveType()` or `PH_PendingType()` (the 2-minute warning) is that type. A different type is allowed. The scheduler can still start a server-wide powerhour during a personal one; the overlap is just lost time.

---

## 35. Champfire - Fixed Damage, Elemental Protection Only

**Files:** `scripts/include/npccastspells.inc` (`SpecialChampFire()`), `scripts/include/spelldata.inc` (`ApplyElementalDamage()`), `scripts/control/firecontrol.src`

`champfire` is cast by Rikktor, Nyx, The Summoner (`summonboss`) and the four Soul Whisperers. Each cast rolls one of six elements (earth, water, fire, air, necro, holy) and hits every player in 20 tiles and line of sight with a bolt, then lays 21 fields of that element around them (60 seconds, a tick every 5 seconds).

### 35.1 The fields hit with the victim's own skills

`SpecialChampFire()` never gave its fields a `Creator`, so `firecontrol.src` made the first mobile in the field the caster: every tick was the player damaging themselves, with their own Magery and Eval Int, and `ApplyTheDamage()` cut it by the PvP 0.6. About 20 a tick to a level 6 Warrior with no resist, 2 to a level 6 Mage with 150. Each field now gets `Creator` (the caster's serial), and firecontrol looks the caster up again each tick, since the fields outlive a champion that dies; once the caster is gone a field falls back to the old behaviour.

### 35.2 Damage by class

The bolt went through `CalcSpellDamage()` (circle 8, /3 against players) and `Resisted()`, so the class matchups swamped it. Against no elemental protection, from a Rikktor or Nyx bolt:

| | Level 6 Warrior, 0 resist | Level 6 Paladin, 150 resist | Level 6 Mage, 150 resist |
|---|---|---|---|
| Bolt before | ~380 | ~89 | ~31 |
| Field tick, 35.1 fix only | ~56 | ~12 | ~4 |

Eval Int 200 doubled the hit against a Warrior with no resist and the class penalty added 1.75x; a Mage resists at 2.5x the chance and divides what lands by 2.5. The Summoner and the Soul Whisperers (`IsMage` commented out) did about a third of the Rikktor/Nyx bolt.

### 35.3 Fixed dice, no `Resisted()`

- `ApplyElementalDamage()` takes an optional `unresistable`. With it, the damage left after elemental protection goes straight to `ApplyTheDamage()`: no Magic Resistance roll, no class or Eval Int adjustment. The protection step is unchanged (a Mage caster pierces 3 points per level; 100% or more heals).
- `SpecialChampFire()` rolls `CHAMPFIRE_BOLT_DICE` (`10d10+115`, about 170) instead of `CalcSpellDamage()`, and passes `unresistable`. Cursed equipment doubles the roll (it used to double the circle). Magic immunity items still block it (`IsProtected()`), and hidden targets and no-PK areas are still skipped (`ApplyElementalDamage()`).
- Each field gets `FieldDice` (`CHAMPFIRE_FIELD_DICE`, `8d8+6`, about 42) and `Unresistable 1`. `firecontrol.src` rolls a field's `FieldDice` when it has one (otherwise `power` d8 as before) and passes `Unresistable` on. Other fire fields are unchanged.

| Protection in the rolled element | Bolt (Rikktor, Nyx) | Field tick (Rikktor, Nyx) | Bolt / tick (Summoner, Soul Whisperers) |
|---|---|---|---|
| 0% | ~200 (147-253) | ~50 (16-82) | ~170 / ~42 |
| 50% | ~116 | ~29 | ~85 / ~21 |
| 95% | ~40 | ~10 | ~9 / ~3 |
| 100%+ | heals | heals | heals |

The same for every class. No Magic Resistance skill gain from champfire any more (`Resisted()` awarded it).

### 35.4 Field props read every tick

`CreateItemAtLocation()` starts a field's `firecontrol.src` at once, and the caster only sets `Creator`, `FieldDice`, `Unresistable`, `specialELE` and `ByTrueMage` afterwards, outside `set_critical`. firecontrol read them once at startup, so a field whose script ran before the caster finished kept the defaults for its whole life: 2d8, resisted, the victim as the caster, plain fire. It now reads all of them every tick (a few CProp reads every 5 s per field). `SetScriptController()` is still called once, when the caster is first found, and a field whose caster can't be found (a dead champion's fields) makes each victim take the hit as its own. It used to make the first victim the caster, so the field's damage and deaths among the other victims were credited to that player. This applies to every fire field (Wall of Fire, Fire Field and the NPC fields), not just champfire's.

---

## 36. Treasure Maps - Level 6 Bards Find Level 7 Chests More Often

**File:** `pkg/std/treasuremap/digtreasure.src`

A Bard digging up a level 6 map rolls for a level 7 chest instead. The chance was the bard level in percent (`Random(100) < IsBard(character)`), so 1-6%. It is now a separate `lvl7_chance`: still the bard level for levels 1-5, and 8% at level 6 (was 6%). Other classes and other map levels are unchanged.

---

## 37. Rangers - Taming, Pet Resurrection, Full Heals, Magic Fish

**Files:** `scripts/include/classes.inc`, `pkg/std/taming/taming.src`, `pkg/std/veterinary/vet.src`, `pkg/std/healing/healing.src`, `pkg/std/fishing/magicfish.src`

From player feedback: a ranger soloing a champion spends most of the run healing or resurrecting 7,000-9,000 HP dragons, with resurrections failing 10+ times in a row, and the magic fish cooldowns block bandages and each other. The heal fish cooldown cut the player also asked for is not in this patch.

### 37.1 Taming and pet resurrection scale with ranger level

New `RANGER_LEVEL_TAME_BONUS := 2` in `classes.inc`. `taming.src` subtracts `2 * level` from the tame difficulty, next to the `PREV_TAMED_MINUS` cut, so the calming roll (`difficulty + 10`) gets it too. `vet.src` `ResAnimal()` rolls at `tameat + 10 - 2 * level`. The skill needed to try at all (`tameskill` for taming, `tameat` Veterinary for resurrection) is unchanged.

For a 150 creature at 150 skill (`GetDifficultyCheckChance()`, with the ranger's second roll on a class skill):

| Ranger level | Tame / res roll | Chance | With second roll | Calming roll (150 Animal Lore) |
|---|---|---|---|---|
| none | 160 | 25% | 44% | 170: 0% |
| 1 | 158 | 30% | 51% | 168: 5%, 10% |
| 6 | 148 | 55% | 80% | 158: 30%, 51% |

Before, a 150 creature's calming roll could never succeed at 150 Animal Lore, so a ranger taming a dragon always had the 50% "angered the beast" roll (`IsABadAss()`).

`ResAnimal()` now takes `BANDAGES_TO_RES` (5) bandages and gives back 5 bloody ones. It checked for 5 but took `BANDAGES_TO_HEAL` (1).

### 37.2 Full heal chance by level

New `RANGER_LEVEL_FULL_HEAL_CHANCE := 3`. A ranger's pet bandage (`vet.src` `TryToHeal()`) fully heals when `RandomInt(100) < 3 * level`, else heals `healed * count / 10 * 2` as before. That replaces a 24-case `case(RandomInt(24))` with full heals on 7 and 19 (8.3% at every level). Level 1-2 get less than before (3%, 6%), level 3 about the same (9%), level 6 gets 18%.

### 37.3 Rangers bandage right after a magic fish

`healing.src` `CheckIfCanUseBandage()` ignores `#magicfish` for a ranger. A ranger's bandage on themselves goes to `vet.src`, which refuses a self target, so this can't stack a fish heal with a bandage self-heal; it only affects bandaging pets and other players. Every other class, Mystic Archers included, still waits out the fish. One bandage at a time is still enforced by the bandage reservation and `Attach()` in `vet.src`.

### 37.4 Each fish type has its own cooldown for rangers

`magicfish.src` reads `spellnum` before the cooldown check. A ranger's cooldown key is `#magicfish<spellnum>` (`#magicfish4` heal, `#magicfish11` cure, `#magicfish44` invisibility, ...), everyone else keeps `#magicfish`. `CheckIfCanUseFish()` and `CanUseFish()` take the key. The delays are unchanged: 20 s, 0 for a ranger's buff fish, 5 s for a ranger's teleport. So a ranger can eat a heal, a cure and an invisibility fish back to back, and the same kind twice still waits. Rangers no longer set `#magicfish` at all, which is a second reason their bandages aren't held up.

---

## 38. Townstones - Every Character Can Be a Citizen

**Files:** `pkg/opt/townstones/citizens.inc` (new), `tstone.src`, `tstone.inc`, `electionwatch.src`, `textcmd/admin/removetownmember.src`, `textcmd/admin/townbankstatus.src`, `textcmd/admin/cleartownmembers.src`

### 38.1 Before

A townstone's `citizen` CProp listed account names. `Citizenship()` refused any character whose account was already on the list ("You can't join more than once"), the population was the number of accounts, and a vote rune stored the voter's account name, so it was one citizen and one vote per account. On live most players keep their characters on separate accounts, so the limit barely held anyone back, while players with several characters on one account could only bring one of them into a town. Leaving with one character (`CanselCityzenship()`) removed the account entry and its vote even if another character on the account was still a citizen, and `.removetownmember` did the same.

### 38.2 One entry per character

New `citizens.inc`, included by `tstone.inc` (so by `tstone.src` and `electionwatch.src`) and the three staff tools:

- An entry is `{character serial, account name}`: the account stays in the list. Helpers `CitizenSerial()`, `CitizenAccount()`, `NewCitizenEntry()`, `IsCitizenChar()`, `RemoveCitizenChar()`.
- `Citizenship()` refuses only a character that is already on the list. `CanselCityzenship()` (leaving and resigning) removes only that character's entry.
- `populacao` is the number of citizen characters, so the 3-member election minimum and the vote percentage count characters.
- **Votes.** `voteto()` stores the character serial in the rune's `serial` and the account in a new `account` prop. `IsVoteRuneOf(runa, character)` matches a rune by serial, or, for a rune from before the deploy (`serial` is a string), by account.

### 38.3 Converting the old lists

`LoadCitizenList(stone)` replaces the `GetObjProperty(stone, "citizen")` reads that use the list: in `tstone.src` when the stone is used, in `electionwatch.src`, and in the staff tools. If the list still holds account names, each character on such an account whose `town` CProp is this town gets an entry, an account with none left is dropped, and `populacao` is set to the new list size. A list that is already per character is returned as is. No wipe or staff step is needed.

A vote cast before the deploy in an election or poll that is still running holds an account name. It keeps blocking every character on that account until that vote ends, so nobody votes twice across the deploy.

### 38.4 Staff tools

- **`.removetownmember`** removes only the targeted character and its vote, drops entries whose character is gone or no longer wears the town, and sets the population to the list size. `RemoveCitizenAccount()` and `AccountHasTownMember()` are gone.
- **`.townbankstatus` member list** shows one row per citizen character, with its account. Removing a row removes that character (new `RemoveTownMemberByChar()`, replacing `RemoveTownMemberByAccount()`), not every character on the account. The mayor check compares the character serial (`IsMayorAccount()` is gone). Old-format votes are matched by account in the new `IsRemovedVoter()`. `GetTownPopulation()` converts an old list before reading the population.
- **`.cleartownmembers`** matches citizens by character (`IsCitizenChar()`) instead of by account, after converting the list. `IsCitizenAccount()` is gone.

The datafile sync (`townstone_data` `citizen` prop) stores whatever is on the stone; `createtownstone` restores either format and `LoadCitizenList()` converts it on first use.

---

## 39. ZH3.0 3.1.4 - Reviewed, Not Ported

ZH3.0 `9fbf224` (Patch 3.1.4) was read theme by theme against 2.5.

- **Themes 1-7, 10 (powerhour scheduler, `.phadmin`, `.ph`, personal powerhour resume, classes, `.classbonusinfo`, Warrior for Hire, areas, `.speedwalk`, house travel, Eon-Prism, Powerplayer curve).** Ported to ZH3.0 *from* 2.5 1.1.4, so 2.5 already has all of them.
- **Theme 11 (mayor resignation).** Ported to ZH3.0 from 2.5 `d11f181` (section 3).
- **Theme 13, areas `SaveRealmPolicies`.** ZH3.0 now skips the cache refresh on an `.areas` Save that wrote and pruned nothing. 2.5 has the equivalent pattern (`PruneStaleRealmPolicies()` always ends in `RebuildPolicyMaskCache()` and a version bump), but a no-edit Save is rare and costs each running script one re-read of a ~200-entry dictionary. In 2.5 that Save is also the in-game way to repair a missing cache. Not worth changing.
- **Theme 9, same-account characters are house owners.** 2.5 has the same bug (owner checks by character serial only), but its housing is a different, older system: no sign `method.src`. Owner checks are `IsCowner` / `IsFriend` in `pkg/std/housing/utility.inc` plus inline `ownerserial` tests in `sign.src`, `ssign.src`, `securecont.src`, `changeowner.src`, omegacache and `magictrap.src`. It's a rewrite rather than a port and was deferred to a later patch. Theme 13's trap sign lookup depends on it and was deferred with it.
- **Theme 12, Runebook / Runic Atlas Transcription Scrolls.** A new vanity feature, not a fix. Not ported.
- **Theme 8 (duplicate elven door names).** ZH3.0-only data.

---

## 40. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `pkg/opt/townstones/tstone.src` | 3 | `:gumps:yesno` include; mayor screen button 3001 "Resign and leave town"; new `ResignMayorship()` (confirm, re-check, clear mayor, reset `nextcitizenelection`, `CanselCityzenship()`, broadcast); 38: `LoadCitizenList()`, join and leave per character |
| `pkg/opt/powerhour/include/powerhour.inc` | 4 | New `PPH_ResetTime()`; `PPH_IsEligible()` and `PPH_SecondsUntilEligible()` both use it; weekday from the use time |
| `pkg/opt/warriorforhire/include/wfhcommon.inc` | 5.1 | `IsCurrentWfhHeart()` refuses unstamped hearts and hearts with no backup |
| `scripts/ai/highpriest.src` | 5.2, 5.3 | Heart from anyone but its master is destroyed; memory revival applies `resnum` stat loss, floor 10 |
| `pkg/systems/combat/include/hitscriptinc.inc` | 6.1, 10, 13, 31.6 | `WFHModifyDamage()` reads each master once as `CInt` and compares only when set; `ModByProt()` uses `GetPhysicalProtection()`; Reactive Armor trigger clears `#ReactiveArmorPid` and its icon; the 0.4 PvP melee cut tests the attacker's master (31.6) |
| `pkg/opt/warriorforhire/warrior.src` | 6.2, 26 | `gWeaponAmmoTypes` / `gHealPotionPowers` caches; new `WeaponAmmoType()`, `LoadHealPotionPowers()`; `HasBow()` and `HealPotionPower()` use them; new `ResolveAttacker()` used by `fight()`'s retarget and the `ENGAGED` / `DAMAGED` handlers, no retarget onto the master (26) |
| `pkg/opt/ArtifactSystem/eonprism.src` | 7 | `include/itemutil`; refused unless in the user's backpack |
| `scripts/ai/merchant.src` | 8 | `MERCHANT_KEYRING_OBJTYPE`; new `CanMerchantBuy()`; `SellAll()` and `SellBag()` buy empty, unowned keyrings |
| `pkg/opt/alryc/textcmd/test/aimovementsim.src` | 9 | `RebuildPolicyMaskCache("britannia")` after the run |
| `pkg/opt/alryc/stresstest/areastressworker.src` | 9 | `RebuildPolicyMaskCache("britannia")` as each worker exits |
| `scripts/include/spellprots.inc` | 10, 10.6 | New: `#SpellPoisonImmunity` / `#DragonSkin` `{ level, pid }` helpers, `GetPoisonImmunityLevel()`, `GetPhysicalProtection()`, `ResyncEquippedProt()`; `SpellProtCasterLevel()`, `SpellProtDuration()` (10.6) |
| `scripts/include/dotempmods.inc` | 10, 11, 15, 31 | Includes `spellprots` and `buffs`; `PoisonProtected()` uses `GetPoisonImmunityLevel()`; `WipeMods()` clears spell poison immunity, Dragon Skin, Reactive Armor, Incognito name; `poisonimmunity` / `dragonskin` buff icons (now in `buffs.inc`); `CurePoison()` turns the poison icon off; buff functions moved out; `SetPoison()` `credit` parameter, passed to `processpoisonmod` (31) |
| `pkg/opt/holybook/sanctuary.src` | 10.3, 10.6 | Spell level via `SetSpellProt()`, applied when not weaker than current immunity; buff icon; ends only its own cast; level and duration from caster class level (10.6) |
| `pkg/opt/earth/antidote.src` | 10.3, 10.6 | Same as Sanctuary; immunity goes to the target instead of the caster; level and duration from caster class level (10.6) |
| `pkg/opt/versebook/Dragon_Skin.src` | 10.4, 10.6 | Rewritten as a one-play timed buff: 20 stamina per target, buff icon; protection = bard level 1-6 and 2-10 min (10.6) |
| `pkg/opt/versebook/include/versefunctions.inc` | 10, 14 | Includes `spellprots`; `AdjustForPhysicalDamage()` uses `GetPhysicalProtection()`; new `EndVerse()` |
| `scripts/control/skilladvancerequip.src` | 10.2 | Physical Protection recalculated only for items that carry it; Dragon Skin special case removed |
| `scripts/textcmd/player/prots.src` | 10.2 | `.prots` shows the higher of gear and spell for Physical Protection and poison immunity |
| `pkg/packethooks/megacliloc/mobiledata.src` | 10.2 | Mobile tooltip protection values include active spells; includes `spellprots` |
| `pkg/std/spells/reactivearmor.src` | 13 | `#ReactiveArmorPid` ownership, buff icon, `include/dotempmods` |
| `scripts/playermanager.src` | 12, 14, 15.4 | Login resyncs players' `PermPoisonImmunity` / `PhysicalProtection` from equipment; `EndBeastalBond()`; re-sends the Magic Reflection icon |
| `pkg/opt/versebook/Beastal_Bond.src` | 14 | Fumble leaves the loop; offline stops it; saved start position; realm-aware search; dead Reporting call removed; `EndVerse()` |
| `pkg/opt/versebook/Beast_Bond_View.src` | 14.2 | Always unfreezes, unconceals, clears `#ViewingBeast` when it ends |
| `pkg/opt/versebook/include/beastalbond.inc` | 14.3 | New: `SaveBeastalBondOrigin()`, `ReturnToBeastalBondOrigin()`, `EndBeastalBond()` |
| `scripts/misc/logoff.src` | 14 | `EndBeastalBond()` replaces the view-only cleanup |
| `scripts/misc/reconnect.src` | 14 | `EndBeastalBond()` replaces the view-only cleanup |
| `scripts/include/buffs.inc` | 15.1 | New: `BuffOn()`, `SendBuffPacket()`, `BuffOff()` moved from `dotempmods.inc`; `poison` text cliloc blanked |
| `scripts/include/spelldata.inc` | 15.1, 15.4, 20.2, 35 | `ApplyElementalDamage()` optional `unresistable` skips `Resisted()` (35); includes `buffs`; `Reflected()` turns the Magic Reflection icon off; `SmartAoE()` / `SmartBoost()` hand NPC casters on; new `IsBystanderNpc()`, `IsMonsterSide()`, `NpcPetCanHitPlayer()`, `NpcSmartAoE()`, `NpcSmartBoost()` |
| `pkg/std/spells/nightsight.src`, `pkg/opt/holybook/lightofday.src`, `pkg/opt/earth/owlsight.src`, `pkg/opt/songbook/songoflight.src`, `pkg/std/alchemy/blackpotion.src` | 15.2 | Night sight buff icon |
| `pkg/opt/necro/darkcloak.src`, `pkg/systems/combat/blindingonhit.src`, `pkg/systems/combat/blindingscript.src` | 15.2 | Blinding turns the night sight icon off |
| `pkg/opt/summoning/processpoisonmod.src` | 15.3, 31 | Poison buff icon on a fresh poisoning; ticks through new `PoisonTick()` (`ApplyTheDamage()` with a poisoner, Thief ticks no-resist), separate `StoreDamage()` and post-death `KilledBy` removed; `SetPoisonCredit()` writes `PoisonedBy` / `PoisonedBySerial` only when a poisoning lands and erases both otherwise; uncredited ticks check attack-type immunity (31) |
| `pkg/std/spells/reflect.src` | 15.4 | Magic Reflection buff icon |
| `pkg/opt/spawnpoint/textcmd/gm/newmobedit.src` | 16 | Moved from `textcmd/admin` (renamed path only) |
| `config/cmds.cfg` | 16 | `pkg/opt/spawnpoint/textcmd/gm` registered under GM |
| `scripts/include/classes.inc` | 17, 37 | `unequipRestrictedItems()` skips NPCs; `RANGER_LEVEL_TAME_BONUS`, `RANGER_LEVEL_FULL_HEAL_CHANCE` |
| `scripts/textcmd/admin/setclass.src` | 17 | Refuses targets without an account |
| `pkg/opt/Staff/include/staff.inc`, `pkg/opt/Staff/logon.src`, `pkg/opt/Staff/logoff.src`, `pkg/packethooks/speech/receivespeechhook.src` | 17 | Staff and speech loggers skip anything without an account |
| `scripts/include/bard.inc` | 18.2, 20.4 | New `IsNpcSong()`, `GetSongSinger()`, `GetSongInstrument()`, `GetNpcBardLevel()`; `ValidSongBoost()` returns 1 for NPC singers |
| `pkg/opt/songbook/songofair.src`, `songofcloaking`, `songofdefense`, `songofdismissal`, `songofearth`, `songoffire`, `songofglory`, `songofhaste`, `songoflife`, `songoflight`, `songofremedy`, `songofsalvation`, `songofwater` | 18.2, 20.4 | Accept an NPC singer (level from Musicianship, no roll, stamina or interrupt check); plain `SmartAoE()` / `SmartBoost()` |
| `pkg/opt/songbook/songofsirens.src` | 18.2, 18.3, 20.4 | NPC singer; out-of-stamina branch returns instead of falling through to the paralyze |
| `pkg/opt/songbook/songoffright.src` | 18.2, 18.3 | NPC singer; NPC target list through `NpcSmartAoE()` |
| `pkg/opt/songbook/songofbeckon.src` | 18.4 | Takes `parms` and returns at once on a `"#MOB"` call; otherwise the committed player version |
| `scripts/include/npccast.inc` | 18.4, 19, 30.3 | `CustomSpells` overrides the template list; `IsNpcSelfSpell()`; `Random()` index fixed; `songofbeckon` commented out of `GetSpellID()`; reads `::npcdesc` (30.3) |
| `scripts/ai/tamed.src` | 19, 31 | Reads `CustomSpells` before the template spells; pet poison passes the pet as credit (31) |
| `scripts/include/npcspellnames.inc` | 19 | New: `GetNpcSpellNames()` for `.newmobedit` |
| `scripts/include/smarttamedaoe.inc` | 20.2 | `SmartFilterTamedTargets()` is a wrapper for `NpcSmartAoE()` |
| `pkg/opt/spawnpoint/textcmd/gm/newmobedit.src` | 21 | AI / Loot and Flags pages; staged Custom Hits Level and Spells; tame 0 writes `untamable`; provoke CProp; right-click close; dead fields removed |
| `pkg/std/provocation/provocation.src`, `pkg/std/peacemaking/peacemaking.src` | 21.4 | A `provoke` CProp overrides the template; 0 = immune |
| `config/npcdesc.cfg` | 22, 29, 33 | 64 Townsfolk-header templates `Type sHuman` -> `sTownsfolk`; header comment; `vortexbeast` -> `sBeholder`, moved to "Beholder Boss's" (29); `magicitemchance` barracoon / erebuschaosgod 85, rikktor / nyxseductress 95 (33) |
| `pkg/std/tracking/tracking.src` | 22 | Townsfolk category |
| `pkg/std/healing/healing.src` | 22, 37 | Townsfolk in the Healing (`withheal`) list; rangers skip the magic fish wait |
| `scripts/textcmd/seer/npclist.src` | 23 | Rewritten: templates grouped by slayer type with script, loot and magic item columns |
| `config/command_synopses.cfg` | 16, 23, 27, 32.6 | `newmobedit` -> Game Master; new `npclist` synopsis; new `arrows` entry (27); new `champkills` entry and `akill` synopsis (32.6) |
| `scripts/textcmd/player/arrows.src` | 27 | New: `.arrows` gump counting arrows, fire arrows, ice arrows, bolts and thunder bolts in the backpack |
| `scripts/include/areas.inc` | 28 | `IsInCityRegion()` reads the region's `Type` and matches `City` |
| `pkg/systems/playervendor/playermerchant.src` | 28 | `CITY_MONTHLY_WAGE_GOLD` 40000 -> 50000 |
| `scripts/textcmd/gm/newiteminfo.src` | 24 | Spell / effect / greater / special hitscript fixes, use-script picker and display, realm on X/Y/Z, redisplay loop, view-only regen page |
| `pkg/opt/shilitems/usescriptdesc.cfg` | 24, 25 | `saferecall` entry fixed; blank `Name` lines removed; `lifestream` commented out |
| `pkg/std/inscription/inscription.cfg` | 25 | `Inscription 0x1828` (of LifeStream) commented out |
| `pkg/opt/spawnpoint/include/customnpc.inc`, `pkg/opt/spawnpoint/checkpoint.src` | 30.1 | `DressNPCCustom(me, point)` takes the spawnpoint; returns without one |
| `scripts/control/firecontrol.src`, `poisoncontrol.src`, `paracontrol.src`, `energycontrol.src` | 30.2, 31, 35 | Realm and location read once per tick; para and energy pass the realm; `poisoncontrol` credits the field's caster and reads `Creator` / `ByTrueMage` every tick (31); `firecontrol` reads the caster, element, `FieldDice` and `Unresistable` every tick, and with no caster each victim is its own source (35) |
| `scripts/include/npccastspells.inc` | 35 | `CHAMPFIRE_BOLT_DICE`, `CHAMPFIRE_FIELD_DICE`; `SpecialChampFire()` rolls fixed dice, unresistable, cursed doubles; its 21 fields get `Creator`, `FieldDice`, `Unresistable` |
| `scripts/ai/main/` `animalsetup`, `criersetup`, `questiesetup`, `setup`; `scripts/ai/setup/` `animalsetup`, `archersetup`, `criersetup`, `killpcssetup`, `questiesetup`, `setup`, `spellsetup` | 30.3 | `::npcdesc` |
| `scripts/ai/setup/modsetup.inc` | 30.3 | `::npcdesc`; template re-equip skipped for Warriors for Hire |
| `scripts/include/anchors.inc`, `canstack.inc`, `speech.inc` | 30.3 | `::npcdesc`, `::stacking`, `::speechgroup` |
| `scripts/ai/combat/fight.inc`, `chaosfight.inc`, `raiderfight.inc` | 30.4 | `Fight()` returns at once on a dead, hidden or concealed opponent, out of war mode |
| `scripts/include/privs.inc` | 30.5 | `who.disable(priv)` |
| `pkg/opt/omegacache/omegacache.inc` | 30.6 | New `GetLeaseTotals()`, `LeaseItemKey()`; `GetStoredAmount()` optional `lease_totals`; item list and `GetStoredAmountByObjtype()` sum leases once |
| `scripts/include/resourcemanager.inc` | 30.6 | Material pickers sum leases once |
| `scripts/include/damages.inc` | 31.2, 31.3, 31.6 | New `RecordKiller()`; `ApplyTheDamage()` records only a mobile killer and erases the old one otherwise; new `pvp_cut` parameter (31.3); master swap before the PvP slowdown (31.6) |
| `scripts/misc/chrdeath.src` | 31.2 | Erases the ghost's `KilledBySerial` after recording the death |
| `pkg/systems/combat/poisononhit.src` | 31.3 | Poison armor credits the wearer |
| `pkg/systems/combat/poisonhit.src` | 31.3 | Player wielder passed as credit only |
| `scripts/items/walkon_pfield.src` | 31.3 | Credits the field's caster |
| `pkg/std/spells/mindblast.src` | 31.4 | `ApplyTheDamage(..., DMGID_MAGIC)`; a reflected blast stays raw |
| `pkg/opt/shilitems/vengeance.src` | 31.4 | `include/damages`; `ApplyTheDamage(..., DMGID_MAGIC + DMGID_NO_RESIST)`, raw on self |
| `pkg/opt/necro/sunderingsword.src` | 31.4 | `ApplyTheDamage(..., DMGID_MAGIC + DMGID_NO_RESIST)` |
| `pkg/opt/farming/swarmofbees.src` | 31.5 | Realm on its mobile scan |
| `pkg/opt/champspawns/include/death.inc` | 32.2-32.4 | `ProcessChampionCorpse()` pays the bags before the gold rain and skips altar-less champions; `RegisterChampionSpawnKill()` tally `{kills, last kill}`, active altars only, `set_critical`; new `FindChampionSpawnKiller()`, `IsChampionCreditable()`; `GetAltarByCorpse()` returns 0 without an `altar` CProp; milestone messages at each bag tier; `FindChampionSpawnKiller()` accepts a staff killer with a matching `StaffChampCredit` (32.6) |
| `pkg/opt/champspawns/include/rewards.inc` | 32.3 | `CHAMP_BAG_MIN_KILLS`, `CHAMP_CREDIT_WINDOW` (7200 s, from the newest kill); new `GiveRewardsToPlayers()` (old bare counts paid), `GiveChampionBag()` (moves to the bank when the contents don't fit), `AddChampionReward()`, `FillChampionBag()`, `ChampionBagTiers()`, `ChampionTallyNewest()` (32.6); `:Events:bag` and `EligibleForLoot()` dropped; `include/random`, `util/bank` |
| `pkg/opt/champspawns/include/skulls.inc` | 32.5 | New `clearWhiteSkulls()`, used by `createNextWhiteSkull()` and `createNextRedSkull()` |
| `pkg/opt/champspawns/scripts/control.src` | 32.5 | Wave reset clears white skulls above 20%, removes a red skull at or below |
| `pkg/opt/ArtifactSystem/championrelic.src` | 32.4 | `SummonedByRelic` CProp on the boss |
| `config/nlootgroup.cfg` | 33 | Lootgroup 150: 2 tomes, 4 scrolls, 4 stat potions, no tokens or presents; lootgroup 151: 3 / 4 / 5, no tokens or presents |
| `scripts/include/starteqp.inc` | 34 | `CreateFromStackString()` doubles the pile under either Hunting powerhour |
| `pkg/opt/powerhour/textcmd/player/setph.src` | 34 | Refuses the type the server-wide powerhour is running or about to start |
| `patchnotes/patch-v1.1.4.md` | - | (`d11f181`) 1.1.4 date set to October 12, 2026; "Warrior for Hire - Brought In Line With ZH3.0" heading shortened |
| `patchnotes/developer-changelog-v1.1.5.md` | - | New - this file |
| `patchnotes/patch-v1.1.5.md` | - | New - player patch notes |
| `patchnotes/launchernotes.md` | - | Refreshed with the 1.1.5 player notes |
| `pkg/std/treasuremap/digtreasure.src` | 36 | `lvl7_chance`: level 6 bards get 8% at a level 7 chest from a level 6 map |
| `scripts/textcmd/admin/akill.src` | 32.6 | `.akill <range> credit`: champion minions count as the caller's kills, logged; new `AKillMobile()` |
| `scripts/textcmd/player/champkills.src` | 32.6 | New - `.champkills`: your tally at the nearest active altar, the next tier and the two-hour warning |
| `pkg/opt/townstones/citizens.inc` | 38 | New - per-character citizen entries `{serial, account}`, `LoadCitizenList()` conversion, `IsVoteRuneOf()` |
| `pkg/opt/townstones/tstone.inc` | 38 | `voteto()`: one vote per character (`serial` = character, new `account` prop) |
| `pkg/opt/townstones/electionwatch.src` | 38 | `LoadCitizenList()` |
| `pkg/opt/townstones/textcmd/admin/removetownmember.src` | 38 | Removes one character and its vote; population = list size |
| `pkg/opt/townstones/textcmd/admin/townbankstatus.src` | 38 | Member list and removal per character (`RemoveTownMemberByChar()`, `IsRemovedVoter()`) |
| `pkg/opt/townstones/textcmd/admin/cleartownmembers.src` | 38 | Matches citizens by character |
| `pkg/std/taming/taming.src` | 37 | Tame and calming difficulty `- 2 * ranger level` |
| `pkg/std/veterinary/vet.src` | 37 | Resurrection difficulty `- 2 * ranger level` and 5 bandages; full heal `3% * level` |
| `pkg/std/fishing/magicfish.src` | 37 | A ranger's cooldown key per fish type (`#magicfish<spellnum>`) |

---

## 41. Risk and Regression Notes

- **Old warrior hearts stop working (section 5.1).** The 1.1.4 notes promised that pre-1.1.4 hearts "still work until a new death writes a stamped backup". That promise is withdrawn: every unstamped heart now crumbles at the priest. Owners of those warriors can still use "resurrect warrior" if they have a backup. If they have neither, the warrior is gone. Worth a line in the Discord announcement.
- **Hearts must be handed in by the master character (section 5.2).** A heart handed in by an alt on the same account, or by a friend, is destroyed, not returned. Stamped hearts handed in by anyone else already crumbled, so in practice only the message changes for them. The player notes say it plainly.
- **Memory revivals cost stats now (section 5.3).** A warrior on its tenth death comes back 10 points lower in each stat (floor 10), where before the priest restored it whole.
- **Personal powerhour (section 4).** No one loses time. Players who used their hour mid-week can now start on the same weekday a week later without waiting for the hour of their last use.
- **Mayor resignation (section 3).** Irreversible from the player's side. The confirmation gump is the only guard. The broadcast goes to the whole shard. The gump layout was compiled, not opened in a client.
- **Keyrings (section 8).**
  - The "sell" window still won't list them, because the engine filters newbie items. Players need "sell all" or "sell bag".
  - Selling is 1 gold against 5 ingots and Tinkering 95 to make one, and merchants sell them at 20, so there is no gold loop.
  - The empty-keyring test reads the Tamed Storage box. A keyring whose keys were dumped (`Use_KeyRing` on itself) has an empty box and sells.
- **Test tools (section 9).** Running `.aimovementsim` or `.shardstress` on live still forces uncached area checks *during* the run, as designed. Only the after-effect is fixed.
- **Dragon Skin is weaker (sections 10.4, 10.6).** It used to add to equipment with no cap and last as long as the bard played. Level 6 gear and a level 6 bard went from 12 (60% weapon damage reduction) to 6 (30%). Without gear, a bard now gives exactly their level, as the old song did, but for 2-10 minutes per play.
- **Sanctuary and Antidote change for every class (section 10.6).** Mages lose the long, Intelligence-boosted durations (up to about 16 minutes) and drop to 2-10 minutes. Powerplayers gain: Sanctuary goes from level 1 to their class level. Antidote's level drops from 7-15+ to 1-6, but 6 already blocked every non-thief poison, so only low class levels lose real protection.
- **Login resets Physical Protection and poison immunity to gear values (section 12).** Players carrying leaked Dragon Skin stacks lose them at their first login after the patch. That is the intent, but expect questions. A GM hand-set value on a player (not staff) is also reset.
- **Dispel reach (section 11).** Everything that calls `WipeMods()` now strips Sanctuary, Antidote, Dragon Skin and Reactive Armor, including Blackrock and the Warrior for Hire AI's periodic wipe (which only affects the warrior itself).
- **Relog ends these buffs (section 11).** Login runs `WipeMods()`, as it already did for Bless and Protection.
- **Buff icon text (sections 10.5, 15)** is unverified in a client.
- **`.newmobedit` is GM level (section 16).** GMs can change any mobile's stats and protections.
- **Deploy: delete the stale `pkg/opt/spawnpoint/textcmd/admin/newmobedit.ecl` (section 16)** on servers that compiled it there.
- **Other verses still leave `#Casting` set on a fumble.** Only Beastal Bond and Dragon Skin call `EndVerse()`.
- **Spellcasting monsters are stronger everywhere (section 19).** The `Random()` fix removes the 1-in-(n+1) dead casts from every caster NPC, and helpful spells now buff the monster instead of the player. Boss and champion fights with casters will feel harder.
- **Boss mass specials now hit pets (section 20.3).** Pets that tanked casters' mass specials without taking them now do. If this is too much, restore players-only for wild casters in `NpcSmartAoE()`'s `!master` branch (or in `SmartFilterTamedTargets()` alone).
- **Bystanders are decided by template alignment (section 20.2).** Any masterless `alignment good` NPC is never hit by an NPC area spell, including good creatures players fight (pixies, wisps, unicorns, the forest lord). A player's own area spells still hit them. An `alignment good` NPC that should be a target needs its alignment changed.
- **`IsBystanderNpc()` reads `::npcdesc`.** Package templates (`:pkg:name`) are never bystanders; none of the town NPCs use one.
- **Existing town NPCs keep `Type Human` (section 22).** The template CProp is applied when an NPC is created, so live townsfolk, guards and Warriors for Hire stay Human until they respawn or are recreated. Respawn town NPCs after the deploy if Tracking and the slayer change should apply at once.
- **Human slayers lose town NPCs (section 22).** A Human slayer weapon or Human protection item no longer counts against guards, merchants or Warriors for Hire. Mostly relevant to anyone fighting a Warrior for Hire or a guard.
- **NPC songs (section 18) have no template users yet.** No `npcdesc.cfg` template lists a song; they only appear through `.newmobedit`'s Spells page. Song of Beckon is refused for NPCs.
- **`npcsummoning` (section 18.4) still has no cap, a Magery-based lifetime and no realm** for every NPC summon spell (`NpcSummonCreature()`). Not changed in this patch.
- **Provoke difficulty 0 is provoke and peace only (section 21.4).** Song of Fright, Taunt, the Grark Shepherd and the mouse-over tooltip still use the template value, so the tooltip can show a number for an NPC that can't be provoked.
- **Old Life Stream items (section 25).** Any item engraved with Life Stream before engraving was turned off still carries `:shilitems:lifestream` and does nothing when used.
- **City vendors start paying the city tier (section 28).** Every player vendor in a `Type City` region goes from 333 to 1,666 gold a day at its next daily charge. Vendors with a small gold pool will start building debt, and after two months of debt they close. Worth a line in the Discord announcement so owners can top up or move to a Vendor Market.
- **Champion relics now refuse cities (section 28).** Relic activation was already blocked in guarded and no-PK areas, so this only changes unguarded parts of a `Type City` region.
- **`ResolveAttacker()` picks the first match (section 26).** If a player and their own pet both attack a warrior, the warrior answers the pet. It scans mobiles within 15 tiles on each damage event from a player, which is cheap but not free in a big fight.
- **Existing Vortex Beasts stay Giantkin (section 29)** until they respawn, as with the Townsfolk change.
- **PvP damage drops for Mind Blast and Vengeance (section 31).** A player's Mind Blast or Vengeance on another player now takes `ApplyTheDamage()`'s 0.6, like every other spell. Mind Blast casters will notice; worth a line in the announcement. Poison ticks skip the cut (`pvp_cut` 0), so poison's PvP damage is unchanged.
- **Pets, summons and Warriors for Hire lose about three quarters of their melee damage on players (section 31.6).** Their melee now takes the 0.4 and 0.6 PvP cuts a player's melee does (0.24 in all), and their spells the 0.6. Tamers, summoners and Warrior for Hire owners in PvP will feel it the most; worth a line in the announcement. Pet poison is not cut.
- **Poison is now an attack for repsys (section 31.3).** Each tick from a player's poison goes through `SetScriptController()` and `ApplyRawDamage()` as the poisoner, so poisoning an innocent keeps flagging the poisoner while it ticks, as melee does. Poison ticks also add the poisoner to the victim's hitlist.
- **Magusbane and Magehunter resist more (section 31).** Their `AttackTypeImmunities 639` includes poison and magic, which raw damage ignored. Non-Thief poison and Mind Blast now do nothing to them, including uncredited poison such as traps; Thief poison, Vengeance and Sundering Sword still do.
- **Personal powerhour now doubles poison kills' loot (section 31).** The loot code finds the poisoner through `KilledBySerial`.
- **Reward bags are new loot (sections 32, 33).** The corpse loses its tokens and presents and part of its tomes, scrolls and stat potions; section 33's table is the balance check. Duos that carry a spawn get up to twice the tomes they did; big groups get fewer. Relic champions lose the tokens, presents and some tomes outright, by design.
- **The two-hour window (section 32.3)** is measured from each player's last credited kill to the newest kill on the tally, i.e. the end of the last wave. A player who left more than two hours before then gets nothing, whatever their count; the champion fight's length doesn't count against anyone. Tune `CHAMP_CREDIT_WINDOW` in `rewards.inc`.
- **Altars mid-spawn at the deploy (section 32.2)** keep their tally if it is a dictionary (old entries are bare counts and become `{count, time}` at the next kill). An old entry with no later kill has no time and pays nothing. Live altars all hold an error at the moment and reset at their next activation, so in practice every altar starts clean.
- **Offline bags go to the bank (section 32.3).** `FindBankBox()` sleeps up to a second per offline player, in the champion's `death.ecl`, before the gold rain starts.
- **The last wave is harder (section 32.5).** A champion can no longer appear with the final wave about a fifth done; players who relied on that now need the full 91%.
- **Gold piles double under one powerhour (section 34).** Every `Stack` drop (mostly corpse gold) is twice as big during any Hunting powerhour, including champion corpse gold (60,000 -> 120,000). More gold enters the economy during powerhours.
- **`.setph` refuses the server-wide type (section 34).** Including during the 2-minute warning, so a player who opened the gump before the warning is refused at Okay.
- **`modsetup.inc` now re-equips package-AI NPCs (section 30.3).** With `::npcdesc` found, any package AI other than the Warrior for Hire that uses `modsetup.inc` is re-equipped from its `equip` template on each AI start, as script-AI NPCs already were.
- **Champfire changes every matchup (section 35).** Mages and Paladins with high resist take several times more than before (a level 6 Mage's bolt goes from ~31 to ~200 against no protection); a Warrior with no resist takes about half (~380 to ~200). The Summoner and the Soul Whisperers go from about a third of that to ~170. Elemental protection is now the only defence, so players will notice which elements they lack. Tune `CHAMPFIRE_BOLT_DICE` / `CHAMPFIRE_FIELD_DICE` in `npccastspells.inc`.
- **Fire fields look their caster up each tick (section 35.1).** This applies to every fire field, players' included: one `SystemFindObjectBySerial()` per field per 5 seconds.
- **`.akill <range> credit` on live (section 32.6).** An Administrator can earn reward bags with it. Every use is logged with the number of minions credited.
- **Town citizens (section 38).** Population and votes now count characters: one player with several characters can make up the 3-member election minimum and cast several votes (accepted). Populations rise when the old lists are converted, since every citizen character on an account now counts. A town whose stone nobody uses keeps its old list until it is opened.
- **Rangers (section 37).** Low-level rangers lose full heals: 3% at level 1 and 6% at level 2, against 8.3% for every level before. Pet resurrection costs 5 bandages where it cost 1. In PvP a ranger can chain a heal, a cure and an invisibility fish and bandage a teammate with no wait between them (accepted). Mystic Archers keep the shared fish cooldown and the bandage wait.
- **Compile state.** The whole tree, sections 18-35 included, compiled with `ecompile -A -w` on 2026-10-28: 1,162 scripts, 0 errors, no new warnings in the touched files. Section 35.4 and the review fixes (31.2 killer erase, 31.3 poison credit and armor, 32.3 window, 30.4 war mode) and the PvP changes (31.3 `pvp_cut`, 31.6 pets, 2026-10-29) were written after that and compiled the same way, 1,162 scripts, 0 errors. Section 36 (`digtreasure.src`), the fixes from the ZH3.0 port review (2026-10-29: `firecontrol` and `poisoncontrol` caster handling, poison immunity and `PoisonedBy`, reward bag fallbacks, `skulls.inc`), section 32.6 (`.champkills`, milestones, `.akill credit`) and section 37 (rangers) were then compiled the same way on 2026-10-29: 1,163 scripts (the new `champkills.src` included), 0 errors, 0 warnings in the touched files. Section 38 (town citizens) was compiled the same way on 2026-10-30: 1,163 scripts, 0 errors, 0 warnings in the townstone scripts. None of sections 18-35 was run in a client. Sections 18-25 were also reviewed (`/code-review`, 2026-10-25).
- **Compiled, not run.** The whole tree was compiled with `ecompile -A -w` on 2026-10-19 (0 errors, no new warnings in the touched files), and `merchant.src` and the two test tools on 2026-10-20 (0 errors, 0 warnings). The spell round (sections 10-14) was compiled with `ecompile -A -w` on 2026-10-22: 1161 scripts, 0 errors, no new warnings in the touched files, and the buff icon round (section 15) on 2026-10-23 with the same result. The section 10.6 balance pass has **not been compiled**. Sections 10-15 were partly exercised on the test shard (jewelry interaction confirmed by a tester); the rest was not run in a client.
