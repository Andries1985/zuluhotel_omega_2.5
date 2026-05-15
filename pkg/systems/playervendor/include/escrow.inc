// Shared player-vendor escrow helpers

use os;
use uo;
use util;

const PLAYERVENDOR_ESCROW_DF_PREFIX := "merchantescrow_";

function BuildEscrowDatafileName(player_obj)
	if(!player_obj)
		return "";
	endif

	var name := player_obj.name;
	var sanitized := StrReplace(name, " ", "_");
	return PLAYERVENDOR_ESCROW_DF_PREFIX + sanitized + "_" + player_obj.serial;
endfunction
