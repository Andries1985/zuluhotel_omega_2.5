# Patch Notes - v1.1.6
**Zuluhotel Omega 2.5 | Live Shard**  
**Date: November 7, 2026**

---

Welcome to **Patch 1.1.6**. This patch is about gathering. Lumberjacking, carving kindling, mining, digging clay and sand, fishing, fishing nets, filling pitchers, cleaning bandages and planting seeds now all check what is **really** on the ground where you aim. Until now they trusted whatever the game client said you clicked, so a macro could chop wood from bare grass or fish in the middle of a field. Every tile list behind these skills was also rebuilt one tile at a time, so a few spots that used to work no longer do, and some that never worked now do.

---

## What Changed

## Gathering - Only What Is Really There

### Player Impact

- **The game checks the real ground, not what your client reports.** When you target a tree, a rock, sand, a swamp or water, the server now looks at the map and the objects really at that spot. A spot that has nothing to gather says so: "You can't get any wood from that." or "You can't mine or dig anything there."
- **This covers every gathering skill.** Lumberjacking, carving kindling with a blade, mining, digging clay and sand, fishing with a pole or a net, filling jugs and pitchers, cleaning bandages and planting seeds.
- **Other lands work properly.** Mining, digging, fishing and seed planting outside Britannia read the map of the land you are standing in. Before, some of them read the Britannia map at the same coordinates.
- **Clicking the ground at the foot of a tree still works.** You don't have to hit the trunk exactly: anything gatherable in the square you click counts.

---

## Lumberjacking and Kindling

### Player Impact

- **More trees give wood:** coconut and date palms, yew trees, some leafy tree tops and the Tuscany pine.
- **Plants that aren't trees no longer give wood.** Grasses, ferns, rushes, weeds, cattails, pampas grass, snake plants, ponytail palms and flowers were on the old list. The juniper bush still gives wood.
- **Yucca still gives no wood.**
- Carving kindling with a blade uses the same tree list.

---

## Mining, Clay and Sand

### Player Impact

- **More cave floors give ore.** All the dungeon cave floor tiles now count, along with a few mountain and dirt tiles that were missing.
- **Dungeon floors give ore, not clay.** On some dungeon floors your pickaxe dug clay instead of ore. Those spots now give ore.
- **The forest tile at the Occlo bank no longer gives ore.**
- **Clay comes only from swamp.** Some grass and cave tiles used to give clay. They no longer do.
- **The sand list matches real sand.** A few beach tiles that are sand were missing and now work; a few that aren't sand no longer do.
- When a spot has more than one resource, ore is checked first, then clay, then sand.

---

## Fishing

### Player Impact

- **You can only fish in real water.** Fishing with a pole or a net checks for real water at the spot.
- **The thin strip of water along the shoreline no longer counts as fishable.** You can still cast up to 6 tiles out from the shore into open water.
- **Waterfalls, fountains, troughs, tubs, barrels and buckets are not fishing spots.**
- **High-skill fishers no longer reel in nothing.** With more than 129 Fishing, about 1 cast in 7 at 150 skill landed no fish at all. Those casts now give a magic fish, the same as rolls of 100-129.
- **A fished-out spot says "There's not enough fish here."** Before, a spot could also refuse you now and then even when it still had fish.

---

## Water for Pitchers and Bandages

### Player Impact

- **More kinds of water work.** Waterfalls, fountains, troughs, tubs, barrels and buckets that are part of the map now fill pitchers and clean bandages, not only ones placed as items.
- **Water inside a container doesn't count.** A trough or barrel has to be out in the world.

---

## Farming

### Player Impact

- **Seeds only plant in real fertile dirt or farmland.** Fertile dirt placed as an item, or part of the map, still works.
- **Cancelling the seed target now simply cancels.**

---

## Summary

- Every gathering skill (lumberjacking, kindling, mining, clay, sand, fishing, nets, pitchers, bandages, seeds) checks what is really at the spot instead of trusting the client, and reads the right map outside Britannia.
- Wood: palms, yews, some leaves and the Tuscany pine added; grasses, ferns, flowers and other plants removed; yucca stays out.
- Mining: more cave floors give ore; dungeon floors give ore instead of clay; the Occlo bank forest tile gives nothing; clay only from swamp; sand list corrected.
- Fishing: only real fishable water; shoreline strips, waterfalls, fountains and troughs are out; casts above 129 skill no longer come up empty.
- Pitchers and bandages accept waterfalls, fountains, troughs, tubs, barrels and buckets on the map.
- Seeds plant only in real fertile dirt or farmland.

Thanks for playing Zuluhotel Omega 2.5.
