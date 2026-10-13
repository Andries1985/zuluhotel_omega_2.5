use uo;

//include "include/spelldata";
include "include/itemutil";
include "include/client";
include ":staff:include/staff";

const CLASSE_BONUS        		:= 1.5;
const CLASSEID_BARD       		:= "IsBard";
const CLASSEID_BLADESINGER 		:= "IsBladesinger";
const CLASSEID_CRAFTER    		:= "IsCrafter";
const CLASSEID_MAGE       		:= "IsMage";
const CLASSEID_MYSTIC_ARCHER	:= "IsMysticArcher";
const CLASSEID_PALADIN     		:= "IsPaladin";
const CLASSEID_RANGER     		:= "IsRanger";
const CLASSEID_THIEF      		:= "IsThief";
const CLASSEID_WARRIOR    		:= "IsWarrior";
const CLASSEID_POWERPLAYER		:= "IsPowerplayer";

const EACH_MUST_REPRESENT := 7.5;
const REPRESENT_LEVEL_MOD := 1.0;
const AVERAGE_SKILL       := 75;
const AVERAGE_LEVEL_MOD   := 15;
const BONUS_PER_LEVEL     := 0.25;
const SMALL_BONUS_PER_LEVEL := 0.15;
const HALF_BONUS_PER_LEVEL := 0.125;

const THIEF_BACKSTAB_BONUS_DAMAGE := 10;
const THIEF_AMBUSH_BONUS_DAMAGE := 10;
const RANGER_LEVEL_ARCHERY_RANGE_BONUS := 1; 		//1 range per level further range before 1/4 base damage done

const CRAFTER_REPAIR_DIFF_BONUS := 2.5; // used in util/repair.inc


function ClasseBonus( who , classeid )

	// Patch 1.1.4: the illegal-equipment sweep that ran here on every call (every hit, every
	// damaging spell) is gone; it runs where class levels are recomputed (AssignClasse).
	var level := CInt( GetObjProperty( who , classeid ) );
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

// Powerplayer skill-gain multiplier, applied to every skill (skillpoints.inc). Kept close to the
// pre-1.1.4 table (1.1 / 1.2 / 1.3 at levels 3-5) but one level earlier and one step longer.
function PowerplayerGainMultiplier( level )

	case( CInt(level) )
		2: return 1.1;
		3: return 1.2;
		4: return 1.3;
		5: return 1.4;
		6: return 1.5;
	endcase
	return 1.0;

endfunction

function ClasseHalfBonusByLevel( level )

	return CDbl( 1.0 + ( HALF_BONUS_PER_LEVEL * CDbl(level) ) );

endfunction


function ClasseBonusBySkillId( who, skillid )

	return( ClasseBonusByLevel( IsSpecialisedIn( who , skillid ) ) );

endfunction


function ByPassForMages( classeid )

	var fract_array := {};
	var restricted_types := {};

	case( classeid )
		CLASSEID_MAGE:		
					fract_array.append( GetLeatherArmorGraphics() );
					fract_array.append( GetStuddedLeatherArmorGraphics() );
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
					fract_array.append( GetGraphicsCrafter() );
					break;
		CLASSEID_BLADESINGER:
					fract_array.append( GetLeatherArmorGraphics() );
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetGraphicsBladesinger() );
					fract_array.append( GetShieldGraphics() );
					fract_array.append( GetGraphicsCrafter() );
					break;
		CLASSEID_CRAFTER:
					break;
		CLASSEID_MAGE:
					fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					fract_array.append( GetGraphicsCrafter() );
					break;
		CLASSEID_MYSTIC_ARCHER:		
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					fract_array.append( GetGraphicsCrafter() );
					break;
 		CLASSEID_PALADIN:
					fract_array.append( GetLeatherArmorGraphics() );
					fract_array.append( GetStuddedLeatherArmorGraphics() );
					fract_array.append( GetBoneArmorGraphics() );
					fract_array.append( GetRingmailArmorGraphics() );
					fract_array.append( GetGraphicsCrafter() );
					fract_array.append( GetGraphicsPaladin() );
					break;
		CLASSEID_RANGER:
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphics() );
					fract_array.append( GetGraphicsCrafter() );
					break;
		CLASSEID_THIEF:		
					fract_array.append( GetChainmailArmorGraphics() );
					fract_array.append( GetPlatemailArmorGraphics() );
					fract_array.append( GetShieldGraphicsThief() );
					fract_array.append( GetGraphicsCrafter() );
					break;
		CLASSEID_WARRIOR:
					fract_array.append( GetGraphicsCrafter() );
					break;
		CLASSEID_POWERPLAYER:
					fract_array.append( GetGraphicsCrafter() );
					break;
	endcase

	foreach type in fract_array
		foreach graphic in type
			restricted_types.append( graphic );
		endforeach
	endforeach

	return restricted_types;

