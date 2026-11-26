use uo;
use polsys;

include "include/client";
include "include/classes";
include "include/itemutil";

const TEXT_COLOR 		:= 319;
const TYPE_PENALTY 		:= 1;

function repair_item( character, item, skillid )

	if(!IsInContainer( character.backpack , item ))
		SendSysmessage( character, "That item has to be in your backpack.", FONT_NORMAL, 2595);
		return 0;
	endif

	//Check if the item is damaged
	if( item.hp == item.maxhp )
		SendSysMessage( character, "That item is in perfect condition already.", FONT_NORMAL, 2595);
		return 0;
	endif

	//The item should be broken already
	if((item.hp < 2) || (item.hp > item.maxhp ) || (item.quality <= 0.1) || (!item.quality))
		SendSysMessage(character, "The brittle material breaks when you handle it!");
		DestroyTheItem( character, item );
		return 0;
	endif

	var to_repair	:= item.maxhp - item.hp;
	var armslore 	:= GetEffectiveSkill( character, SKILLID_ARMSLORE );
	//Check if you have a chance to repair it
	if( armslore < Min(to_repair,150))
		SendSysMessage( character, "You don't know enough about Arms to repair such extensive damage!", FONT_NORMAL, 2595);
		return 0;
	endif

	SendSysMessage( character, "Select the material to use for repairs.", FONT_NORMAL, TEXT_COLOR);
	var material := Target( character , TGTOPT_CHECK_LOS );
	if( !material )
		SendSysmessage( character, "Canceled", FONT_NORMAL, 2595);
		return 0;
	elseif( !Accessible( character , material ) )
		SendSysMessage( character , "You can't reach that.", FONT_NORMAL, 2595);
		return 0;
	elseif( !IsInContainer( character.backpack , material ) )
		SendSysmessage( character, "That item has to be in your backpack.", FONT_NORMAL, 2595);
		return 0;
	endif
	var material_cfg;
	case (skillid)
		SKILLID_CARPENTRY:
			if(!IsLog(material))
				SendSysmessage( character, "You need logs to repair wooden items.", FONT_NORMAL, 2595);
				return 0;
			endif
			material_cfg := ReadConfigFile( ":carpentry:carpentry" );
		SKILLID_BLACKSMITHY:
			if(!IsIngot(material))
				SendSysmessage( character, "You need ingots to repair metal items.", FONT_NORMAL, 2595);
				return 0;
			endif
			material_cfg := ReadConfigFile( ":blacksmithy:blacksmithy" );
		SKILLID_TAILORING:
			if(!IsHide(material.objtype))
				SendSysmessage( character, "You need hides to repair tailored items.", FONT_NORMAL, 2595);
				return 0;
			endif
			material_cfg := ReadConfigFile( ":tailoring:tailoring" );
		SKILLID_BOWCRAFT:
			if(!IsLog(material))
				SendSysmessage( character, "You need logs to repair wooden items.", FONT_NORMAL, 2595);
				return 0;
			endif
			material_cfg := ReadConfigFile( ":carpentry:carpentry" );
	endcase
	// Begin changes to make repairing easier.
	var diff := to_repair + 20;
	var crafter_level := IsCrafter(character);
	diff := CInt(diff - crafter_level*CRAFTER_REPAIR_DIFF_BONUS);
	
	if (diff < 0)
		diff := 0;
	elseif (diff > 160)
		diff := 160;
	endif
	
	var item_quality := item.quality;
	if (GetObjProperty(item, "IsGMItem") || GetObjProperty(item, "Artifact"))
		item_quality += TYPE_PENALTY;
	endif

	//get material quality from 
	var material_quality := material_cfg[material.objtype].Quality;
	
	//Efficency based on qualities and crafter level
	var repair_efficiency := (10 + crafter_level) * (pow(material_quality,2)/pow(item_quality,2));
	//Consumption of material based on quality and damage
	//At least one piece: rounding down made repairs smaller than one piece's worth free.
	var to_consume := Max( 1, Cint(to_repair / repair_efficiency) );

	//Check if he repair it
	if( CheckSkill( character, skillid, diff, 0 ) )
		if ( to_consume >= material.amount)
			//Capped: hp above maxhp makes the next repair attempt destroy the item.
			item.hp := Min( item.maxhp, item.hp + CInt(material.amount * repair_efficiency) );
			DestroyItem(material);
			SendSysMessage( character, "Your use up all the material.", FONT_NORMAL, TEXT_COLOR );
		else
			item.hp := item.maxhp;
			SubtractAmount(material, to_consume);
		endif
		SendSysMessage( character, "Repair complete!", FONT_NORMAL, TEXT_COLOR );
	//You fail to repair it	
	else
		//destroy material based on skill and crafter level
		var destroy := 75 - GetEffectiveSkill( character , skillid )/2;
		destroy /= ClasseBonus( character , CLASSEID_CRAFTER );
		
		destroy := Min( destroy, 50);
		var to_destroy := CInt( to_consume * destroy/100 );
		SubtractAmount(material, to_destroy);
		
		//Damage item by another 30%, reduce 5% per crafter level
		item.hp := CInt(item.hp * (1-0.05*(6-crafter_level)));
		if( item.hp < 1 )
			SendSysMessage( character, "You fail and destroy the item", FONT_NORMAL, 2595 );
			DestroyTheItem( character, item );
		else
			SendSysMessage( character, "You fail in your repairing attempt and damage the item!" , FONT_NORMAL, 2595);
		endif
	endif
	IncRevision(item);
endfunction