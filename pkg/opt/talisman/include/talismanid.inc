use uo;
use os;
use polsys;

include "include/client";
include "include/itemid_core";
include "include/classes";
//include "include/skillpoints";
//include "include/itemutil";
//include "include/chests";
//include ":containers:checks";


function TalismanID( who, delay, talisman )

	var the_item;
	// delay := 4; // Keep caller-provided delay (e.g. BASE_DELAY) instead of overriding it here.
	var skillID;
	var skill_name := "Item Identification";
	var intelligence := GetAttribute( who, ATTRIBUTEID_INTELLIGENCE );
	const MIN_DELAYMS := 250;
	const MIN_SKILLFORBATCH := 100;
	const MIN_INTFORBATCH := 100;

	if ( GetObjProperty( who , CLASSEID_BARD ) )
		skillID := GetEffectiveSkill( who, SKILLID_TASTEID );
		skill_name := "Taste Identification";
		if( skillID >= 100)
			delay := 0;
		endif
    elseif ( GetObjProperty( who , CLASSEID_CRAFTER ) )
		skillID := CInt(GetEffectiveSkill( who, SKILLID_ARMSLORE ));
		skill_name := "Arms Lore";
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_WARRIOR ) || GetObjProperty( who , CLASSEID_BLADESINGER) )
		skillID := GetEffectiveSkill( who, SKILLID_ANATOMY );
		skill_name := "Anatomy";
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_THIEF ) )
		skillID := GetEffectiveSkill( who, SKILLID_REMOVETRAP );
		skill_name := "Remove Trap";
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_RANGER ) )
		skillID := GetEffectiveSkill( who, SKILLID_CAMPING );
		skill_name := "Camping";
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_POWERPLAYER) )
		skill_name := "Eval Int, Anatomy, Arms Lore, Taste ID, Remove Trap, Camping, or Item ID";
        if( GetEffectiveSkill( who, SKILLID_EVALINT ) >= 100)
            delay := 0;
            skillID := 100;
        elseif( GetEffectiveSkill( who, SKILLID_ANATOMY ) >= 100)
            delay := 0;
            skillID := 100;
        elseif( GetEffectiveSkill( who, SKILLID_ARMSLORE ) >= 100)
            delay := 0;
            skillID := 100;
        elseif( GetEffectiveSkill( who, SKILLID_TASTEID ) >= 100)
            delay := 0;
            skillID := 100;
        elseif( GetEffectiveSkill( who, SKILLID_REMOVETRAP ) >= 100)
            delay := 0;
            skillID := 100;
        elseif( GetEffectiveSkill( who, SKILLID_CAMPING ) >= 100)
            delay := 0;
            skillID := 100;
        elseif( GetEffectiveSkill( who, SKILLID_ITEMID ) >= 100)
            delay := 0;
            skillID := 100;
        endif    
    elseif ( GetObjProperty( who , CLASSEID_MAGE ) )
		skillID := GetEffectiveSkill( who, SKILLID_ITEMID );
		skill_name := "Item Identification";
		if( skillID >= 100)
			delay := 0;
		endif
    elseif ( GetObjProperty( who , CLASSEID_MYSTIC_ARCHER) || GetObjProperty( who , CLASSEID_PALADIN ) )
        skillID := GetEffectiveSkill( who, SKILLID_EVALINT );
		skill_name := "Evaluating Intelligence";
        if( skillID >= 100)
            delay := 0;
        endif
	else
		skillID := GetEffectiveSkill( who, SKILLID_ITEMID );
		skill_name := "Item Identification";
		if( skillID >= 100)
			delay := 0;
		endif
	endif

	SendSysMessage( who, "Select an object to identify." );
	the_item := Target( who, TGTOPT_CHECK_LOS );
	if( !the_item )
		return 1;
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
					0,
					Max(delay*1000,MIN_DELAYMS),
					the_item,
					1,
					talisman
				);
				if( result[3] )
					ReleaseItem( the_item );
					return 1;
				endif
			endif
			ReleaseItem( the_item );
			return 1;
		else
			SendSysMessage(who, "You need 100 " + skill_name + " and 100 Intelligence to identify the contents of a container.");
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

	ItemID(who, the_item, talisman);
	ReleaseItem( the_item );
	return 1;
	
endfunction

function ItemID(who, the_item, talisman)

	if( !talisman )
		SendSysMessage( who, "You need a talisman of identification to do that.", FONT_NORMAL, 2595 );
		return 0;
	endif

	var idcharges := CInt(GetObjProperty(talisman, "idcharges"));
	if( idcharges < 1 )
		SendSysMessage( who, "Your talisman has no identification charges left.", FONT_NORMAL, 2595 );
		return 0;
	endif
	var identified := IDCore_IdentifyItem(who, the_item, 0);
	if( identified )
		SetObjProperty( talisman, "idcharges", idcharges - 1 );
	endif
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
