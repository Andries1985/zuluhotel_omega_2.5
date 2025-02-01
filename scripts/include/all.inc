include "include/damages";
include "include/managers";
include "include/attributes";

function GetNpcElement( byref npc )

	if( !npc.IsA(POLCLASS_NPC) )
		return 0;
	endif

	return FindConfigElem( ReadConfigFile(CFGFILE_NPCS), npc.npctemplate );

endfunction


function KillMobile( byref mobile, guard_killed := 0, by_who := 0 )

	if( mobile.cmdlevel >= CMDLEVEL_COUNSELOR )
		return 0;
	endif
	
	if( guard_killed )
		SetObjProperty( mobile, PROPID_MOBILE_GUARD_KILLED, 1 );
		if( mobile.IsA(POLCLASS_NPC) )
			DestroyAllEquippedItemsOn(mobile);
		endif
	endif
	
	RevokePrivilege( mobile, PRIVILEGEID_INVULNERABLE );
	if( by_who )
		return ApplyTheDamage( mobile, by_who, GetMaxHP(mobile)+3 );
	else
		EraseObjProperty( mobile, PROPID_MOBILE_HITLIST );
		return ApplyRawDamage( mobile, GetMaxHP(mobile)+3 );
	endif
	
endfunction


function DestroyAllEquippedItemsOn( byref mobile )

	foreach item in ListEquippedItems( mobile )
		if( item != mobile.backpack )
			if( item.graphic < UOBJ_HAIRS_START )
				DestroyEquippedItem( mobile, item );
			elseif( item.graphic > UOBJ_HAIRS_END )
				DestroyEquippedItem( mobile, item );
			endif
		endif
	endforeach

	return 1;

endfunction


function DestroyEquippedItem( byref equipped_on, byref item )

	if( GetCurseLevel( item ) )
		SetCurseLevel( item, SETTING_CURSE_LEVEL_REVEALED_CAN_UNEQUIP );
	endif
	
	var backpack := FindBackpack( equipped_on );
	if( !backpack )
		MoveItemToContainer( item, equipped_on.backpack );
	else
		MoveItemToContainer( item, backpack );
	endif
	
	return DestroyItem( item );

endfunction


function DestroyEquippedItemOnLayer( byref mobile, byref layer )

	var item := GetEquipmentByLayer( mobile, layer );
	if( !item )
		return 0;
	endif
	
	return DestroyEquippedItem( mobile, item );

endfunction


function FindBackpack( byref mobile )

	var backpack := mobile.backpack;
	if( !backpack )
		backpack := CreateItemAtLocation( 5283, 1184, 0, 0xe75, 1, "britannia" );
		if( backpack )
			if( !EquipItem( mobile, backpack ) )
				return mobile.backpack;
			endif
		endif
	endif
	
	return backpack;
	
endfunction