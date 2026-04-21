use uo;
use os;

include "include/client";
include "include/namingbyenchant";
include "include/skillpoints";
include "include/itemutil";
include "include/constants/propids";

function IDCore_ValidateTargetItem(
	who,
	the_item,
	require_access := 1,
	require_distance := 1,
	max_distance := 2,
	deny_locked_spawnpoint := 1
)

	if( !the_item )
		return 0;
	endif

	if( !the_item.IsA(POLCLASS_ITEM) )
		SendSysMessage( who , "That's not an item!" );
		return 0;
	endif

	if( require_access && !Accessible( who , the_item ) )
		SendSysMessage( who , "You can't reach that." );
		return 0;
	endif

	if( require_distance && Distance( who , the_item ) > max_distance )
		SendSysMessage( who , "That is too far." );
		return 0;
	endif

	if( deny_locked_spawnpoint && IDCore_IsLockedSpawnPointChest(the_item) )
		SendSysMessage( who, "You must unlock that spawnpoint chest first." );
		return 0;
	endif

	return 1;

endfunction

function IDCore_IdentifyReservedItem(who, the_item, check_skill := 1, thepoints := 0)

	if( IDCore_IsLockedSpawnPointChest(the_item) )
		SendSysMessage( who, "You must unlock that spawnpoint chest first." );
		return 0;
	endif

	var ided := GetObjProperty( the_item , "IDed" );

	if( !ided )
		if( !IsIDable( the_item ) )
			SendSysmessage( who , "This is a normal item." );
			SetObjProperty( the_item , "IDed" , 1 );
			return 1;
		elseif( check_skill )
			if( !thepoints )
				thepoints := get_default_points( SKILLID_ITEMID );
			endif
			if( !CheckSkill( who , SKILLID_ITEMID , -2 , thepoints ) )
				SendSysmessage (who, "I'm not sure . . .");
				return 0;
			endif
		endif
	endif

	if( GetObjProperty( the_item , "UnIDedMagicalItem" ) or
		GetObjProperty( the_item , "Enchanted" ) or
		GetObjProperty( the_item , "IsGMItem" ) or
		GetObjProperty( the_item , "EnchantColor" ) or
		GetObjProperty( the_item , "StaticName" ) )
		if( !ided )
			SetNameByEnchant( the_item );
		endif
		IDCore_GetItemDetails(who, the_item);
	else
		SendSysmessage( who , "This is a normal item." );
	endif

	SetObjProperty( the_item , "IDed" , 1 );

	return 1;

endfunction

function IDCore_IdentifyItem(who, the_item, check_skill := 1, thepoints := 0)

	if( !ReserveItem( the_item ) )
		return 0;
	endif

	var identified := IDCore_IdentifyReservedItem(who, the_item, check_skill, thepoints);
	ReleaseItem( the_item );

	return identified;

endfunction

function IDCore_CollectContainerMagicItems(container)

	var itemlist := {};
	foreach thing in EnumerateItemsInContainer(container)
		if( GetObjProperty(thing, "UnIDedMagicalItem") )
			itemlist.append(thing);
		endif
	endforeach

	return itemlist;

endfunction

// Returns array {total_items, identified_items, aborted_flag}
function IDCore_IdentifyContainerItems(
	who,
	container,
	check_skill := 1,
	delayms := 0,
	range_anchor := 0,
	enforce_access := 1,
	talisman := 0
)

	var result := {0, 0, 0};
	var itemlist := IDCore_CollectContainerMagicItems(container);
	result[1] := itemlist.Size();

	foreach item in itemlist
		if( enforce_access && (!Accessible( who , item ) || (range_anchor && Distance( who , range_anchor ) > 2)) )
			SendSysMessage( who , "You can't reach that." );
			result[3] := 1;
			return result;
		endif

		var idcharges := 0;
		if( talisman )
			idcharges := CInt(GetObjProperty(talisman, "idcharges"));
			if( idcharges < 1 )
				SendSysMessage( who, "Your talisman has no identification charges left.", FONT_NORMAL, 2595 );
				result[3] := 1;
				return result;
			endif
		endif

		if( IDCore_IdentifyItem(who, item, check_skill) )
			result[2] := result[2] + 1;
			if( talisman )
				SetObjProperty( talisman, "idcharges", idcharges - 1 );
			endif
		endif

		if( delayms > 0 )
			sleepms(delayms);
		endif
	endforeach

	return result;

