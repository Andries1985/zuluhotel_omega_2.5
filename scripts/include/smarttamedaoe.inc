// Smart AOE filtering for Tamed NPC spells
// Applies party/faction/safe zone logic similar to SmartAoE in spelldata.inc
// but uses the tamed pet's MASTER as the reference for filtering

use uo;

function SmartFilterTamedTargets(tamed_pet, candidates)
	// For tamed NPCs, filter targets using SmartAoE logic with the master as reference.
	// For released or wild NPCs, preserve the original behavior: only hit players.
	if (!tamed_pet.master)
		var wild_remove := {};
		var wild_i;
		for(wild_i:=1; wild_i<=candidates.Size(); wild_i:=wild_i+1)
			if (candidates[wild_i].serial == tamed_pet.serial || candidates[wild_i].isa(POLCLASS_NPC))
				wild_remove.append(wild_i);
			endif
		endfor
		for(wild_i:=wild_remove.Size(); wild_i>=1; wild_i:=wild_i-1)
			candidates.erase(wild_remove[wild_i]);
		endfor
		return candidates;
	endif
	
	var master := tamed_pet.master;
	var master_RPer := GetObjProperty(master, "IsRPer");
	var master_noPK := GetObjProperty(master, "NOPKAREA");
	var master_CW := GetObjProperty(master, "cwarring");
	var master_CWcolor := GetObjProperty(master, "cwcolor");
	var to_remove := {};
	var i;
	
	for(i:=1; i<=candidates.Size(); i:=i+1)
		var victim_RPer := GetObjProperty(candidates[i], "IsRPer");
		var victim_noPK := GetObjProperty(candidates[i], "NOPKAREA");
		var victim_CWcolor := GetObjProperty(candidates[i], "cwcolor");
		var same_faction := master_RPer == victim_RPer;
		var same_CWcolor := 0;
		
		if (master_CW)
			if (GetGlobalProperty("cwtype") != "k")
				same_CWcolor := victim_CWcolor == master_CWcolor;
			endif
		endif
		
		// Don't hit self
		if (candidates[i].serial == tamed_pet.serial)
			to_remove.append(i);
		// Don't hit the master
		elseif (candidates[i].serial == master.serial)
			to_remove.append(i);
		// Don't hit pets of the master or master's party
		elseif ((candidates[i].isa(POLCLASS_NPC)) && candidates[i].master && (candidates[i].master.serial == master.serial || (master.party && candidates[i].master.party == master.party)))
			to_remove.append(i);
		// Don't hit players in the master's party
		elseif ((!candidates[i].isa(POLCLASS_NPC)) && master.party && (candidates[i].party == master.party))
			to_remove.append(i);
		// For players not in party: check faction, safe zones, clan war
		elseif (!candidates[i].isa(POLCLASS_NPC) && !master.cmdlevel)
			if((master_noPK) || (victim_noPK) || (!same_faction) || (same_CWcolor))
				to_remove.append(i);
			endif
		endif
	endfor
	
	// Erase in reverse order so indices stay valid
	for(i:=to_remove.Size(); i>=1; i:=i-1)
		candidates.erase(to_remove[i]);
	endfor
	
	return candidates;
endfunction
