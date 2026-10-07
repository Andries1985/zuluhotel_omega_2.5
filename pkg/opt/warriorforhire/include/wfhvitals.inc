// wfhvitals.inc - Warrior for Hire vital ceilings (Zuluhotel Omega 2.5 version).
//
// Ported from ZH3.0's 2026-09-30 change. There the ceilings go through the attributes package's
// SetNpcVitalOverride(); this shard has no such package, but pkg/opt/shilhook/regen.src already
// lets an NPC override its maximums through the CustomHitsLevel / CustomManaLevel /
// CustomStaminaLevel CProps (stored in hundredths, the unit the vital maximum functions return),
// so the same rule is stamped through those instead.
//
// Rule: HP = 2 x base Str; Mana = base Int; Stam = base Dex (temporary mods excluded). The CProps
// are static, so this is re-stamped on hire, on hourly stat growth and on every High Priest
// restore. Before this a warrior's max HP simply equalled its Strength.

use uo;
use vitals;

include "include/attributes";

const WFH_HITS_PER_STR := 2;

function SetWfhVitalCeilings( wfh )

	var basestr := GetStrength( wfh ) - GetStrengthMod( wfh );
	var baseint := GetIntelligence( wfh ) - GetIntelligenceMod( wfh );
	var basedex := GetDexterity( wfh ) - GetDexterityMod( wfh );

	SetObjProperty( wfh, "CustomHitsLevel", basestr * WFH_HITS_PER_STR * 100 );
	SetObjProperty( wfh, "CustomManaLevel", baseint * 100 );
	SetObjProperty( wfh, "CustomStaminaLevel", basedex * 100 );

	// The maximum functions in regen.src only run when the engine recalculates, so force it now.
	RecalcVitals( wfh );

endfunction

// Ceilings plus a full top-up: used where the warrior is (re)built from scratch.
function SetWfhVitalCeilingsAndFill( wfh )

	SetWfhVitalCeilings( wfh );
	SetHp( wfh, GetMaxHp( wfh ) );
	SetMana( wfh, GetMaxMana( wfh ) );
	SetStamina( wfh, GetMaxStamina( wfh ) );

endfunction
