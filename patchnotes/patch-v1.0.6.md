# Patch Notes - v1.0.6
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: July 1, 2026**

---

Welcome to **Patch 1.0.6**. This update introduces **Champion Relics** and the **World Gem**, fixes several crafting/resource edge cases, resolves runebook overflow behavior, adds follow-up gameplay tuning for High Priest, Dual Planar, loot tables, INT-based skill advancement, tracking, and reanimation behavior, and overhauls the Townstone system with treasury, election, and persistence improvements.

---

## What Changed

## 🏆 Champion Relics - New Artifact Items

Two new artifacts have been added to the Artifact System: **Champion Relics**.

### Player Impact

- Champion Relics can now drop from the Artifact Box.
- **Tier 1 Relic** summons a Tier 10 champion boss (Barracoon or Chaos God) when activated.
- **Tier 2 Relic** summons a Tier 11 champion boss (Rikktor or Nyx) when activated.
- Relics cannot be used in safe areas, guarded areas, or NOPK zones.
- The relic is destroyed on use.
- Relics have a **14-day expiry timer**. You can check remaining time in the item tooltip.
- Expired relics are automatically removed from storage by a background sweeper.

---

## 🗺️ Cartography - Resource and Skill Gain Fixes

Cartography had bugs where skill gain could occur or maps could be created even without the required materials.

### Player Impact

- All map tiers now correctly check you have enough resources **before** granting skill gain or creating a map.
- Blank maps must now be in your backpack to use — you can no longer drag-target one from a bank or container.
- No functional changes to map content, costs, or difficulty.

---

## 🔥 Camping - Kindling Source Fix

### Player Impact

- Kindling wood used for camping must be in your backpack or on the ground. Targeting wood inside a bank container is no longer allowed.

---

## ⚒️ Blacksmithy - Jewelry Crafting Fix

### Player Impact

- Making jewelry now correctly stops when you don't have enough gold, instead of proceeding to the skill check.

---

## 🐾 Tamed Pets - Overhang Following Fix

### Player Impact

- Tamed pets now follow correctly through tower and multi overhangs. Previously, pets could stall or drop follow state when the path crossed overhang geometry.

---

## 🏪 Player Merchant - Stock Announcement Fix

### Player Impact

- Hidden or concealed characters controlling a player merchant will no longer trigger a stock announcement event, preventing unintended reveals.

---

## 💎 World Gem - New Artifact Utility

### Player Impact

- A new artifact item, the **World Gem**, has been added to the Artifact Box.
- The World Gem can bless a targeted item when used.
- World Gems have a **2-week expiry timer** and show their remaining time in the item tooltip.
- Artifact tooltip styling has been updated to a brighter highlighted color.
- The Artifact Box itself is now movable.

---

## 📘 Runebook - Recall Scroll Overflow Fix

### Player Impact

- Runebooks now recharge correctly when a scroll stack is larger than remaining charges.
- Example: dropping 2 recall scrolls on a runebook missing 1 charge now recharges the book by 1 as expected.
- Any extra scrolls are returned to backpack when possible.
- If backpack is full, extras are dropped to the ground when possible.
- If backpack return and ground drop both fail, only the excess scrolls are destroyed and recharge still succeeds.

---

## 🛠️ Internal - Script API Compatibility Cleanup

### Player Impact

- No gameplay change expected.
- Deprecated `MoveItemToLocation` script calls were migrated to `MoveObjectToLocation` in affected scripts for modern POL compatibility.

---

## ⛪ High Priest - Relationship Cost Prompt

### Player Impact

- If your relationship with the High Priest is upset, you can now say **relationship** to see exactly how much gold is required to repair it.
- The required donation scales with your class level (`class level * 2500`).
- If no class level is detected, the priest now gives a fallback hint that any donation amount can begin repairing the relationship.
- If your relationship is already in good standing, the priest now says so directly.

---

## ⚔️ Dual Planar - Protection Gate Removed

### Player Impact

- The protection/immunity gate on Dual Planar's on-hit effect was removed.
- This means previously protected targets are no longer skipped by that specific immunity branch.

---

## 💎 Loot Tables - More Life Crystal Coverage

### Player Impact

- `lifecrystal` was added to additional entries within the Junk loot group.
- You should now see broader life crystal availability from junk-group loot rolls.

