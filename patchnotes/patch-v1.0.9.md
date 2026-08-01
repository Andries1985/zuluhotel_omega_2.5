# Patch Notes - v1.0.9
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: July 30, 2026**

---

Welcome to **Patch 1.0.9**. This is a smaller follow-up patch focused on **NPC skill fixes**, **Rainbow Ostard toughness**, **bard song fixes**, a **power scroll (Alchemy tome) cap bug**, and a couple of small crafting and guild updates.

---

## What Changed

## NPC Skills - Fixed Missing Skills

### Player Impact

- Treasure Hunters, Thieves, and Kobold Mercenaries now actually have working Detect Hidden - a config typo meant they were never getting this skill before.
- Jewelers, Mages, and the Listener NPC now actually have working Item Identification.
- Mages now actually have working Spirit Speak.
- Shadow Golems are now fully immune to poison instead of only lightly resistant.

---

## Rainbow Ostards - Much Tougher

### Player Impact

- Rainbow Ostard and Rainbow Frenzied Ostard now have significantly more max HP (3000 and 7000) than before. These were briefly going to ship far weaker than intended due to a data-entry mistake, caught and corrected before release.

---

## Loot and Crafting

### Player Impact

- Corrupt Angels now drop feathers.
- Elven Glasses and Dragon Helm can now be used with the tailoring Fortify Hat feature - they weren't recognized as valid hat/helm items before.

---

## Bard Songs - Cloaking and Boost Fixes

### Player Impact

- Song of Cloaking (bard invisibility) now hides everyone nearby, not just members of the bard's own party/group.
- Fixed a bug where two players who each had no party could sometimes get incorrectly treated as being in the same party for song-boost purposes.

---

## Guilds - Guild Tag Shown

### Player Impact

- The guild member list now shows your guild's tag/abbreviation above the member count.

---

## Power Scrolls - Alchemy Tome Fix

### Player Impact

- Fixed a bug where an Alchemy power scroll (tome) could still be used to attempt raising your Alchemy skill cap even after it was already at your maximum, when every other skill correctly blocked this. Alchemy tomes now respect the cap like every other skill.

---

## Areas - No Damage Zone Removed

### Player Impact

- No direct gameplay change expected. The "No Damage Zone" area type added in 1.0.8 was never assigned to any live area and wasn't working correctly, so it has been fully removed from the area system and admin tools.

---

## Spawnpoints - Behind-the-Scenes Default Change

### Player Impact

- No direct gameplay change expected. Newly created or reset spawnpoints now default to removing their spawned monsters when the spawnpoint itself is destroyed, instead of leaving them behind.

---

## Staff Tools and Behind-the-Scenes Fixes

### Player Impact

- No direct gameplay change expected. Fixed a bug where a death with no recorded killer could leave bad data behind instead of being recorded cleanly, and staff tools were hardened to display old bad records safely.
- Staff have a new tool to look up an account's login IP history, and to look up a character's houses, boats, and corpses with quick "go to" navigation, plus an offline tool for building new NPC templates. None of this affects players directly.

---

## Chests - Powerplayers Can Now Pick Spawn-Point Chests

### Player Impact

- Powerplayers can now use lockpicking to open locked treasure/spawn-point chests, previously restricted to Thieves only.
- Level 7 (top-tier) spawn-point chests remain Thief-only - a Powerplayer who isn't also a Thief will be told the lock is too advanced for them.

---

## Townstones - Upgrade Store Changes

### Player Impact

- The "Shady Merchant" vendor upgrade has been removed from the townstone upgrade store.
- Safe Area and No-PK Area townstone upgrades now cost 3,000,000 gold, up from 1,000,000.

---

## Summary

- Treasure Hunters, Thieves, Kobold Mercenaries, Jewelers, Mages, and the Listener NPC now have the skills their templates always listed but weren't actually granting.
- Shadow Golems are now fully poison immune.
- Rainbow Ostard and Rainbow Frenzied Ostard max HP significantly increased (3000 / 7000).
- Corrupt Angels now drop feathers.
- Elven Glasses and Dragon Helm now work with Fortify Hat.
- Song of Cloaking now hides everyone nearby, not just your own party.
- Fixed unpartied players sometimes being mismatched as party members for song boosts.
- Guild member list now shows your guild's tag.
- Alchemy power scrolls (tomes) now correctly respect your skill cap.
- The unused, non-functional "No Damage Zone" area type has been removed.
- Spawnpoint default behavior changed so new/reset spawnpoints despawn their monsters on destroy.
- Fixed a death-recording bug and hardened staff tools against old bad data; staff also gained new account/asset lookup tools.
- Powerplayers can now pick locked spawn-point chests (level 7 chests remain Thief-only).
- Removed the "Shady Merchant" townstone vendor upgrade; Safe Area and No-PK Area upgrades now cost 3,000,000 gold.

Thanks for playing Zuluhotel Omega 2.5.
