# Patch Notes - v1.1.1
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: August 8, 2026**

---

Welcome to **Patch 1.1.1**. This patch focuses on **shard stability**, a new **house escrow system** for staff-managed house teardowns, more reliable **house teleporters**, a new **per-account house limit**, and corrections to which areas of the world count as "city" for several gameplay systems.

---

## What Changed

## Shard Stability

### Player Impact

- Fixed several places where the server was leaking resources over time in the email system and the area-policy system, which was a likely contributor to shard crashes. No direct gameplay change, but you should see fewer unexpected crashes/restarts going forward.
- NPCs now wait about 5 minutes of inactivity (up from about 2 minutes) before settling into a dormant "sleep" state, and the radius that wakes a dormant NPC back up is now consistent with the radius that keeps it from falling asleep in the first place.

---

## House Escrow System (New)

### Player Impact

- Staff now have a "Demolish and Escrow" option for tearing down a house that preserves everything inside it (secures, redeedable furniture, bank/vendor contents, and the house deed itself) instead of destroying it outright.
- If your house is torn down this way, use the new `.houseescrow` command to see and claim your recovered items - to your bank or your backpack, your choice. If you can't claim everything at once (destination full), whatever's left stays safely in escrow until you can.

---

## House Teleporters - More Reliable Tracking

### Player Impact

- House teleporters are now tracked more reliably per-house, so removing or replacing them works correctly and they no longer risk being left behind when a house is demolished, redeeded, or escrowed.
- If your house had teleporters placed before this patch, a one-time maintenance pass has been run to bring them up to date with the new tracking - no action needed on your part.

---

## Per-Account House Limit (New)

### Player Impact

- An account can now own a maximum of **5 houses**. This only affects acquiring additional houses (placing a new deed, taking ownership of a house, or buying a static housing plot) - if your account already owns houses, none of them are taken away.

---

## City Area Corrections

### Player Impact

- Fixed the game's detection of which areas count as "city" so it's now accurate per-realm and matches the shard's actual city regions, instead of an old, incomplete hardcoded list.
- The farmland/outbuilding areas around Yew, Vesper, Minoc, Britain, and Skara Brae are no longer treated as "city" - this affects things like fishing net use, tree planting, and exploding potion strength in those specific zones.
- Occlo Isle is now correctly recognized as a city.

---

## Summary

- Fixed resource leaks in the email and area-policy systems that were a likely contributor to shard crashes.
- NPCs take longer to fall asleep when inactive, and their sleep/wake radius is now consistent.
- New staff "Demolish and Escrow" house teardown that preserves all contents instead of destroying them.
- New `.houseescrow` player command to claim recovered items from an escrowed house, to bank or backpack.
- House teleporters are now tracked reliably and no longer risk being left behind on house demolish/redeed/escrow.
- New 5-house-per-account limit on acquiring additional houses (existing houses are unaffected).
- City detection fixed to be realm-accurate; several farmland/outbuilding areas are no longer treated as city, and Occlo Isle now correctly is.

Thanks for playing Zuluhotel Omega 2.5.
