include ":spawnpoint:customnpc";

set_critical(0);
var me := self();
var npccfg := readconfigfile("npcdesc");
var speechelem := npccfg[me.npctemplate];
var speech := getconfigstring(speechelem, "speech") ;
var saywords := speechelem.saywords;
var flee_point := speechelem.flee_point;
if (!flee_point)
        flee_point := 10;
endif

if (!me.backpack)
	var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75,1,me.realm);
	EquipItem(me, newbackpack);

	if (GetObjProperty(me, "customnpc"))

		foreach item in ListEquippedItems(me)
			if (item != newbackpack)
				Destroyitem(item);
			endif
		endforeach
		DressNPCCustom(me);
	endif

	if( me.name["<random>"] )
		me.gender := Random(2);
		me.graphic := 400 + me.gender;
		var tname := randomname(me);
		me.name := tname + " the town crier";
		if (npccfg[me.npctemplate].equip)
    			EquipFromTemplate(me, npccfg[me.npctemplate].equip);
		endif
	endif
	var myhome := { };
	myhome[1] := me.x;
	myhome[2] := me.y;
	myhome[3] := me.z;
	SetObjProperty(me,"myhome",myhome);
endif
