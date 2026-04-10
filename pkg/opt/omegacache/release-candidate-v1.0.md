# Omega Cache — Phase 1 + Phase 2 Release

## Summary

The Omega Cache is a house furniture item that provides virtual, unlimited-capacity storage for stackable resources. Players deposit items into the cache where they are destroyed and recorded as quantities in a per-house DataFile. On withdrawal, items are recreated with all their original properties (color, quality, CProps) preserved. The primary goal is to **reduce server item count** — thousands of physical resource stacks in player houses are replaced by lightweight DataFile entries.

Each house can have 1-3 physical Omega Cache Containers (based on house size), placed via deeds with orientation selection. All containers in a house share the same storage pool. The gump interface provides category-based browsing with pagination, deposit (single item, targeting loop, deposit-all, drag-and-drop), and withdrawal (to target container or directly to backpack).

Phase 2 integrates the cache with all 9 crafting skills: blacksmithy, tinkering, tailoring, carpentry, alchemy, bowcraft/fletching, cooking, inscription, and cartography. Players can target the cache container when prompted for materials, selecting specific material variants from a gump. Alternatively, the **autodraw** system transparently falls back to cache when backpack materials run out mid-craft. A resource lease system prevents concurrent crafters from consuming the same stock. Players can toggle autodraw on/off with `.cache autodraw`.

```
                         +------------------+
                         |   Omega Cache    |
                         |   Container(s)   |
                         |  (house furniture)|
                         +--------+---------+
                                  |
                    +-------------+-------------+
                    |                           |
              Double-click /               Drag & Drop
              .cache command               (OnInsertScript)
                    |                           |
                    v                           v
           +-------+----------+        DepositSingleItem()
           | RunOmegaCacheGump|         - Eligibility check
           | - Categories     |         - BuildItemKey()
           | - Item list      |         - DepositItem()
           | - Deposit/       |         - DestroyItem()
           |   Withdraw       |
           +-------+----------+
                   |
        +----------+----------+
        |                     |
   Deposit flow            Withdraw flow
    - DepositSingleItem     - RecreateItem()
    - DepositFromContainer  - WithdrawItem() [lease-aware]
    - DepositAll            - PromptDestination()
```

```
  Crafting Flow (Phase 2):

  Player uses tool --> Target material
        |                    |
        |              +-----+------+
        |              |            |
        |         Physical    Cache Container
        |          Item       (SelectMaterialFromCache)
        |              |            |
        |              v            v
        |        MakeBackpack   ResourceRequest
        |        Request()     {cache-first}
        |              |            |
        |              +-----+------+
        |                    |
        |             ResourceRequest
        |          {objtype, key, color,
        |           quality, preferredSourceOrder,
        |           dataFileHandle, leaseKey}
        |                    |
        v                    v
   AutoLoop  <----  LeaseResource()
      |                (reserves next iteration)
      |
      +---> GetAvailableResource() >= material?
      |         |
      |    CheckSkill / Craft
      |         |
      |    ConsumeResource()
      |     (backpack first or cache first
      |      per preferredSourceOrder)
      |         |
      |    ExtendResourceLease()
      |         |
      +----< loop more? >
                |
         ReleaseResourceLease()
         AutoLoop_finish()
```

---

## Risk Areas

- **Concurrent access**: Two players crafting from the same cache simultaneously. Mitigated by the lease system (`WithdrawItem` is lease-aware at the data layer, POL uses cooperative multitasking with no preemption within script execution). Test 38 (two-player concurrent access) is outstanding — requires two active players to validate.
- **DataFile persistence**: POL has no `DeleteDataFile` function. Empty DataFiles persist on disk after house demolition. `UnloadDataFile` may discard unsaved changes — removed `CloseOmegaCacheStore` after element deletion to ensure world save persists the cleanup.
- **Pre-existing crafting bugs exposed**: `CreateItemInBackpack` auto-stacks items by objtype before crafting scripts set CProps/color/name, causing different material variants to merge incorrectly (e.g., Iron + Lavarock Sextant Parts). Not introduced by Omega Cache.
- **Missing itemdesc entries**: Several crafting components (Axle `0x105b`, Springs `0x105d`, Clock Parts `0x104f`, Young Oak Logs `0xBA2A`, Cloth `0x1765`) have no `itemdesc.cfg` entry. These items show hex objtypes in the gump and cannot be `.create`'d. Pre-existing.
- **eScript `.+` operator**: Only adds NEW struct members. If a member already exists, `.+` silently does nothing. All code uses plain `.` assignment for existing ResourceRequest members.

---

## Changes

### Core Storage (Phase 1)

- **DataFile storage layer** (`omegacache.inc`): Complete rewrite of the data layer. `BuildItemKey()` creates unique keys from objtype + MD5 hash of non-default properties. `DepositItem()`, `WithdrawItem()`, `RecreateItem()` handle the full deposit/withdraw/recreate lifecycle with property preservation (color, quality, CProps, scripts).

- **Housing integration** (`sign.src`): Added `numomegacache`/`maxnumomegacache` properties (1-3 slots per house type, 37 house types). Lazy-init on first access. House sign displays cache count. House Management gump has "Remove Omega Cache Container" and "Recount Cache Containers" (GM) buttons. Demolition warns about stored items and cleans up DataFile.

