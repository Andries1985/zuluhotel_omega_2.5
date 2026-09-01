# Latest Changes
Always check Discord announcements for all the patch notes.

## What Changed

## Vendor Training - Magic Resistance Fixed

### Player Impact

- Mage, Alchemist, and Scribe vendors can now train you in Magic Resistance. Previously no vendor on the shard offered it at all, no matter who you asked. Note: this only applies to vendors spawned after this patch - existing vendors pick it up the next time they respawn.

---

## Shard Stability

### Player Impact

- Fixed a recurring server error where region/area data (safe zones, no-PK areas, guard zones, etc.) could fail to save correctly after certain rare disconnect timing, spamming the server log every save cycle. This didn't reset your area's rules, but it's now fully fixed.
- NPC corpse cleanup is now more reliable if a corpse gets removed early by other systems while it's still waiting to decay.
- Fixed a handful of additional rare edge-case errors reported from the server logs, covering: tamed-pet "Heart" creation on death, opening a trap on a spawnpoint-placed chest, boat encounters and plank-walking (including the boat "destroy everything" drydock option), binding/storing a mount via a Mount Stone (and the Bear/Horse/Llama/Ostard Donator mount stones), viewing guild information, guard-calling near a tamed pet's owner, staff confiscating a tamed pet inside a house, and a necromancer's Release spell. No new behavior - just fewer failures in these specific edge cases.

---

## Healing & Veterinary

### Player Impact

- Attempting to heal or cure a patient (or a creature, including one that isn't yours) that's already dead now correctly stops there instead of continuing on into the heal/cure attempt.

---

## Housing

### Player Impact

- House signs now always show the current owner's name, even if the owner is offline or has changed their name/citizenship since the sign was last opened by them personally.
- The "your backpack is too full" death message is now clearer about what's happening: your items on the corpse aren't lost, you can still loot them, and they'll drop to the ground when the corpse decays if you don't.

---

## Staff Tools & Backend (No Player Impact)

### Player Impact

- No direct gameplay change expected. Two staff migration commands were corrected to require the proper staff level and moved to their proper internal folder; a fix was made so the house-escrow system's audit logs actually get written to disk; a staff diagnostic tool (`.memdump`) was added (and later expanded to also cover killpcs AI memory) to help investigate server memory usage; and developer staff can now use `.resetph` to fix a player stuck unable to start a new personal powerhour. A new Artifact System item, the Eon-Prism, was also added - it lets a player clear their own stuck/used-up personal powerhour - but it isn't in the artifact pool or any loot table yet, so you won't see it in-game this patch. (Its appearance was also touched up since first added - it's now a tinted crystal rather than a runed prism.)

---

## .ph - Now Reports Your Personal Powerhour Too

### Player Impact

- `.ph` used to only tell you about server-wide powerhours. Now it also tells you about your own personal powerhour (from `.setph`): if one's active, how many minutes are left; if not, either that you can start one right now or a countdown to when you'll next be eligible.

---

## Summary

- Mage, Alchemist, and Scribe vendors can now train Magic Resistance.
- Fixed a recurring area-policy server error, made NPC corpse cleanup more reliable, and fixed a large batch of additional rare script errors across mounts, guilds, boats, traps, tamed pets, and NPC death handling.
- Healing/curing an already-dead patient now stops correctly instead of continuing.
- House signs now always show the correct, current owner name; the full-backpack death message is clearer.
- Staff-side housekeeping: corrected command permission levels, fixed house-escrow audit logging, expanded the memory-usage diagnostic tool, and added `.resetph` to unstick players from a broken personal-powerhour state.
- `.ph` now reports your personal powerhour status alongside the server-wide status.
- Added the Eon-Prism, a new Artifact System item (not yet in circulation) that lets a player reset their own personal powerhour.

Thanks for playing Zuluhotel Omega 2.5.
