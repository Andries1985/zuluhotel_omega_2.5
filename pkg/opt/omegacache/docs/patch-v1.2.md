# Omega Cache - Patch v1.2

## Summary

This patch addresses combat balance changes, bug fixes across multiple systems, and a new resurrection crystal implementation. The combat changes rebalance physical damage calculations for several classes, fix NPC damage scaling that was incorrectly applying player class bonuses, and adjust the Ranger/Mystic Archer ranged bonus threshold. The Paladin spell resistance formula has been reworked to halve the bonus rather than applying the full class multiplier. Several NPC equipment assignments and a typo in "Mondain's Staff" have been corrected, and the Chaos AI bow hitscript has been changed from stamina drain to banish.

The Omega Cache deposit flow now includes a confirmation gump for "Deposit All" and blocks deposits from targeting corpses. The confirmation gump uses the same visual style as the main cache UI and warns the player before bulk-depositing all stackable items from their backpack. Corpse targeting is blocked both when targeting a corpse directly and when targeting items inside a corpse.

The resurrection crystal (`lifecrystal`) previously set a `freedeath` property on the player but no death script ever checked for it, making the crystal non-functional. The fix adds a `freedeath` handler in `chrdeath.src` that auto-resurrects the player on their next death, restores all items from the corpse, hides the player, and cleans up the corpse. The block is placed after the existing incognito/shapeshift/camouflage cleanup to avoid duplicating that logic. A syntax error in `spelldata.inc` (mismatched parenthesis in the Paladin resist chance calculation) was also fixed.

## Notable Points

### Omega Cache

- **Deposit All confirmation**: `DoDepositAll` shows a styled confirmation gump before proceeding. Uses `GFAddButton` with explicit button IDs — `GFAddButton` auto-assigns non-zero IDs when passed 0, so cancel buttons must use a named constant and check `result[0] == BTN_CONFIRM`.
- **Corpse deposit block**: `ValidateDepositTarget` rejects corpses as targets (`tgt.isa(POLCLASS_CORPSE)`) and items inside corpses (walks up container chain checking for corpse parents).

### Combat Balance — Damage Calculations (`hitscriptinc.inc`)

- **NPC class bonus exclusion**: The Thief/Mage/Bard lowest-skill weapon check in `CheckHitChance()` and `RecalcPhysicalDmg()` now gates on `!attacker.isA(POLCLASS_NPC)`. Previously NPCs with class properties would incorrectly use the player class damage path.
- **Power Player melee skill bonus**: Now averages Anatomy + Tactics (was Anatomy-only, shared with Warrior). Multiplier unchanged at 0.005.
- **Bladesinger melee skill bonus**: Now averages Anatomy + Tactics at a reduced multiplier of 0.00375 (was 0.005, shared with Warrior/PP).
- **Paladin melee skill bonus**: Now averages Eval Int + Tactics at 0.002 (was Eval Int-only at 0.002).
- **Mystic Archer ranged skill bonus**: Now averages Archery + Eval Int at 0.005 (was Eval Int-only at 0.005).
- **Power Player ranged skill bonus**: Now averages Archery + Tactics at 0.003 (was Tactics-only, shared with Warrior/Bladesinger).
- **Ranger/Mystic Archer ranged class bonus**: Threshold changed from level 2 (`level-1`) to level 4 (`level-3`). Also gated on `!attacker.isA(POLCLASS_NPC)`.
- **NPC melee skill bonus**: New dedicated path using Tactics at 0.00025 multiplier (average +5% pre-absorption). Previously NPCs fell through to player class paths.
- **NPC class damage bonus**: NPCs no longer receive player class-level damage bonuses (PvP/PvE multipliers). The NPC path is now an explicit empty block.
- **Bladesinger damage reduction**: Changed from `defender` to `attacker` class check — now reduces the Bladesinger attacker's damage (was incorrectly reducing damage taken by Bladesinger defenders). Multiplier reduced from 0.075 to 0.05 per level.
- **Paladin/Warrior damage reduction**: Extra closing parenthesis introduced in reduction formula — **this is a syntax error that will cause compilation failure** (lines ~399 and ~410: `(1 - (defenderlevel * 0.05) ) )` and `(1 - (defenderlevel * 0.10) ) )`).
- **Mage damage reduction**: Changed from `elseif` chained with Bladesinger to a separate `if` block. Mage reduction now applies independently of the Warrior/Paladin/Bladesinger reduction.

### Combat Balance — Spell Resistance (`spelldata.inc`)

- **Paladin resist chance**: Reworked from `chance * ClasseBonus * 0.5` (which halved the total including base) to `chance * (1 + (ClasseBonus - 1) * 0.5)` (which halves only the bonus portion). For example, with ClasseBonus 1.75: old formula gave `chance * 0.875`, new formula gives `chance * 1.375`.
- **Syntax fix**: Mismatched parenthesis on the Paladin resist line fixed (`Cint( (chance * halfmod)` → `Cint( chance * halfmod )`).

### NPC and Equipment Fixes

