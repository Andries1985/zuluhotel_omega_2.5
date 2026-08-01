# Patch Notes - v1.0.8
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: July 28, 2026**

---

Welcome to **Patch 1.0.8**. This update focuses on **player-run town administration**, **area-policy fixes**, **name-change exploit closure**, **house placement rules**, **crafting/Omega Cache fixes**, a new **No Damage Zone** area type, a **shield armor bug fix**, and a set of support-side script, performance, and balance updates.

---

## What Changed

## Townstones - Player-Run Town Management

### Player Impact

- Townstone status tools now surface more of the live town state, including treasury gold, population, upgrade and donation state, and player-town availability and purchase flags.
- Town member cleanup is more reliable, and town membership data now stays in sync when members are removed.
- Townstone creation and region conflict checks were tightened so townstone state is restored more consistently from the saved datafile.

---

## Name Changes - Exploit Closed and Fixes

### Player Impact

- Closed an exploit where a player could end up holding both a plain name (like "Pig") and a town-suffixed version of it ("Pig of Trinsic") at the same time by juggling alt characters while joining and leaving a town.
- Name matching is now case-insensitive — "Bop", "bop", and "BOP" are treated as the same name and can no longer all exist at once.
- New characters, and anyone using the rename gump, can no longer pick a name that contains a town name.
- Fixed a bug that let a name end up with a leading/trailing/double space, which could silently create duplicate-looking character names.
- Fixed the cause of a reported ~50 second delay and stacked rename gumps when changing your name.
- Leaving a town with a name that's already taken by someone else no longer traps you — you'll be prompted to pick a new name instead.
- Staff renaming tools (`.setname`, `.setprop name`, and the rename button in `.info`) now check names the same way player renames do, so a player can no longer end up with an invalid or duplicate name through a staff tool. This does not affect NPC renaming.

---

## Housing - Placement Restrictions

### Player Impact