endfunction


function GetClasseIdForSkill( who, skillid )

	// Patch 1.1.4: look at the classes the CHARACTER holds, not at the first class that lists the
	// skill. The old lookup took only the skill and returned the first class in GetClasseIds()
	// order whose list held it, so a Warrior's Swordsmanship resolved to Bladesinger (level 0) and
	// got no class skill-gain bonus or failed-roll second chance; only Healing and Wrestling
	// counted. Paladins lost six of eight skills, Powerplayers 48 of 49. Of the classes the
	// character has a level in, the one with the highest level that lists the skill wins.
	// A Powerplayer is deliberately left out (returns 0): its bonus is the skill-wide small curve
	// in skillpoints.inc, with no second roll.
	var best := 0;
	var bestlevel := 0;

	foreach id in GetClasseIds()
		if( id == CLASSEID_POWERPLAYER )
			continue;
		endif
		var level := CInt( GetObjProperty( who, id ) );
		if( level > bestlevel )
			if( skillid in GetClasseSkills( id ) )
				best := id;
				bestlevel := level;
			endif
		endif
	endforeach

	return best;

endfunction


function GetClasseIds()

	return { 
			CLASSEID_BARD,
			CLASSEID_BLADESINGER,
			CLASSEID_CRAFTER,
			CLASSEID_MAGE,
			CLASSEID_MYSTIC_ARCHER,
			CLASSEID_PALADIN,
			CLASSEID_RANGER,
			CLASSEID_THIEF,
			CLASSEID_WARRIOR,
			CLASSEID_POWERPLAYER
			};

endfunction


function GetClasseName( classeid )

        case( classeid )

                CLASSEID_BARD:			return( "Bard" );
				CLASSEID_BLADESINGER:	return( "Bladesinger" );
                CLASSEID_CRAFTER:		return( "Crafter" );
                CLASSEID_MAGE:			return( "Mage" );
				CLASSEID_MYSTIC_ARCHER:	return( "Mystic Archer" );
				CLASSEID_PALADIN:		return( "Paladin" );
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

		CLASSEID_BLADESINGER:			
				return { SKILLID_ANATOMY	,
						 SKILLID_ENTICEMENT	,
						 SKILLID_FENCING	,
						 SKILLID_MUSICIANSHIP	,
						 SKILLID_PEACEMAKING	,
						 SKILLID_PROVOCATION	,
						 SKILLID_SWORDSMANSHIP	,
						 SKILLID_TACTICS	};

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

		CLASSEID_MYSTIC_ARCHER:			
				return { SKILLID_ARCHERY	,
						 SKILLID_COOKING	,
						 SKILLID_EVALINT	,
						 SKILLID_FISHING	,
						 SKILLID_TRACKING	,
						 SKILLID_MAGERY		,
						 SKILLID_MAGICRESISTANCE,
						 SKILLID_MEDITATION	};

		CLASSEID_PALADIN:
				return { SKILLID_MACEFIGHTING, 
						 SKILLID_PARRY, 
						 SKILLID_SWORDSMANSHIP,
						 SKILLID_TACTICS ,
						 SKILLID_MAGERY	,
						 SKILLID_EVALINT ,
						 SKILLID_MEDITATION	,
						 SKILLID_MAGICRESISTANCE};

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
		CLASSEID_BLADESINGER:	return 1;
		CLASSEID_CRAFTER:		return 7;
		CLASSEID_MAGE:			return 0;
		CLASSEID_MYSTIC_ARCHER:	return 0;
		CLASSEID_PALADIN:		return 0;
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

	// Patch 1.1.4: wired into AwardPoints() in skillpoints.inc for every class. A character whose
	// classes disagree on a stat follows the higher-level one; a tie favours the affinity.
	var affinity   := HaveStatAffinity( who , stat );
	var difficulty := HaveStatDifficulty( who , stat );

	if( affinity and affinity >= difficulty )
		return ClasseBonusByLevel( affinity );
	elseif( difficulty )
		return CDbl( 1.0 / ClasseBonusByLevel( difficulty ) );
	endif

	return 1.0;

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

	if( classeid == CLASSEID_THIEF && skillnum == SKILLID_DETECTINGHIDDEN )
		ret := 0;
	endif

	if( classeid == CLASSEID_CRAFTER && skillnum == SKILLID_MAGERY )
		ret := 0;
	endif

	return ret;

