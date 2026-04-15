# Omega Cache - Patch v1.1

## Summary

This patch addresses bugs and improvements across four areas: Omega Cache post-RC security and data integrity fixes, a magic absorption self-cast bypass, a treasure map Personal Power Hour fix, and a boss pet house confiscation system.

### Omega Cache

The Omega Cache changes close a security vulnerability where players could deposit items from other houses' secure containers into their own cache, effectively stealing items. A centralised `ValidateDepositTarget()` function now gates all deposit operations, verifying the player is inside the cache's house, the target is accessible and within 2 tiles, and secured container permissions are respected. The permission check walks from the target itself up the full container chain looking for secured containers (identified by `usescript == USESCRIPTID_SECURE_CONTAINER`) and validates `REMOVE_FROM_SECURE` via the existing housing `IsFriend` function. The `movable` check was moved to `IsEligibleForStorage` at the item level, so locked-down non-secure containers can be targeted for depositing their contents while non-movable items are still rejected. This constant was adopted across all housing scripts replacing string literals to prevent future regressions.

The gump and command signatures were refactored to thread the `access` struct (from `FindAccessibleContainer`) through all deposit paths, eliminating redundant cache lookups. `RunOmegaCacheGump`, `DoDepositTargeting`, and `DoDepositAll` now take `access` instead of the raw DataFile handle, extracting `df` internally.

A new `stacking_ignore.cfg` configuration aligns Omega Cache item identity with POL's `stacking.cfg` ignored CProps. Items differing only by `BackPackXYZ`, `IDed`, `#SecureRemove`, or `fromLoot` now merge into the same cache entry. `BaseName` and `foodvalue` are deliberately preserved as they are gameplay-meaningful. Ignored CProps are stripped on deposit and not restored on withdrawal. The `CanStack()` function in `canstack.inc` was also updated to match POL core's full `can_add_to_self()` behaviour, adding `inuse` checks and `stacking.cfg` CProp filtering.

Approximately 160 new item category mappings were added covering cooking items, AlchemyPlus potions, talisman gems, fishing shells, verse book scrolls, and candlemaking materials.

### Boss Pet House Confiscation

Previously, boss pets entering a house were killed outright by the sign listener loop — including tamed boss pets that wandered in because their master ran past. This caused players to permanently lose valuable pets through no fault of their own.

The fix replaces the kill with a confiscation system for tamed boss pets. When a tamed boss enters a house, the pet is destroyed but a claim ticket (`0xDF0C`) is created for the owner. The ticket is placed in the owner's backpack (if online and space available), failing that their bank box, failing that the pet is destroyed with a message to the owner. Wild (untamed) bosses are still killed outright.

The ticket is owner-locked — only the original master can redeem it. Redemption is done by giving the ticket to an Animal Trainer and paying a fine based on the pet's `CustomHitsLevel` (or native MaxHP as fallback) divided by 40 (7,500–62,500 gold depending on the boss). The trainer recreates the pet from its NPC template at full health with the original name, color, and master relationship.

The existing operator precedence bug in the boss detection condition was also fixed — `SuperBoss` previously bypassed the `POLCLASS_NPC` check due to `||` binding looser than `&&`.

### Magic Absorption self-cast bypass

The second fix corrects an issue where players could not cast beneficial spells (such as Dispel) on themselves when equipped with Blackrock magic absorption items. The `IsProtected()` function in `spelldata.inc` was missing a self-cast bypass that the `Reflected()` function already had — magic absorption would consume the player's own spell before it could take effect. A simple `caster == cast_on` guard was added to match the existing reflection behaviour.

### Treasure Map Personal Power Hour

The third fix ensures that Personal Power Hour (PPHH) hunting bonuses correctly apply to treasure map loot. The loot system in `starteqp.inc` determines Power Hour eligibility by reading a `KilledBySerial` property from the loot container and looking up the associated player's `#PPHH` flag. Treasure chests created by `digtreasure.src` never had this property set, so the personal bonus was silently ignored (the global Power Hour still worked). The fix passes the digger's character reference into `CreateTreasureChest()` and sets `KilledBySerial` on the chest before `MakeLoot()` populates it, allowing the existing loot functions to find the player and apply the bonus.

## Notable Points

### Omega Cache

- **Security**: `ValidateDepositTarget` is the single gate for all deposit operations — gump, commands, and drag-and-drop all flow through it. Checks house membership, accessibility, 2-tile range (via top-level world object for nested items), backpack/house containment, and secured container permissions. The `movable` check lives in `IsEligibleForStorage` (item-level) so locked-down non-secure containers can be targeted for depositing their contents.
- **Signature changes**: `RunOmegaCacheGump(who, df)` → `RunOmegaCacheGump(who, access)`. `DoDepositTargeting` and `DoDepositAll` also take `access` instead of `df`.
- **Stripped CProps**: `IDed`, `BackPackXYZ`, `#SecureRemove`, `fromLoot` are permanently removed on deposit and not restored on withdrawal. This is intentional and matches POL's stacking philosophy.
- **Cross-package**: `USESCRIPTID_SECURE_CONTAINER` constant adopted in `sign.src`, `signcontrol.src`, `ssign.src` replacing string literals.

