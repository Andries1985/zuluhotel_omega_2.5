use uo;

include "include/spelldata";
include "include/itemutil";
include "include/client";
include ":staff:include/staff";

const CLASSE_BONUS        	:= 1.5;
const CLASSEID_BARD       	:= "IsBard";
const CLASSEID_CRAFTER    	:= "IsCrafter";
const CLASSEID_MAGE       	:= "IsMage";
const CLASSEID_CLERIC     	:= "IsCleric";
const CLASSEID_RANGER     	:= "IsRanger";
const CLASSEID_THIEF      	:= "IsThief";
const CLASSEID_WARRIOR    	:= "IsWarrior";
const CLASSEID_POWERPLAYER	:= "IsPowerplayer";

const EACH_MUST_REPRESENT := 7.5;
const REPRESENT_LEVEL_MOD := 1.0;
const AVERAGE_SKILL       := 75;
const AVERAGE_LEVEL_MOD   := 15;
const BONUS_PER_LEVEL     := 0.25;
const SMALL_BONUS_PER_LEVEL := 0.15;

const THIEF_BACKSTAB_BONUS_DAMAGE := 10;
const THIEF_AMBUSH_BONUS_DAMAGE := 10;
const RANGER_LEVEL_ARCHERY_RANGE_BONUS := 1; 		//1 range per level further range before 1/4 base damage done

const CRAFTER_REPAIR_DIFF_BONUS := 2.5; // used in util/repair.inc


function ClasseBonus( who , classeid )

	var level := CInt( GetObjProperty( who , classeid ) );
	unequipRestrictedItems(who);
	if( level )
		return( 1.0 + BONUS_PER_LEVEL * level );
	else
		return( 1.0 );
	endif

endfunction


function ClasseBonusByLevel( level )

	return CDbl( 1.0 + ( BONUS_PER_LEVEL * CDbl(level) ) );

endfunction

function ClasseSmallBonusByLevel( level )

	return CDbl( 1.0 + ( SMALL_BONUS_PER_LEVEL * CDbl(level) ) );

endfunction


function ClasseBonusBySkillId( who, skillid )

	unequipRestrictedItems(who);
	return( ClasseBonusByLevel( IsSpecialisedIn( who , skillid ) ) );

endfunction


function ByPassForMages( classeid )

	var fract_array := {};
	var restricted_types := {};

	case( classeid )
		CLASSEID_MAGE:		fract_array.append( GetLeatherArmorGraphics() );
					break;

	endcase

	foreach type in fract_array
		foreach graphic in type
			restricted_types.append( graphic );
		endforeach
	endforeach

	return restricted_types;

endfunction


function EnumerateRestrictedItemTypesFromClasse( classeid )

	var fract_array := {};
	var restricted_types := {};

	case( classeid )
		CLASSEID_BARD:		
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;
		CLASSEID_CRAFTER:
					//fract_array.append( GetChainmailArmorGraphics() );
					//fract_array.append( GetPlatemailArmorGraphics() );
					break;
		CLASSEID_MAGE:		
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;
/* 		CLASSEID_CLERIC:
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break; */
		CLASSEID_RANGER:	
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;
		CLASSEID_THIEF:		
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					break;

		CLASSEID_WARRIOR:

		CLASSEID_POWERPLAYER:
	endcase

	foreach type in fract_array
		foreach graphic in type
			restricted_types.append( graphic );
		endforeach
	endforeach

	return restricted_types;

endfunction


function GetClasseIdForSkill( skillid )

	foreach id in GetClasseIds()
		if( skillid in GetClasseSkills( id ) )
			return id;
		endif
	endforeach

	return 0;

endfunction


function GetClasseIds()

	return { CLASSEID_BARD,
		 CLASSEID_CRAFTER,
		 CLASSEID_MAGE,
		// CLASSEID_CLERIC,
		 CLASSEID_RANGER,
		 CLASSEID_THIEF,
		 CLASSEID_WARRIOR,
		 CLASSEID_POWERPLAYER };

endfunction


function GetClasseName( classeid )

        case( classeid )

                CLASSEID_BARD:			return( "Bard" );
                CLASSEID_CRAFTER:		return( "Crafter" );
                CLASSEID_MAGE:			return( "Mage" );
				//CLASSEID_CLERIC:		return( "Cleric" );
                CLASSEID_RANGER:		return( "Ranger" );
                CLASSEID_THIEF:			return( "Thief" );
                CLASSEID_WARRIOR:		return( "Warrior" );
				CLASSEID_POWERPLAYER:	return( "Powerplayer" );
                default:		return( "INVALID" );
        endcase