endfunction

function IDCore_GetItemDetails(who, the_item)

	var hasdetails := 0;

	var ncharges   := GetObjProperty( the_item , "charges" );
	var manastored := GetObjProperty( the_item , "ManaLevel" );

	if( GetObjProperty( the_item , "IsGMItem" ))
		SendSysMessage( who , "It appears to be a " + the_item.desc + "," , FONT_NORMAL, 2589 );
	elseif( GetObjProperty( the_item , "isLegendary" ))
		SendSysMessage( who , "Woohoo! It appears to be a " + the_item.desc + "," , FONT_BOLD, 2578 );
	elseif( GetObjProperty( the_item , "isRare" ))
		SendSysMessage( who , "It appears to be a " + the_item.desc + "," , FONT_NORMAL, 2567 );
	elseif( GetObjProperty( the_item , "isRareProt" ))
		SendSysMessage( who , "It appears to be a " + the_item.desc + "," , FONT_NORMAL, 68 );
	elseif( GetObjProperty( the_item , "isUncommonProt" ))
		SendSysMessage( who , "It appears to be a " + the_item.desc + "," , FONT_NORMAL, 6 );
	elseif( GetObjProperty( the_item , "isUncommon" ))
		SendSysMessage( who , "It appears to be a " + the_item.desc + "," , FONT_NORMAL, 53 );
	else
		SendSysmessage( who , "It appears to be a " + the_item.desc + "," );
	endif
	if( ncharges )
		var staticname := GetObjProperty( the_item , "StaticName" );
		var true_name := GetObjProperty( the_item , "tname" );

		if( staticname )
			Setname( the_item , staticname );
		elseif( true_name )
			Setname( the_item , true_name );
		endif

		SendSysmessage( who , "and there's " + ncharges + " charges left in that item." );
		hasdetails := 1;

	elseif( manastored )
		var staticname := GetObjProperty( the_item , "StaticName" );
		var true_name := GetObjProperty( the_item , "tname" );

		if( staticname )
			Setname( the_item , staticname );
		elseif( true_name )
			Setname( the_item , true_name );
		endif

		SendSysmessage( who , "and there's " + manastored + " mana stored in it." );
		hasdetails := 1;
	endif

	if( GetObjProperty( the_item , "EffectCircle") )
		SendSysmessage( who , "Circle: " + GetObjProperty( the_item , "EffectCircle") );
		SendSysmessage( who , "Chance: " + GetObjProperty( the_item , "ChanceOfEffect") );
		hasdetails := 1;
	endif

	if( GetObjProperty( the_item , "Poisonlvl") )
		SendSysmessage( who , "And it seems to be imbued with a Deadly level " + GetObjProperty( the_item , "Poisonlvl") + " poison!", FONT_NORMAL, 2580 );
		hasdetails := 1;
	endif

	return hasdetails;

endfunction

function IDCore_CanBeIDed(the_item)

	if( IDCore_IsLockedSpawnPointChest(the_item) )
		return 1;
	endif

	if(GetObjProperty( the_item , "IDed" ))
		return 1;
	elseif(GetObjProperty( the_item , "UnIDedMagicalItem"))
		return 0;
	elseif(GetObjProperty( the_item , "Enchanted" ))
		return 0;
	elseif(GetObjProperty( the_item , "IsGMItem" ))
		return 0;
	elseif(GetObjProperty( the_item , "EnchantColor" ))
		return 0;
	elseif(GetObjProperty( the_item , "StaticName" ))
		return 0;
	else
		return 1;
	endif

endfunction

function IDCore_IsLockedSpawnPointChest(the_item)

	if( !the_item || !the_item.IsA(POLCLASS_CONTAINER) )
		return 0;
	endif

	if( GetObjProperty(the_item, PROPID_CHEST_SPAWNPOINT_CHEST) && the_item.locked )
		return 1;
	endif

	return 0;

endfunction