### Magic Absorption

- **Self-cast absorption bypass** (`scripts/include/spelldata.inc`): Added `if( caster == cast_on ) return 0; endif` at the top of `IsProtected()`. This mirrors the identical check already present in `Reflected()` at line 885. Affects all spells that call `IsProtected()`, not just Dispel — any beneficial self-cast that was previously blocked by absorption will now work correctly.

### Treasure Map PPHH

- **Treasure map PPHH fix** (`pkg/std/treasuremap/digtreasure.src`): `CreateTreasureChest()` now accepts a `digger` parameter. The digger's serial is written as `KilledBySerial` on the chest before loot generation. No changes were made to `starteqp.inc` — the existing PHH/PPHH checks in `CreateFromItemString()`, `CreateFromRandomGroup()`, and `CreateFromStackString()` now work as intended for treasure chests.
- **Global Power Hour was unaffected**: The global `PHH` check (`GetGlobalProperty("PHH")`) in the loot functions always worked for treasure maps. Only the per-player `#PPHH` personal bonus was broken.
- **Digger vs. killer**: The fix uses the player who dug the treasure (the `character` in the dig script) rather than the player who killed the last guardian. This is a pragmatic choice — guardian corpses are destroyed immediately on death (via `guardkill` flag in `death.src`), making it unreliable to retrieve `KilledBySerial` from them after the fact.

### Boss Pet House Confiscation

- **Confiscation ticket**: New item `0xDF0C` in `config/itemdesc.cfg`. Graphic `0x14F0`, color 1404. Exclusive to confiscation — does not affect the existing stable ticket system (`0x186E`).
- **Fine formula**: `CustomHitsLevel / 40`, falling back to `GetMaxHP / 40` if no custom HP. SuperBoss: 25k–62.5k gold. Boss: 7.5k–27.5k gold.
- **Ticket fallback**: Backpack (online) → bank (online/offline) → pet destroyed with notification.
- **Owner-locked**: `owner_serial` stored on ticket, checked on redemption. Non-owners are rejected and the ticket is returned.
- **Operator precedence fix**: `&&`/`||` replaced with `and`/`or` with explicit parentheses around the `Boss`/`SuperBoss` check.

---

## Acceptance Testing Criteria

### Omega Cache — Deposit Security