endfunction


function HaveRestrictedModEnchantmentForClasse( unused who , unused item , classeid )

	var ret := 0;

	case( classeid )
		CLASSEID_BARD:			break;
		CLASSEID_BLADESINGER:	break;
		CLASSEID_CRAFTER:		break;
		CLASSEID_MAGE:			break;
		CLASSEID_MYSTIC_ARCHER:	break;
		CLASSEID_PALADIN:		break;
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


// The highest level "who" holds in any of classeids, 0 if none. Replaces the first-match walk
// the two stat tables used, which let GetClasseIds() order decide for a dual-class character.
function HighestHeldClasseLevel( who, classeids )

	var best := 0;
	foreach id in classeids
		var level := CInt( GetObjProperty( who, id ) );
		if( level > best )
			best := level;
		endif
	endforeach

	return best;

endfunction


// Patch 1.1.4: stat affinity (grows faster). Table as decided 2026-10-07: Ranger gains Dexterity,
// Thief keeps Dexterity, Mystic Archer gains Intelligence as well as Dexterity.
function HaveStatAffinity( who , stat )

	var classeids;

	case( stat )
		"str":	classeids := { CLASSEID_CRAFTER, CLASSEID_WARRIOR };
				break;
		"dex":	classeids := { CLASSEID_BARD, CLASSEID_BLADESINGER, CLASSEID_THIEF, CLASSEID_MYSTIC_ARCHER, CLASSEID_RANGER };
				break;
		"int":	classeids := { CLASSEID_BARD, CLASSEID_MAGE, CLASSEID_PALADIN, CLASSEID_MYSTIC_ARCHER };
				break;
		default:	classeids := {};
	endcase

	return HighestHeldClasseLevel( who, classeids );

endfunction


// Stat difficulty (grows slower). Ranger and Thief are slow on Strength, Thief also on Intelligence.
function HaveStatDifficulty( who , stat )

	var classeids;

	case( stat )
		"str":	classeids := { CLASSEID_BARD, CLASSEID_BLADESINGER, CLASSEID_MAGE, CLASSEID_MYSTIC_ARCHER, CLASSEID_RANGER, CLASSEID_THIEF };
				break;
		"dex":	classeids := { CLASSEID_MAGE, CLASSEID_PALADIN };
				break;
		"int":	classeids := { CLASSEID_CRAFTER, CLASSEID_WARRIOR, CLASSEID_THIEF };
				break;
		default:	classeids := {};
	endcase

	return HighestHeldClasseLevel( who, classeids );

endfunction


function IsSpecialisedIn( who, skillid )

	var classeid := GetClasseIdForSkill( who, skillid );
	if( classeid )
		return CInt( GetObjProperty( who, classeid ) );
	endif

	return 0; // explicit, so "no class for this skill" can never read as a truthy default

endfunction


function IsBard( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_BARD ) );
endfunction

function IsBladesinger( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_BLADESINGER ) );
endfunction

function IsCrafter( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_CRAFTER ));
endfunction

function IsMage( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_MAGE ) );
endfunction

function IsMysticArcher( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_MYSTIC_ARCHER ) );
endfunction

function IsPaladin( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_PALADIN ) );
endfunction

function IsRanger( who )
	return IsFromThatClasse( who , GetClasseSkills( CLASSEID_RANGER ), 1);
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


function IsFromThatClasse( who , classe_skills , forensicsallowed := 0 )

	var classe	:= 0;
	var total	:= 0;
	var number := Len( classe_skills );
	var amount := 0;

	for i := 0 to SKILLID__HIGHEST
		if(forensicsallowed  == 1)
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
		return level;
	endif

endfunction