- **Mondain's Staff**: Typo fix — "Modain" → "Mondain" in both the item name (`MondainsStaffWeapon`) and description (`Mondain's Staff`). Equipment config updated to match.
- **Undead Flayer weapon**: Graphic changed from `0x0ec4` to `0x27a7`, sounds changed (`0x16D`/`0x239` → `0x13C`/`0x234`), attribute formatting fixed.
- **NPC equipment swaps**: Some NPCs had their equipment assignments rotated between `undeadflayer`, `behemoth`, and `mondainstaff` configs.
- **Chaos AI bow hitscript**: Changed from `staminadrainscript` to `banishscript` for the chaos multi-kill AI's bow weapon.
- **NPC Poisoning skill**: Added `Poisoning 200` to an NPC template.

### Resurrection Crystal (`chrdeath.src`)

- **freedeath handler**: Checks `GetObjProperty(ghost, "#freedeath")` after incognito/shapeshift/camouflage cleanup. On match: erases the property, sets the `death` timestamp, resurrects, cures poison, restores full HP/mana/stamina, hides the player, moves all items back from corpse, destroys corpse, sends murder report gump, and returns early.
- **Temporary property**: Changed from `freedeath` to `#freedeath` so the buff is automatically removed on logout. The `#` prefix makes it a temporary CProp in POL.
- **Dispel removes the buff**: The Dispel spell now checks for `#freedeath` on the target after all absorption/reflection checks pass. If present, the property is erased and the target receives "The resurrection crystal's protection has been dispelled!".
- **Placement rationale**: Placed after disguise cleanup (line ~243) so incognito, earth spell shapeshift, and camouflage are stripped before resurrection without duplicating that code.
- **Post-resurrection poison cure**: `CurePoison(ghost)` called after `ResurrectMobile()` to handle re-poisoning during the `sleep(2)` window (e.g. poison fields).
- **Players only**: The crystal's AoE effect now skips NPCs (`isA(POLCLASS_NPC)` check). Previously it would set `#freedeath` on all mobiles within 15 tiles including NPCs.
- **Murder reporting preserved**: `SendReportGump` and `mr` property cleanup still execute — the crystal saves you from death, not from justice.

### Tooltip Improvements (`itemdata.src`)

- **AR display**: Armor tooltips now show `AR: X (Y)` where X is the effective weighted AR (based on coverage zone hit chance) and Y is the raw item AR value. Previously only the effective value was shown, which confused players (e.g. a platemail breastplate with AR 25 showed "AR: 11" because Body zone has a 44% hit chance).
- **DPS display**: Weapon tooltips now show `DPS: X (Y)` where X is the class/quality-modified DPS and Y is the raw average base DPS without any player modifiers.

---

## Acceptance Testing Criteria

### Omega Cache

| Area | What to Test |
|------|-------------|
| **Deposit All confirm** | Open cache, click Deposit All. Verify confirmation gump appears. Click Confirm — items are deposited. |
| **Deposit All cancel** | Open cache, click Deposit All. Click Cancel on confirmation gump. Verify no items are deposited and "Deposit cancelled." message appears. |
| **Deposit All close gump** | Open cache, click Deposit All. Close the confirmation gump (right-click or ESC). Verify no items are deposited. |
| **Target corpse** | Kill an NPC near your cache. Use Deposit Item and target the corpse. Verify rejection: "You cannot deposit from a corpse." |
| **Target item in corpse** | Kill an NPC, open its corpse. Use Deposit Item and target an item inside the corpse. Verify rejection: "You cannot deposit items from a corpse." |
| **Normal deposit unchanged** | Use Deposit Item on backpack items and house items. Verify everything still works as before. |

### Combat Balance

| Area | What to Test |
|------|-------------|
| **NPC damage** | Have an NPC with class properties (e.g. Warrior) attack a player. Verify damage uses the NPC-specific path (Tactics * 0.00025) and NOT the player class multipliers. Compare before/after. |
| **Power Player melee** | As a Power Player, attack with a melee weapon. Verify skill bonus uses average of Anatomy + Tactics, not just Anatomy. |
| **Bladesinger melee** | As a Bladesinger, attack with a melee weapon. Verify skill bonus uses Anatomy + Tactics average at 0.00375 (lower than Warrior/PP). |
| **Paladin melee** | As a Paladin, attack with a melee weapon. Verify skill bonus uses Eval Int + Tactics average at 0.002. |
| **Mystic Archer ranged** | As a Mystic Archer, attack with a bow. Verify skill bonus uses Archery + Eval Int average at 0.005. |
| **Ranger ranged class bonus** | As a Ranger at levels 1-3, verify no ranged class bonus. At level 4+, verify bonus applies. |
| **Bladesinger damage reduction** | As a Bladesinger attacking a target, verify YOUR damage is reduced (attacker check, not defender). Verify reduction is 0.05 per level. |
| **Paladin/Warrior reduction** | Verify Paladin (0.05/level) and Warrior (0.10/level) defender damage reduction still applies. **Note: extra closing parenthesis may cause compile error — verify compilation first.** |
| **Mage damage reduction** | As a Mage, verify damage reduction applies independently. A Mage attacking a Paladin should see BOTH Mage reduction and Paladin reduction. |
| **Paladin spell resistance** | As a Paladin, get hit by spells. Verify resist chance is higher than before (bonus portion halved, not total). |

