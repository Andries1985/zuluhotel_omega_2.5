# Change Summary: Finalising plan for Omega Cache -> Present

## Scope
- Start commit: 8b0de10 (2026-03-23, "Finalising plan for Omega Cache")
- End commit: e9f4d44 (2026-05-02, current HEAD)
- Commit window: 8b0de10^..HEAD
- **Appendix G covers the additional ~50 commits between 3f3bc98 (2026-04-19) and e9f4d44 (2026-05-02)**

## High-level totals
- ~280 commits (including post-April-19 additions)
- 174+ files changed

## Executive summary
From the Omega Cache planning finalization point onward, the branch delivered:

1. Omega Cache end-to-end implementation
- New package introduced with house-linked DataFile storage, placement/removal lifecycle, gump UI, commands, category mapping, blacklist support, and docs.
- Core Omega Cache files were mostly new additions rather than incremental edits.

2. Omega Cache hardening and follow-up patching
- Security, access, and usability passes were made through iterative "testing feedback" cycles.
- v1.0 RC, v1.1 patch, and v1.2 patch documentation were added and maintained.

3. Crafting/resource integration across shard systems
- Added shared resource manager include and extended crafting flows so material consumption can integrate with cache-backed resources.
- Major touchpoints include blacksmithy, tinkering, tailoring, carpentry, alchemy, bowcraft/fletching, cartography, cooking, and inscription paths.

4. Class and progression changes with direct gameplay impact
- Thief-only dungeon treasure chests were enforced in lockpicking flow (`PickTreasureChest` checks thief class), with spawnpoint chest levels now reaching level 7 and lock difficulty scaling up to top-tier chests.
- Bards gained a chance to roll treasure maps up to level 7, including level-7 guardian/chest paths.
- Paladin spell-resist scaling was rebalanced in `Resisted()` (`spelldata.inc`) from `chance * ClasseBonus * 0.5` to the newer half-bonus model (example: a `1.75` class bonus now yields a `1.375x` chance multiplier), improving reliability of paladin resistance checks.
- Skill/cap maintenance tooling was expanded in the same window (test/admin support scripts and equip/unequip cap checks).

5. Loot pipeline and drop-table expansion
- Treasure map lootgroups were moved to a dedicated level band (`201`-`207`) instead of the previous low-number map slots.
- Spawnpoint chest lootgroups were normalized to a dedicated band (`301`-`307`) with weighted selection and level-aware lock/magic scaling.
- Magic progression was extended from level 10 to level 11 across loot application paths (weapon/armor/shield/jewelry/clothing enchant logic).
- Shield loot logic was split into dedicated shield application functions in `starteqp.inc`, including separate proc/mod handling from generic armor paths.

6. Ranger-class boss pet confiscation system
- A ranger who has tamed a boss or super-boss pet can now have it confiscated if it enters a player house, instead of the pet being permanently destroyed.
- A claim ticket (`0xDF0C`) is issued to the owner (backpack → bank → destroyed with notification). The ticket is owner-locked and redeemable at an Animal Trainer for a gold fine equal to `MaxHP / 20` (Boss: ~15k–55k gold; SuperBoss: ~50k–125k gold).
- Wild (untamed) boss NPCs entering a house are still killed outright. A pre-existing operator-precedence bug that caused `SuperBoss` to bypass the `POLCLASS_NPC` check was also fixed in the same change.

7. RPer stone updated for new classes
- Three new starter classes were added to the RPer faction stone: **Paladin**, **Mystic Archer**, and **Bladesinger**.
- Each class has a dedicated starter bag function in `rper.inc` provisioning class-appropriate weapons (marked `IsRPer`, `dmg_mod 20`), armor, consumables, reagents/arrows, and starting gold.
- The stone gump layout in `rperstone.src` was extended with three additional radio entries and text labels, and the Okay button position adjusted to accommodate the longer list.
- The stone program signature was also cleaned up (unused `newstone` parameter removed).

8. Housing, NPC, AI, combat, and balancing changes in same merge window
- House/sign flows, NPC definitions/AI behaviors, loot and treasure map logic, spell/death handling, and tooltip/combat formula files were updated.
- These are not all Omega Cache-only changes, but they are part of the same commit window and changed-file set.

9. New utility/admin/test scripts and balancing support data
- New test/admin commands were introduced (including cache/admin helpers and skill-cap related utilities).
- New/updated config datasets (including NPC magic item level table and loot/NPC/config updates) were included.

10. Banking flow and transaction hardening
- `scripts/ai/banker.src` was substantially refactored: explicit `deposit`, `withdraw`, `check/cheque`, `balance`, and `count` flows were split into dedicated functions with stronger validation and failure rollback behavior.
- A new Banker's Order item (`0x14000`) replaced the old check object path, with stricter banker intake rules and a 5,000,000 gold per-transaction cap.
- Deposit-all gained a confirmation gump to prevent accidental full-backpack transfers.

11. Omega Cache v1.3 — AlchemyPlus cache integration and talisman cache fix
- Talisman crafting now supports Omega Cache autodraw, applies the Crafting Power Hour half-material discount, and consumes materials before the skill check (closing the duplication window).
- AlchemyPlus reagent consumption was fully ported to the resource-manager pattern (autodraw, lease lifecycle, basicpot level resolution, and container autodraw).
- `ConsumeResource` gained partial-consume detection and structured `SysLog` output for support tracing.
- The "run out of flasks" autoloop bug was fixed (create-then-consume ordering).

12. Tamed AI and pet count improvements
- Tamed caster AI was overhauled: casters now stay within 2–10 tiles of their target, self-defend on first loop, and check NPC template spell lists before casting.
- Boss/super-boss pet count caps were clarified: c5 allows 2 pets + mount, c6 allows 2 boss pets + 2 mounts.
- Mass spell casting fixes for tamed NPCs (released pets no longer cast mass spells on players).
- Shield skill advancement fixed (`parry` / `shields` now advances correctly in tamed AI).