- **Container placement** (`placecache.src`): Deed-based placement with standard UO orientation menu (South/East). Validates house ownership, slot availability, and interior placement. Lazy-init of cache slots on first deed use.

- **Container lifecycle** (`destroycache.src`, `cacheinsert.src`): DestroyScript blocks destruction of last container when items stored, re-credits slot. OnInsertScript enables drag-and-drop deposit with eligibility check and permission validation.

- **Gump UI** (`omegacache.inc`): Category-based browsing with config-defined ordering, pagination, tile icons with material colors, BaseName display. Withdrawal via target destination or direct-to-backpack buttons. Deposit Item (targeting loop), Deposit All, and drag-and-drop.

- **Text commands** (`cache.src`): `.cache` (open gump), `.cache deposit` / `.cache deposit target`, `.cache list` / `.cache list <category>`, `.cache withdraw <amount>`, `.cache autodraw` (toggle), `.cache dump` (GM debug).

- **Shared utilities** (`omegacache_utils.inc`): `OpenOmegaCacheStore`, `CloseOmegaCacheStore`, `IsOmegaCacheEmpty`, `GetMaxOmegaCacheForHouse`, `OMEGACACHE_OBJTYPE` constants.

- **Item categorization** (`categories.cfg`): 21 categories across crafting, mage, general, special, and miscellaneous groups. 600+ item objtypes mapped. Config-defined sort order and icons with hue support.

- **Access control**: `who.multi` check ensures player is inside the house. `FindAccessibleContainer` validates ownership, co-ownership, or friend privileges (VIEW_SECURE, ADD_TO_SECURE, REMOVE_FROM_SECURE). GM bypass for cmdlevel >= 4.

### Crafting Integration (Phase 2)

- **Resource Manager** (`resourcemanager.inc`): Centralised `ResourceRequest` struct pattern with `preferredSourceOrder` (backpack-first or cache-first). `GetAvailableResource()` counts across backpack + cache (respects autodraw toggle). `ConsumeResource()` depletes in source order. `MakeBackpackRequest()` builds backpack-first requests with autodraw fallback. `SelectMaterialFromCache()` opens category/item selection gumps for cache-first requests. `PromptBulkAmount()` prompts for quantity on bulk creates (only when cache involved).

- **Resource lease system** (`omegacache.inc`): `CreateLease()`, `ExtendLease()`, `ReleaseLease()`, `GetLeasedAmount()`. Leases reserve a specific quantity of a cache resource for a crafting loop. Key format: `RL#<item_key>|<serial>_<pid>`. TTL-based (60s default), extended each loop iteration. Lease shortfall calculation — only leases `material - backpack_amount`. `WithdrawItem` enforces lease protection at the data layer.

- **Blacksmithy** (`make_blacksmith_items.src`): Cache targeting for ingots and bone. Dual-material bone armor with independent leases. Availability check before create. `ApplyMaterialProperties` backwards-compatible with ResourceRequest.

- **Tinkering** (`tinkering.src`): All paths cache-aware — main loop (wood/metal/glass/clay), complex items (axle+gears, clock, sextant), obsidian golem, gem jewelry, traps, potion keg bottles. All `SubtractAmount`/`.amount`/`DestroyItem` on materials converted. No duplicate `FromCache` functions.

- **Tailoring** (`make_cloth_items.src`): Hides and cloth from cache. Bandage create-all with bulk amount prompt and 240k cloth cap.

- **Carpentry** (`carpentry.src`): Logs, ingots, cloth from cache with independent leases for dual-material crafts. Young oak staff retrofitted.

- **Alchemy** (`alchemy.src`): `CanMake`, `GetBottle`, `TryToMakePotion` retrofitted with optional `regRequest`. Lease lifecycle in loop. Bottles from cache fallback in `GetBottle`. Menu building uses `GetAvailableResource` for recipe availability.

- **Bowcraft/Fletching** (`bladed.src`, `fletch.src`): Shafts, kindling, bows, crossbows, special bows (fire/ice/thunder with cache reagent hides), arrows/bolts, special arrows (fire/ice/thunder). Bulk amount prompt for create-all paths. `CarveLogs` and `MakeArrows` retrofitted with optional ResourceRequest params.

- **Cooking** (`cooking.src`): Autodraw for backpack ingredients. Cache resolved once at entry, reused throughout. Lease on main ingredient. Multi-ingredient recipes draw supporting ingredients from cache.

- **Inscription** (`inscription.src`): `scrollRequest` built from backpack or cache. Lease lifecycle in scroll creation loop. Lease release on all exits (mana depletion, scroll exhaustion).

- **Cartography** (`cartography.src`): `mapRequest` built from backpack with autodraw. `ConsumeMap` always uses `ConsumeResource`. Create-before-consume in `makeNewmap`.

---

## Known Issues

