// include "include/starteqp";
include ":spawnpoint:customnpc";

	set_critical(0);
	var me := self();
	var flee_point := 10;
	drop_anchor(me);

	if (!me.backpack)
		var npccfg := readconfigfile("npcdesc");
		var newbackpack := CreateItemAtLocation(5288 , 1176 , 0, 0xe75, 1, "britannia");
		EquipItem(me, newbackpack);
		if (npccfg[me.npctemplate].colorrange)
			var parms := splitwords(npccfg[me.npctemplate].colorrange);
			me.color := Random(cint(parms[2]) - cint(parms[1]))+cint(parms[1]);
			setobjproperty(me,"color",me.color);
		endif

		if (npccfg[me.npctemplate].Name == "a Horse")
			if(!GetObjProperty(me, "nocut"))
				if(Random(100000) > 90000)
					me.color := Random(1050)+1;
					setobjproperty(me,"color",me.color); 
				endif
			endif
		endif

		if( me.name["<random>"] )
			SetName ( me , RandomName( me ) );
		endif

		// custom npc dress
			if (GetObjProperty(me, "customnpc"))
			foreach item in ListEquippedItems(me)
				if (item != newbackpack)
					Destroyitem(item);
				endif
			endforeach
			DressNPCCustom(me);
		endif
		// MagicalEquip(me);

	endif