13. Combat and buff rebalancing
- Fire katana (`sunshinespear`) now deals fire damage instead of default physical damage.
- Bard song buffs (Song of Defense, Glory, Haste) normalized to deterministic spec-tier tables; Song of Haste now also buffs STR and INT.
- Taint (polymorphed AR bonus) cap raised to 75; Mego buff changed from a random roll to a flat 5-minute duration.
- PowerPlayer class modifier raised to `3x` in Enlightenment and Earth Blessing (was `2x`).
- Exceptional crafting difficulty is now capped at 150 across blacksmithy, tinkering, tailoring, inscription, and crafter boost.

14. Loot table tuning
- Armor stat distribution in `ApplyArmourModNew` rebalanced at all levels 2–11: HP/AR-skill mods have substantially higher chances (previously as low as 0.7% at level 4); on-hit script chance correspondingly reduced.
- Nyx Seductress and Rikktor promoted to `Magicitemlevel 11` (from 10).
- Barracoon and Erebus Chaos God promoted to `Magicitemlevel 10` (from 9).
- Treasure map magic item chance is now randomized per level (level 1–4: 75–100, level 5: 80–100, level 6: 85–100, level 7: 90–100).

15. Totem changes
- Obsidian golems (totems) now require the Crafter class to activate.
- Totems can no longer be spellbound (added `totem` property check to `spellbind.src`).
- Magery skill requirement for crafting totems was removed; Tinkering (70) is now the sole skill gate.

16. Townsfolk AI and city-bounding
- Townsfolk (minstrel, noble, performer, person, townperson) now stay within the city they were spawned in via a new `InitTownBounds()` / `WanderWithinTown()` pattern.
- Town AI consolidated into shared `townsfolk.inc` and `anchors.inc` includes, reducing code duplication across five NPC scripts.

17. Miscellaneous fixes and housekeeping
- Tent deeds (`0x601E` blue tent, `0x601F` green tent) removed from MRC merchant product pool.
- Begging blocked inside multis and from NPCs that are inside multis.
- Bankers were excluded from the begging interaction entirely.
- Player merchants can now issue Banker's Order cheques (up to 1,000,000 gold) via the `cheque` command.
- Bladesinger RPer starter bag updated with revised equipment.
- Wyrm level 9 and 10 weapon/armor NPC colors corrected.

