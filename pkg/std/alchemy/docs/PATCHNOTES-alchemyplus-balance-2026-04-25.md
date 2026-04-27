# Alchemy and AlchemyPlus - Balance Patch Notes (2026-04-25)

## Summary

This patch set converts key buff potions from random-roll outcomes to deterministic tier outcomes, aligns mage progression breakpoints to explicit level gates, and standardizes cooldown handling for stat-buff potions.

The work covers DEX/STR/INT potions, Taint transmutations, and Homeric/Greater Homeric bless lines.

Primary goals delivered:

- Deterministic stat gains and durations for DEX/STR/INT.
- Explicit mage-level unlock progression for brewed variants.
- Deterministic Homeric bless tiers.
- Deterministic Taint poly tiers with updated stat curve.
- Verified AR conversion for poly path.

---

## Notable Points

### Common Formula and Gating Notes

- Brewing class bonus uses mage level bonus scaling already present in class logic.
- Brewing bonus level fed into variant selection uses integer truncation and a minimum bump to 1 for mages.
- Consumption formulas for DEX/STR/INT are now deterministic:
  - rank = tier rank from base strength + ByTrueMage mapping
  - stat gain = rank * 10 + 5
  - duration = (rank + 1) * 480 seconds
- Stat potion cooldown override added at use time:
  - potion lockout set to 2 seconds

### Poly and AR Conversion

- Poly AR is derived in temp-mod processing:
  - AR gain = floor(poly mod amount / 2)
- Taint now uses deterministic poly mods by effective strength tier (1..5):
  - 10, 25, 40, 55, 65

### Homeric and Greater Homeric

- Bless effect for itemtype 31/32 moved to deterministic tiering.
- Homeric tiers are now fixed by ByTrueMage-derived tier instead of random dice.

---

## Potion Tables

Notes for all tables below:

- Mage level column is non-mage, then mage 1..6.
- Values shown are final consume outcomes after brewed variant mapping.
- Duration values are shown in minutes.

### DEX Potions

| Potion | Mage Level | Gain | Duration | Notes |
|---|---:|---:|---:|---|
| Lesser Agility | Non, M1, M2, M3, M4, M5, M6 | +5 | 8 | No effective scaling tier beyond base |
| Agility | Non | +15 | 16 | Base |
| Agility | M1 | +25 | 24 | Lv1 variant |
| Agility | M2 | +35 | 32 | Lv2 variant |
| Agility | M3 | +35 | 32 | Capped |
| Agility | M4 | +35 | 32 | Capped |
| Agility | M5 | +35 | 32 | Capped |
| Agility | M6 | +35 | 32 | Capped |
| Greater Agility | Non | +45 | 40 | Base |
| Greater Agility | M1 | +45 | 40 | Base retained |
| Greater Agility | M2 | +55 | 48 | Lv1 variant |
| Greater Agility | M3 | +55 | 48 | Lv1 retained |
| Greater Agility | M4 | +65 | 56 | Lv2 variant |
| Greater Agility | M5 | +65 | 56 | Lv2 retained |
| Greater Agility | M6 | +75 | 64 | Lv3 variant max |

### STR Potions

| Potion | Mage Level | Gain | Duration | Notes |
|---|---:|---:|---:|---|
| Lesser Strength | Non, M1, M2, M3, M4, M5, M6 | +5 | 8 | No effective scaling tier beyond base |
| Strength | Non | +15 | 16 | Base |
| Strength | M1 | +25 | 24 | Lv1 variant |
| Strength | M2 | +35 | 32 | Lv2 variant |
| Strength | M3 | +35 | 32 | Capped |
| Strength | M4 | +35 | 32 | Capped |
| Strength | M5 | +35 | 32 | Capped |
| Strength | M6 | +35 | 32 | Capped |
| Greater Strength | Non | +45 | 40 | Base |
| Greater Strength | M1 | +45 | 40 | Base retained |
| Greater Strength | M2 | +55 | 48 | Lv1 variant |
| Greater Strength | M3 | +55 | 48 | Lv1 retained |
| Greater Strength | M4 | +65 | 56 | Lv2 variant |
| Greater Strength | M5 | +65 | 56 | Lv2 retained |
| Greater Strength | M6 | +75 | 64 | Lv3 variant max |

