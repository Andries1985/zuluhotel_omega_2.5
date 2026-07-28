Design notes for a random/procedural NPC generator, tabled mid-design. **Nothing has been implemented — no cfg files or scripts exist yet.** This is a snapshot of the plan so it can be picked up later.

## The ask

Generate NPCs procedurally instead of from fixed `NpcTemplate` entries: random graphic, random name (prefix + base + suffix, e.g. "The Great Alryc of Renown" / "Necromancer Alryc, The Foul One"), a rolled loot level that drives stats, and per-category equipment (e.g. a "Swordfighter" category rolls a random sword+shield combo each spawn).

## Verdict: no new spawn engine needed

The existing spawnpoint system ([pkg/opt/spawnpoint/](pkg/opt/spawnpoint/)) already has all the plumbing — wander range, expiry, kill-on-destroy, group spawning. The generator is an additive layer that plugs into `CreateSpawnPointNpc()` in [checkpoint.src:440](pkg/opt/spawnpoint/checkpoint.src#L440), which already branches on `if (GetObjProperty(point,"CustomPoint")) ... else CreateNpcFromTemplate(...)`. Plan: add a third branch, `elseif (GetObjProperty(point,"RandomCategory")) critter := GenerateRandomNpc(...)`, so despawn/expiry/wander-range/gump all keep working untouched.

## What already exists and gets reused

- **`CreateNpcFromTemplate(template, x, y, z, override_properties)`** ([uo.em:282](scripts/modules/uo.em#L282)) — the `override_properties` struct stamps `.Name`, `.Graphic`, `.STR/.INT/.DEX`, `.color`, `.script` etc. directly onto the new mobile on top of the base template. Proven in use at [checkpoint.src:747](pkg/opt/spawnpoint/checkpoint.src#L747) (`CreateCustomNPC`). This is the core mechanism the generator hangs off of.
- **`title_prefix`/`title_suffix`** are plain writable mobile properties (`critter.title_prefix := "..."`), used all over ([virtue.inc](scripts/include/virtue.inc), [townguard.src](scripts/ai/townguard.src)). No engine work needed for the "Necromancer Alryc, The Foul One" style naming — just string assembly + direct assignment.
  - Side note (unrelated bug, not something we're fixing here): the existing Custom NPC clone feature captures `titleprefix`/`titlesuffix` into its saved struct ([customnpc.inc:107-108](pkg/opt/spawnpoint/include/customnpc.inc#L107-L108)) but never re-applies them on respawn.
- **`config/names.cfg` + [randname.inc](scripts/include/randname.inc)** — existing random-name-by-body-graphic pool system (`Names <idx> { Count N, Name1..NameN }`). No prefix/suffix pools yet; same row-based pattern to extend.
- **Loot is driven by a `CustomLoot` obj-property, not the base template — this was the key finding.** [death.src:148-191](scripts/misc/death.src#L148-L191) → `MakeLoot(corpse)` in [starteqp.inc:68](scripts/include/starteqp.inc#L68) checks `GetObjProperty(corpse, "CustomLoot")` (`{magic_chance, magic_level, lootgroup}`) **first**, before falling back to the base template's own npcdesc.cfg entry. So the generator can fully decouple "what it looks like/fights like" (base template) from "what it drops" (rolled loot level → CustomLoot).
  - Must **NOT** set `noloot`, `guardkill`, or `summoned` on the generated NPC — any of those zero out loot entirely at death ([death.src:155-158](scripts/misc/death.src#L155-L158), [175-184](scripts/misc/death.src#L175-L184)).
  - Must **NOT** have a `master` property (marks it as a tamed/owned pet).
- **`config/nlootgroup.cfg`** — existing loot-table-by-group-number system, consumed by `MakeLoot`. Reused as the target of our per-level loot mapping, not reinvented.
- **`pkg/opt/spawnpoint/config/groups.cfg`** — existing "pick one random template from a named category pool" system (`group N { spawn xxx }`), used by `CreateSpawnPointNpcFromGroup`. Same shape of idea as our category system, but it only picks a whole fixed template — doesn't randomize graphic/stats/gear within the pick.
- **`config/equip.cfg` + `EquipFromTemplate()`** — static, fixed outfit per template, **no randomness**. Not reusable as-is; the category equipment pools need their own new loader.
- **AR (armor rating)** — not a field we set. It's computed from whatever armor items end up equipped (`item.ar`), so the `armor` pool in the category cfg is suficient; no separate stat needed.
- **Combat capability caveat**: for standard `killpcs`-style AI, actual melee damage comes from the equipped weapon + weapon skill, not template fields like `AttackDamage` (those only apply to special caster/shrine AI scripts). So the equipment pool is load-bearing for combat, not just cosmetic.

## New pieces that need building

1. **`randomnpcgroups.cfg`** (new file, not yet created) — one `RandomNpcGroup <category>` block per category. Draft schema (see full example below):
   - `script` — AI script name
   - `hostile` / `alignment` / `virtue` — flat, since we're overriding everything else too; open question below on whether to keep these here vs. inherit from a real base "shell" template
   - `graphic` — repeatable, curated per category (NOT a blind scan of npcdesc.cfg's used graphics — that would pull in shrines/boats/townsfolk)
   - `weapon` / `shield` / `armor` — repeatable, one slot-pool each; roll one entry per pool, resolve name → objtype via `GetObjtypeByName` (same as [loot.inc](scripts/util/loot.inc) does)
   - `skill <name> <min> <max>` — repeatable, flat range (not level-scaled, per current plan — could change, see open question)
   - `statrange <level> <STAT> <min> <max>` — repeatable, one row per (level, stat) pair, deliberately flat/verbose to match this codebase's cfg style (npcdesc.cfg never nests either)
   - `lootlevel <level> <magicchance> <magiclevel> <lootgroup>` — repeatable, feeds the `CustomLoot` obj-property directly

2. **Name prefix/suffix pools** — separate from the category cfg. Extends the `names.cfg` row format. Open question: should naming themes be tied to category (Swordfighter → knightly "Sir"/"Dame" prefixes) or stay universal/graphic-keyed like the existing system?

3. **The generator function itself**, `GenerateRandomNpc(category, lootlevel, point)`:
   - resolve category → cfg block
   - roll graphic, weapon, shield, armor from their pools
   - roll STR/DEX/INT/HITS from `statrange` rows for the given level
   - roll each skill within its min/max
   - build the `override_properties` struct → `CreateNpcFromTemplate(shell, x, y, z, overrides)`
   - set `.title_prefix`/`.title_suffix` from rolled name parts
   - equip the rolled weapon/shield/armor
   - `SetObjProperty(critter, "CustomLoot", {magicchance, magiclevel, lootgroup})` from the `lootlevel` row

4. **Spawnpoint integration** — the one-branch addition to `CreateSpawnPointNpc()` described above, storing `RandomCategory` + `RandomLootLevel` on the spawnpoint instead of a snapshot.

## Example category block (draft, not final, not yet written to disk)

```
RandomNpcGroup swordfighter
{
	script			killpcs

	hostile			1
	alignment		evil
	virtue			0

	graphic			0x190		// human male
	graphic			0x191		// human female
	graphic			0x11		// orc
	graphic			0x2a		// ratman

	weapon			Longsword
	weapon			Broadsword
	weapon			Katana
	weapon			Scimitar

	shield			BronzeShield
	shield			HeaterShield
	shield			Buckler

	armor			RingmailTunic
	armor			ChainmailTunic
	armor			PlateChest

	skill			Swordsmanship	60	90
	skill			Tactics			50	80
	skill			Anatomy			40	70
	skill			Parrying		30	60
	skill			Healing			20	50

	statrange		1	STR	40	60
	statrange		1	DEX	30	50
	statrange		1	INT	10	20
	statrange		1	HITS	40	60
	statrange		2	STR	50	70
	statrange		2	DEX	35	55
	statrange		2	INT	10	25
	statrange		2	HITS	60	90

	lootlevel		1	20	1	3
	lootlevel		2	30	2	5
}
```

## Open questions (unresolved when work paused)

- Should `lootgroup` in the `lootlevel` rows reference existing pools in `nlootgroup.cfg`, or get swordfighter-specific lootgroups authored too?
- Keep `hostile`/`alignment`/`virtue` as flat fields on the category block (current draft), or drop them and inherit from a real existing `NpcTemplate` used as a "shell"?
- Loot level scale: proposed 1–11 to match the existing `MagicItemLevel` range used throughout `starteqp.inc` — confirm or change.
- Should `skill` min/max also scale by level (like `statrange` does), or stay flat per category as currently drafted?
- Name themes: per-category or universal?

## Suggested build order once resumed

1. Data first: write `randomnpcgroups.cfg` for one category (swordfighter) fully, plus name pools.
2. Standalone generator function + a GM test command (no spawnpoint integration yet), so appearance/stats/gear can be iterated on live.
3. Wire into spawnpoint via the one-branch addition to `CreateSpawnPointNpc()`.
4. Verify despawn/kill-on-destroy/expiry/group-spawn all still behave correctly with generated NPCs.
