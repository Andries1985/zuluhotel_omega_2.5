// Smart AOE filtering for Tamed NPC spells
// Applies party/faction/safe zone logic similar to SmartAoE in spelldata.inc
// but uses the tamed pet's MASTER as the reference for filtering

use uo;

function SmartFilterTamedTargets(tamed_pet, candidates)
	// For a tamed NPC, filter targets using SmartAoE logic with the master as reference
	// This prevents hitting: master, master's party, pets of master's party, safe zone players, etc.
	if (!tamed_pet.master)
		return candidates;  // If not tamed, return all candidates
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
