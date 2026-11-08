use uo;
use os;
use basic;

include ":karmafame:karmafame";
include "include/attributes";

const MAX_LOSS_PERCENT	:= 0.5;

function ResurrectMobile( who, by_who := 0 )
	by_who := by_who;

	SetObjProperty(who, "ResProtection", ReadGameClock()+5);

	Resurrect( who );
	FixColor( who );
	
	RestoreHpRegenRate( who );

	// Destroy a base colour non newbie robe in players pack after res
	var robe := GetEquipmentByLayer( who, 0x16 );
	if (robe && robe.color == 0 && !robe.newbie && robe.objtype == 0x1f03)
		DestroyItem(robe);
	endif

endfunction


function FixColor( who )

   var newcolor, racepicked := who.title_race;

   if (racepicked=="Elf")
	newcolor := 770;
   elseif (racepicked=="Dark-Elf")
	newcolor := 33877;
   elseif (racepicked=="Goblin")
	newcolor := 34186;
   elseif (racepicked=="Barbarian")
	newcolor := 33804;
   elseif (racepicked=="Dwarf")
	newcolor := 33888;
   elseif (racepicked=="Orc")
	newcolor := 1056;
   elseif (racepicked=="Frost-Elf")
	newcolor := 91;
   else
	newcolor := who.truecolor;
   endif

   who.color := newcolor;

endfunction

