use uo;
use os;

// time to wait for house events before checking for banned people inside
const EVENT_TIMEOUT := 20;

// reload list of bans and owners every CACHE_RELOAD times the script cycles (EVENT_TIMEOUT seconds)
const CACHE_RELOAD := 6;

// maximum number of co-owners of the house
const MAX_CO_OWNERS := 5;

// maximum number of friends of the house
const MAX_FRIENDS := 20;

// maximum number of friends of the house
const MAX_BANNED := 20;

// if set to 1 the house can decay over time
const DECAY := 0;

var tile_list := {
	0x755d,
	0x755c
};

function addsign(byref sign)

	set_critical(1);

	var signs:=GetGlobalProperty("signs"),csize:=signs.size();

	if (signs[1]=="NONE") 
		csize:=0; 
	endif

	signs[csize+1]:=sign.serial;

	SetGlobalProperty("signs",signs);
	SetObjProperty(sign,"#Z",1);
	set_critical(0);

endfunction