---

## 🧠 INT Skill Advancement Tuning

### Player Impact

- Several INT-based skill advancement rates have been retuned.
- This affects how quickly some intelligence-linked skills advance over time.
- No new skill system was added; this is a balance/tuning pass.

---

## 🛍️ Vanity Shop - Bulk Transcription Options

### Player Impact

- New bundle purchase options were added for transcription-related vanity items.
- You can now buy certain transcription items in bulk packs (including higher-count bundles) instead of one-by-one only.
- Transcendence scroll bulk bundle options were added back in.
- Bundle creation now includes stricter backpack-space handling to reduce failed purchase edge cases.

---

## 🎨 Vanity Item Tools - Backpack Scope Restriction

### Player Impact

- Custom item dye and custom item rename tools now only work on items inside your backpack.
- Attempting to target items outside backpack scope now shows an explicit message and safely exits.

---

## 🛡️ Staff Command Safety - .akill

### Player Impact

- `.akill` now skips mobiles with command level, preventing staff characters from being killed by the command.
- NPC and normal-player kill behavior remains unchanged.

---

## 🏛️ Townstones - Treasury, Elections, and Persistence Rework

### Player Impact

- Townstones were reworked to use persistent datastore-backed town data.
- Town treasury gold is now tracked persistently and shown directly on the townstone.
- Citizens can donate cheque gold to the town treasury from the townstone gump.
- Election and poll flows were reworked for improved reliability.
- Election/poll expiry now resolves even if nobody has the townstone gump open.
- Latest poll results are shown on the townstone after a poll ends.
- Town election cooldown messaging is clearer, including time-until-next-election information.
- Townstone data now survives delete/recreate flows for the same region.

---

## 🏦 Banker - Cheque Prompt Improvement

### Player Impact

- Bankers now better account for the total gold you can convert into cheques when prompting cheque creation.
- This improves cheque usability when your available value includes cheque-backed amounts, not just loose gold.

---

## 🧭 Tracking - Player Tracking Restored

### Player Impact

- Tracking can now properly detect and list nearby players again.
- The tracking menu once again restores a dedicated **Players** category when valid player targets are nearby.

---

## 🌪️ Winds Breath - Duration Update

### Player Impact

- Winds Breath paralysis duration now scales directly from Holy Protection and Free Action instead of the previous layered resistance/class formula.
- Targets with Free Action now receive a shorter max paralysis duration than targets without it.
- Very high Holy Protection can fully negate the paralysis duration.

---

## ☠️ Reanimated Creatures - Loot and Summon Fixes

### Player Impact

- Reanimated creatures now transfer and preserve loot correctly during rise/animation flows.
- Reanimated Soul Whisperers can no longer summon bosses after being raised.

---

## 📋 Summary

✅ Champion Relics added — summon a boss from your artifact  
✅ Cartography resource checks corrected; blank maps must be in backpack  
✅ Camping kindling sourced only from backpack or ground  
✅ Jewelry crafting gold check now properly exits on failure  
✅ Tamed pet following fixed for tower/multi overhang geometry  
✅ Player merchant stock announcements suppressed while hidden/concealed  
✅ Runebook recall-scroll overflow now recharges safely and handles extras correctly  
✅ Deprecated item-move API calls migrated for core compatibility  
✅ High Priest now reports relationship repair gold requirement  
✅ Dual Planar immunity/protection gate removed on hit logic  
✅ Life crystal appears in more Junk loot entries  
✅ World Gem added and Artifact Box made movable  
✅ High Priest gives a good-standing relationship response  
✅ INT-based skill gain tuning updated  
✅ Vanity shop now supports bulk options for transcription items  
✅ Transcendence scroll bulk bundles were restored  
✅ Custom item dye/rename now require target item in backpack  
✅ `.akill` now excludes staff mobiles  
✅ Townstones now persist treasury/election data more reliably  
✅ Town treasury donations, balances, and poll/election handling were reworked  
✅ Banker cheque creation now uses improved total-available prompting  
✅ Tracking can detect players again  
✅ Winds Breath paralysis duration was reworked around Holy Protection and Free Action  
✅ Reanimated creatures now preserve loot correctly and Soul Whisperers no longer summon bosses when raised

---

Thanks for playing Zuluhotel Omega 2.5.