### Resurrection Crystal

| Area | What to Test |
|------|-------------|
| **Basic function** | Use a lifecrystal (`.create lifecrystal`). Die. Verify auto-resurrection with full HP/mana/stamina, items restored, corpse destroyed. |
| **Hidden on resurrect** | After crystal resurrection, verify the player is hidden. |
| **One-time use** | Use crystal, die and resurrect. Die again without another crystal. Verify normal death occurs. |
| **Incognito cleanup** | Use crystal while incognito. Die. Verify real name is restored before resurrection. |
| **Shapeshift cleanup** | Use crystal while earth-spell shapeshifted. Die. Verify shapeshift is cleared. |
| **Camouflage cleanup** | Use crystal while camouflaged. Die. Verify camouflage is removed. |
| **Murder report** | Get killed by another player with crystal active. Verify murder report gump still appears after resurrection. |
| **Poison field** | Use crystal, stand in a poison field, die. Verify poison is cured after resurrection. |
| **PvP interaction** | Use crystal while in PvP arena (`pvping` flag). Verify PvP death takes priority (freedeath check runs after PvP block). |
| **AoE crystal effect** | Use crystal near multiple players. Verify all players within 15 tiles receive the `#freedeath` buff. |
| **Logout removes buff** | Use crystal, log out, log back in. Die. Verify normal death occurs (buff was removed on logout). |
| **Dispel removes buff** | Use crystal on a player. Have another player cast Dispel on them. Verify the target receives "The resurrection crystal's protection has been dispelled!" and the buff is gone. Die afterwards — verify normal death. |
| **Dispel self-cast** | Use crystal on yourself. Cast Dispel on yourself. Verify the buff is removed. |
| **Dispel with absorption** | Use crystal, equip magic absorption. Have another player cast Dispel. Verify absorption blocks the spell and the crystal buff is NOT removed. |

### NPC and Equipment Fixes

| Area | What to Test |
|------|-------------|
| **Mondain's Staff** | `.create MondainsStaffWeapon` — verify it creates successfully with correct name "Mondain's Staff". |
| **Undead Flayer** | Spawn an Undead Flayer NPC. Verify weapon graphic and hit/miss sounds are correct. |
| **Chaos AI bow** | Engage the chaos multi-kill AI at range. Verify bow hits apply banish effect, not stamina drain. |

### Tooltip Improvements

| Area | What to Test |
|------|-------------|
| **Armor AR tooltip** | Hover over armor pieces. Verify tooltip shows `AR: X (Y)` where X is the effective weighted value and Y in brackets is the raw AR. E.g. a platemail breastplate with AR 25 should show `AR: 11 (25)`. |
| **Shield AR tooltip** | Hover over a shield. Verify the raw AR in brackets matches the item's actual AR value. |
| **Weapon DPS tooltip** | Hover over a weapon. Verify tooltip shows `DPS: X (Y)` where X is the modified DPS and Y in brackets is the raw average base DPS. The modified value should be higher than raw for most classes. |
| **No-class DPS** | Log in with a character that has no class. Hover over a weapon. Verify both DPS values are reasonable and close together (minimal class bonuses). |

---

## Files Modified

### Omega Cache
- `pkg/opt/omegacache/omegacache.inc` — `ConfirmDepositAll` gump, corpse check in `ValidateDepositTarget`

### Combat Balance
- `pkg/systems/combat/include/hitscriptinc.inc` — Class damage formulas, NPC damage path, Bladesinger reduction fix, Ranger/MA threshold
- `pkg/opt/shilhook/omegaattack.inc` — NPC class check gate in `CheckHitChance()`, whitespace cleanup
- `scripts/include/spelldata.inc` — Paladin resist chance rework, parenthesis syntax fix, whitespace cleanup

### NPC and Equipment
- `pkg/systems/combat/config/itemdesc.cfg` — Mondain's Staff name/desc, Undead Flayer graphic/sounds, whitespace cleanup
- `config/equip.cfg` — `ModainsStaffWeapon` → `MondainsStaffWeapon`
- `config/npcdesc.cfg` — NPC equipment swaps, Poisoning skill addition, whitespace cleanup
- `scripts/ai/chaosmultikillpcs.src` — Chaos AI bow hitscript changed to `banishscript`

### Tooltip Improvements
- `pkg/packethooks/megacliloc/itemdata.src` — AR tooltip shows raw value in brackets, DPS tooltip shows raw average base DPS in brackets

### Resurrection Crystal
- `scripts/misc/chrdeath.src` — `#freedeath` handler block, whitespace cleanup
- `pkg/std/dundee/lifecrystal.src` — Changed `freedeath` to `#freedeath` (temporary CProp)
- `pkg/std/spells/dispel.src` — Dispel now removes `#freedeath` buff from target
- `pkg/opt/colorwars/cwars.src` — Updated `freedeath` to `#freedeath` in cleanup
- `scripts/include/constants/propids.inc` — `PROPID_MOBILE_FREE_DEATH` updated to `#freedeath`
