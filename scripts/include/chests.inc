// Locking Spawning Chest system  version .1
// by Zulu (zulu@zuluhotel.com)   July 5, 2000
use cfgfile;


include "util/key";
include "include/random";
include "include/constants";

function lockem(chest)

	var level;
	if( GetObjProperty( chest , "treasurechest" ) )
		level := Random(5) + 1;
		set_critical(1);
		if( RandomDiceStr("1d100") <= 15 )
			SetObjProperty( chest , "trap_strength", level );
			SetObjProperty( chest , "trap_type", cstr(RandomDiceStr("1d3")) );
			chest.usescript := ":traps:traps";
		endif
		set_critical(0);
	else
		set_critical(1);
		level := RandomDiceStr("1d8");
		var diff  := level * 15;
		SetObjProperty( chest, "spawnchest", level );
		SetObjProperty( chest, "LockDifficulty", diff );
		SetObjProperty( chest, "lockid" , AllocLockId());
		chest.locked := 1;
	
		if( RandomDiceStr("1d100") <= (level * 5) )
			SetObjProperty( chest , "trap_strength", level );
			SetObjProperty( chest , "trap_type", cstr(RandomDiceStr("1d3")) );
			chest.usescript := ":traps:traps";
		endif
		set_critical(0);
	endif

endfunction


function finddiff(chest)

	var diff := GetObjProperty(chest, "LockDifficulty");
	if( diff )
		return diff;
	endif

	var cfg := ReadConfigFile( CFGFILE_LOCK_DIFFICULTIES );
	if( !cfg )
		return 0;
	endif
	
	return (cfg[chest.objtype].difficulty);

endfunction


function canpick(chest)

	if ((GetObjProperty(chest,"houseserial")) or (chest.multi))
		return 0;
	else
		return 1;
	endif

endfunction


function addchest(chest)

set_critical(1);

var chests:=getglobalproperty("unlockedchests"),csize:=chests.size();

if (chests[1]=="NONE") csize:=0; endif

chests[csize+1]:=chest.serial;

setglobalproperty("unlockedchests",chests);
set_critical(0);

endfunction