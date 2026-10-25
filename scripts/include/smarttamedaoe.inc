// Smart AOE filtering for npc mass spells (npccast.inc)
// Kept as a name for the mass spell callers; the rules live in NpcSmartAoE() in spelldata.inc
// so mass spells, regular area spells and songs cast by an npc all pick targets the same way.

use uo;

function SmartFilterTamedTargets(tamed_pet, candidates)
	return NpcSmartAoE( tamed_pet, candidates );
endfunction
