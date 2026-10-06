# Patch Notes - v1.1.3
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: October 6, 2026**

---

Welcome to **Patch 1.1.3**. A new **`.classinfo`** command that shows you the math behind your class level, **townsfolk are no longer killed for leaving their city**, and a quiet shard-performance fix in the guild system.

---

## What Changed

## New Command - .classinfo

### Player Impact

- New player command: **`.classinfo`**. It opens a gump showing your current class, your class level, and the actual numbers the shard uses to work that out. Until now there was no way to see any of this in-game - you could see your class level take effect, but not how close you were to changing it.
- What the gump shows:
  - Your class name and current class level at the top.
  - **In-class points** - your total skill points across your class's skills, and what percentage of your overall points that represents.
  - **Out-of-class points** - everything else, and its percentage.
  - **Next level** - how many more in-class points you need to reach it, and the percentage of your total that the next level requires.
  - **Or drop X out-of-class points** - the other route to the same level: how many out-of-class points you would need to come off (by unlocking or lowering those skills) to get there without training anything new.
  - **Headroom before dropping** - once you are level 1 or higher, how many more out-of-class points you can safely train before your class level drops a level.
- If a level genuinely cannot be reached by training class skills alone - because your out-of-class points are high enough to hold the percentage requirement out of reach - the gump tells you that directly instead of printing a target you could never hit. The same applies to the drop route: if removing out-of-class skills alone would not be enough, it says so.
- If you are at the maximum class level (6), it says so rather than showing a next-level target.
- If you are not currently qualified for any class, it tells you that instead of opening the gump.
- `.classinfo` only reads your skills and reports on them. It does not change your class, your class level, or any of your skills.

A note for Rangers: Forensic Evaluation is deliberately left out of your point total, because that is the one skill the shard excuses for Rangers when it works out your class level. The gump matches that, so the numbers you see are the numbers actually being used.

---

## Townsfolk - No Longer Killed For Leaving The City

### Player Impact

- Townspeople, persons, nobles and minstrels are no longer confined to the city they spawned in. Until now, any of them that crossed their city's border was teleported straight to jail and killed on the spot, and any that spawned outside a city region was killed the moment it came to life. Both behaviours are gone.
- They now go back to wandering their own neighbourhood the way they did before that system went in: they mill around near where they live, open and step through doors, and if you attack them they still run - but being chased past the city gate no longer kills them.
- You should see fewer townsfolk vanishing for no apparent reason, and civilian NPCs in outlying or newly built settlements that are not flagged as cities will now actually stay alive.
- Two caveats worth knowing. Townspeople and persons will keep to a tighter patch of ground than they did in 1.1.2 - roughly ten tiles around their spawn - because the old "stay near home" leash came back along with the rest of the old behaviour. And because they can now leave town at all, a civilian that strays far enough can run into something dangerous and die to it, with no guards out there to help.
- Nothing changes for already-spawned townsfolk until they respawn or the server restarts.

---

## Shard Performance - Guild Lists Built Only When Needed

### Player Impact

- No gameplay change. The guild system kept two long lookup tables - the list of clothing types a guild uniform can use, and the list of hues a guild can dye itself - in a shared file. Because of how that file was written, both tables were being rebuilt from scratch every time *any* script that touches the guild system started up, and the two biggest offenders were the scripts that run whenever you equip or unequip an item. That meant both tables were being built for every item you put on or took off, and once per item you were already wearing every time the world loaded, even though only the `.guilds` menu ever actually looks at them.
- They are now built the first time something genuinely needs them, which in practice means only when you open the guild uniform or guild colour screens. Everything else - guild chat, verse books, guild uniforms, equipping and unequipping - does no work for them at all.
- The uniform and colour options available to you are exactly the same as before. This was already fixed on ZH3.0; this patch brings the same fix to 2.5.

---

## Also Recorded Here - Shipped With 1.1.2, Missing From Its Notes

### Player Impact

- **Lower server memory use per spawned NPC.** This change went live with Patch 1.1.2 but was left out of that patch's notes, so it is recorded here instead. The NPC setup scripts - the ones that run for animals, archers, town criers, sheep, spellcasters and the aggressive "killpcs" NPCs, plus the shrink wand - each used to load the entire custom-NPC editing library just to reach one small function, the one that dresses a spawned NPC in the gear stored on its spawn point. That library pulls the full older gump toolkit in behind it, well over a thousand lines, none of which those scripts ever used. That one function now lives on its own, so none of the rest gets loaded with it. NPCs are still created, dressed and behave exactly as before - the function itself is unchanged, byte for byte - but the shard carries less memory for every spawned NPC. This is part of the same memory-reduction work as 1.1.2's area-policy changes.

---

## Summary

- New `.classinfo` player command: a breakdown of your class level showing in-class versus out-of-class points and percentages, how many points you still need for the next class level, how many out-of-class points you could drop to reach it instead, and how much headroom you have before losing the level you have now.
- Townspeople, persons, nobles and minstrels are no longer teleported to jail and killed for leaving their city, or killed at spawn for being spawned outside one. They wander their own neighbourhood again, keeping closer to home than in 1.1.2.
- Guild clothing and guild colour lookup tables are now built only when the guild uniform or colour screens actually need them, instead of on every item equip/unequip and once per worn item at world load. No change to the options you see.
- Recorded late from 1.1.2: NPC setup scripts no longer load the whole custom-NPC editing library (and the gump toolkit behind it) just to dress a spawned NPC, lowering server memory use per NPC. No change to how NPCs look or behave.

Thanks for playing Zuluhotel Omega 2.5.
