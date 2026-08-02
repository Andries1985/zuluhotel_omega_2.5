# Patch Notes - v1.1.0
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: August 2, 2026**

---

Welcome to **Patch 1.1.0**. This is a smaller follow-up patch focused on **tamed pet ordered-attack fixes** and a **fix for high-HP monsters not dying/despawning properly** in several places around the shard.

---

## What Changed

## Tamed Pets - Ordered Attack Fixes

### Player Impact

- Ordering your pet to attack a monster while inside a no-PK dungeon now actually works - this was being blocked entirely before.
- Ordering your pet to attack a player while in a no-PK area is still refused, but now gives you a clear message instead of occasionally leaving your pet stuck and unresponsive to further commands.
- Ordering your pet to attack anyone while your pet, you, or the target is in a safe area is still blocked, and an untamed (non-donator-mount) pet is still confiscated as before.
- Fixed a bug where an "all kill"/"all attack" order could cause one of your pets to process the command twice on itself.

---

## Monsters - High-HP Death/Despawn Fix

### Player Impact

- Fixed a bug where certain very tough monsters and summoned creatures (including some superboss-tier spawns, summoned blade spirits/energy vortexes/animated dead, Color Wars arena combatants, and Spirit Flock summons) could fail to actually die or despawn when they were supposed to, instead lingering at full health. These now die/despawn correctly.
- Spawn points configured as "Custom NPC" type now correctly remove their monster when the spawn point is destroyed or reset - previously the old monster could be left behind, sometimes resulting in extra/duplicate monsters at that spawn.

---

## Summary

- Ordering your pet to fight a monster in a no-PK dungeon now works.
- Ordering your pet to fight a player in a no-PK area gives a clear refusal message instead of occasionally freezing your pet's commands.
- Fixed pets sometimes double-processing "all kill"/"all attack" orders.
- Fixed certain very tough/summoned monsters not dying or despawning correctly when they should have.
- Fixed "Custom NPC" spawn points not removing their monster on destroy/reset.

Thanks for playing Zuluhotel Omega 2.5.