endfunction


function GetClasseSkills( classeid )

	case( classeid )

		CLASSEID_BARD:			
				return { SKILLID_BEGGING	,
						 SKILLID_CARTOGRAPHY	,
						 SKILLID_ENTICEMENT	,
						 SKILLID_HERDING	,
						 SKILLID_MUSICIANSHIP	,
						 SKILLID_PEACEMAKING	,
						 SKILLID_PROVOCATION	,
						 SKILLID_TASTEID	};

		CLASSEID_CRAFTER:		
				return { SKILLID_ARMSLORE	,
						 SKILLID_BLACKSMITHY	,
						 SKILLID_BOWCRAFT	,
						 SKILLID_CARPENTRY	,
						 SKILLID_LUMBERJACKING	,
						 SKILLID_MINING		,
						 SKILLID_TAILORING	,
						 SKILLID_TINKERING	};

		CLASSEID_MAGE:			
				return { SKILLID_ALCHEMY	,
						 SKILLID_EVALINT	,
						 SKILLID_INSCRIPTION	,
						 SKILLID_ITEMID		,
						 SKILLID_MAGERY		,
						 SKILLID_MEDITATION	,
						 SKILLID_MAGICRESISTANCE,
						 SKILLID_SPIRITSPEAK	};

		CLASSEID_CLERIC:		
				return { SKILLID_ANIMALLORE	,
						 SKILLID_EVALINT	,
						 SKILLID_VETERINARY	,
						 SKILLID_HEALING	,
						 SKILLID_MAGERY		,
						 SKILLID_MEDITATION	,
						 SKILLID_MAGICRESISTANCE,
						 SKILLID_ANATOMY	};

		CLASSEID_RANGER:		
				return { SKILLID_ANIMALLORE	,
						 SKILLID_TAMING		,
						 SKILLID_ARCHERY	,
						 SKILLID_CAMPING	,
						 SKILLID_COOKING	,
						 SKILLID_FISHING	,
						 SKILLID_TRACKING	,
						 SKILLID_VETERINARY };

		CLASSEID_THIEF:			
				return { SKILLID_DETECTINGHIDDEN,
						 SKILLID_HIDING		,
						 SKILLID_LOCKPICKING	,
						 SKILLID_POISONING	,
						 SKILLID_REMOVETRAP	,
						 SKILLID_SNOOPING	,
						 SKILLID_STEALING	,
						 SKILLID_STEALTH	};

		CLASSEID_WARRIOR:		
				return { SKILLID_ANATOMY	,
						 SKILLID_FENCING	,
						 SKILLID_HEALING	,
						 SKILLID_MACEFIGHTING	,
						 SKILLID_PARRY		,
						 SKILLID_SWORDSMANSHIP	,
						 SKILLID_TACTICS	,
						 SKILLID_WRESTLING	};

		CLASSEID_POWERPLAYER:	
				return { SKILLID_ANATOMY			, //Warrior
						 SKILLID_FENCING			,
						 SKILLID_HEALING			,
						 SKILLID_MACEFIGHTING		,
						 SKILLID_PARRY				,
						 SKILLID_SWORDSMANSHIP		,
						 SKILLID_TACTICS			,
						 SKILLID_WRESTLING			,
						 SKILLID_DETECTINGHIDDEN	, //Thief
						 SKILLID_HIDING				,
						 SKILLID_LOCKPICKING		,
						 SKILLID_POISONING			,
						 SKILLID_REMOVETRAP			,
						 SKILLID_SNOOPING			,
						 SKILLID_STEALING			,
						 SKILLID_STEALTH			,
						 SKILLID_ANIMALLORE			, //Ranger
						 SKILLID_TAMING				,
						 SKILLID_ARCHERY			,
						 SKILLID_CAMPING			,
						 SKILLID_COOKING			,
						 SKILLID_FISHING			,
						 SKILLID_TRACKING			,
						 SKILLID_VETERINARY 		,
						 SKILLID_ALCHEMY			, //Mage
						 SKILLID_EVALINT			,
						 SKILLID_INSCRIPTION		,
						 SKILLID_ITEMID				,
						 SKILLID_MAGERY				,
						 SKILLID_MEDITATION			,
						 SKILLID_MAGICRESISTANCE 	,
						 SKILLID_SPIRITSPEAK	 	,
						 SKILLID_ARMSLORE			, //Crafter
						 SKILLID_BLACKSMITHY		,
						 SKILLID_BOWCRAFT			,
						 SKILLID_CARPENTRY			,
						 SKILLID_LUMBERJACKING		,
						 SKILLID_MINING				,
						 SKILLID_TAILORING			,
						 SKILLID_TINKERING			,
						 SKILLID_BEGGING			, //Bard
						 SKILLID_CARTOGRAPHY		,
						 SKILLID_ENTICEMENT			,
						 SKILLID_HERDING			,
						 SKILLID_MUSICIANSHIP		,
						 SKILLID_PEACEMAKING		,
						 SKILLID_PROVOCATION		,
						 SKILLID_TASTEID 			,
						 SKILLID_FORENSICS			}; //Misc.

		default:		return {};
	endcase