| Area | What to Test |
|------|-------------|
| **Deposit from own house** | Deposit Item / Deposit All / drag-and-drop / `.cache deposit` from inside the cache's house. All should work. |
| **Deposit from wrong house** | Walk to a different house with items in backpack. Attempt deposit. Should be rejected. |
| **Deposit from secure without permission** | As a friend with VIEW_SECURE but without REMOVE_FROM_SECURE, target items inside a secured container. Should be rejected. |
| **Deposit from loose bag** | Target items inside a non-secured bag on the house floor. Should be allowed (bag's `.multi` should match cache house). |
| **Deposit from nested secure** | Target an item inside a bag inside a secured container. Should check secure permissions. |
| **Deposit from neighbour's house** | Stand in own house near the wall. Target an item visible in the adjacent neighbour's house. Should be rejected (different `.multi` serial). |
| **Deposit range check** | Target an item more than 2 tiles away within the same house. Should be rejected with "That is too far away." |
| **Deposit from loose bag in neighbour's house** | Drop a bag on the floor in neighbour's house. Stand in own house. Target items inside that bag. Should be rejected (bag's `.multi` belongs to neighbour's house). |

### Omega Cache — Stacking & Categories

| Area | What to Test |
|------|-------------|
| **Stacking — ignored CProps** | Deposit two identical items differing only by `IDed` or `fromLoot`. Should merge into same cache entry. |
| **Stacking — preserved CProps** | Deposit two items of same objtype with different `BaseName` or `foodvalue`. Should be stored separately. |
| **Withdrawal — stripped CProps** | Withdraw an item originally deposited with `IDed`. The withdrawn item should NOT have `IDed`. |
| **Categories** | Browse cache gump. Verify cooking items, potions, gems, fishing shells, verse scrolls are in correct categories, not in "Other". |
| **CanStack consistency** | Merge two backpack stacks differing only by ignored CProps. Verify they stack. |
| **Blacklist enforcement** | Temporarily add a known stackable item (e.g., `Blacklist 0x0F7A { Reason Test }` for Black Pearl) to `blacklist.cfg`, restart server. Attempt to deposit via gump, `.cache deposit`, and drag-and-drop. All should reject with "That item cannot be stored." Verify item is returned to backpack on drag-and-drop. Remove the test entry after. |

### Magic Absorption Self-Cast

| Area | What to Test |
|------|-------------|
| **Self-cast with absorption** | Equip a Blackrock absorption item. Cast a beneficial spell on yourself. Verify it takes effect and is not absorbed. |
| **Absorption vs. hostile spells** | Have another player cast on you with absorption gear. Verify the spell is still absorbed. |
| **Reflection self-cast unchanged** | Equip magic reflection gear. Cast a spell on yourself. Verify it still works (regression check — this was already working). |
| **Reflection vs. hostile spells** | Have another player cast on you with reflection active. Verify the spell is still reflected. |
| **Combined absorption + reflection** | Equip both absorption and reflection items. Verify self-cast works, hostile spells are reflected or absorbed as expected. |

### Treasure Map Personal Power Hour

| Area | What to Test |
|------|-------------|
| **PPHH active during dig** | Activate a Personal Power Hour (Hunting). Dig a treasure map, defeat guardians, and open the chest. Verify loot quantities/chances are doubled compared to a baseline dig without PPHH. |
| **PPHH inactive** | Dig a treasure map without any Power Hour active. Verify loot is at normal levels (no bonus applied). |
| **Global PHH** | Activate a server-wide PHH. Dig a treasure map. Verify bonus loot applies (regression check — this was already working). |
| **PPHH expires mid-dig** | Start a treasure map dig with PPHH active. Let it expire before killing the last guardian. Verify loot reflects the state at chest creation time (PPHH no longer active = no bonus). |
| **Multiple map levels** | Test with different map levels (1-7) under PPHH to verify the bonus scales correctly across loot groups 201-207. |

### Boss Pet House Confiscation

| Area | What to Test |
|------|-------------|
| **Tamed boss enters house** | Have a tamed boss pet follow you into a house. Within 60 seconds it should be confiscated and a ticket created in your backpack. Verify message is sent. |
| **Wild boss enters house** | Lure a wild (untamed) boss into a house. Verify it is killed outright with no ticket. |
| **Ticket in backpack** | Confiscate a tamed boss while online with backpack space. Verify ticket appears in backpack with correct name, fine amount, and pet data. |
| **Ticket in bank (backpack full)** | Fill backpack completely, then trigger confiscation. Verify ticket is placed in bank and message says "bank". |
| **Offline master** | Log out the boss pet owner, trigger confiscation via another player or NPC aggro. Log back in and verify ticket is in bank. |
| **Both full** | Fill backpack and bank, trigger confiscation. Verify pet is destroyed and message warns the player. |
| **Redeem ticket — owner** | Give confiscation ticket to Animal Trainer. Verify fine is announced, gold is deducted, and pet is recreated with correct name, color, and master. |
| **Redeem ticket — non-owner** | Have a different player give the ticket to Animal Trainer. Verify it is rejected and returned to the player's backpack. |
| **Redeem ticket — insufficient gold** | Give ticket with less gold than the fine. Verify rejection message and ticket returned. |
| **Fine amount** | Verify fine equals `MaxHP / 40` for the boss type. Check a Boss (~15k–55k) and a SuperBoss (~50k–125k). |
| **Operator precedence fix** | Verify a SuperBoss NPC (not pet) entering a house is still killed. Previously the `SuperBoss` check bypassed the NPC check. |
| **Normal stable unchanged** | Stable and unstable a regular pet via Animal Trainer. Verify existing `0x186E` ticket flow is unaffected. |

---

## Files Modified

### Omega Cache
- `pkg/opt/omegacache/omegacache.inc` — `ValidateDepositTarget()`, `RunOmegaCacheGump` signature, deposit function signatures, `GetNonDefaultProperties` reads `stacking_ignore.cfg`
- `pkg/opt/omegacache/omegacache.src` — Updated `RunOmegaCacheGump` call
- `pkg/opt/omegacache/stacking_ignore.cfg` — New file
- `pkg/opt/omegacache/categories.cfg` — ~160 new item mappings
- `scripts/textcmd/player/cache.src` — Updated deposit/gump calls
- `scripts/include/canstack.inc` — `inuse` check, `stacking.cfg` filtering
- `config/stacking.cfg` — Comment referencing `stacking_ignore.cfg`
- `pkg/std/housing/sign.src` — `USESCRIPTID_SECURE_CONTAINER` constant
- `pkg/std/housing/signcontrol.src` — `USESCRIPTID_SECURE_CONTAINER` constant, boss pet confiscation (`ConfiscateBossPet`), operator precedence fix
- `pkg/opt/statichousing/ssign.src` — `USESCRIPTID_SECURE_CONTAINER` constant

### Magic Absorption
- `scripts/include/spelldata.inc` — Self-cast bypass in `IsProtected()`

### Treasure Map PPHH
- `pkg/std/treasuremap/digtreasure.src` — `CreateTreasureChest()` accepts digger, sets `KilledBySerial`

### Boss Pet House Confiscation
- `config/itemdesc.cfg` — New `Item 0xDF0C` (confiscation ticket, graphic 5360, color 5184)
- `pkg/std/housing/signcontrol.src` — `ConfiscateBossPet()` function, operator precedence fix, `include "util/bank"`
- `scripts/ai/animaltrainer.src` — `0xDF0C` confiscation ticket branch in `Load_Ticket_Data` (ownership check, fine payment)