- **Deed acquisition**: Omega Cache Container deeds are not included in any crafting recipe, NPC vendor inventory, or loot table. Currently they can only be created by a GM via `.create OmegaCacheContainerDeed` (objtype `0xDF0B`). A distribution method (vendor, quest, crafting, etc.) must be decided before go-live.
- **Empty DataFiles persist after house demolition**: POL has no `DeleteDataFile` function. Files accumulate in `data/ds/omegacache/`. Elements are deleted but the file shell remains. Harmless but requires periodic manual cleanup or a server startup script.
- **Lease created on last loop iteration**: `ExtendResourceLease` creates a lease for the "next" iteration at the end of each loop. On the final iteration, this lease is immediately released. No practical impact.
- **Pre-existing: Crafted items with different materials auto-stack incorrectly**: POL core's `CreateItemInBackpack` auto-stacks by objtype before CProps/color/name are set. Different material variants of the same objtype merge, adopting the newer item's properties.
- **Pre-existing: Missing itemdesc entries**: Axle (`0x105b`), Springs (`0x105d`), Clock Parts (`0x104f`), Young Oak Logs (`0xBA2A`) have no `itemdesc.cfg`. Cannot be created or tested.
- **Pre-existing: Missing `Desc` fields**: Some items (EmptyBottle, Blankscroll, etc.) show raw `Name` field (e.g., "EmptyBottle") instead of readable display name. Fix: add `Desc` fields.
- **Pre-existing: Cartography shows all map types**: Static menu, no material-based filtering. Player can select recipes they can't afford.
- **Observed once, unreproducible**: Stale `#omegacache_open` CProp blocking gump after access denied then granted. Self-resolves after 10 minutes (TTL).

---

## Acceptance Testing Guide

### Phase 1 — Core Storage

| Area | What to Test |
|------|-------------|
| **Placement** | Place deed inside owned house. Verify orientation selection (South/East). Verify slot decrement. Try placing when slots full — should reject. Try placing outside house or in someone else's house — should reject. |
| **Gump browsing** | Double-click container (range 4 tiles). Verify categories display in correct order with icons. Browse items — verify alphabetical sort, colored tile icons, BaseName display. Paginate through large categories. |
| **Deposit** | Deposit via gump (Deposit Item targeting, Deposit All). Drag-and-drop stackable onto container. Verify non-stackable items rejected and returned to backpack. Verify friend without ADD_TO_SECURE rejected. |
| **Withdraw** | Enter amount, click target icon (prompts for destination) or backpack icon (direct to backpack). Verify correct amount withdrawn, correct properties restored (color, quality, CProps, name). Cancel via ESC — nothing withdrawn. |
| **Access control** | Open from inside house — works. Open from outside — blocked. Friend with VIEW_SECURE can browse but not deposit/withdraw. GM can bypass. `.cache autodraw` toggle — verify crafting respects it. |
| **Container removal** | Remove via House Management. Verify deed returned to backpack. Verify non-last container removable when items stored. Verify last container blocked when items stored. Recount (GM) — verify correct slot count. |
| **House demolition** | Demolish house with items in cache. Verify warning message. Verify containers destroyed. Verify DataFile elements deleted (empty file may persist — known issue). |

### Phase 2 — Crafting Integration

| Area | What to Test |
|------|-------------|
| **Cache targeting** | For each crafting skill: use tool, target cache container, select material from gump, craft. Verify material consumed from cache. |
| **Autodraw fallback** | Have small stack in backpack, more in cache, autodraw enabled. Craft until backpack depletes. Verify seamless fallback to cache mid-loop. Verify lease created when cache kicks in. |
| **Autodraw disabled** | `.cache autodraw` to disable. Craft from backpack near cache. Verify NO cache consumption. Loop should abort when backpack depletes. |
| **Lease lifecycle** | During loop crafting: verify `CreateLease` at start, `ExtendResourceLease` each iteration (with correct shortfall amount), `ReleaseResourceLease` at end. Verify on all exit paths (normal, material depletion, player death). |
| **Availability check** | Remove materials from backpack between opening crafting menu and crafting. Verify "not enough materials" before item creation — no free crafting. |
| **Bulk amount prompt** | For create-all items (shafts, kindling, arrows, bandages): when cache involved, verify prompt appears. Enter valid amount — correct quantity created. Enter over-max — error + retry. Cancel — nothing created. Backpack-only — no prompt. |
| **Dual materials** | Carpentry (logs + ingots), blacksmithy (ingots + bone), bowcraft (logs + reagent hides): verify independent leases, both materials consumed correctly from their respective sources. |
| **Material properties** | Craft with non-default material from cache (e.g., Malachite Ingots, Dragon Hides). Verify crafted product has correct color, quality, material name. |
| **Mixed source** | Have some materials in backpack, some only in cache. Verify backpack consumed first, cache for remainder (backpack-first request). Cache consumed first when player targeted cache (cache-first request). |
| **Skill-specific paths** | Tinkering: complex items (axle+gears+hinge), obsidian golem, traps, potion keg bottles, gem jewelry. Alchemy: menu shows recipes based on total available (backpack+cache), bottles from cache fallback. Inscription: scroll loop with mana gating. Cooking: multi-ingredient recipes. Bowcraft: shafts/kindling (create-all), bows (loop), special bows (reagent hides), special arrows. |