endfunction


function GetGeneralSkills()

	return { SKILLID_BEGGING,
		 SKILLID_ARMSLORE,
		 SKILLID_MAGICRESISTANCE,
		 SKILLID_ARCHERY,
		 SKILLID_HIDING,
		 SKILLID_TACTICS};

endfunction


function GetMaxCircle( who )

	foreach classeid in GetClasseIds()
		if( GetObjProperty( who , classeid ) )
			return GetMaxCircleForClasse( classeid );
		endif
	endforeach

	return 0;

endfunction


function GetMaxCircleForClasse( classeid )

	case( classeid )
		CLASSEID_BARD:			return 1;
		CLASSEID_CRAFTER:		return 1;
		CLASSEID_MAGE:			return 0;
		CLASSEID_CLERIC:		return 1;
		CLASSEID_RANGER:		return 5;
		CLASSEID_THIEF:			return 5;
		CLASSEID_WARRIOR:		return 1;
		CLASSEID_POWERPLAYER:	return 0;
		default:		return 0;
	endcase

endfunction


function GetSkillPointsMultiplier( who, skillid )

	return ClasseBonusByLevel( IsSpecialisedIn( who, skillid ) );

endfunction


function GetStatPointsMultiplier( who , stat )

	var level   := HaveStatAffinity( who , stat );
	var mult    := 1.0;

	if( level )
		mult  := ClasseBonusByLevel( level );
	else
		level := HaveStatDifficulty( who , stat );
		if( level )
			mult := CDbl( 1.0 / ClasseBonusByLevel( level ) );
		endif
	endif

	return mult;

endfunction


function HaveInvalidSkillEnchantmentForClasse( item , classeid )

	var ret	:= 0;
	var skillnum	:= GetObjProperty( item , "skilladv" );

	if( (skillnum) or (skillnum == 0) and !(skillnum in GetGeneralSkills()) )
		if( !(skillnum in GetClasseSkills( classeid )) )
			ret := 1;
		endif
	endif

	if( classeid == CLASSEID_RANGER && skillnum == SKILLID_FORENSICS )
		ret := 0;
	endif

	return ret;

endfunction


function HaveRestrictedModEnchantmentForClasse( who , item , classeid )

	var ret := 0;

	case( classeid )
		CLASSEID_BARD:			break;
		CLASSEID_CRAFTER:		break;
		CLASSEID_MAGE:			break;
		CLASSEID_CLERIC:		break;
		CLASSEID_RANGER:		break;
		CLASSEID_THIEF:			break;
		CLASSEID_WARRIOR:		break;
		CLASSEID_POWERPLAYER:	break;
	endcase

	//Warrior original restrictions
	/*if( GetObjProperty( item , "MagicImmunity" ) )
		ret := 1;
	elseif( GetObjProperty( item , "SpellReflect" ) )
		ret := 1;
	elseif( GetObjProperty( item , "PermMagicImmunity" ) )
		ret := 1;
	elseif( GetObjProperty( item , "Permmr" ) )
		ret := 1;
	endif
	break;
*/
	return ret;

endfunction


function HaveStatAffinity( who , stat )

	var classeids;

	case( stat )
		"str":	classeids := {  CLASSEID_CRAFTER,
					CLASSEID_WARRIOR };	break;

		"dex":	classeids := {  CLASSEID_BARD,
					CLASSEID_THIEF   };	break;

		"int":	classeids := {  CLASSEID_BARD,
					CLASSEID_MAGE,
					CLASSEID_CLERIC   };	break;

		default:	classeids := {};
	endcase

	foreach id in classeids
		var level := CInt( GetObjProperty( who , id ) );

		if( level )
			return level;
		endif
	endforeach

	return 0;

