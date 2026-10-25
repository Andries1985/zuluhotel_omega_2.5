# Developer Changelog - v1.1.5
**Range:** `4abd02c` (last patchnotes commit of v1.1.4) -> `fd57000` + working tree (section 10.6 balance pass, sections 18-25 and this release's own `patchnotes/` files)  
**Branch:** Patch-1.1.5  
**Date:** 2026-10-18 to 2026-10-25 (mayor resignation 2026-10-18; ZH3.0 3.1.4 review-fix port, keyrings and test-tool cache restore 2026-10-19/20; spell protections, Reactive Armor and Beastal Bond 2026-10-21/22; buff icons, `.newmobedit` and the NPC world-save fix 2026-10-23; class-level balance pass, NPC songs and spell lists, NPC area targeting, `.newmobedit` NPC pages, the Townsfolk type, `.npclist`, `.newiteminfo` and the use-script config 2026-10-25, working tree)  
**Commits in range:** 5 with content (`d11f181`, `ea4722a`, `2587db3`, `07e7bc6`, `ccc8d27`), the merge `fd57000` of #345, plus merge commits that carry only 1.1.4 work (`git diff 4abd02c 469834b` is empty); see section 2.  
**Files changed:** 43 code files modified, 1 renamed (`newmobedit.src`, admin -> gm) and 3 new includes (`scripts/include/spellprots.inc`, `scripts/include/buffs.inc`, `pkg/opt/versebook/include/beastalbond.inc`) in `4abd02c..fd57000`. In the working tree: 4 files for section 10.6 (`sanctuary.src`, `antidote.src`, `Dragon_Skin.src`, `spellprots.inc`), 32 files and 1 new include (`scripts/include/npcspellnames.inc`) for sections 18-25, plus this release's 3 `patchnotes/` files. `d11f181` also touched `patchnotes/patch-v1.1.4.md` (release date and one heading); that is a 1.1.4 document fix, not 1.1.5 content.

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
26. [ZH3.0 3.1.4 - Reviewed, Not Ported](#26-zh30-314---reviewed-not-ported)
27. [Exhaustive File-by-File Change List](#27-exhaustive-file-by-file-change-list)
28. [Risk and Regression Notes](#28-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.5 is a follow-up to 1.1.4.

Section 3 gives the mayor a way out of a town. The townstone gump's "give up your citizenship" button (3000) was only drawn on the citizen screen, and the mayor branch is tested before the citizen branch, so a mayor had no in-game way to leave short of losing an election or a staff `.removetownmember`. ZH3.0 ported this commit to its own 3.1.4 as theme 11.

Sections 4-7 port the fixes from ZH3.0 commit `9fbf224` (Patch 3.1.4, "theme 13", a `/code-review` round over 3.1.4) that apply to 2.5. ZH3.0's changelog itself notes that "ZH2.5 has the same code for the powerhour lock, both heart findings, the Eon-Prism, the damage compare and the memory-revival stats". Each one was checked against 2.5's current code first, and all were still present unchanged. The heart and memory-revival rules follow the choices made for ZH3.0: refuse unstamped hearts, destroy a heart handed in by anyone but the master, and apply stat loss on a memory revival. The warrior AI's itemdesc lookup caching (section 6) came with them. What was looked at and left out is in section 26.

Section 8 makes keyrings sellable. They already had `VendorBuysFor 1`, but they are `newbie 1` and the merchant AI refuses newbied items. They stay newbie, and the merchant's own sell paths accept empty ones.

Section 9 fixes two developer test tools that erased the shared area-policy mask cache and never rebuilt it.

Sections 10-14 rework the timed spell protections. It started from a player report: Sanctuary's poison immunity, when it ran out, also switched off poison immunity jewelry put on during the spell. Sanctuary, Antidote and Dragon Skin wrote straight into the CProps that equipment maintains. That caused the jewelry bug, immunity and Dragon Skin protection stuck on after restarts, and Dragon Skin stacking without limit. The spells now keep their own temporary levels, and the higher of gear and spell counts. Dragon Skin becomes a timed buff. After test-shard feedback (section 10.6), all three take their level and a 2-10 minute duration from the caster's class level alone. Dispel removes all of them plus Reactive Armor. Beastal Bond gets ZH3.0's fixes plus a saved start position.

Sections 15-17 are smaller follow-ups: three more buff icons, `.newmobedit` opened to GMs, and a fix for `.setclass` on an NPC breaking every world save.

Sections 18-21 are an NPC casting round. The 16 songbook songs could not run for an NPC at all: `castspell()` already started them with `{ "#MOB", caster, target }`, but each song took that array as its singer. They now accept an NPC singer (Song of Beckon stays players-only). NPCs can carry their own spell list (`CustomSpells`), cast helpful spells on themselves, and no longer lose part of their casts to an off-by-one `Random()` index. Every area spell or song an NPC casts now goes through one set of target rules inside `SmartAoE()` / `SmartBoost()`, shared with the boss mass specials. `.newmobedit` gains NPC pages to set all of this per NPC. A `/code-review` of the round found the fixes folded into sections 18-21 (Song of Sirens' stamina fall-through, the spells NPCs can't cast, Tame Diff 0 not stopping Spell Bind, newmobedit edits surviving Cancel, the duplicated targeting rules).

Sections 22-25 are smaller: town NPCs become the `Townsfolk` creature type, `.npclist` is rewritten, `.newiteminfo` gets a full bug pass, and the use-script config and the Life Stream engraving recipe are cleaned up.

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
| *(next commit)* | - | Class-level balance pass for Sanctuary, Antidote and Dragon Skin; NPC songs, spell lists and area targeting; `.newmobedit` NPC pages; Townsfolk type; `.npclist`; `.newiteminfo`; use-script config; patch notes | 10.6, 18-25 |

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

## 26. ZH3.0 3.1.4 - Reviewed, Not Ported

ZH3.0 `9fbf224` (Patch 3.1.4) was read theme by theme against 2.5.

- **Themes 1-7, 10 (powerhour scheduler, `.phadmin`, `.ph`, personal powerhour resume, classes, `.classbonusinfo`, Warrior for Hire, areas, `.speedwalk`, house travel, Eon-Prism, Powerplayer curve).** Ported to ZH3.0 *from* 2.5 1.1.4, so 2.5 already has all of them.
- **Theme 11 (mayor resignation).** Ported to ZH3.0 from 2.5 `d11f181` (section 3).
- **Theme 13, areas `SaveRealmPolicies`.** ZH3.0 now skips the cache refresh on an `.areas` Save that wrote and pruned nothing. 2.5 has the equivalent pattern (`PruneStaleRealmPolicies()` always ends in `RebuildPolicyMaskCache()` and a version bump), but a no-edit Save is rare and costs each running script one re-read of a ~200-entry dictionary. In 2.5 that Save is also the in-game way to repair a missing cache. Not worth changing.
- **Theme 9, same-account characters are house owners.** 2.5 has the same bug (owner checks by character serial only), but its housing is a different, older system: no sign `method.src`. Owner checks are `IsCowner` / `IsFriend` in `pkg/std/housing/utility.inc` plus inline `ownerserial` tests in `sign.src`, `ssign.src`, `securecont.src`, `changeowner.src`, omegacache and `magictrap.src`. It's a rewrite rather than a port and was deferred to a later patch. Theme 13's trap sign lookup depends on it and was deferred with it.
- **Theme 12, Runebook / Runic Atlas Transcription Scrolls.** A new vanity feature, not a fix. Not ported.
- **Theme 8 (duplicate elven door names).** ZH3.0-only data.

---

## 27. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `pkg/opt/townstones/tstone.src` | 3 | `:gumps:yesno` include; mayor screen button 3001 "Resign and leave town"; new `ResignMayorship()` (confirm, re-check, clear mayor, reset `nextcitizenelection`, `CanselCityzenship()`, broadcast) |
| `pkg/opt/powerhour/include/powerhour.inc` | 4 | New `PPH_ResetTime()`; `PPH_IsEligible()` and `PPH_SecondsUntilEligible()` both use it; weekday from the use time |
| `pkg/opt/warriorforhire/include/wfhcommon.inc` | 5.1 | `IsCurrentWfhHeart()` refuses unstamped hearts and hearts with no backup |
| `scripts/ai/highpriest.src` | 5.2, 5.3 | Heart from anyone but its master is destroyed; memory revival applies `resnum` stat loss, floor 10 |
| `pkg/systems/combat/include/hitscriptinc.inc` | 6.1, 10, 13 | `WFHModifyDamage()` reads each master once as `CInt` and compares only when set; `ModByProt()` uses `GetPhysicalProtection()`; Reactive Armor trigger clears `#ReactiveArmorPid` and its icon |
| `pkg/opt/warriorforhire/warrior.src` | 6.2 | `gWeaponAmmoTypes` / `gHealPotionPowers` caches; new `WeaponAmmoType()`, `LoadHealPotionPowers()`; `HasBow()` and `HealPotionPower()` use them |
| `pkg/opt/ArtifactSystem/eonprism.src` | 7 | `include/itemutil`; refused unless in the user's backpack |
| `scripts/ai/merchant.src` | 8 | `MERCHANT_KEYRING_OBJTYPE`; new `CanMerchantBuy()`; `SellAll()` and `SellBag()` buy empty, unowned keyrings |
| `pkg/opt/alryc/textcmd/test/aimovementsim.src` | 9 | `RebuildPolicyMaskCache("britannia")` after the run |
| `pkg/opt/alryc/stresstest/areastressworker.src` | 9 | `RebuildPolicyMaskCache("britannia")` as each worker exits |
| `scripts/include/spellprots.inc` | 10, 10.6 | New: `#SpellPoisonImmunity` / `#DragonSkin` `{ level, pid }` helpers, `GetPoisonImmunityLevel()`, `GetPhysicalProtection()`, `ResyncEquippedProt()`; `SpellProtCasterLevel()`, `SpellProtDuration()` (10.6) |
| `scripts/include/dotempmods.inc` | 10, 11, 15 | Includes `spellprots` and `buffs`; `PoisonProtected()` uses `GetPoisonImmunityLevel()`; `WipeMods()` clears spell poison immunity, Dragon Skin, Reactive Armor, Incognito name; `poisonimmunity` / `dragonskin` buff icons (now in `buffs.inc`); `CurePoison()` turns the poison icon off; buff functions moved out |
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
| `scripts/include/spelldata.inc` | 15.1, 15.4, 20.2 | Includes `buffs`; `Reflected()` turns the Magic Reflection icon off; `SmartAoE()` / `SmartBoost()` hand NPC casters on; new `IsBystanderNpc()`, `IsMonsterSide()`, `NpcPetCanHitPlayer()`, `NpcSmartAoE()`, `NpcSmartBoost()` |
| `pkg/std/spells/nightsight.src`, `pkg/opt/holybook/lightofday.src`, `pkg/opt/earth/owlsight.src`, `pkg/opt/songbook/songoflight.src`, `pkg/std/alchemy/blackpotion.src` | 15.2 | Night sight buff icon |
| `pkg/opt/necro/darkcloak.src`, `pkg/systems/combat/blindingonhit.src`, `pkg/systems/combat/blindingscript.src` | 15.2 | Blinding turns the night sight icon off |
| `pkg/opt/summoning/processpoisonmod.src` | 15.3 | Poison buff icon on a fresh poisoning |
| `pkg/std/spells/reflect.src` | 15.4 | Magic Reflection buff icon |
| `pkg/opt/spawnpoint/textcmd/gm/newmobedit.src` | 16 | Moved from `textcmd/admin` (renamed path only) |
| `config/cmds.cfg` | 16 | `pkg/opt/spawnpoint/textcmd/gm` registered under GM |
| `scripts/include/classes.inc` | 17 | `unequipRestrictedItems()` skips NPCs |
| `scripts/textcmd/admin/setclass.src` | 17 | Refuses targets without an account |
| `pkg/opt/Staff/include/staff.inc`, `pkg/opt/Staff/logon.src`, `pkg/opt/Staff/logoff.src`, `pkg/packethooks/speech/receivespeechhook.src` | 17 | Staff and speech loggers skip anything without an account |
| `scripts/include/bard.inc` | 18.2, 20.4 | New `IsNpcSong()`, `GetSongSinger()`, `GetSongInstrument()`, `GetNpcBardLevel()`; `ValidSongBoost()` returns 1 for NPC singers |
| `pkg/opt/songbook/songofair.src`, `songofcloaking`, `songofdefense`, `songofdismissal`, `songofearth`, `songoffire`, `songofglory`, `songofhaste`, `songoflife`, `songoflight`, `songofremedy`, `songofsalvation`, `songofwater` | 18.2, 20.4 | Accept an NPC singer (level from Musicianship, no roll, stamina or interrupt check); plain `SmartAoE()` / `SmartBoost()` |
| `pkg/opt/songbook/songofsirens.src` | 18.2, 18.3, 20.4 | NPC singer; out-of-stamina branch returns instead of falling through to the paralyze |
| `pkg/opt/songbook/songoffright.src` | 18.2, 18.3 | NPC singer; NPC target list through `NpcSmartAoE()` |
| `pkg/opt/songbook/songofbeckon.src` | 18.4 | Takes `parms` and returns at once on a `"#MOB"` call; otherwise the committed player version |
| `scripts/include/npccast.inc` | 18.4, 19 | `CustomSpells` overrides the template list; `IsNpcSelfSpell()`; `Random()` index fixed; `songofbeckon` commented out of `GetSpellID()` |
| `scripts/ai/tamed.src` | 19 | Reads `CustomSpells` before the template spells |
| `scripts/include/npcspellnames.inc` | 19 | New: `GetNpcSpellNames()` for `.newmobedit` |
| `scripts/include/smarttamedaoe.inc` | 20.2 | `SmartFilterTamedTargets()` is a wrapper for `NpcSmartAoE()` |
| `pkg/opt/spawnpoint/textcmd/gm/newmobedit.src` | 21 | AI / Loot and Flags pages; staged Custom Hits Level and Spells; tame 0 writes `untamable`; provoke CProp; right-click close; dead fields removed |
| `pkg/std/provocation/provocation.src`, `pkg/std/peacemaking/peacemaking.src` | 21.4 | A `provoke` CProp overrides the template; 0 = immune |
| `config/npcdesc.cfg` | 22 | 64 Townsfolk-header templates `Type sHuman` -> `sTownsfolk`; header comment |
| `pkg/std/tracking/tracking.src` | 22 | Townsfolk category |
| `pkg/std/healing/healing.src` | 22 | Townsfolk in the Healing (`withheal`) list |
| `scripts/textcmd/seer/npclist.src` | 23 | Rewritten: templates grouped by slayer type with script, loot and magic item columns |
| `config/command_synopses.cfg` | 16, 23 | `newmobedit` -> Game Master; new `npclist` synopsis |
| `scripts/textcmd/gm/newiteminfo.src` | 24 | Spell / effect / greater / special hitscript fixes, use-script picker and display, realm on X/Y/Z, redisplay loop, view-only regen page |
| `pkg/opt/shilitems/usescriptdesc.cfg` | 24, 25 | `saferecall` entry fixed; blank `Name` lines removed; `lifestream` commented out |
| `pkg/std/inscription/inscription.cfg` | 25 | `Inscription 0x1828` (of LifeStream) commented out |
| `patchnotes/patch-v1.1.4.md` | - | (`d11f181`) 1.1.4 date set to October 12, 2026; "Warrior for Hire - Brought In Line With ZH3.0" heading shortened |
| `patchnotes/developer-changelog-v1.1.5.md` | - | New - this file |
| `patchnotes/patch-v1.1.5.md` | - | New - player patch notes |
| `patchnotes/launchernotes.md` | - | Refreshed with the 1.1.5 player notes |

---

## 28. Risk and Regression Notes

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
- **Sections 18-25 were not compiled.** Written and reviewed (`/code-review`, 2026-10-25) but neither compiled nor run. `ecompile -A -w` on the whole tree is owed before the deploy, since `spelldata.inc` and `npccast.inc` are included widely.
- **Compiled, not run.** The whole tree was compiled with `ecompile -A -w` on 2026-10-19 (0 errors, no new warnings in the touched files), and `merchant.src` and the two test tools on 2026-10-20 (0 errors, 0 warnings). The spell round (sections 10-14) was compiled with `ecompile -A -w` on 2026-10-22: 1161 scripts, 0 errors, no new warnings in the touched files, and the buff icon round (section 15) on 2026-10-23 with the same result. The section 10.6 balance pass has **not been compiled**. Sections 10-15 were partly exercised on the test shard (jewelry interaction confirmed by a tester); the rest was not run in a client.
