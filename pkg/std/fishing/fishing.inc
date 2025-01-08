use uo;
use cfgfile;
use os;

include "include/random";
include "include/itemutil";

function RandomFish(character)

var currentskill := GetEffectiveSkill( character, SKILLID_FISHING);
var thisone := Random(currentskill) + 1;
var fishtype;

if (thisone<50 )
	fishtype := Random(UOBJ_FISH_END - UOBJ_FISH_START);
	fishtype := UOBJ_FISH_START + fishtype;
elseif (thisone<120)
	fishtype := Random(UOBJ_SFISH_END - UOBJ_SFISH_START);
	fishtype := UOBJ_SFISH_START + fishtype;
elseif (thisone<130)
        fishtype := Random(9) + 0xA368;
endif
return fishtype;
endfunction

function createfish( who,fish,fish_amount, equipped )


	if (equipped==1 or fish == 0xA368 or fish == 0xA369 or fish == 0xA36A or fish == 0xA36B or fish == 0xA36C or fish == 0xA36D or fish == 0xA36E or fish == 0xA36F or fish == 0xA370 )
		CreateItemInBackpack(who, cint(fish), cint(fish_amount));
	else
		PlaySoundEffect(who, SFX_57);
		PerformAction( who, ANIM_SALUTE );
		CreateItemInBackpack(who, UOBJ_FISHSTEAK, (4 * fish_amount));
		SendSysmessage(who, "You carve up the fish.");
	endif

endfunction

function whatequipped(who, pole)

var item := GetEquipmentByLayer( who, 2 );
var object1:=item.graphic;

item := GetEquipmentByLayer( who, 1 );
var object2:=item.graphic;
var hands:=0;

if (object1==0xDBF) hands := 1; endif
if (object1==0x26BA) hands := 1; endif

if (object2==0xF51 or object2==0x13f6) hands := 2; endif

if (hands==0 and pole<>666)
if (equipitem(who,pole)) hands:=1; endif
endif

return hands;

endfunction

function check_for_water (theobj)
	//static water tiles
	if( (theobj.objtype >=0x346E) and (theobj.objtype <= 0x3485) )
		return 1;
	elseif( (theobj.objtype >= 0x3494) and (theobj.objtype <= 0x34AB) )
		return 1;
	elseif( (theobj.objtype >= 0x34B8) and (theobj.objtype <= 0x34BB) )
		return 1;
	elseif( (theobj.objtype >= 0x34BD) and (theobj.objtype <= 0x34C0) )
		return 1;
	elseif( (theobj.objtype >= 0x34C2) and (theobj.objtype <= 0x34C5) )
		return 1;
	elseif( (theobj.objtype >= 0x34C7) and (theobj.objtype <= 0x34CA) )
		return 1;
	elseif( (theobj.objtype >= 0x1796) and (theobj.objtype <= 0x17B2) )
		return 1;
	elseif(IsWater(theobj))
		return 1;
	else
		return 0;
	endif
endfunction

//Depreciated Code
/*
function povernut(ch, us)
    var xd, yd;

    xd := ch.x - us.x;
    yd := ch.y - us.y;
	var signt;


if ((xd == 0) or (yd == 0))
	if (xd == 0)
		if(yd >= 0)
			ch.facing:=0;
		else
			ch.facing:=4;
		endif
	else
		if(xd >= 0)
			ch.facing:=6;
		else
			ch.facing:=2;
		endif
	endif
else
    if (xd < 0)
	if (yd < 0)
		signt := 1;
	else
		signt := 2;
	endif
    else
	if (yd < 0)
		signt := 3;
	else
		signt := 4;
	endif
    endif
	case (signt)
        1: if ( Tan(-xd, -yd)>=0.4142)
		if (Tan(-xd, -yd)<=2.4142)
			ch.facing:=3;
		else
			ch.facing:=2;
		endif
	   else
			ch.facing:=4;
	   endif
        2: if ( Tan(-xd, yd)>=0.4142)
		if (Tan(-xd, yd)<=2.4142)
			ch.facing:=1;
		else
			ch.facing:=2;
		endif
	   else
			ch.facing:=0;
	   endif
        3: if ( Tan(xd, -yd)>=0.4142)
		if (Tan(xd, -yd)<=2.4142)
			ch.facing:=5;
		else
			ch.facing:=6;
		endif
	   else
			ch.facing:=4;
	   endif
        4: if ( Tan(xd, yd)>=0.4142)
		if (Tan(xd, yd)<=2.4142)
			ch.facing:=7;
		else
			ch.facing:=6;
		endif
	   else
			ch.facing:=0;
	   endif

 	endcase

endif
endfunction

function CreateSosBottle(char)

var cfg := ReadConfigFile("sosarea");
var maxI := GetConfigMaxIntKey(cfg);
var sosnum := Random(maxI)+1;
if ( sosnum > 10 )
	sosnum := 10;
endif
var elem := cfg[Random(sosnum)];
var X := cint(elem.x)+Random(cint(elem.range)*2) - cint(elem.range);
var Y := cint(elem.y)+Random(cint(elem.range)*2) - cint(elem.range);
var color := cint(elem.color);

// var Z := GetWorldHeight(X,Y);

// syslog(X+" "+Y+" "+Z);

var bottle :=CreateItemInContainer(char.backpack, 0xa360);

SetObjProperty(bottle, "tileX", X);
SetObjProperty(bottle, "tileY", Y);
SetObjProperty(bottle, "colormes", color);
// SetObjProperty(bottle, "tileZ", (Z+1));

UnloadConfigFile( "sosarea");

endfunction
*/