endfunction


function HaveStatDifficulty( who , stat )

	var classeids, level;

	case( stat )
		"str":
			classeids := {
				CLASSEID_BARD,
				CLASSEID_MAGE,
				CLASSEID_CLERIC
			}; break;
		"dex":
			classeids := {
				CLASSEID_MAGE
			};	break;
		"int":
			classeids := {
				CLASSEID_CRAFTER,
				CLASSEID_WARRIOR
			};	break;
		default:
			classeids := {
			};
	endcase

	foreach id in classeids
		level := CInt( GetObjProperty( who , id ) );
		if( level )
			return level;
		endif
	endforeach

	return 0;

endfunction


function IsSpecialisedIn( who, skillid )

	var classeid := GetClasseIdForSkill( skillid );
	if( classeid )
		return CInt( GetObjProperty( who, classeid ) );
	endif

endfunction


function IsBard( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_BARD ) );
endfunction

function IsCrafter( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_CRAFTER ) );
endfunction

function IsMage( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_MAGE ) );
endfunction

function IsCleric( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_CLERIC ) );
endfunction

function IsRanger( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_RANGER ) );
endfunction

function IsThief( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_THIEF ) );
endfunction

function IsWarrior( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_WARRIOR ) );
endfunction

function IsPowerplayer( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_POWERPLAYER ) );
endfunction


function IsFromThatClasse( who , classe_skills , ranger := 0)

	var classe	:= 0;
	var total	:= 0;
	var number := Len( classe_skills );
	var amount := 0;

	for i := 0 to SKILLID__HIGHEST
		if(ranger)
			if(i != SKILLID_FORENSICS)	//Ignores forensics from spec calculation
				amount := GetEffectiveSkill( who , i );
				total := total + amount;
			endif
		else
			amount := GetEffectiveSkill( who , i );
			total := total + amount;
		endif
		if( i in classe_skills )
			classe := classe + amount;
		endif
	endfor

	if( classe >= 3675 ) //3675 = lvl 1 PP threshold
		var level     := 1;
		var average_t := CInt( (AVERAGE_SKILL + AVERAGE_LEVEL_MOD) * number );

		while( ( classe >= average_t ) and ( level < 6 ) )
			level     := level + 1;
			average_t := CInt( average_t + AVERAGE_LEVEL_MOD * number );
		endwhile
		unequipRestrictedItems(who);
		return level;
	elseif ( classe < AVERAGE_SKILL * number )	
		return 0;
	elseif( classe < CInt(total * number * EACH_MUST_REPRESENT * 0.01) )
		return 0;
	else
		var level     := 1;
		var represent := EACH_MUST_REPRESENT + REPRESENT_LEVEL_MOD;
		var percent   := CInt( total * number * represent * 0.01 );
		var average_t := CInt( (AVERAGE_SKILL + AVERAGE_LEVEL_MOD) * number );

		while( (classe >= average_t) and (classe >= percent) )
			level     := level + 1;
			represent := CDbl( represent + REPRESENT_LEVEL_MOD );
			percent   := CInt( total * number * represent * 0.01 );
			average_t := CInt( average_t + AVERAGE_LEVEL_MOD * number );
		endwhile
		unequipRestrictedItems(who);
		return level;
	endif

endfunction


function IsProhibitedByClasse( who , item )

	var ret := 0;

	if( GetObjProperty( who , CLASSEID_BARD	) )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_BARD ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_BARD ) )
			ret := 1;
		endif
	elseif( GetObjProperty( who , CLASSEID_CRAFTER ) )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_CRAFTER ) )
			ret := 1;
		elseif( GetObjProperty( item , "str" ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_CRAFTER ) )
			ret := 1;
		endif
	elseif( GetObjProperty( who , CLASSEID_MAGE ) )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_MAGE ) )
			ret := 1;
		elseif( GetObjProperty( item , "str" ) )
			ret := 1;
		//elseif( GetObjProperty( item , "ArBonus" ) )
			//ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_MAGE ) )
			ret := 1;
		endif
	elseif( GetObjProperty( who , CLASSEID_CLERIC ) )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_CLERIC ) )
			ret := 1;
		elseif( GetObjProperty( item , "str" ) )
			ret := 1;
		elseif( GetObjProperty( item , "ArBonus" ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_CLERIC ) )
			ret := 1;
		endif
	elseif( GetObjProperty( who , CLASSEID_RANGER ) )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_RANGER ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_RANGER ) )
			ret := 1;
		endif
	elseif( GetObjProperty( who , CLASSEID_THIEF ) )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_THIEF ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_THIEF ) )
			ret := 1;
		endif
	elseif( GetObjProperty( who , CLASSEID_WARRIOR ) )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_WARRIOR ) )
			ret := 1;
		elseif( GetObjProperty( item , "int" ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_WARRIOR ) )
			ret := 1;
		elseif( HaveRestrictedModEnchantmentForClasse( who , item , CLASSEID_WARRIOR ) )
			ret := 1;
		endif
	endif

	return ret;

