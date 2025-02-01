include ":spawnpoint:customnpc";


set_critical(0);
var me := self();

var flee_point;

drop_anchor();

flee_point := 10;

if (!me.backpack)
	var newbackpack := CreateItemAtLocation(me.x, me.y, me.z, 0xe75,1,me.realm);
	EquipItem(me, newbackpack);
	if (!Random(100) )
		me.color := 1109;
		setobjproperty(me,"color",1109);
	endif
if (GetObjProperty(me, "customnpc"))
		foreach item in ListEquippedItems(me)
			if (item != newbackpack)
				Destroyitem(item);
			endif
		endforeach
		DressNPCCustom(me);
	endif
endif
