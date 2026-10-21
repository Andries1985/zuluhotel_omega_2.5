# Developer Changelog - v1.1.5
**Range:** `4abd02c` (last patchnotes commit of v1.1.4) -> `ea4722a` + working tree (this release's own `patchnotes/` files)  
**Branch:** Patch-1.1.5  
**Date:** 2026-10-18 to 2026-10-20 (mayor resignation 2026-10-18; ZH3.0 3.1.4 review-fix port, keyrings and test-tool cache restore 2026-10-19/20)  
**Commits in range:** 2 with content (`d11f181`, `ea4722a`) plus merge commits that carry only 1.1.4 work (`git diff 4abd02c 469834b` is empty); see section 2.  
**Files changed:** 11 code files modified, no new files, plus this release's 3 `patchnotes/` files. `d11f181` also touched `patchnotes/patch-v1.1.4.md` (release date and one heading); that is a 1.1.4 document fix, not 1.1.5 content.

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
10. [ZH3.0 3.1.4 - Reviewed, Not Ported](#10-zh30-314---reviewed-not-ported)
11. [Exhaustive File-by-File Change List](#11-exhaustive-file-by-file-change-list)
12. [Risk and Regression Notes](#12-risk-and-regression-notes)

---

## 1. Scope Summary

Patch 1.1.5 is a small follow-up to 1.1.4 with four topics.

Section 3 gives the mayor a way out of a town. The townstone gump's "give up your citizenship" button (3000) was only drawn on the citizen screen, and the mayor branch is tested before the citizen branch, so a mayor had no in-game way to leave short of losing an election or a staff `.removetownmember`. ZH3.0 ported this commit to its own 3.1.4 as theme 11.

Sections 4-7 port the fixes from ZH3.0 commit `9fbf224` (Patch 3.1.4, "theme 13", a `/code-review` round over 3.1.4) that apply to 2.5. ZH3.0's changelog itself notes that "ZH2.5 has the same code for the powerhour lock, both heart findings, the Eon-Prism, the damage compare and the memory-revival stats". Each one was checked against 2.5's current code first, and all were still present unchanged. The heart and memory-revival rules follow the choices made for ZH3.0: refuse unstamped hearts, destroy a heart handed in by anyone but the master, and apply stat loss on a memory revival. The warrior AI's itemdesc lookup caching (section 6) came with them. What was looked at and left out is in section 10.

Section 8 makes keyrings sellable. They already had `VendorBuysFor 1`, but they are `newbie 1` and the merchant AI refuses newbied items. They stay newbie, and the merchant's own sell paths accept empty ones.

Section 9 fixes two developer test tools that erased the shared area-policy mask cache and never rebuilt it.

## 2. Commit Timeline

| Commit | Date | Subject | Sections |
|---|---|---|---|
| `01bb007` ... `f93dfbb`, `469834b` | 2026-10-06 to 2026-10-12 | Merge pull requests #331, #333, #335-#342 (Patch-1.1.3, Patch-1.1.4, Test-Shard) | - (no new content; tree at `469834b` equals `4abd02c`) |
| `d11f181` | 2026-10-18 | Mayor can leave town | 3 |
| `ea4722a` | 2026-10-20 | Fixes: ZH3.0 3.1.4 review fixes (powerhour lock, warrior hearts, priest, damage compare, AI caching, Eon-Prism), keyrings, test tools restore the area mask cache | 4-9 |
| *(next commit)* | - | Patch notes for 1.1.5 | - |

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

## 10. ZH3.0 3.1.4 - Reviewed, Not Ported

ZH3.0 `9fbf224` (Patch 3.1.4) was read theme by theme against 2.5.

- **Themes 1-7, 10 (powerhour scheduler, `.phadmin`, `.ph`, personal powerhour resume, classes, `.classbonusinfo`, Warrior for Hire, areas, `.speedwalk`, house travel, Eon-Prism, Powerplayer curve).** Ported to ZH3.0 *from* 2.5 1.1.4, so 2.5 already has all of them.
- **Theme 11 (mayor resignation).** Ported to ZH3.0 from 2.5 `d11f181` (section 3).
- **Theme 13, areas `SaveRealmPolicies`.** ZH3.0 now skips the cache refresh on an `.areas` Save that wrote and pruned nothing. 2.5 has the equivalent pattern (`PruneStaleRealmPolicies()` always ends in `RebuildPolicyMaskCache()` and a version bump), but a no-edit Save is rare and costs each running script one re-read of a ~200-entry dictionary. In 2.5 that Save is also the in-game way to repair a missing cache. Not worth changing.
- **Theme 9, same-account characters are house owners.** 2.5 has the same bug (owner checks by character serial only), but its housing is a different, older system: no sign `method.src`. Owner checks are `IsCowner` / `IsFriend` in `pkg/std/housing/utility.inc` plus inline `ownerserial` tests in `sign.src`, `ssign.src`, `securecont.src`, `changeowner.src`, omegacache and `magictrap.src`. It's a rewrite rather than a port and was deferred to a later patch. Theme 13's trap sign lookup depends on it and was deferred with it.
- **Theme 12, Runebook / Runic Atlas Transcription Scrolls.** A new vanity feature, not a fix. Not ported.
- **Theme 8 (duplicate elven door names).** ZH3.0-only data.

---

## 11. Exhaustive File-by-File Change List

| File | Section | Summary |
|---|---|---|
| `pkg/opt/townstones/tstone.src` | 3 | `:gumps:yesno` include; mayor screen button 3001 "Resign and leave town"; new `ResignMayorship()` (confirm, re-check, clear mayor, reset `nextcitizenelection`, `CanselCityzenship()`, broadcast) |
| `pkg/opt/powerhour/include/powerhour.inc` | 4 | New `PPH_ResetTime()`; `PPH_IsEligible()` and `PPH_SecondsUntilEligible()` both use it; weekday from the use time |
| `pkg/opt/warriorforhire/include/wfhcommon.inc` | 5.1 | `IsCurrentWfhHeart()` refuses unstamped hearts and hearts with no backup |
| `scripts/ai/highpriest.src` | 5.2, 5.3 | Heart from anyone but its master is destroyed; memory revival applies `resnum` stat loss, floor 10 |
| `pkg/systems/combat/include/hitscriptinc.inc` | 6.1 | `WFHModifyDamage()` reads each master once as `CInt` and compares only when set |
| `pkg/opt/warriorforhire/warrior.src` | 6.2 | `gWeaponAmmoTypes` / `gHealPotionPowers` caches; new `WeaponAmmoType()`, `LoadHealPotionPowers()`; `HasBow()` and `HealPotionPower()` use them |
| `pkg/opt/ArtifactSystem/eonprism.src` | 7 | `include/itemutil`; refused unless in the user's backpack |
| `scripts/ai/merchant.src` | 8 | `MERCHANT_KEYRING_OBJTYPE`; new `CanMerchantBuy()`; `SellAll()` and `SellBag()` buy empty, unowned keyrings |
| `pkg/opt/alryc/textcmd/test/aimovementsim.src` | 9 | `RebuildPolicyMaskCache("britannia")` after the run |
| `pkg/opt/alryc/stresstest/areastressworker.src` | 9 | `RebuildPolicyMaskCache("britannia")` as each worker exits |
| `patchnotes/patch-v1.1.4.md` | - | (`d11f181`) 1.1.4 date set to October 12, 2026; "Warrior for Hire - Brought In Line With ZH3.0" heading shortened |
| `patchnotes/developer-changelog-v1.1.5.md` | - | New - this file |
| `patchnotes/patch-v1.1.5.md` | - | New - player patch notes |
| `patchnotes/launchernotes.md` | - | Refreshed with the 1.1.5 player notes |

---

## 12. Risk and Regression Notes

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
- **Compiled, not run.** The whole tree was compiled with `ecompile -A -w` on 2026-10-19 (0 errors, no new warnings in the touched files), and `merchant.src` and the two test tools on 2026-10-20 (0 errors, 0 warnings). None of it was exercised in a client.
