Good question — the zone roll and the damage-reduction math are actually two separate systems that don't talk to each other much. Total armor is what matters for how much damage gets through; the zone only decides which single piece takes the durability hit.

## The actual reduction formula

In [pkg/systems/combat/include/hitscriptinc.inc:107-189](pkg/systems/combat/include/hitscriptinc.inc#L107-L189) (`RecalcPhysicalDmg`):

```
var ar := defender.ar;          // <-- the mobile's aggregate AR stat, NOT the hit-zone item
...
absorbed := ArAbsorptionCalc(ar, basedamage);
rawdamage -= absorbed;
```

`armor` (the item `GetHitArmor` picked for that zone) is passed into this function too, but it's only used for `CheckEquipmentDamage(defender, armor)` (durability wear) and to check for an `OnHitScript` on that specific piece. **It never feeds into `absorbed`.** The number that reduces damage is `defender.ar` — a single number the core (and `CS_GetEffectiveArmor`, which mirrors it) computes across **every equipped piece**, not just whichever one the RNG says got struck.

The absorption curve itself, [hitscriptinc.inc:777-789](pkg/systems/combat/include/hitscriptinc.inc#L777-L789):
```
percent := (ar/5)^0.5 * 0.05     // capped at 0.99
absorbed := basedamage * percent
```

## Applying your numbers

`defender.ar` is built the same way `CS_GetEffectiveArmor` computes it ([armorZones.inc:121-152](pkg/items/armor/include/armorZones.inc#L121-L152)): for **every** equipped item, `item.ar * (sum of that item's covered zones' Chance)/100`, all added together. Critically, this is a straight sum — it does *not* pick the best piece per zone the way `GetHitArmor` does. So both your platemail legs (25) and the ringmail legs underneath (20) each contribute independently, since both cover "Legs/feet."

Using `armrzone.cfg` weights (Body 44, Arms 14, Head 14, Legs/feet 14, Neck 7, Hands 7):

| Piece | AR | Zone | Contribution |
|---|---|---|---|
| Breastplate | 60 | Body 44% | 26.4 |
| Arms | 25 | Arms 14% | 3.5 |
| Head | 25 | Head 14% | 3.5 |
| Neck | 25 | Neck 7% | 1.75 |
| Hands | 25 | Hands 7% | 1.75 |
| Platemail legs | 25 | Legs/feet 14% | 3.5 |
| Ringmail legs (under) | 20 | Legs/feet 14% | 2.8 |

Total (shield aside) ≈ **43.2 AR**

Plug into `ArAbsorptionCalc(43.2, 100)`:
`percent = sqrt(43.2/5) * 0.05 ≈ 0.147` → absorbs **~14.7 damage**, leaving ~85 before other modifiers (protection spell, mace stamina drain, etc.).

**This number is identical whether the RNG rolled Body or Legs/feet.** The only thing the zone roll changes is which item `CheckEquipmentDamage` degrades and whether that specific piece's `OnHitScript`/reactive-armor trigger fires ([mainhit.src:631-725](pkg/systems/combat/include/hitscriptinc.inc#L631)).

## The shield is the exception

Your 60-AR shield doesn't fold into `defender.ar` the normal way — shields are excluded from the zone-coverage sum (per the comment at [armorZones.inc:119](pkg/items/armor/include/armorZones.inc#L119)) and instead only matter through `GetShieldAbsorption()` ([hitscriptinc.inc:550-628](pkg/systems/combat/include/hitscriptinc.inc#L550-L628)): on a **successful parry roll** (skill-based), it flatly knocks off `shield.ar / 2` from basedamage — independent of, and in addition to, the AR-percent absorption above. If parry doesn't proc, the shield does nothing for that swing.

**Bottom line:** total AR (summed across every worn piece, weighted by its zone coverage) is what determines the damage reduction percentage — not whichever single piece the zone roll happened to land on. The zone roll only decides who takes durability wear and whose on-hit effects trigger.