function IsProhibitedByClasse( who , item )

	var ret := 0;
	// Patch 1.1.4: the rules of the class the character primarily is (highest level), not of the
	// first held class in list order.
	var primary := GetPrimaryClasseId( who );

	if( primary == CLASSEID_BARD )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_BARD ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_BARD ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_BARD ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_BLADESINGER )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_BLADESINGER ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_BLADESINGER ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_BLADESINGER ) )
			ret := 1;
		elseif( GetObjProperty( item , "int" ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_CRAFTER )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_CRAFTER ) )
			ret := 1;
		//elseif( GetObjProperty( item , "str" ) )
			//ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_CRAFTER ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_CRAFTER ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_MAGE )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_MAGE ) )
			ret := 1;
		elseif( GetObjProperty( item , "str" ) )
			ret := 1;
		elseif( GetObjProperty( item , "dex" ) )
			ret := 1;
		//elseif( GetObjProperty( item , "ArBonus" ) )
			//ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_MAGE ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_MAGE ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_MYSTIC_ARCHER )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_MYSTIC_ARCHER ) )
			ret := 1;
		elseif( GetObjProperty( item , "str" ) )
			ret := 1;
		//elseif( GetObjProperty( item , "ArBonus" ) )
			//ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_MYSTIC_ARCHER ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_MYSTIC_ARCHER ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_PALADIN )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_PALADIN ) )
			ret := 1;
		elseif( GetObjProperty( item , "dex" ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_PALADIN ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_PALADIN ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_RANGER )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_RANGER ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_RANGER ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_RANGER ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_THIEF )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_THIEF ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_THIEF ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_THIEF ) )
			ret := 1;
		elseif( GetObjProperty( item , "int" ) )
			ret := 1;
		endif
	elseif( primary == CLASSEID_WARRIOR )
		if( HaveInvalidSkillEnchantmentForClasse( item , CLASSEID_WARRIOR ) )
			ret := 1;
		elseif( GetObjProperty( item , "int" ) )
			ret := 1;
		elseif( item.graphic in EnumerateRestrictedItemTypesFromClasse( CLASSEID_WARRIOR ) )
			ret := 1;
		elseif( item.objtype in EnumerateRestrictedItemTypesFromClasse( CLASSEID_WARRIOR ) )
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
		endif
	endforeach

	// Patch 1.1.4: the one place illegal equipment is swept. Class levels only change here, so a
	// single pass after every class has been stamped covers it; the bonus functions and the Is*
	// predicates used to sweep on every call, which cost every hit and spell 100-200 ms.
	unequipRestrictedItems(character);

endfunction

// This function gets the weapons that certains classes can use. uses graphics
// It runs a seperate "canhit" script based on the players class level..
function ClassWeapons(classString)

	case (classString)
		//Shepherds Crook, bow, heavy crossbow, crossbow, Yumi, Composite Bow, Repeating Crossbow, Lajatang
		"IsBard": return {0x0e81, 0x13B2, 0x13fd, 0x0f4f, 0x27a5, 0x26c2, 0x26c3, 0x27a7};
		//crossbow, bow, heavy crossbow, Yumi, Sai, Composite Bow, Repeating Crossbow, Kryss, Short Spear, Spear, Warfork, Dagger, Skinning Knife, Pitchfork
		"IsThief": return {0x0f4f, 0x13B2, 0x13fd, 0x27a5, 0x27af, 0x26c2, 0x26c3, 0x1401, 0x1402, 0x0f63, 0x1404, 0x0f51, 0x0ec4, 0x0e87};
		//Black Staff, Gnarled Staff, QuarterStaff, Lajatang
		"IsMage": return {0x0df0, 0x13f9, 0x0e8a, 0x27a7};
		default:
	endcase

	return 0;

endfunction


// Patch 1.1.4: the class a character "is" for every level-based rule: the held class with the
// highest level, ties going to GetClasseIds() order (so a single-class character is unchanged).
// GetClass() used to return the first held class in list order, which judged the one dual-class
// character on the shard (Ranger 3 / Mystic Archer 1) as a level 1 Mystic Archer everywhere,
// item rules included. Only sibling classes sharing four skills can be held together.
function GetPrimaryClasseId( who )

	var best := 0;
	var bestlevel := 0;

	foreach key in GetClasseIds()
		var level := CInt( GetObjProperty( who, key ) );
		if( level > bestlevel )
			best := key;
			bestlevel := level;
		endif
	endforeach

	return best;

endfunction


function GetClass(who)

	return GetPrimaryClasseId( who ); // returns "IsRanger" etc, 0 when no class

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
