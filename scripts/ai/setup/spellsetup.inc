use npc;

include "include/starteqp";
include ":spawnpoint:customnpc";
include "include/random";
include "include/randname";

set_critical(0);
var speech;
var master;
var spells := {};
var me := self();

var cast_pct;
var num_casts;
var count_casts ;
var saywords := 1;
var summons := 3;

var will_cast;
var flee_point;

var npccfg := readconfigfile("npcdesc");
var speechelem := npccfg[me.npctemplate];
speech := getconfigstring(speechelem, "speech") ;
spells := GetConfigStringArray( speechelem, "spell" );
cast_pct := speechelem.cast_pct;
num_casts  := speechelem.num_casts;
count_casts  := speechelem.count_casts;
saywords := speechelem.saywords;
if (!cast_pct)
        cast_pct := 10;
endif
flee_point := speechelem.flee_point;
if (!flee_point)
        flee_point := 10;
endif

if(GetObjProperty(me, "Champion"))
	me.title_guild := "Champion";
endif

if (!me.backpack)
	var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75, 1);
	EquipItem(me, newbackpack);
	if (npccfg[me.npctemplate].colorrange)
		var parms := splitwords(npccfg[me.npctemplate].colorrange);
		me.color := Random(cint(parms[2]) - cint(parms[1]))+cint(parms[1]);
		setobjproperty(me,"color",me.color);
	endif
	if (npccfg[me.npctemplate].dress)
		var parms := {};
		parms[1]:= me;
		parms[2]:= npccfg[me.npctemplate].dress;
		start_script("::/misc/dressme", parms);
	elseif (GetObjProperty(me, "customnpc"))
		foreach item in ListEquippedItems(me)
			if (item != newbackpack)
				Destroyitem(item);
			endif
		endforeach
		DressNPCCustom(me);
	elseif (npccfg[me.npctemplate].equip)
    		EquipFromTemplate(me, npccfg[me.npctemplate].equip);
	endif

	if( me.name["<random>"] )
		if ((me.graphic == 400) or (me.graphic == 401))
			me.gender := Random(2);
			me.graphic := 400 + me.gender;
		endif
		SetName ( me , RandomName( me ) );
	endif

	if ( npccfg[me.npctemplate].mount )
		var mymount := CreateItemAtLocation(5288, 1176, 0, 0x1F021, 1);
		var mountparms := splitwords(npccfg[me.npctemplate].mount);
		mymount.graphic := cint(mountparms[1]);
		mymount.color := cint(mountparms[2]);
		EquipItem(me, mymount);
	endif

	// MagicalEquip(me);

endif
var ammotype;
var ammoamount;
var theammo;
if (npccfg[me.npctemplate].ammotype)
	ammotype := npccfg[me.npctemplate].ammotype;
	if (ammotype)
       		ammoamount := npccfg[me.npctemplate].ammoamount;
		if (!ammoamount)
			ammoamount := 30;
		endif
		theammo := createiteminbackpack(me, ammotype, ammoamount);
	endif
endif