### INT Potions

| Potion | Mage Level | Gain | Duration | Notes |
|---|---:|---:|---:|---|
| Phandel's Fine Intellect | Non, M1, M2, M3, M4, M5, M6 | +5 | 8 | Base-only mapping |
| Phandel's Fabulous Intellect | Non | +15 | 16 | Base |
| Phandel's Fabulous Intellect | M1 | +25 | 24 | Lv1 variant |
| Phandel's Fabulous Intellect | M2 | +35 | 32 | Lv2 variant |
| Phandel's Fabulous Intellect | M3 | +35 | 32 | Capped |
| Phandel's Fabulous Intellect | M4 | +35 | 32 | Capped |
| Phandel's Fabulous Intellect | M5 | +35 | 32 | Capped |
| Phandel's Fabulous Intellect | M6 | +35 | 32 | Capped |
| Phandel's Fantastic Intellect | Non | +45 | 40 | Base |
| Phandel's Fantastic Intellect | M1 | +45 | 40 | Base retained |
| Phandel's Fantastic Intellect | M2 | +55 | 48 | Lv1 variant |
| Phandel's Fantastic Intellect | M3 | +55 | 48 | Lv1 retained |
| Phandel's Fantastic Intellect | M4 | +65 | 56 | Lv2 variant |
| Phandel's Fantastic Intellect | M5 | +65 | 56 | Lv2 retained |
| Phandel's Fantastic Intellect | M6 | +75 | 64 | Lv3 variant max |

### Taint Transmutations (Poly + AR)

Formula notes:

- Effective strength tier determines deterministic poly mod.
- AR gain is floor(poly / 2).

Tier map:

| Effective Strength Tier | Poly Mod | AR |
|---:|---:|---:|
| 1 | +10 | +5 |
| 2 | +25 | +12 |
| 3 | +40 | +20 |
| 4 | +55 | +27 |
| 5 | +65 | +32 |

Taint Minor (itemtype 34):

| Mage Level | Poly Mod | AR | Duration |
|---:|---:|---:|---:|
| Non | +10 | +5 | 12 |
| M1 | +25 | +12 | 24 |
| M2 | +25 | +12 | 24 |
| M3 | +25 | +12 | 24 |
| M4 | +25 | +12 | 24 |
| M5 | +25 | +12 | 24 |
| M6 | +25 | +12 | 24 |

Taint Major (itemtype 35):

| Mage Level | Poly Mod | AR | Duration |
|---:|---:|---:|---:|
| Non | +40 | +20 | 36 |
| M1 | +40 | +20 | 36 |
| M2 | +55 | +27 | 48 |
| M3 | +55 | +27 | 48 |
| M4 | +65 | +32 | 60 |
| M5 | +65 | +32 | 60 |
| M6 | +65 | +32 | 60 |

### Homeric Might (Bless)

Homeric (itemtype 31):

| Mage Level | Bless Mod | Duration | Tier |
|---:|---:|---:|---:|
| Non | +15 | 12 | 1 |
| M1 | +30 | 24 | 2 |
| M2 | +45 | 36 | 3 |
| M3 | +45 | 36 | 3 |
| M4 | +45 | 36 | 3 |
| M5 | +45 | 36 | 3 |
| M6 | +45 | 36 | 3 |

Greater Homeric (itemtype 32):

| Mage Level | Bless Mod | Duration | Tier |
|---:|---:|---:|---:|
| Non | +45 | 36 | 3 |
| M1 | +45 | 36 | 3 |
| M2 | +45 | 36 | 3 |
| M3 | +60 | 48 | 4 |
| M4 | +60 | 48 | 4 |
| M5 | +60 | 48 | 4 |
| M6 | +75 | 60 | 5 |

---

## Files Modified

### Core potion behavior

- pkg/std/alchemy/bluepotion.src
- pkg/std/alchemy/whitepotion.src
- pkg/opt/alchemyplus/newpotions.src

### Brew mapping and progression gates

- pkg/std/alchemy/alchemy.src
- pkg/opt/alchemyplus/alchemyplus.src