- House placement now checks your entire house footprint (not just where you're standing) against city, dungeon, shrine, and graveyard zones, so houses can no longer be placed overlapping those areas.

---

## Areas - Castle Boundary Fixes and Performance

### Player Impact

- Area policy data was moved into per-realm datafiles, and the resolver now caches parsed area lines and policy flags for lower server load.
- The area map was expanded and refreshed across all realms, including new shrines, dungeon entrances, and many more points of interest.
- Enter and leave text was updated alongside the area data, and the Lord British Castle and Lord Blackthornes Castle boundaries were corrected as part of that refresh.
- Fixed a datafile save error that could occur before an area's realm was fully set up.

---

## Crafting - Omega Cache Fixes

### Player Impact

- The smithy hammer can now correctly pull ingots (and bone, for bone armor) directly from your Omega Cache — this was supposed to work before but was broken.
- The smithy retort (used to craft crafting-power upgrades like Oil, Alloy, Varnish, Compound, and Recharge Powder) can now also pull materials from your Omega Cache if you run out in your backpack.
- Fixed several leveled Strength and Cure Potions (and their Greater versions) showing up under "Other" in the Omega Cache instead of "Potions."

---

## Holy Book - Remove Curse Update

### Player Impact

- Remove Curse now only accepts cursed weapons and armor as valid targets.
- Paladins now use a magery and magic resistance formula for the success chance instead of the older item-identification-only approach.
- The curse removal success and failure messaging is unchanged.

---

## Protection Effects - Longer Duration

### Player Impact

- Protection-style potion effects now last much longer than before.
- The effect strength is unchanged, but the duration was extended significantly.

---

## Skill Gain - High-Skill Tuning

### Player Impact

- Skill gain now correctly checks your base skill when worn gear pushes your effective skill to the cap, so skill-boosting items no longer block advancement.
- This fixes the edge case where capped effective skill could prevent gains even though your underlying skill still had room to improve.

---

## Town NPCs - Spawn Behavior Polish

### Player Impact

- Town NPCs and nobles now begin wandering immediately after spawn instead of waiting for their first movement cycle.
- This reduces the chance of freshly spawned town NPCs appearing idle.

---

## Performance

### Player Impact

- Fixed two commands (`.go` and the developer house/boat locator tools) that were heavy enough to occasionally trip the server's runaway-script safeguard — no player-facing behavior change, just faster and lighter on the server.

---

## Staff Tools - Character and Account History Tracking

### Player Impact

- No direct gameplay change expected. Staff now have a new internal tool to look up an account's characters and review name-change, death, poisoning, and account-note history, so support and moderation questions can be answered faster.
- If a staff member ever rejects a name change (via `.setprop`, `.setname`, or `.info`), the message now tells you the actual reason (too long/short, bad spacing, reserved word, contains a town name, or already in use) instead of one generic "invalid or already in use" message.
- Fixed a bug where `.setprop name` could silently reject a valid new name due to an invisible trailing space it was adding itself.

---

## Staff and Support Tools

### Player Impact

- No direct gameplay change expected for regular players.
- Staff tools were updated for area policy management, go-location browsing, spawnpoint restarts, town member cleanup, and related command synopsis coverage.
- The area-policy admin tool was further reorganized (categorized area list, an info popup, and a page-jump control) to make it easier for staff to manage the growing number of areas.

---

## No Damage Zone - New Area Type

### Player Impact

- A new zone type has been added to the area system: **No Damage Zone**. Standing in one blocks combat entirely - you can't target or attack anything (player or NPC) while inside, and nothing can deal damage to you.
- Unlike Safe Areas, a No Damage Zone also blocks looting and magic, the same way anti-looting and anti-magic areas already do.
- If you order a tamed pet to attack something inside a No Damage Zone from outside it, the pet is confiscated rather than left standing uselessly at the edge of the zone - donator mounts are safely stored to their mount stone, other pets are ticketed back to their owner the normal confiscation way.
- You can still walk a pet into a No Damage Zone and release it (for a zoo/exhibit, for example) - a released pet can't attack or be attacked there anyway, so it's left alone unless it's ordered to attack from outside.
- Guards will not spawn against a criminal or murderer who is standing inside a No Damage Zone.
- No live areas currently use this new zone type - it's new infrastructure staff can apply to areas going forward.

---

## Combat - Shield Armor Fix

### Player Impact

- Fixed a bug where several shields (including Shield of Alryc, Sunshine Shield, Shield of Wonders, the AR5-AR50 reward shields, Shield of Stygian Darkness, and a handful of others) were granting a small amount of permanent bonus Armor Rating just from being worn, in addition to their intended bonus on a successful parry. That extra always-on AR has been removed - these shields now only help when you successfully parry, as intended.
- Animation's Shield keeps a flat armor bonus, but it's now applied through the correct body zones instead of the one it was incorrectly using.

---

## Omega Cache - Lockout Fix

### Player Impact

- Fixed a bug where a server restart could leave your Omega Cache permanently stuck showing "You already have the Omega Cache open," even though it wasn't. This could persist for a very long time until it cleared on its own - it's now detected and cleared automatically.

---

## Class Firsts - Behind-the-Scenes Storage Change

### Player Impact

- No direct gameplay change expected. "First to reach level N [Class]" tracking now uses more reliable internal storage. All previously recorded firsts are preserved.

---

## Town NPCs - Script Cleanup After Jail/Death

### Player Impact

- Fixed a bug where town NPCs (minstrels, nobles, townsfolk) that got jailed or killed for leaving their town's bounds (whether wandering or fleeing a fight) could keep executing leftover script logic afterward instead of stopping cleanly.
- Fixed a bug where a tamed pet that permanently couldn't follow its owner (for example, due to a boat/house ownership mismatch) could get stuck unable to respond to new orders.

---

## Maintenance and Data Updates

### Player Impact

- No direct gameplay change expected.
- Support data and scripts were refreshed for the new area and go-location workflow, and the backup script path was updated.
- A batch of boat and decorative structure data was corrected - several boats and non-house structures were incorrectly tagged internally as houses; this is now fixed so house-only tools (including this patch's house-placement restrictions) no longer misidentify them.
- A handful of ship-part tiles (mast, spar, tiller, ballista) had their internal placement/equip data corrected.

---

## Summary

- Townstone status and membership handling were expanded.
- A town-suffix name-collision exploit was closed, and several related naming bugs were fixed.
- House placement now respects city, dungeon, shrine, and graveyard boundaries.
- Lord British Castle and Blackthornes area boundaries were corrected.
- The smithy hammer and smithy retort now correctly use the Omega Cache.
- A handful of leveled potions were moved into the correct Omega Cache category.
- Remove Curse now uses a better success formula and valid target filter.
- Protection effects now last longer.
- Skill gain handling was retuned for higher-skill edge cases.
- Town NPCs start wandering immediately after spawning, and no longer keep running after being jailed or killed for leaving town bounds.
- Two hot commands were made significantly faster.
- Staff now have a new character/account history tracking tool, and name-rejection messages are clearer.
- Staff support commands and data generators were refreshed, and the area-policy admin tool was reorganized for easier management.
- Added a new No Damage Zone area type that fully blocks combat, looting, and magic, with proper tamed-pet handling.
- Fixed several shields granting unintended permanent bonus Armor Rating.
- Fixed a bug that could permanently lock players out of their Omega Cache after a server restart.
- Migrated "first to reach class level" tracking to more reliable internal storage.
- Fixed tamed pets getting stuck after a permanent follow failure.
- Corrected internal boat/structure data and a few ship-part tiles.

Thanks for playing Zuluhotel Omega 2.5.
