// wfhvitals.inc - Warrior for Hire vitals (Zuluhotel Omega 2.5 version).
//
// A warrior's vitals work like a player's: max HP is its Strength, max Mana its Intelligence and
// max Stamina its Dexterity, buffs included, so a Bless or the Staff of Nagash raises them and
// they drop back when the buff ends. That is pkg/opt/shilhook/regen.src's default for an NPC
// with no CustomHitsLevel / CustomManaLevel / CustomStaminaLevel CProp.
//
// The first 1.1.4 port of ZH3.0's vital ceilings stamped those CProps instead (HP = 2 x base Str,
// mods excluded). They were static, so buffs never moved max HP, and the doubling made the
// warrior far tankier than a player of the same Strength. Any warrior still carrying them has
// them erased here, from the AI at startup and from every restore path.

use uo;
use vitals;

function ResetWfhVitals( wfh )

	EraseObjProperty( wfh, "CustomHitsLevel" );
	EraseObjProperty( wfh, "CustomManaLevel" );
	EraseObjProperty( wfh, "CustomStaminaLevel" );

	// The maximum functions in regen.src only run when the engine recalculates, so force it now.
	RecalcVitals( wfh );

endfunction

// Plus a full top-up: used where the warrior is (re)built from scratch.
function ResetWfhVitalsAndFill( wfh )

	ResetWfhVitals( wfh );
	SetHp( wfh, GetMaxHp( wfh ) );
	SetMana( wfh, GetMaxMana( wfh ) );
	SetStamina( wfh, GetMaxStamina( wfh ) );

endfunction