### Potion data and strength values

- pkg/std/alchemy/itemdesc.cfg
- pkg/opt/alchemyplus/itemdesc.cfg
- pkg/opt/alchemyplus/alchemyplus.cfg

---

## Buff Stacking Enforcement

### Global TempModConflicts System (dotempmods.inc)

A global conflict-resolution function `TempModConflicts(existing_key, incoming_key)` was added to `scripts/include/dotempmods.inc`. It is called by `AddToStatMods` before applying any new temp-mod. If a conflict is detected the mod is rejected and the potion or spell is not consumed.

**Category groupings enforced:**

| Category | Keys |
|---|---|
| stat | `str`, `cstr`, `dex`, `cdex`, `int`, `cint` |
| bless/poly | `all`, `call`, `ebless`, `cebless`, `poly`, `cpoly` |
| ar | `ar`, `car` |
| paralyze | `p` |

**Allow/Block matrix:**

| Combination | Result |
|---|---|
| stat + bless/poly | ALLOW |
| stat + ar | ALLOW |
| bless/poly + ar | ALLOW |
| stat + stat (different) | BLOCK |
| bless/poly + bless/poly (different) | BLOCK |
| poly + ar | BLOCK (poly already contributes AR) |
| ar + ar | BLOCK |
| paralyze + paralyze | BLOCK |

Any combination not explicitly listed as BLOCK is allowed. The check is category-level, not name-level, so e.g. `poly` blocks a subsequent `ar` application regardless of which source (spell or potion) produced each.

### Protection Spell AR Gating

`pkg/std/spells/protection.src`, `pkg/std/spells/protection with timer.src`, and `pkg/std/spells/archprot.src` all received explicit pre-application checks:

- Before applying the `ar` mod, the caster's (or each target's) existing `#mods` are inspected.
- If any active mod key is in the armor-affecting set (`ar`, `car`, `poly`, `cpoly`), the spell is blocked.
- Blocked casts send a message to the player and do not proceed. For Arch Protection the caster receives the message; targets who already have an AR mod are silently skipped.

### Mego Potion AR Gating (DoProtectionEffect)

`DoProtectionEffect` in `pkg/opt/alchemyplus/newpotions.src` received the same armor-affecting pre-check. If `ar`, `car`, `poly`, or `cpoly` is already active on the consumer, the potion is blocked and not consumed.

---

## Debug Command: .showbuffcats

A new staff-level debug command was added at `scripts/textcmd/test/showbuffcats.src`.

**Usage:** `.showbuffcats` — target any mobile.

**Output:**

- One line per active `#mod` entry showing: key / category / amount / seconds remaining.
- A summary line listing which buff categories are currently occupied.

**Categories reported:**

| Category label | Keys included |
|---|---|
| stat | `str`, `cstr`, `dex`, `cdex`, `int`, `cint` |
| bless/poly | `all`, `call`, `ebless`, `cebless`, `poly`, `cpoly` |
| ar | `ar`, `car` |
| paralyze | `p` |
| other | anything else |

This command is intended for verifying stacking enforcement in-game without needing to read raw object data.

---

## Files Modified (Full List)

### Core potion behavior

- pkg/std/alchemy/bluepotion.src
- pkg/std/alchemy/whitepotion.src
- pkg/opt/alchemyplus/newpotions.src

### Brew mapping and progression gates

- pkg/std/alchemy/alchemy.src
- pkg/opt/alchemyplus/alchemyplus.src

### Potion data and strength values

- pkg/std/alchemy/itemdesc.cfg
- pkg/opt/alchemyplus/itemdesc.cfg
- pkg/opt/alchemyplus/alchemyplus.cfg

### Buff stacking enforcement

- scripts/include/dotempmods.inc
- pkg/std/spells/protection.src
- pkg/std/spells/protection with timer.src
- pkg/std/spells/archprot.src

### Debug tooling

- scripts/textcmd/test/showbuffcats.src (new file)

---

## Verification Notes

- Poly AR path validated as floor(poly mod / 2) in temp-mod processing.
- Taint curve verified after latest adjustment to 10..65 tiers.
- Deterministic formulas and variant mappings checked against current working tree script data.