## Major change domains (by impact and breadth)
- Omega Cache package and docs: pkg/opt/omegacache/*
- Crafting resource flows: scripts/include/resourcemanager.inc and multiple pkg/std crafting scripts
- Loot/progression pipeline: config/nlootgroup.cfg, pkg/std/treasuremap/digtreasure.src, pkg/opt/spawnpoint/checkpoint.src, scripts/include/starteqp.inc
- Class-specific progression gates: pkg/opt/chests/lockpicking.src, pkg/std/treasuremap/digtreasure.src
- Housing and permissions/access: pkg/std/housing/sign.src, pkg/std/housing/signcontrol.src (including boss pet confiscation, `ConfiscateBossPet`)
- Combat/tooltips/spells/death handling: pkg/systems/combat/*, scripts/include/spelldata.inc, scripts/misc/chrdeath.src
- AI and town/vendor behavior updates: scripts/ai/*
- Banking and cheque system rewrite: scripts/ai/banker.src, config/itemdesc.cfg (new `bankersorder` `0x14000`)
- Omega Cache v1.3: pkg/opt/omegacache/*, pkg/opt/alchemyplus/alchemyplus.src, pkg/std/tinkering/tinkering.src, scripts/include/resourcemanager.inc
- Tamed AI overhaul: scripts/ai/tamed.src, config/npcdesc.cfg
- Combat/buff rebalancing: scripts/include/starteqp.inc, pkg/opt/songbook/*.src, pkg/opt/alchemyplus/newpotions.src, pkg/systems/combat/config/itemdesc.cfg
- Townsfolk AI consolidation: scripts/ai/minstrel|noble|performer|person|townperson.src, scripts/include/townsfolk.inc, anchors.inc

---

## Appendix C: Complete changed-file inventory (status + path)
```text
STATUS	FILE
M	config/equip.cfg
M	config/itemdesc.cfg
M	config/menus.cfg
M	config/nlootgroup.cfg
M	config/npcdesc.cfg
M	config/stacking.cfg
A	npc_magicitemlevel_table.csv
M	pkg/items/armor/include/armorZones.inc
M	pkg/opt/alchemyplus/alchemyplus.cfg
M	pkg/opt/alchemyplus/alchemyplus.src
M	pkg/opt/alchemyplus/itemdesc.cfg
M	pkg/opt/alchemyplus/newpotions.src
M	pkg/opt/alchemyplus/potionbook.src
M	pkg/opt/capper/capper.src
M	pkg/opt/chests/lockpicking.src
M	pkg/opt/christmas/giftopen.src
M	pkg/opt/colorwars/cwars.src
M	pkg/opt/crafterboost/crafterboost.cfg
M	pkg/opt/crafterboost/itemdesc.cfg
M	pkg/opt/crafterboost/make_crafter_boosts.src
A	pkg/opt/crafterboost/rechargeflask.src
M	pkg/opt/farming/spinning.src
M	pkg/opt/guilds/include/guildconstants.inc
M	pkg/opt/lootlottery/include/lootlottery.inc
A	pkg/opt/omegacache/blacklist.cfg
A	pkg/opt/omegacache/cacheinsert.src
A	pkg/opt/omegacache/categories.cfg
A	pkg/opt/omegacache/destroycache.src
A	pkg/opt/omegacache/docs/PLAN.md
A	pkg/opt/omegacache/docs/changelog.md
A	pkg/opt/omegacache/docs/patch-v1.1.md
A	pkg/opt/omegacache/docs/patch-v1.2.md
A	pkg/opt/omegacache/docs/release-candidate-v1.0.md
A	pkg/opt/omegacache/itemdesc.cfg
A	pkg/opt/omegacache/omegacache.inc
A	pkg/opt/omegacache/omegacache.src
A	pkg/opt/omegacache/pkg.cfg
A	pkg/opt/omegacache/placecache.src
A	pkg/opt/omegacache/stacking_ignore.cfg
M	pkg/opt/powerhour/powerhour.src
M	pkg/opt/powerscrolls/itemdesc.cfg
M	pkg/opt/powerscrolls/powerscroll.src
A	pkg/opt/powerscrolls/textcmd/test/lowerallchosencaps.src
A	pkg/opt/powerscrolls/textcmd/test/lowercaps.src
A	pkg/opt/powerscrolls/textcmd/test/raiseallchosencaps.src
R094	pkg/opt/powerscrolls/textcmd/admin/raisecaps.src	pkg/opt/powerscrolls/textcmd/test/raisecaps.src
M	pkg/opt/powerscrolls/transcendscroll.src
M	pkg/opt/roleplaying/rper.inc
M	pkg/opt/roleplaying/rperstone.src
M	pkg/opt/shilhook/omegaattack.inc
M	pkg/opt/shrink/textcmd/test/shrink.src
M	pkg/opt/spawnpoint/checkpoint.src
M	pkg/opt/statichousing/ssign.src
A	pkg/opt/talisman/config/icp.cfg
A	pkg/opt/talisman/config/itemdesc.cfg
A	pkg/opt/talisman/include/talismanid.inc
A	pkg/opt/talisman/pkg.cfg
A	pkg/opt/talisman/talisman/method.src
A	pkg/opt/talisman/talisman/use.src
M	pkg/opt/zuluitems/TestBoostStone.src
M	pkg/opt/zuluitems/booststone.src
M	pkg/opt/zuluitems/testclassbooststone.src
M	pkg/packethooks/megacliloc/itemdata.src
M	pkg/packethooks/packethook/packethook.src
M	pkg/std/alchemy/alchemy.src
M	pkg/std/begging/begging.src
M	pkg/std/blacksmithy/blacksmithy.cfg
M	pkg/std/blacksmithy/make_blacksmith_items.src
M	pkg/std/carpentry/carpentry.cfg
M	pkg/std/carpentry/carpentry.src
M	pkg/std/cartography/cartography.src
M	pkg/std/cooking/cooking.src
M	pkg/std/cooking/grinding.src
M	pkg/std/dundee/lifecrystal.src
M	pkg/std/dundee/totem.src
M	pkg/std/healing/healing.src
M	pkg/std/housing/sign.src
M	pkg/std/housing/signcontrol.src
M	pkg/std/inscription/inscription.src
M	pkg/std/itemid/itemid.inc
M	pkg/std/mining/mining.src
M	pkg/std/removetrap/removetrap.src
M	pkg/std/spells/dispel.src
M	pkg/std/spells/unlock.src
M	pkg/std/tailoring/itemdesc.cfg
M	pkg/std/tailoring/make_cloth_items.src
M	pkg/std/tailoring/scissors.src
M	pkg/std/tinkering/tinker.cfg
M	pkg/std/tinkering/tinkering.src
D	pkg/std/tracking/tracking.cfg
M	pkg/std/tracking/tracking.src
M	pkg/std/traps/traps.src
M	pkg/std/treasuremap/digtreasure.src
M	pkg/std/treasuremap/guardians.cfg
M	pkg/systems/combat/config/enchantableitems.cfg
M	pkg/systems/combat/config/itemdesc.cfg
M	pkg/systems/combat/dualplanarscript.src
M	pkg/systems/combat/include/hitscriptinc.inc
M	pkg/systems/crafting/include/craftingfunctions.inc
M	scripts/ai/aloof.src
M	scripts/ai/animaltrainer.src
M	scripts/ai/banker.src
M	scripts/ai/chaosmultikillpcs.src
M	scripts/ai/combat/doppelcombatevent.inc
M	scripts/ai/cstguard.src
M	scripts/ai/daves_healer.src
M	scripts/ai/main/animalsetup.inc
M	scripts/ai/main/chaoskillpcsloop.inc
M	scripts/ai/main/criersetup.inc
M	scripts/ai/main/dumbkillpcsloop.inc
M	scripts/ai/main/killpcsloop.inc
M	scripts/ai/main/mainloopanimal.inc
M	scripts/ai/main/mainloopbarker.inc
M	scripts/ai/main/mainloopcat.inc
M	scripts/ai/main/mainloopchicken.inc
M	scripts/ai/main/mainloopgood.inc
M	scripts/ai/main/mainloophelp.inc
M	scripts/ai/main/mainloopkillany.inc
M	scripts/ai/main/mainloopmeek.inc
M	scripts/ai/main/mainloopquestie.inc
M	scripts/ai/main/mainloopsheep.inc
M	scripts/ai/main/mainloopwolf.inc
M	scripts/ai/main/questiesetup.inc
M	scripts/ai/main/setup.inc
M	scripts/ai/main/vortexloopkill.inc
M	scripts/ai/merchant.src
M	scripts/ai/noble.src
M	scripts/ai/person.src
M	scripts/ai/playermerchant.src
M	scripts/ai/rockthrower.src
M	scripts/ai/setup/animalsetup.inc
M	scripts/ai/setup/archersetup.inc
M	scripts/ai/setup/questiesetup.inc
M	scripts/ai/setup/setup.inc
M	scripts/ai/setup/sheepsetup.inc
M	scripts/ai/townperson.src
M	scripts/ai/warrior.src
M	scripts/control/corpsedecay.src
M	scripts/control/skilladvancerequip.src
M	scripts/control/skilladvancerunequip.src
M	scripts/include/anchors.inc
M	scripts/include/attributes.inc
A	scripts/include/canstack.inc
M	scripts/include/classes.inc
M	scripts/include/client.inc
M	scripts/include/constants/cfgfiles.inc
M	scripts/include/constants/layers.inc
M	scripts/include/constants/locations.inc
M	scripts/include/constants/propids.inc
M	scripts/include/itemutil.inc
M	scripts/include/objtype.inc
A	scripts/include/omegacache_utils.inc
M	scripts/include/randname.inc
A	scripts/include/resourcemanager.inc
M	scripts/include/skillpoints.inc
M	scripts/include/spelldata.inc
M	scripts/include/starteqp.inc
M	scripts/items/bladed.src
M	scripts/items/fletch.src
M	scripts/misc/chrdeath.src
M	scripts/misc/death.src
M	scripts/misc/dressme.src
M	scripts/misc/logoff.src
M	scripts/misc/logon.src
M	scripts/misc/oncreate.src
M	scripts/misc/reconnect.src
M	scripts/modules/cliloc.em
A	scripts/textcmd/admin/nukeserial.src
A	scripts/textcmd/player/cache.src
M	scripts/textcmd/player/showclasse.src
M	scripts/textcmd/player/undressme.src
A	scripts/textcmd/test/createinbag.src
A	scripts/textcmd/test/recalcskillmods.src
A	scripts/textcmd/test/restartscript.src
```

## Appendix D: Loot and Class Progression Changes (focused)

### Class-specific changes

1. Thief chest progression in dungeons/spawnpoints
- Spawnpoint treasure chests now roll up through lootgroup `307` (level 7 chest tier), with `PROPID_CHEST_SPAWN_LEVEL` set from `lootgroup - 300`.
- Lock difficulty scaling now reaches top-end level-7 ranges (`140`-`150` on lootgroup `307`).
- Spawnpoint chest lockpicking path is thief-gated (`PickTreasureChest` rejects non-thief characters).

2. Bard treasure map progression
- Bards now get a class-based chance roll to upgrade map digs to level `7`.
- Level-7 bard roll uses dedicated guardian/chest handling (including level-7 lootgroup path and high-end magic scaling).

### Loot-table and loot-level changes

1. Treasure map lootgroups were remapped
- Map loot moved from old low-number groups (`5`-`10`) to dedicated map groups (`201`-`207`).
- This separated map progression from older shared group slots and enabled explicit level-7 map support.

2. Spawnpoint chest lootgroups were normalized
- Spawnpoint container creation now uses weighted groups `301`-`307`.
- Resulting chest level, lock difficulty, trap strength, and magic settings are derived from that weighted band.

3. Loot progression extended to level 11
- `starteqp.inc` loot application paths were expanded to include level `11` handling across item families.
- This includes the weapon/armor/shield/jewelry/clothing enchant distribution pipelines.

4. Shield logic split from general armor handling
- Shield roll application is now routed through dedicated shield functions (`ApplyShieldModNew` and shield-specific apply helpers) instead of sharing generic armor paths.
- This enables shield-specific proc/stat behavior independent from non-shield armor enchant routing.

### Files most directly involved
- `pkg/opt/chests/lockpicking.src`
- `pkg/opt/spawnpoint/checkpoint.src`
- `pkg/std/treasuremap/digtreasure.src`
- `config/nlootgroup.cfg`
- `scripts/include/starteqp.inc`

### RPer stone — New class support (Paladin, Mystic Archer, Bladesinger)

The RPer faction stone (`rperstone.src`) and its companion include (`rper.inc`) were updated to support three new playable classes introduced during this window. Previously the stone offered six choices (Ranger, Warrior, Mage, Thief, Bard, Crafter); it now offers nine.

**Gump changes (`rperstone.src`):**
- Three new `text` layout entries added (positioned at y=800, 825, 850) for the class name labels.
- Three new `radio` entries added (y=802, 827, 852) with response IDs 700 (Paladin), 800 (Mystic Archer), 900 (Bladesinger).
- Okay button moved from y=880 to y=955 to clear the expanded list.
- The program signature was cleaned up: unused `newstone` parameter removed from `OnUse_RPerFactionStone`.
- `gfdata` array extended with the three new class name strings.

**Starter bag functions added to `rper.inc`:**

| Class | Weapons | Armor | Notable supplies |
|-------|---------|-------|------------------|
| Paladin | `SKatana`, `SMace` (`dmg_mod 20`, `IsRPer`) | Full plate set (7 pieces, `0x1410`–`0x1B74`) | 200 bandages, 10 greater heal potions, 5 cure potions, 2000 gold |
| Mystic Archer | `SBow` (`dmg_mod 20`, `IsRPer`) | Leather set (7 pieces) | 500 arrows, fishing gear (pole + raw fish + 5 magic fish types), full reagent set ×60, 2000 gold |
| Bladesinger | `SDaisho` (`dmg_mod 20`, `IsRPer`) | Leather set (7 pieces) | 20 greater heal potions, lute, 4000 gold |

All three weapons are named via `SetNameByEnchant` after the `dmg_mod` is applied, matching the pattern used for existing RPer starter weapons.

**Files involved:**
- `pkg/opt/roleplaying/rper.inc` — Three new `createRPer*Bag()` functions (Paladin, Mystic Archer, Bladesinger)
- `pkg/opt/roleplaying/rperstone.src` — Gump layout extension, new case branches, program signature fix

### Ranger class — Boss pet house confiscation

Prior to this change, any boss or super-boss NPC (tamed or wild) that entered a player house was killed outright by the sign listener loop. This silently destroyed tamed boss pets that wandered in while following their ranger master.

The `ConfiscateBossPet()` function in `signcontrol.src` now distinguishes tamed from wild bosses by checking both the `master` object property and whether the NPC is running the `"tamed"` script:

- **Wild boss (no master / not on tamed script):** killed outright as before.
- **Tamed boss (has master, running tamed script):** pet is destroyed and a claim ticket (`0xDF0C`) is created for the owner.
  - Ticket delivery order: owner's backpack (if online and space available) → bank box → pet destroyed with sys message.
  - Ticket is owner-locked via `owner_serial` property; non-owners are rejected at the Animal Trainer.

Redemption flow in `animaltrainer.src` (`Load_Ticket_Data`):
- Player gives ticket to any Animal Trainer NPC.
- Trainer announces the fine amount: `CInt(GetMaxHP(mobile) / 20)` gold.
  - Boss pets: approximately 15,000–55,000 gold.
  - SuperBoss pets: approximately 50,000–125,000 gold.
- If the player can afford it (`who.spendgold(fine)`), the NPC is recreated from its original template at the trainer's location with the original name, color, and master relationship restored.
- Pet HP is reset to full (`SetHp(newpet, GetMaxHp(newpet))`).

The same commit also fixed an operator-precedence bug in the original boss detection condition:
- **Before:** `mobile.isa(POLCLASS_NPC) && GetObjProperty(mobile,"Boss") || GetObjProperty(mobile,"SuperBoss")` — `||` had lower precedence than `&&`, meaning a SuperBoss bypassed the `POLCLASS_NPC` guard.
- **After:** `mobile.isa(POLCLASS_NPC) and (GetObjProperty(mobile, "Boss") or GetObjProperty(mobile, "SuperBoss"))` — explicit grouping enforces correct evaluation.

**Files involved:**
- `pkg/std/housing/signcontrol.src` — `ConfiscateBossPet()` function, operator precedence fix, `include "util/bank"`
- `scripts/ai/animaltrainer.src` — new `0xDF0C` branch in `Load_Ticket_Data` (ownership check, fine payment, pet recreation)
- `config/itemdesc.cfg` — new `Item 0xDF0C` ("Confiscated Pet Ticket", graphic `0x14F0`, color `5184`)

## Appendix E: Talismans merge specifics (new items + categorization)

### Merge point used
- `3f014f6` (Merge pull request #220 from Andries1985/Talisman)
- Diff basis for this appendix: `3f014f6^1..3f014f6`

### New Talismans package item definitions
Source: `pkg/opt/talisman/config/itemdesc.cfg`

- Talismans: `0x213a` to `0x213e` (`talismanofid` variants)
- New gem/material family: `0x213f` to `0x2146`
	- `darksapphire`, `turquoise`, `perfectemerald`, `ecrucitrine`, `whitepearl`, `fireruby`, `bluediamond`, `brilliantamber`
- Additional item: `0x2147` (`glowingbrain`)

### How those new items were categorized

1. Tinkering item definitions
Source: `pkg/std/tinkering/tinker.cfg`

- Added explicit `TinkerItem` entries for:
	- `0x213e` (Talisman of Identification)
	- `0x213f` to `0x2146` (new gem set)

2. Talisman crafting path
Source: `pkg/opt/alchemyplus/alchemyplus.cfg`, `pkg/std/tinkering/tinkering.src`

Producing a Talisman of Identification is a two-step process requiring both Alchemy and Tinkering at high skill.

**Step 1 — Brew the Flask of Crystallized Intelligence (Alchemy, skill 150)**

Using an Alchemy Plus mortar, brew all of the following into an empty flask (`consume_flask 1`):

| Ingredient | Objtype | Qty |
|---|---|---|
| Wyrm Heart | `0x0F91` | 14 |
| Daemon Bone | `0x0F80` | 12 |
| Dragon Blood | `0x0F82` | 17 |
| Obsidian | `0x0F89` | 13 |
| Dark Sapphire | `0x213f` | 1 |
| Turquoise | `0x2140` | 1 |
| Perfect Emerald | `0x2141` | 1 |
| Ecru Citrine | `0x2142` | 1 |
| White Pearl | `0x2143` | 1 |
| Fire Ruby | `0x2144` | 1 |
| Blue Diamond | `0x2145` | 1 |
| Brilliant Amber | `0x2146` | 1 |
| Glowing Brain | `0x2147` | 1 |

The nine gem types (`0x213f`–`0x2146`) are the new Talisman gem set introduced in this merge and obtained via mining (see section 3 below). The Glowing Brain (`0x2147`) is a rare mob drop (vendor buy price 2,000 gp; no vendor sell price). On success the brewer receives a **Flask of Crystallized Intelligence** (`0xffa3`).

**Step 2 — Forge the Talisman (Tinkering, skill 130)**

Use the Flask of Crystallized Intelligence (`0xffa3`) in the Tinkering menu, or tinker the flask. The game calls `TryToMakeTalisman(character, use_on, 0x213e, 0xffa3, SFX_HAMMER)`.

The crafter is prompted to target an ingot stack from their backpack (or an Omega Cache):
- Minimum **125 ingots** required.
- The ingot type determines the talisman's color and material name prefix.
- Crafter class bonus applies to exceptional chance (`excep_ch` starts at 10, scaled by `ClasseBonus`).
- On success, a **Talisman of Identification** (`0x213e`) is placed in the backpack with:
	- `idcharges` set to `crafter_level × 1000` (max level 6 → 6,000 charges).
	- `CraftedBy` builder mark if the crafter has the toggle enabled.
	- Exceptional quality possible (Arms Lore quality multiplier applies).
- On failure, the flask is consumed with no talisman produced.

3. Mining drop-table categorization
Source: `pkg/std/mining/mining.src`

- Added `RandomInt(8)` gem rolls that produce the new talisman gem family (`0x213f`..`0x2146`) in mining reward cases.

4. Objtype constant grouping
Source: `scripts/include/objtype.inc`

- Added grouped constants:
	- `UOBJ_GEM2_START := 0x213f`
	- `UOBJ_DARK_SAPPHIRE` .. `UOBJ_BRILLIANT_AMBER`
	- `UOBJ_GEM2_END := 0x2146`

### Other same-merge item additions that were categorized

1. Dragon armor set
Definitions: `pkg/systems/combat/config/itemdesc.cfg`

- `dragonhelm`, `dragongloves`, `dragonlegs`, `dragonsleeves`, `dragonbreastplate`

Categorized into:
- Loot: `config/nlootgroup.cfg` -> `Group NormalArmor`
- Enchant tables: `pkg/systems/combat/config/enchantableitems.cfg` -> `Group 2` (Armors)

2. Order/Chaos shields
Categorized into:
- Loot: `config/nlootgroup.cfg` -> `Group NormalArmor`
- Enchant tables: `pkg/systems/combat/config/enchantableitems.cfg` -> `Group 3` (Shields)

3. New clothing set entries
Definitions: `pkg/std/tailoring/itemdesc.cfg`

- `malekimono`, `femalekimono`, `kamishimo`
- `checkeredshirt`, `jinbaori`, `obi`, `hakamashita`, `tattsukehakama`, `hakama`
- `elvenglasses`, `elvenrobe1`, `elvenrobe2`, `elvenmaleshirt`, `elvenmaleshirt2`, `elvenfemaleshirt`, `elvenfemaleshirt2`, `elvenpants`, `elvenboots`

Categorized into:
- Loot: `config/nlootgroup.cfg` -> `Group Clothes` (and selected entries in `Group Junk`)
- Enchant tables: `pkg/systems/combat/config/enchantableitems.cfg` -> `Group 4` (Clothing)

### Related itemdefs added in same merge window
- `config/itemdesc.cfg`: `elvenchest` (`0x2DF2`)
- `pkg/opt/alchemyplus/itemdesc.cfg`: `flaskofcrystallizedintelligence` (`0xffa3`)
- `pkg/opt/crafterboost/itemdesc.cfg`: `rechargeflask` (`0x8B05`)

## Appendix F: Banker changes (in-depth)

### Change window and commit sequence

Primary commits touching banker behavior in this report range:

1. `db2fdc0` - initial banking rewrite (deposit/withdraw/cheque command paths)
2. `1c010bb` - bankers only accept Banker's Orders when given items
3. `6aba1cb` - item-return parameter order fix in give-item rejection flow
4. `3f3bc98` - final item-given flow simplification and ReleaseItem cleanup
5. `d27a7c4` - confirmation gump before `deposit` command executes

### Core functional redesign (`db2fdc0`)

`scripts/ai/banker.src` moved from mixed legacy logic into explicit command handlers:

- `bank` -> opens bank box
- `withdraw` -> prompts amount and withdraws coin stacks
- `deposit` -> deposits all backpack gold (later gated by confirm gump)
- `check`/`cheque` -> creates a Banker's Order from banked gold
- `balance` -> totals only true gold coin objtype (`0x0EED`)
- `count` -> reports item count and aggregate weight in bank container

Key structural additions:

- `const MAX_BANK_TRANSACTION := 5000000`
- `CreateCoins(container, coin_type, amt)` helper:
	- Creates split stacks in 60,000 chunks.
	- Tracks created stacks and destroys partial output on failure for atomic behavior.
- `MakeCheque(you)` replacement for old `MakeCheck` path.

### New Banker's Order object (`config/itemdesc.cfg`)

`Item 0x14000` added as `bankersorder`:

- Description: `Banker's Order`
- Graphic: `0x14EF`
- Newbie item flag enabled
- Used as the canonical item-given deposit instrument for bankers

The banker now reads cheque amount from item properties (`Amount` with fallback to `checkamount`) and rejects malformed or out-of-range values.

### Validation and safety rules introduced

For cheque creation (`MakeCheque`):

- Minimum cheque value: 5,000 gold
- Maximum cheque value: 5,000,000 gold (shared transaction cap)
- Requires sufficient banked gold
- Uses `ConsumeSubstance(bankbox, 0x0EED, tamount)` before creating the Banker's Order
- If order creation fails (container limits), gold is restored via `CreateCoins`

For order deposit (`addtobank` when item is handed to banker):

- Rejects missing bank box (returns item)
- Rejects amount < 5,000 as invalid
- Rejects amount > 5,000,000
- Converts order amount into bank coin stacks via `CreateCoins`
- On overflow/failure, returns order to player backpack instead of deleting value

For withdraw:

- Amount prompt via text-entry gump
- Cap at 5,000,000 per withdraw
- Prevents overdraft
- If backpack placement fails, withdrawn gold is restored to bank (rollback)

For deposit-all:

- Enumerates only backpack `0x0EED` gold
- Moves all backpack gold into bank via `CreateCoins`
- If bank placement fails, restores gold to backpack

### Follow-up hardening across subsequent commits

`1c010bb` introduced strict item-given behavior:

- Banker accepts only `0x14000` orders.
- Any other item is rejected with `"I only accept banker's orders."` and returned.

`6aba1cb` corrected argument order bug in item return:

- `MoveItemToContainer(ev.source.backpack, ev.item)` fixed to `MoveItemToContainer(ev.item, ev.source.backpack)`.

`3f3bc98` simplified and stabilized give-item flow:

- Removed redundant reserve/release handling in this path.
- Consolidated rejection path to immediate return-to-backpack behavior.
- Removed unnecessary `ReleaseItem` calls from `addtobank` failure paths where no reservation remained.

`d27a7c4` added user confirmation for bulk deposit:

- New `ConfirmBankDepositAll()` gump using `:gumps:gumps`.
- Buttons: Confirm (`BTN_CONFIRM_BANK_DEPOSIT_ALL := 1`) and Cancel (`BTN_CANCEL_BANK_DEPOSIT_ALL := 2`).
- `DepositAllBackpackGold()` now aborts cleanly unless the player confirms.

### Net behavior after all banker commits

- Banker economy actions are now explicit, capped, and mostly transactional (with rollback on partial failures).
- Bankers no longer silently process arbitrary given items; they only process Banker's Orders.
- Deposit-all has a UI guardrail against accidental bulk transfer.
- Coin accounting consistently uses actual gold objtype (`0x0EED`), preventing prior false positives from lookalike graphics/colors.

## Appendix G: Changes since 3f3bc98 (2026-04-19 → 2026-05-02)

This appendix covers the ~50 commits that landed between the previous document edit and the current HEAD (`e9f4d44`).

---

### G.1 Banking follow-up (post Appendix-F window)

**`c7d8f22` — Banker speech queue fix**
- Z-level check made tolerant: speech is now accepted from players within ±8 Z units of the banker (was exact match), preventing silent failures in rooms with non-flat floors.
- Speech cooldown constant (`BANK_SPEECH_COOLDOWN := 5`) added for future use.

**`9599f1d` — Bankers removed from begging interaction**
- Banker NPC AI no longer participates in the begging speech event.

**`738bcb0` — Player Merchants can issue cheques**
- New `cheque` speech command added to `playermerchant.src`.
- Merchant issues a `0x14000` Banker's Order for up to 1,000,000 gold from their held gold (`g` property).
- Delivery: backpack first, then bank box (with `placed_in_bank` confirmation message).
- Remaining gold reported after issuance.
- Help text updated to include `cheque` in the command list.

**`ed3f499` — Cheque text entry numeric-only fix**
- Cheque amount text entry gump now validates numeric input only.

---

### G.2 Omega Cache v1.3 — AlchemyPlus integration, talisman cache fix (Merge PR #223)

**Commits: `04805b3`, `234a140` (merge), and supporting `32fb617`–`6ceef47` series**

**Talisman crafting (`pkg/std/tinkering/tinkering.src`)**
- Replaced `ingot.amount < 125` gate with `GetAvailableResource(...).total` check, enabling Omega Cache autodraw across split backpack+cache stacks.
- Crafting Power Hour (`PHC`/`#PPHC`) now halves the ingot requirement from 125 to 63, matching sibling crafting functions.
- Materials are now consumed **before** the skill check, closing the duplication window that previously existed.
- Dead secondary-component check (`needed_objtype`) and redundant `ReserveItem`/`ReleaseItem` calls removed.

**AlchemyPlus cache integration (`pkg/opt/alchemyplus/alchemyplus.src`)**
- Reagents now consumed via `BuildReagentRequests` → `GetAvailableResource`/`ConsumeResource` with autodraw support.
- Player can target the Omega Cache container as the primary reagent source.
- Flask/bottle container resolution ported to cache-aware `containerRequest`.
- Full lease lifecycle (acquire/extend/release) added to the main autoloop.
- "Run out of flasks/bottles" autoloop bug fixed: container is now consumed **after** the potion is created, not before.

**`ConsumeResource` hardening (`scripts/include/resourcemanager.inc`)**
- Partial-consume detection added with structured `SysLog` output (player serial, objtype, amount requested, unfulfilled remainder, per-source amounts, house serial).
- `houseSerial` field threaded through `ResourceRequest` struct so partial-consume logs identify which house's cache was involved.
- `GetBottle` signature changed from `(conts, user, dataFileHandle)` to `(conts, user, parentRequest)` for consistent context propagation.

**`bdbd978` — v1.3 patch notes committed** (`pkg/opt/omegacache/docs/patch-v1.3.md`)

---

### G.3 Tamed AI and pet count

**`ba2ac09` — Boss tamed pet count, tamed AI spell checks, poisoning skill tuning**
- Ranger class pet cap clarified: c5 = 2 regular pets + 1 mount; c6 = 2 boss pets + 2 mounts.
- Tamed AI now checks the NPC template for available spells before selecting a casting AI slot.
- Poisoning skill thresholds adjusted on several NPC templates to hit correct poison AI tiers:
  - `slime`: 40 → 25 (below threshold), `razz`: 40 → 50, `darklingslime`: 40 → 75, `beetle`: 90 → 100, `snake`: 45 → 50, `scorp`: 90 → 100.
- `razz` and `darklingslime` gained `tameskill` properties (40 and 75 respectively).
- New `scripts/textcmd/test/testpanel.src` uploaded (admin test panel, initial commit).

**`0b21453` — Tamed caster range**
- Tamed caster NPCs now maintain a target distance of 2–10 tiles (was uncontrolled).

**`330b72b` — Tamed summon/animated defense**
- Summoned and animated tamed NPCs now perform an automated defense action on their first loop iteration.

**`a57f8bf`, `ef1365b` — Tamed mass spell fixes**
- Tamed NPCs that are released no longer cast mass spells at players.
- Fix for tamed NPCs inadvertently casting area-effect spells on allied targets during release.

**`cd78bb2` — Shield skill advancement and tamed AI updates**
- Shield skill advancement (`parry`) was not incrementing correctly in the tamed AI update loop; fixed.
- Armorzone include errors corrected.

---

### G.4 Combat and buff rebalancing

**`f6d8da2` — Fire katana, buff normalization, taint cap, Mego duration**
- `sunshinespear` (fire katana, `0x9000`) now applies fire damage instead of default physical damage.
- **Taint (polymorph AR buff)** cap raised from 65 to 75 for top-tier strength.
- **Mego buff** changed from a random-roll duration to a flat 5-minute (`+300 seconds`) bonus: `duration := strength * 15 + 300` (was `strength * 15`); `mod_amount` changed from `RandomDiceStr(strength + "d2")` to `strength * 2` (deterministic).
- **PowerPlayer class bonus** raised to `3x` in both `earthblessing.src` (was `2x`) and `enlightenment.src` (was `2x`), aligning with Mystic Archer bonus.
- **Shapeshift** Mystic Archer bonus reduced from `4x` to `3x` (now consistent with PowerPlayer).
- Enlightenment now also applies an INT mod in addition to STR and DEX.

**`f588f97` — Bard song normalization and checkpoint loop fix**
- **Song of Defense**: mod amount changed from `RandomInt(5) + peace/10 + spec*3` to `spec * 9` (max 54 AR); duration doubled to 3600 seconds.
- **Song of Glory**: mod amount changed from formula to deterministic spec table (0/15/30/45/60/65/75); guard added to prevent overwriting existing `poly` buff.
- **Song of Haste**: mod amount changed to same spec table (0/15/30/45/60/65/75); now also applies STR and INT mods in addition to DEX; duration doubled to 3600 seconds.
- **Checkpoint infinite loop fix**: `CreateSpawnPointNpc` no longer force-sets the NPC's HP from `NPCVits` every spawn iteration for non-custom spawn points, preventing the stat-reset loop that caused infinite respawn cycles.

---

### G.5 Loot and NPC table tuning

**`e9f4d44` — Armor stat distribution rebalance (`scripts/include/starteqp.inc`)**
- `ApplyArmourModNew` stat-roll probabilities overhauled across all levels 2–11.
- HP mod and AR-skill mod chances were previously near-zero at high levels (e.g., level 4: 0.7% HP, 1.8% AR-skill); all levels now get 12–19% HP chance and ~27–30% AR-skill chance.
- AR mod (plain AR increase) reduced from 68–80% to ~40% across mid/high levels.
- On-hit script chance reduced from 18–30% to 11–16%.
- Levels 7–9 previously had no HP or AR-skill mod paths at all; all three now appear at every level.

**`2f1894d` — Nyx and Rikktor loot tier**
- `nyxseductress` and `rikktor` promoted to `Magicitemlevel 11` (from 10).

**`ed3f499` — Barracoon and Erebus loot tier**
- `barracoon` and `erebuschaosgod` promoted to `Magicitemlevel 10` (from 9).

**`f9a303e` — Treasure map magic item chance randomized**
- Magic item chance per treasure map level changed from fixed values (50/60/70/80/90/95/99%) to randomized ranges:
  - Levels 1–4: `Random(25) + 75` (75–100%)
  - Level 5: `Random(20) + 80` (80–100%)
  - Level 6: `Random(15) + 85` (85–100%)
  - Level 7: `Random(10) + 90` (90–100%)

**`287d1f2` — HP spawning adjustment**
- NPC HP at spawn was being set too high by the spawn system; corrected to prevent over-statted spawned mobs.

---

### G.6 Exceptional crafting cap

**`27bec51` — Exceptional difficulty capped at 150**
- `excep_diff` is now capped at a maximum of 150 in all crafting flows:
  - `make_blacksmith_items.src`, `tinkering.src`, `make_cloth_items.src`, `inscription.src`, `make_crafter_boosts.src`.
- Prevents high-skill crafters from exceeding the CheckSkill cap and getting artificially reduced exceptional rates at extreme skill levels.

---

### G.7 Totem overhaul

**`a2855ea` + `a672db6` — Totem crafting and activation restrictions**
- Obsidian golems can now only be **activated** by Crafter-class players (`CLASSEID_CRAFTER` check in `totem.src`).
- Totems are now **immune to spellbind** (`is_totem` property check added to `spellbind.src`).
- Magery skill requirement removed from `MakeTotem` in `tinkering.src`: only Tinkering (70) is now required.
- The old Magery+Tinkering dual skill check (`CheckSkill(SKILLID_MAGERY, 90) or CheckSkill(SKILLID_TINKERING, 90)`) replaced with `CheckSkill(SKILLID_TINKERING, 90)` only.

---

### G.8 Townsfolk AI consolidation

**`c442d6c`, `b662b52` — City-bounded townsfolk and shared AI includes**
- Townsfolk NPC scripts (minstrel, noble, performer, person, townperson) now call `InitTownBounds()` on startup and `WanderWithinTown()` on their main loop.
- If a townsfolk NPC leaves its assigned city boundaries, it is killed and will respawn in place.
- Townsfolk can only be spawned by spawn points inside registered city regions.
- Shared behavior extracted to `scripts/include/townsfolk.inc` (sayings arrays, `RunFromOpponent`, `SayRandomFromArray`, `SendBoostMessage`) and `scripts/include/anchors.inc` (wander/bound logic).
- Reduces ~550 lines of duplicated logic across 5 scripts.

---

### G.9 RPer equipment update

**`0937f1f` — Bladesinger RPer starter bag updated**
- Bladesinger starter bag in `rper.inc` was revised with updated equipment load-out.

---

### G.10 Miscellaneous fixes

| Commit | Change |
|--------|--------|
| `24c5cd5` | Blue tent (`0x601E`) and green tent (`0x601F`) deeds removed from MRC merchant product group |
| `185a08e` | Begging now blocked inside multis and from NPCs that are inside multis |
| `9599f1d` | Banker NPC excluded from begging interactions |
| `c35bae3` | Non-donator mount graphics no longer blockable by the mount-check |
| `b674d56` | Command-line scripts updated with a synopsis block |
| `7c04901`, `f5b7a86` | Staff/command-level cliloc string updates |
| `ed3f499` | Wyrm level 9 weapon (`Wyrm9Weapon`) color corrected: `1160` → `1764`; Wyrm level 10 weapon and armor color corrected: `2159` → `1568` |
