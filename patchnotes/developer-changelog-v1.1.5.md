# Developer Changelog - v1.1.5
**Range:** `4abd02c` (last patchnotes commit of v1.1.4) -> `ea4722a` + working tree (this release's own `patchnotes/` files)  
**Branch:** Patch-1.1.5  
**Date:** 2026-10-18 to 2026-10-22 (mayor resignation 2026-10-18; ZH3.0 3.1.4 review-fix port, keyrings and test-tool cache restore 2026-10-19/20; spell protections, Reactive Armor and Beastal Bond 2026-10-21/22)  
**Commits in range:** 2 with content (`d11f181`, `ea4722a`) plus merge commits that carry only 1.1.4 work (`git diff 4abd02c 469834b` is empty); see section 2.  
**Files changed:** 25 code files modified (11 in the first round, 15 in the spell round, `hitscriptinc.inc` in both) and 2 new includes (`scripts/include/spellprots.inc`, `pkg/opt/versebook/include/beastalbond.inc`), plus this release's 3 `patchnotes/` files. `d11f181` also touched `patchnotes/patch-v1.1.4.md` (release date and one heading); that is a 1.1.4 document fix, not 1.1.5 content.

> Scope was computed against `4abd02c`, the last commit that edited the 1.1.4 notes, not `master`.

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
15. [ZH3.0 3.1.4 - Reviewed, Not Ported](#15-zh30-314---reviewed-not-ported)
16. [Exhaustive File-by-File Change List](#16-exhaustive-file-by-file-change-list)
17. [Risk and Regression Notes](#17-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.5 is a small follow-up to 1.1.4 with five topics.

Section 3 gives the mayor a way out of a town. The townstone gump's "give up your citizenship" button (3000) was only drawn on the citizen screen, and the mayor branch is tested before the citizen branch, so a mayor had no in-game way to leave short of losing an election or a staff `.removetownmember`. ZH3.0 ported this commit to its own 3.1.4 as theme 11.

Sections 4-7 port the fixes from ZH3.0 commit `9fbf224` (Patch 3.1.4, "theme 13", a `/code-review` round over 3.1.4) that apply to 2.5. ZH3.0's changelog itself notes that "ZH2.5 has the same code for the powerhour lock, both heart findings, the Eon-Prism, the damage compare and the memory-revival stats". Each one was checked against 2.5's current code first, and all were still present unchanged. The heart and memory-revival rules follow the choices made for ZH3.0: refuse unstamped hearts, destroy a heart handed in by anyone but the master, and apply stat loss on a memory revival. The warrior AI's itemdesc lookup caching (section 6) came with them. What was looked at and left out is in section 15.

Section 8 makes keyrings sellable. They already had `VendorBuysFor 1`, but they are `newbie 1` and the merchant AI refuses newbied items. They stay newbie, and the merchant's own sell paths accept empty ones.

Section 9 fixes two developer test tools that erased the shared area-policy mask cache and never rebuilt it.

Sections 10-14 rework the timed spell protections. It started from a player report: Sanctuary's poison immunity, when it ran out, also switched off poison immunity jewelry put on during the spell. Sanctuary, Antidote and Dragon Skin wrote straight into the CProps that equipment maintains. That caused the jewelry bug, immunity and Dragon Skin protection stuck on after restarts, and Dragon Skin stacking without limit. The spells now keep their own temporary levels, and the higher of gear and spell counts. Dragon Skin becomes a timed buff with new numbers. Dispel removes all of them plus Reactive Armor. Beastal Bond gets ZH3.0's fixes plus a saved start position.

## 2. Commit Timeline

| Commit | Date | Subject | Sections |
|---|---|---|---|
| `01bb007` ... `f93dfbb`, `469834b` | 2026-10-06 to 2026-10-12 | Merge pull requests #331, #333, #335-#342 (Patch-1.1.3, Patch-1.1.4, Test-Shard) | - (no new content; tree at `469834b` equals `4abd02c`) |
| `d11f181` | 2026-10-18 | Mayor can leave town | 3 |
| `ea4722a` | 2026-10-20 | Fixes: ZH3.0 3.1.4 review fixes (powerhour lock, warrior hearts, priest, damage compare, AI caching, Eon-Prism), keyrings, test tools restore the area mask cache | 4-9 |
| *(next commit)* | - | Spell protections, Dragon Skin rework, Reactive Armor, Beastal Bond, and patch notes for 1.1.5 | 10-14 |

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

Power and duration formulas are unchanged. The cast is applied when `power >= GetPoisonImmunityLevel(target)`: a target with higher immunity from jewelry or a stronger cast is skipped, and an equal one is refreshed. The scripts `SetSpellProt()`, `BuffOn("poisonimmunity")`, `Detach()`, sleep, then `EndSpellProt()` / `BuffOff()`. Antidote now applies to `cast_on`, including NPC targets (no buff icon for NPCs; `BuffOn` returns 0 for them).

### 10.4 Dragon Skin

The channelled loop is gone. After `CanPlay()` and `SucceedPlayingVerse()`:

- Level `IsBard(who)` (1-6) picks the protection from `{ 1, 2, 3, 5, 7, 9 }` and the duration `(15 + 3 * (level - 1)) * 60` seconds.
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

## 15. ZH3.0 3.1.4 - Reviewed, Not Ported

ZH3.0 `9fbf224` (Patch 3.1.4) was read theme by theme against 2.5.

- **Themes 1-7, 10 (powerhour scheduler, `.phadmin`, `.ph`, personal powerhour resume, classes, `.classbonusinfo`, Warrior for Hire, areas, `.speedwalk`, house travel, Eon-Prism, Powerplayer curve).** Ported to ZH3.0 *from* 2.5 1.1.4, so 2.5 already has all of them.
- **Theme 11 (mayor resignation).** Ported to ZH3.0 from 2.5 `d11f181` (section 3).
- **Theme 13, areas `SaveRealmPolicies`.** ZH3.0 now skips the cache refresh on an `.areas` Save that wrote and pruned nothing. 2.5 has the equivalent pattern (`PruneStaleRealmPolicies()` always ends in `RebuildPolicyMaskCache()` and a version bump), but a no-edit Save is rare and costs each running script one re-read of a ~200-entry dictionary. In 2.5 that Save is also the in-game way to repair a missing cache. Not worth changing.
- **Theme 9, same-account characters are house owners.** 2.5 has the same bug (owner checks by character serial only), but its housing is a different, older system: no sign `method.src`. Owner checks are `IsCowner` / `IsFriend` in `pkg/std/housing/utility.inc` plus inline `ownerserial` tests in `sign.src`, `ssign.src`, `securecont.src`, `changeowner.src`, omegacache and `magictrap.src`. It's a rewrite rather than a port and was deferred to a later patch. Theme 13's trap sign lookup depends on it and was deferred with it.
- **Theme 12, Runebook / Runic Atlas Transcription Scrolls.** A new vanity feature, not a fix. Not ported.
- **Theme 8 (duplicate elven door names).** ZH3.0-only data.

---

## 16. Exhaustive File-by-File Change List

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
| `scripts/include/spellprots.inc` | 10 | New: `#SpellPoisonImmunity` / `#DragonSkin` `{ level, pid }` helpers, `GetPoisonImmunityLevel()`, `GetPhysicalProtection()`, `ResyncEquippedProt()` |
| `scripts/include/dotempmods.inc` | 10, 11 | Includes `spellprots`; `PoisonProtected()` uses `GetPoisonImmunityLevel()`; `WipeMods()` clears spell poison immunity, Dragon Skin, Reactive Armor, Incognito name; `poisonimmunity` / `dragonskin` buff icons |
| `pkg/opt/holybook/sanctuary.src` | 10.3 | Spell level via `SetSpellProt()`, applied when not weaker than current immunity; buff icon; ends only its own cast |
| `pkg/opt/earth/antidote.src` | 10.3 | Same as Sanctuary; immunity goes to the target instead of the caster |
| `pkg/opt/versebook/Dragon_Skin.src` | 10.4 | Rewritten as a one-play timed buff: 1/2/3/5/7/9 by level, 15-30 min, 20 stamina per target, buff icon |
| `pkg/opt/versebook/include/versefunctions.inc` | 10, 14 | Includes `spellprots`; `AdjustForPhysicalDamage()` uses `GetPhysicalProtection()`; new `EndVerse()` |
| `scripts/control/skilladvancerequip.src` | 10.2 | Physical Protection recalculated only for items that carry it; Dragon Skin special case removed |
| `scripts/textcmd/player/prots.src` | 10.2 | `.prots` shows the higher of gear and spell for Physical Protection and poison immunity |
| `pkg/packethooks/megacliloc/mobiledata.src` | 10.2 | Mobile tooltip protection values include active spells; includes `spellprots` |
| `pkg/std/spells/reactivearmor.src` | 13 | `#ReactiveArmorPid` ownership, buff icon, `include/dotempmods` |
| `scripts/playermanager.src` | 12, 14 | Login resyncs players' `PermPoisonImmunity` / `PhysicalProtection` from equipment; `EndBeastalBond()` |
| `pkg/opt/versebook/Beastal_Bond.src` | 14 | Fumble leaves the loop; offline stops it; saved start position; realm-aware search; dead Reporting call removed; `EndVerse()` |
| `pkg/opt/versebook/Beast_Bond_View.src` | 14.2 | Always unfreezes, unconceals, clears `#ViewingBeast` when it ends |
| `pkg/opt/versebook/include/beastalbond.inc` | 14.3 | New: `SaveBeastalBondOrigin()`, `ReturnToBeastalBondOrigin()`, `EndBeastalBond()` |
| `scripts/misc/logoff.src` | 14 | `EndBeastalBond()` replaces the view-only cleanup |
| `scripts/misc/reconnect.src` | 14 | `EndBeastalBond()` replaces the view-only cleanup |
| `patchnotes/patch-v1.1.4.md` | - | (`d11f181`) 1.1.4 date set to October 12, 2026; "Warrior for Hire - Brought In Line With ZH3.0" heading shortened |
| `patchnotes/developer-changelog-v1.1.5.md` | - | New - this file |
| `patchnotes/patch-v1.1.5.md` | - | New - player patch notes |
| `patchnotes/launchernotes.md` | - | Refreshed with the 1.1.5 player notes |

---

## 17. Risk and Regression Notes

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
- **Dragon Skin is weaker with good gear (section 10.4).** It used to add to equipment with no cap. Level 6 gear and a level 6 bard went from 12 (60% weapon damage reduction) to 9 (45%). Players without Physical Protection gear get more than before at levels 4-6 (5/7/9 instead of 4/5/6).
- **Login resets Physical Protection and poison immunity to gear values (section 12).** Players carrying leaked Dragon Skin stacks lose them at their first login after the patch. That is the intent, but expect questions. A GM hand-set value on a player (not staff) is also reset.
- **Dispel reach (section 11).** Everything that calls `WipeMods()` now strips Sanctuary, Antidote, Dragon Skin and Reactive Armor, including Blackrock and the Warrior for Hire AI's periodic wipe (which only affects the warrior itself).
- **Relog ends these buffs (section 11).** Login runs `WipeMods()`, as it already did for Bless and Protection.
- **Buff icon text (section 10.5)** is unverified in a client.
- **Other verses still leave `#Casting` set on a fumble.** Only Beastal Bond and Dragon Skin call `EndVerse()`.
- **Compiled, not run.** The whole tree was compiled with `ecompile -A -w` on 2026-10-19 (0 errors, no new warnings in the touched files), and `merchant.src` and the two test tools on 2026-10-20 (0 errors, 0 warnings). The spell round (sections 10-14) was compiled with `ecompile -A -w` on 2026-10-22: 1161 scripts, 0 errors, no new warnings in the touched files. None of it was exercised in a client.
