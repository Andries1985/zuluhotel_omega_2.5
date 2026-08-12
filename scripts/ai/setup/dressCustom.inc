use uo;
use os;

function DressNPCCustom(me)

var point := SystemFindObjectBySerial(GetObjProperty(me, "SpawnPoint"));

foreach item in EnumerateItemsInContainer(point)
	var copy := CreateItemCopyAtLocation(5273, 1181, 0, item);
	copy.movable := 1;
	if (GetObjProperty(copy, "packitem"))
		EraseObjProperty(copy, "packitem");
		MoveItemToContainer(copy, me.backpack);
	else
		EquipItem(me, copy);
	endif
	copy.movable := 1;
endforeach

endfunction