endfunction


function AssignClasse( character )

	var t;

	foreach classe in GetClasseIds()
		EraseObjProperty( character , classe );
		if(classe == "IsRanger")
			t := IsFromThatClasse( character , GetClasseSkills(classe), 1 );
		else
			t := IsFromThatClasse( character , GetClasseSkills( classe ) );
		endif
		if( t )
			SetObjProperty( character , classe , t );
			unequipRestrictedItems(character);
		endif
	endforeach

endfunction

// This function gets the weapons that certains classes can use. uses graphics
// It runs a seperate "canhit" script based on the players class level..
function ClassWeapons(classString)

	case (classString)
		"IsBard": return {0x0e81, 0x13B2, 0x13fd, 0x0f4f};
		"IsThief": return {0x0f4f, 0x13B2, 0x13fd, 0x1401, 0x1402, 0x0f63, 0x1404, 0x0f51, 0x0ec4, 0x0e87};
		"IsMage": return {0x0df0, 0x13f9, 0x0e8a};
		default:
	endcase

	return 0;

endfunction


function GetClass(who)

	foreach key in GetClasseIds()
		if (GetObjProperty(who, key))
			return key; // returns "Isranger" etc
		endif
	endforeach

	return 0; //no class found

endfunction


function GetClasseSkillTotal(who, aclass)

	var classtotal:=0;
	var skills:=GetClasseSkills(aclass);
	for i:= 0 to SKILLID__HIGHEST
		if (i in skills)
			classtotal:=classtotal+ CDbl(GetEffectiveSkillBaseValue(who, i)) / 10;
		endif
	endfor

return classtotal;

endfunction

// Returns class level (regardless of class)
function GetClasseLevel(who)

	var characterClass := GetClass(who);
	if (characterClass)
		return GetObjProperty(who, characterClass);
	else
		return 0;
	endif

endfunction


function unequipRestrictedItems(who)

	var IsRPer := GetObjProperty( who, "IsRPer" );

	if(!who.cmdlevel)
		if(!IsRPer)
			foreach item in ListEquippedItems( who )
				var isRperItem := GetObjProperty(item, "IsRPer");
				
				SleepMS(5);

				if(isRperItem)
					MoveItemToContainer( item, who.backpack);
					SendSysMessage(who, "You are carrying items intended for [RPer]'s. If we see you with an item equipped that you shouldn't have, you will be jailed without questions.", FONT_NORMAL, 2595 );
					// Broadcast to staff
					var msg := "Warning: " + who.name + " was carrying a RPer flagged item: " + item.name + " with serial: " + item.serial + " illegally.";
					foreach staff in EnumerateOnlineCharacters()
						if( staff.cmdlevel >= 1 )
							SendSysMessage( staff, msg, 3, 2581 );
						endif
					endforeach
				endif

				SleepMS(5);

				if(IsProhibitedByClasse( who , item ))
					MoveItemToContainer( item, who.backpack);
					SendSysMessage(who, "Illegal items for your class found equipped on you, if we see you with an item equipped that you shouldn't have, you will be jailed without questions.", FONT_NORMAL, 2595);
					LogCommand(who, GetProcess(GetPid()).name, 0, item.desc+"/"+item.serial);
				endif
			endforeach
		else
			foreach item in ListEquippedItems( who )

				SleepMS(5);

				if(IsProhibitedByClasse( who , item ))
					MoveItemToContainer( item, who.backpack);
					SendSysMessage(who, "Illegal items for your class found equipped on you, if we see you with an item equipped that you shouldn't have, you will be jailed without questions.", FONT_NORMAL, 2595);
				endif
			endforeach
		endif
	endif

	return;

endfunction
