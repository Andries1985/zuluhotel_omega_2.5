use uo;
use os;

include "include/client";
include "include/namingbyenchant";
include "include/skillpoints";
include "include/itemutil";

function SelectItemToID( who, delay )

	var the_item;
	delay := 4;
	var skillID;
	var intelligence := GetAttribute( who, ATTRIBUTEID_INTELLIGENCE );
	const MIN_DELAYMS := 250;
	const MIN_SKILLFORBATCH := 80;
	const MIN_INTFORBATCH := 100;

	if ( GetObjProperty( who , CLASSEID_BARD ) )
		skillID := GetEffectiveSkill( who, SKILLID_TASTEID );
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_WARRIOR ) )
		skillID := GetEffectiveSkill( who, SKILLID_ANATOMY );
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_THIEF ) )
		skillID := GetEffectiveSkill( who, SKILLID_REMOVETRAP );
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_CRAFTER ) )
		skillID := GetEffectiveSkill( who, SKILLID_ARMSLORE );
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_RANGER ) )
		skillID := GetEffectiveSkill( who, SKILLID_CAMPING );
		if( skillID >= 100)
			delay := 0;
		endif
	elseif ( GetObjProperty( who , CLASSEID_POWERPLAYER) || GetObjProperty( who , CLASSEID_MAGE ) )
		skillID := GetEffectiveSkill( who, SKILLID_ITEMID );
		if( skillID >= 100)
			delay := 0;
		endif
	else
		skillID := GetEffectiveSkill( who, SKILLID_ITEMID );
		if( skillID >= 100)
			delay := 0;
		endif
	endif

	SendSysMessage( who, "Select an object to identify." );
	the_item := Target( who, TGTOPT_CHECK_LOS );
	if( !the_item )
		return 0;
	endif
		
	if( !Accessible( who , the_item ) )
		SendSysMessage( who , "You can't reach that." );
		return 1;
	endif
	
	if( Distance( who , the_item ) > 2 )
		SendSysMessage( who , "That is too far." );
		return 1;
	endif
		
	if( !the_item.IsA(POLCLASS_ITEM) )
		SendSysMessage( who , "That's not an item!" );
		return 1;
	endif

	if (the_item.isa(POLCLASS_CONTAINER))
	 	if ((skillID >= MIN_SKILLFORBATCH) && (intelligence >= MIN_INTFORBATCH))
			var itemlist := {};
			foreach thing in EnumerateItemsInContainer(the_item)
				if (GetObjProperty(thing, "UnIDedMagicalItem"))
					itemlist.append(thing);
				endif
			endforeach
			if (itemlist.Size() == 0)
				SendSysMessage(who, "There's nothing to identify in there.");
				return 1;
			else
				SendSysMessage(who,"Your vast intellect tries to uncover the mysteries of your hoard.", FONT_NORMAL, 2601 );
				SendSysMessage(who,"There's "+itemlist.Size()+" magical items to identify.", FONT_NORMAL, 2601 );
				foreach item in itemlist
					if( !Accessible( who , item ) || Distance( who , the_item ) > 2)
						SendSysMessage( who , "You can't reach that." );
						return 1;
					endif
					ItemID( who, item);
					sleepms( Max( delay * 1000, MIN_DELAYMS ) );
				endforeach
			endif
			return 1;
		else
			SendSysMessage(who, "You cannot do that yet.");
			return 1;
		endif
	endif
	
	if( !the_item.movable )
		SendSysMessage( who, "You can't id unmovable items." );
		return 1;
	endif

	var ided := CanBeIDed(the_item);
	var hasdetails := GetItemDetails(who, the_item);
	if(hasdetails && ided)
		return 1;
	endif

	if( ided )
		SendSysMessage( who, the_item.desc+" has already been identified.", FONT_NORMAL, 2595 );
		return 1;
	endif

	ItemID( who, the_item );

endfunction

function ItemID( who, the_item )

	var thepoints := get_default_points( SKILLID_ITEMID );
	var ided := GetObjProperty( the_item , "IDed" );

	if( !ided )		
		if( !IsIDable( the_item ) )
			SendSysmessage( who , "This is a normal item." );
			SetObjProperty( the_item , "IDed" , 1 );
			return 1;
		elseif( !CheckSkill( who , SKILLID_ITEMID , -2 , thepoints ) )
			SendSysmessage (who, "I'm not sure . . .");
			return 1;
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
		GetItemDetails(who, the_item);			
	else
		SendSysmessage( who , "This is a normal item." );
	endif
	
	SetObjProperty( the_item , "IDed" , 1 );
	
	return 1;
		
endfunction


function GetItemDetails(who, the_item)

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


function CanBeIDed(the_item)

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