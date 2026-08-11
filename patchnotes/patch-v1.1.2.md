# Patch Notes - v1.1.2
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: August 11, 2026**

---

Welcome to **Patch 1.1.2**. This is a small maintenance patch: **Magic Resistance is now trainable from vendors again**, plus a batch of quiet bug fixes and staff-side housekeeping.

---

## What Changed

## Vendor Training - Magic Resistance Fixed

### Player Impact

- Mage, Alchemist, and Scribe vendors can now train you in Magic Resistance. Previously no vendor on the shard offered it at all, no matter who you asked. Note: this only applies to vendors spawned after this patch - existing vendors pick it up the next time they respawn.

---

## Stability - Rare Script-Error Fixes

### Player Impact

- Fixed a handful of rare edge-case errors reported from the server logs, covering: tamed-pet "Heart" creation on death, opening a trap on a spawnpoint-placed chest, boat encounters and plank-walking, binding/storing a mount via a Mount Stone, and the Bardic Boulders bard spell effect. No new behavior - just fewer failures in these specific edge cases.

---

## Staff Tools & Backend (No Player Impact)

### Player Impact

- No direct gameplay change expected. Two staff migration commands were corrected to require the proper staff level and moved to their proper internal folder; a fix was made so the house-escrow system's audit logs actually get written to disk; and a new staff diagnostic tool (`.memdump`) was added to help investigate server memory usage.

---

## .ph - Now Reports Your Personal Powerhour Too

### Player Impact

- `.ph` used to only tell you about server-wide powerhours. Now it also tells you about your own personal powerhour (from `.setph`): if one's active, how many minutes are left; if not, either that you can start one right now or a countdown to when you'll next be eligible.

---

## Summary

- Mage, Alchemist, and Scribe vendors can now train Magic Resistance.
- Fixed several rare script errors: tamed-pet heart creation, spawnpoint chest traps, boat/plank edge cases, Mount Stones, and Bardic Boulders.
- Staff-side housekeeping: corrected command permission levels, fixed house-escrow audit logging, and added a new memory-usage diagnostic tool for staff.
- `.ph` now reports your personal powerhour status alongside the server-wide status.

Thanks for playing Zuluhotel Omega 2.5.
