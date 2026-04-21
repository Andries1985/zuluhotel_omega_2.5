use uo;
use os;

include "include/client";
include "include/itemid_core";

function SelectItemToID(who,delay)

	var the_item;
	var skillID := GetEffectiveSkill( who, SKILLID_ITEMID );
	var intelligence := GetAttribute( who, ATTRIBUTEID_INTELLIGENCE );
	const MIN_DELAYMS := 250;
	const MIN_SKILLFORBATCH := 100;
	const MIN_INTFORBATCH := 100;

	SendSysMessage( who, "Select an object to identify." );
	the_item := Target(who, TGTOPT_CHECK_LOS);
	if( !the_item )
		return 0;
	endif

	if( !IDCore_ValidateTargetItem(who, the_item, 1, 1, 2, 1) )
		return 1;
	endif

	if (!ReserveItem( the_item ))
		//Print("Item "+the_item.serial+" is already reserved.");
		return 1;
	endif

	if (the_item.isa(POLCLASS_CONTAINER))
	 	if ((skillID >= MIN_SKILLFORBATCH) & (intelligence >= MIN_INTFORBATCH))
			var itemlist := IDCore_CollectContainerMagicItems(the_item);
			if (itemlist.Size() == 0)
				SendSysMessage(who, "There's nothing to identify in there.");
				ReleaseItem( the_item );
				return 1;
			else
				SendSysMessage(who,"Your vast intellect tries to uncover the mysteries of your hoard.", FONT_NORMAL, 2601 );
				SendSysMessage(who,"There's "+itemlist.Size()+" magical items to identify.", FONT_NORMAL, 2601 );
				var result := IDCore_IdentifyContainerItems(
					who,
					the_item,
					1,
					Max(delay*1000,MIN_DELAYMS),
					the_item,
					1
				);
				if( result[3] )
					ReleaseItem( the_item );
					return 1;
				endif
			endif
			ReleaseItem( the_item );
			return 1;
		else
			SendSysMessage(who, "You cannot do that yet.");
			ReleaseItem( the_item );
			return 1;
		endif
	endif
	
	if( !the_item.movable )
		SendSysMessage( who, "You can't id unmovable items." );
		ReleaseItem( the_item );
		return 1;
	endif

	var ided := CanBeIDed(the_item);
	var hasdetails := GetItemDetails(who, the_item);
	if(hasdetails && ided)
		ReleaseItem( the_item );
		return 1;
	endif

	if( ided )
		SendSysMessage( who, the_item.desc+" has already been identified.", FONT_NORMAL, 2595 );
		ReleaseItem( the_item );
		return 1;
	endif

	ItemID(who,the_item);

	ReleaseItem( the_item );
	return 1;
	
endfunction

function ItemID(who,the_item)
	IDCore_IdentifyItem(who, the_item, 1);
	//Print("Item IDed.");
	//Print("Releasing item..." + the_item.serial + " ." + the_item.desc);
	
	return 1;
		
endfunction


function GetItemDetails(who, the_item)
	return IDCore_GetItemDetails(who, the_item);

endfunction


function CanBeIDed(the_item)
	return IDCore_CanBeIDed(the_item);

endfunction

function IsLockedSpawnPointChest(the_item)
	return IDCore_IsLockedSpawnPointChest(the_item);

endfunction