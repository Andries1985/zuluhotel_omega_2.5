//Edited August 25, 2024

/************************************************************************************************************
**
** ATTRIBUTES.INC
**
** This file contains most of the possibly useful functions and constants relative
** to the core 094 attributes system.
**
** INSTALLATION: You might need to change SKILLID_BATTLE_DEFENSE to SKILLID_PARRY in
**               client.inc in order to have attributes.inc to compile well.
**               You can also want to move the SKILLID_* constants from client.inc
**               to this file. in this case you should remove the include "include/client";
**               statement from this file and instead add include "include/attributes"; to your
**               client.inc
**
** CREATING NEW SKILLS PROCEDURE:
**	1) Add the new skillid to the specified CFGFILESPEC_SKILLSDEF.
**	2) Add the new attribute to your attributes.cfg.
**	3) Create a new SKILLID_ constant to fit the given skillid.
**	4) Create a new ATTRIBUTEID_ constant to fit the given attribute name.
**	5) Add a line to GetAttributeIdBySkillId( skillid ) function like:
**		newSKILLID_: return newATTRIBUTEID_;
**	6) Add a line to GetSkillIdByAttributeId( attributeid ) function like:
**		newATTRIBUTEID_: return newSKILLID_;
**	7) Add the new SKILLID_ to GetSkillIds() function.
**	8) Add the new ATTRIBUTEID_ to GetAttributeIds() function.
**	9) Be happy :)
**
************************************************************************************************************/

// Needed module files
use uo;
use os;
use math;
use util;
use cfgfile;
use attributes;
use vitals;

// Include client.inc to get the SKILLID_* constants
include "include/client";
include "include/areas";
include "include/classes";


/**************************************************************************************************
** CONSTANTS
**************************************************************************************************/

// This is the path to the custom skills.cfg
// This file work exactly like a normal skills.cfg except that skillids can go above 48.
const CFGFILESPEC_SKILLSDEF		:= ":shilhook:skillsdef";

// The core minimum and maximum value for an attribute in base value
const CORE_SETTINGS_MIN_ATTRIBUTE	:= 0;
const CORE_SETTINGS_MAX_ATTRIBUTE	:= 60000;

// The core minimum and maximum value for a vital in hundrenth
const CORE_SETTINGS_MIN_VITAL		:= 100;
const CORE_SETTINGS_MAX_VITAL		:= 10000000;

// The core minimum and maximum for regen rates
const CORE_SETTINGS_MIN_REGEN_RATE	:= -30000;
const CORE_SETTINGS_MAX_REGEN_RATE	:= 30000;

// CProp name prefix for regen rate changes
const CPROP_NAME_PREFIX_REGEN_RATE	:= "NewRegenRateFor";

// The attributeid constants

const ATTRIBUTEID_ALCHEMY 		:= "Alchemy";
const ATTRIBUTEID_ANATOMY		:= "Anatomy";
const ATTRIBUTEID_ANIMALLORE		:= "Animallore";
const ATTRIBUTEID_ARCHERY		:= "Archery";
const ATTRIBUTEID_ARMSLORE		:= "Armslore";
const ATTRIBUTEID_BEGGING		:= "Begging";
const ATTRIBUTEID_BLACKSMITHY		:= "Blacksmithy";
const ATTRIBUTEID_BOWCRAFT		:= "Bowcraft";
const ATTRIBUTEID_CAMPING		:= "Camping";
const ATTRIBUTEID_CARPENTRY		:= "Carpentry";
const ATTRIBUTEID_CARTOGRAPHY		:= "Cartography";
const ATTRIBUTEID_COOKING		:= "Cooking";
const ATTRIBUTEID_DETECTINGHIDDEN	:= "Detectinghidden";
const ATTRIBUTEID_ENTICEMENT		:= "Enticement";
const ATTRIBUTEID_EVALINT		:= "Evaluatingintelligence";
const ATTRIBUTEID_FENCING		:= "Fencing";
const ATTRIBUTEID_FISHING		:= "Fishing";
const ATTRIBUTEID_FORENSICS		:= "Forensicevaluation";
const ATTRIBUTEID_HEALING		:= "Healing";
const ATTRIBUTEID_HERDING		:= "Herding";
const ATTRIBUTEID_HIDING		:= "Hiding";
const ATTRIBUTEID_INSCRIPTION		:= "Inscription";
const ATTRIBUTEID_ITEMID		:= "Itemid";
const ATTRIBUTEID_LOCKPICKING		:= "Lockpicking";
const ATTRIBUTEID_LUMBERJACKING		:= "Lumberjacking";
const ATTRIBUTEID_MACEFIGHTING		:= "Macefighting";
const ATTRIBUTEID_MAGERY		:= "Magery";
const ATTRIBUTEID_MAGICRESISTANCE	:= "Magicresistance";
const ATTRIBUTEID_MEDITATION		:= "Meditation";
const ATTRIBUTEID_MINING		:= "Mining";
const ATTRIBUTEID_MUSICIANSHIP		:= "Musicianship";
const ATTRIBUTEID_PARRY			:= "Parry";
const ATTRIBUTEID_PEACEMAKING		:= "Peacemaking";
const ATTRIBUTEID_POISONING		:= "Poisoning";
const ATTRIBUTEID_PROVOCATION		:= "Provocation";
const ATTRIBUTEID_REMOVETRAP		:= "Removetrap";
const ATTRIBUTEID_SNOOPING		:= "Snooping";
const ATTRIBUTEID_SPIRITSPEAK		:= "Spiritspeak";
const ATTRIBUTEID_STEALING		:= "Stealing";
const ATTRIBUTEID_STEALTH		:= "Stealth";
const ATTRIBUTEID_SWORDSMANSHIP		:= "Swordsmanship";
const ATTRIBUTEID_TACTICS		:= "Tactics";
const ATTRIBUTEID_TAILORING		:= "Tailoring";
const ATTRIBUTEID_TAMING		:= "Animaltaming";
const ATTRIBUTEID_TASTEID		:= "Tasteidentification";
const ATTRIBUTEID_TINKERING		:= "Tinkering";
const ATTRIBUTEID_TRACKING		:= "Tracking";
const ATTRIBUTEID_VETERINARY		:= "Veterinary";
const ATTRIBUTEID_WRESTLING		:= "Wrestling";

// Attribute ids for stats
const ATTRIBUTEID_DEXTERITY		:= "dexterity";
const ATTRIBUTEID_INTELLIGENCE		:= "intelligence";
const ATTRIBUTEID_STRENGTH		:= "strength";

// Main vital ids constants
const VITALID_LIFE			:= "life";
const VITALID_MANA			:= "mana";
const VITALID_STAMINA			:= "stamina";

Const SKILLSTATE := "SkillStates";
Const SKILLID_MIN := 1;
Const SKILLID_MAX := 49;
Const SKILLSTATE_NORMAL := "R";
Const SKILLSTATE_LOCKED := "L";
Const SKILLSTATE_LOWER  := "D";
Const SKILLGAIN_MULTI	:=  1;
Const SKILL_STAT_CAP	:= 130;

//skill/stat caps.
const BASE_CAP 				:= 1300;

/**************************************************************************************************
** USERS FUNCTIONS
**************************************************************************************************/


/***********************************************************************************
**
** function AlterDexterityMod( who, delta )
**
** Usage:
**	Alter the dexterity temporary mod of the mobile by the specified delta.
**	Equivalent to the old who.dexterity_mod := who.dexterity_mod + delta;
**
** Parameters:
**	who:   The mobile. (Mobileref).
**	delta: The change in the mod. (integer)
**
** return:
**	0/1 if the mod could be changed or not
**
***********************************************************************************/
function AlterDexterityMod( who, delta )

	var ret := AlterAttributeTemporaryMod(who, ATTRIBUTEID_DEXTERITY, Cint(delta*10));
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function AlterIntelligenceMod( who, delta )
**
** Usage:
**	Alter the intelligence temporary mod of the mobile by the specified delta.
**	Equivalent to the old who.intelligence_mod := who.intelligence_mod + delta;
**
** Parameters:
**	who:   The mobile. (Mobileref).
**	delta: The change in the mod. (integer)
**
** return:
**	0/1 if the mod could be changed or not
**
***********************************************************************************/
function AlterIntelligenceMod( who, delta )

	var ret := AlterAttributeTemporaryMod(who, ATTRIBUTEID_INTELLIGENCE, Cint(delta*10));
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function AlterSkillTemporaryMod( who, skillid, delta )
**
** Usage:
**	Alter the skill temporary mod of the mobile by the specified delta.
**	not in TENTH!
**
** Parameters:
**	who:     The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**	delta:   The change in the mod. (integer)
**
** return:
**	0/1 if the mod could be changed or not
**
***********************************************************************************/
function AlterSkillTemporaryMod( who, skillid, delta )

	return AlterAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid), Cint(delta*10));

endfunction


/***********************************************************************************
**
** function AlterSkillTemporaryModBaseValue( who, skillid, delta_tenth )
**
** Usage:
**	Alter the skill temporary mod of the mobile by the specified delta in tenth.
**
** Parameters:
**	who:     The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**	delta:   The change in the mod. (integer)
**
** return:
**	0/1 if the mod could be changed or not
**
***********************************************************************************/
function AlterSkillTemporaryModBaseValue( who, skillid, delta_tenth )

	return AlterAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid), delta_tenth);
	
endfunction


/***********************************************************************************
**
** function AlterStrengthMod( who, delta )
**
** Usage:
**	Alter the strength temporary mod of the mobile by the specified delta.
**	Equivalent to the old who.strength_mod := who.strength_mod + delta;
**
** Parameters:
**	who:   The mobile. (Mobileref).
**	delta: The change in the mod. (integer)
**
** return:
**	0/1 if the mod could be changed or not
**
***********************************************************************************/
function AlterStrengthMod( who, delta )

	var ret := AlterAttributeTemporaryMod(who, ATTRIBUTEID_STRENGTH, Cint(delta*10));
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function AwardPoints( who, skillid, skillpoints )
**
** Usage:
**	Awards the given raw skillpoints in the skillid and the stats
**	increase specified into CFGFILESPEC_SKILLSDEF for this skillid.
**
** Parameters:
**	who:	     The mobile to whom the skillpoints are awarded. (Mobileref)
**	skillid:     The skillid to add the points to. (integer)
**	skillpoints: The amount of raw skill points to be awarded. (integer)
**
** return:
**	0/1		Weither if the points could be awarded or not.
**
***********************************************************************************/
function AwardPoints( who, skillid, skillpoints )

	if( !who.isA(POLCLASS_MOBILE) )
		var ret := error;
		ret.+errortext := "Invalid parameter type: " + who + " isn't a mobileref.";
		return ret;
	elseif( Cint(skillid) == error )
		var ret := error;
		ret.+errortext := "Invalid parameter type: " + skillid + " isn't an integer.";
		return ret;
	endif
	
	// Make sure that skillid is in its integer form
	skillid := Cint(skillid);
	
	// Find the config element of the skillid.
	var elem := GetSkillConfigElem(skillid);

	// If don't exists then return an error and don't awards the points.
	if( !elem )
		return elem;
	endif
	
	// Award the raw skill points.
	
	AwardRawSkillPoints(who, skillid, skillpoints);

	// Check the strength advancement
	var adv := elem.StrAdv;
	if( adv )
		adv := SplitWords(adv);
		if( RandomInt(1000) < Cdbl(adv[1])*10 )
			AwardRawAttributePoints(who, ATTRIBUTEID_STRENGTH, RandomDiceRoll(adv[2]));
		endif
	endif
	
	// Check the dexterity advancement
	adv := elem.DexAdv;
	if( adv )
		adv := SplitWords(adv);
		if( RandomInt(1000) < Cdbl(adv[1])*10 )
			AwardRawAttributePoints(who, ATTRIBUTEID_DEXTERITY, RandomDiceRoll(adv[2]));
		endif
	endif
	
	// Check the intelligence advancement
	adv := elem.IntAdv;
	if( adv )
		adv := SplitWords(adv);
		if( RandomInt(1000) < Cdbl(adv[1])*10 )
			AwardRawAttributePoints(who, ATTRIBUTEID_INTELLIGENCE, RandomDiceRoll(adv[2]));
		endif
	endif
	
	return 1;

endfunction


/***********************************************************************************
**
** function AwardRawAttributePoints( who, attributeid, rawpoints )
**
** Usage:
**	Awards the given raw points in the attributeid.
**
** Parameters:
**	who:	     The mobile to whom the skillpoints are awarded. (Mobileref)
**	attributeid: The attributeid to add the points to. (String)
**	rawpoints:   The amount of raw skill points to be awarded. (integer)
**
** return:
**	0/1		Weither if the points could be awarded or not.
**
***********************************************************************************/
function AwardRawAttributePoints( who, attributeid, rawpoints )
	
	if( !who.isA(POLCLASS_MOBILE) )
		var ret := error;
		ret.+errortext := "Invalid parameter type: " + who + " isn't a mobileref.";
		return ret;
	endif
	
	rawpoints := Cdbl(rawpoints);
	// The points parameter isn't numerical so return an error.
	if( rawpoints == error )
		return rawpoints;
	// No points need to be awarded so don't bother about going any further.
	elseif( rawpoints == 0.0 )
		return 1;
	endif
	
	set_critical(1);
	var initial := GetAttributeBaseValue(who, attributeid);
	var current := initial;

	// Awards a positive amount of raw points.
	if( rawpoints > 0 )
		var to_increase	:= GetNeededRawPointsToIncrease(current);
		while( rawpoints >= to_increase )
			rawpoints	:= rawpoints - to_increase;
			current		:= current + 1;
			to_increase	:= GetNeededRawPointsToIncrease(current);
		endwhile
		if( RandomInt(Cint(to_increase)) < rawpoints )
			current := current + 1;
		endif

	// Awards a negative amount of raw points.
	elseif( rawpoints < 0 )
		rawpoints := 0 - rawpoints;
		var to_decrease	:= GetNeededRawPointsToIncrease(current-1);
		while( rawpoints >= to_decrease && current > 0 )
			rawpoints	:= rawpoints - to_decrease;
			current		:= current - 1;
			to_decrease	:= GetNeededRawPointsToIncrease(current-1);
		endwhile

		if( RandomInt(Cint(to_decrease)) < rawpoints )
			current := current - 1;
		endif
		
		// Make sure the skill never get into the negative values
		if( current < CORE_SETTINGS_MIN_ATTRIBUTE )
			current := CORE_SETTINGS_MIN_ATTRIBUTE;
		endif
	endif
	
	// Alter the skill only if the new skill is different than the initial one.
	if( current != initial )
		if( current > CORE_SETTINGS_MAX_ATTRIBUTE )
			current := CORE_SETTINGS_MAX_ATTRIBUTE;
		endif
		
		if(attributeid != ATTRIBUTEID_STRENGTH && attributeid != ATTRIBUTEID_DEXTERITY && attributeid != ATTRIBUTEID_INTELLIGENCE)
		var matrix := GetObjProperty(who, "pScrollMatrix");
		var iSkill := GetSkillIdByAttributeId(attributeid);
			if(!who.cmdlevel && matrix)
				var pscrollSkillCap := BASE_CAP+(matrix[iSkill]*10);
				if(iSkill == 0)
					pscrollSkillCap := BASE_CAP+(matrix[49]*10);
				endif
				if(pscrollSkillCap > 1500);
					pscrollSkillCap := 1500;
				endif
				if(current > pscrollSkillCap)
					current := pscrollSkillCap;
				endif
			elseif(!who.cmdlevel && (current>BASE_CAP))
				current := BASE_CAP;
			endif
		endif
		
		//Check for stat caps
		var statMatrix := GetObjProperty(who, "statMatrix");
		if(attributeid == ATTRIBUTEID_STRENGTH || attributeid == ATTRIBUTEID_DEXTERITY || attributeid == ATTRIBUTEID_INTELLIGENCE)
			if(!who.cmdlevel && statMatrix)
				var statPotCap := 0;
				if(attributeid == ATTRIBUTEID_STRENGTH)
					statPotCap := BASE_CAP+(statMatrix[1]*10);
				elseif(attributeid == ATTRIBUTEID_DEXTERITY)
					statPotCap := BASE_CAP+(statMatrix[2]*10);
				elseif(attributeid == ATTRIBUTEID_INTELLIGENCE)
					statPotCap := BASE_CAP+(statMatrix[3]*10);
				endif
				if(current > statPotCap)
					current := statPotCap;
				endif
			elseif(!who.cmdlevel && (current>BASE_CAP))
				current := BASE_CAP;
			endif
		endif


		SetAttributeBaseValue(who, attributeid, current);
		// Recalc the vitals if the attribute is a vital related one.
		case( attributeid )
			ATTRIBUTEID_STRENGTH:
			ATTRIBUTEID_DEXTERITY:
			ATTRIBUTEID_INTELLIGENCE:	RecalcVitalsIfOnline(who);
		endcase
	endif

	set_critical(0);
	return 1;

endfunction


/***********************************************************************************
**
** function AwardRawSkillPoints( who, skillid, rawpoints )
**
** Usage:
**	Awards the given raw points in the skillid.
**
** Parameters:
**	who:	   The mobile to whom the skillpoints are awarded. (Mobileref)
**	skillid:   The skillid to add the points to. (integer)
**	rawpoints: The amount of raw skill points to be awarded. (integer)
**
** return:
**	0/1		Weither if the points could be awarded or not.
**
***********************************************************************************/
function AwardRawSkillPoints( who, skillid, rawpoints )

var skill := GetAttributeIdBySkillId(skillid);

	if(GetObjProperty (who,"IsMage"))
		var classweps := ClassWeapons("IsMage");
		var wepfound := 0;
		foreach wep in classweps
			if(who.weapon.graphic == wep)
				wepfound := 1;
			endif
		endforeach

		if(wepfound)
			if(skill == who.weapon.attribute || skill == ATTRIBUTEID_TACTICS)
				rawpoints := 0;
			endif
		endif
	elseif(GetObjProperty (who,"IsThief"))
		var classweps := ClassWeapons("IsThief");
		var wepfound := 0;
		foreach wep in classweps
			if(who.weapon.graphic == wep)
				wepfound := 1;
			endif
		endforeach

		if(wepfound)
			if(skill == who.weapon.attribute || skill == ATTRIBUTEID_TACTICS)
				rawpoints := 0;
			endif
		endif
	elseif(GetObjProperty (who,"IsBard"))
		var classweps := ClassWeapons("IsBard");
		var wepfound := 0;
		foreach wep in classweps
			if(who.weapon.graphic == wep)
				wepfound := 1;
			endif
		endforeach

		if(wepfound)
			if(skill == who.weapon.attribute || skill == ATTRIBUTEID_TACTICS)
				rawpoints := 0;
			endif
		endif
	endif

	if (IsInSafeArea(who))
		if (skill == ATTRIBUTEID_ARCHERY or skill == ATTRIBUTEID_PARRY or skill == ATTRIBUTEID_MACEFIGHTING or skill == ATTRIBUTEID_SWORDSMANSHIP or skill == ATTRIBUTEID_FENCING or skill == ATTRIBUTEID_WRESTLING or skill == ATTRIBUTEID_TACTICS or skill == ATTRIBUTEID_MAGICRESISTANCE)
			rawpoints := 0;
		elseif (skill == ATTRIBUTEID_MACEFIGHTING + ATTRIBUTEID_HERDING)
			return;
		endif
	endif
	
	//Ocllo Extention 
	if( who.x >= 3562 )
		if( who.x <= 3793 )
			if( who.y >= 2410 )
				if( who.y <= 2729 )
					if (skill == ATTRIBUTEID_ARCHERY or skill == ATTRIBUTEID_PARRY or skill == ATTRIBUTEID_MACEFIGHTING or skill == ATTRIBUTEID_SWORDSMANSHIP or skill == ATTRIBUTEID_FENCING or skill == ATTRIBUTEID_WRESTLING or skill == ATTRIBUTEID_TACTICS or skill == ATTRIBUTEID_MAGICRESISTANCE)
					rawpoints := 0;
					endif
				endif
			endif
		endif
	endif

	if( who.x >= 5272 )
		if( who.x <= 5310 )
			if( who.y >= 1160 )
				if( who.y <= 1190 )
					rawpoints := 0;
				endif
			endif
		endif
	endif

	return AwardRawAttributePoints(who, GetAttributeIdBySkillId(skillid), rawpoints);

endfunction


/***********************************************************************************
**
** function GetAttributeIdBySkillId( skillid )
**
** Usage:
**	Get the attibuteid equivalent to the specified skillid.
**
** Parameters:
**	skillid: The skillid to get the equivalent from. (integer)
**
** return:
**	The equivalent attributeid if one was found or an
**	error {errortext := "No equivalent attributeid found for skillid "} if not
**
***********************************************************************************/
function GetAttributeIdBySkillId( skillid )

	case( skillid )
	SKILLID_ALCHEMY:		return ATTRIBUTEID_ALCHEMY;
	SKILLID_ANATOMY:		return ATTRIBUTEID_ANATOMY;
	SKILLID_ANIMALLORE:		return ATTRIBUTEID_ANIMALLORE;
	SKILLID_ITEMID:			return ATTRIBUTEID_ITEMID;
	SKILLID_ARMSLORE:		return ATTRIBUTEID_ARMSLORE;
	SKILLID_PARRY:			return ATTRIBUTEID_PARRY;
	SKILLID_BEGGING:		return ATTRIBUTEID_BEGGING;
	SKILLID_BLACKSMITHY:		return ATTRIBUTEID_BLACKSMITHY;
	SKILLID_BOWCRAFT:		return ATTRIBUTEID_BOWCRAFT;
	SKILLID_PEACEMAKING:		return ATTRIBUTEID_PEACEMAKING;
	SKILLID_CAMPING:		return ATTRIBUTEID_CAMPING;
	SKILLID_CARPENTRY:		return ATTRIBUTEID_CARPENTRY;
	SKILLID_CARTOGRAPHY:		return ATTRIBUTEID_CARTOGRAPHY;
	SKILLID_COOKING:		return ATTRIBUTEID_COOKING;
	SKILLID_DETECTINGHIDDEN:	return ATTRIBUTEID_DETECTINGHIDDEN;
	SKILLID_ENTICEMENT:		return ATTRIBUTEID_ENTICEMENT;
	SKILLID_EVALINT:		return ATTRIBUTEID_EVALINT;
	SKILLID_HEALING:		return ATTRIBUTEID_HEALING;
	SKILLID_FISHING:		return ATTRIBUTEID_FISHING;
	SKILLID_FORENSICS:		return ATTRIBUTEID_FORENSICS;
	SKILLID_HERDING:		return ATTRIBUTEID_HERDING;
	SKILLID_HIDING:			return ATTRIBUTEID_HIDING;
	SKILLID_PROVOCATION:		return ATTRIBUTEID_PROVOCATION;
	SKILLID_INSCRIPTION:		return ATTRIBUTEID_INSCRIPTION;
	SKILLID_LOCKPICKING:		return ATTRIBUTEID_LOCKPICKING;
	SKILLID_MAGERY:			return ATTRIBUTEID_MAGERY;
	SKILLID_MAGICRESISTANCE:	return ATTRIBUTEID_MAGICRESISTANCE;
	SKILLID_TACTICS:		return ATTRIBUTEID_TACTICS;
	SKILLID_SNOOPING:		return ATTRIBUTEID_SNOOPING;
	SKILLID_MUSICIANSHIP:		return ATTRIBUTEID_MUSICIANSHIP;
	SKILLID_POISONING:		return ATTRIBUTEID_POISONING;
	SKILLID_ARCHERY:		return ATTRIBUTEID_ARCHERY;
	SKILLID_SPIRITSPEAK:		return ATTRIBUTEID_SPIRITSPEAK;
	SKILLID_STEALING:		return ATTRIBUTEID_STEALING;
	SKILLID_TAILORING:		return ATTRIBUTEID_TAILORING;
	SKILLID_TAMING:			return ATTRIBUTEID_TAMING;
	SKILLID_TASTEID:		return ATTRIBUTEID_TASTEID;
	SKILLID_TINKERING:		return ATTRIBUTEID_TINKERING;
	SKILLID_TRACKING:		return ATTRIBUTEID_TRACKING;
	SKILLID_VETERINARY:		return ATTRIBUTEID_VETERINARY;
	SKILLID_SWORDSMANSHIP:		return ATTRIBUTEID_SWORDSMANSHIP;
	SKILLID_MACEFIGHTING:		return ATTRIBUTEID_MACEFIGHTING;
	SKILLID_FENCING:		return ATTRIBUTEID_FENCING;
	SKILLID_WRESTLING:		return ATTRIBUTEID_WRESTLING;
	SKILLID_LUMBERJACKING:		return ATTRIBUTEID_LUMBERJACKING;
	SKILLID_MINING:			return ATTRIBUTEID_MINING;
	SKILLID_MEDITATION:		return ATTRIBUTEID_MEDITATION;
	SKILLID_STEALTH:		return ATTRIBUTEID_STEALTH;
	SKILLID_REMOVETRAP:		return ATTRIBUTEID_REMOVETRAP;
	default:	var ret        := error;
				ret.+errortext := "No equivalent attributeid found for skillid " + skillid;
				return ret;
	endcase
		
endfunction

            
/***********************************************************************************
**
** function GetAttributeIds()
**
** Usage:
**	Get all the known attributeids.
**
** Parameters:
**	None.
**
** return:
**	An array of all known attributeids.
**
***********************************************************************************/
function GetAttributeIds()

	return { ATTRIBUTEID_ALCHEMY,
		 ATTRIBUTEID_ANATOMY,
		 ATTRIBUTEID_ANIMALLORE,
		 ATTRIBUTEID_ARCHERY,
		 ATTRIBUTEID_ARMSLORE,
		 ATTRIBUTEID_BEGGING,
		 ATTRIBUTEID_BLACKSMITHY,
		 ATTRIBUTEID_BOWCRAFT,
		 ATTRIBUTEID_CAMPING,
		 ATTRIBUTEID_CARPENTRY,
		 ATTRIBUTEID_CARTOGRAPHY,
		 ATTRIBUTEID_COOKING,
		 ATTRIBUTEID_DETECTINGHIDDEN,
		 ATTRIBUTEID_DEXTERITY,
		 ATTRIBUTEID_ENTICEMENT,
		 ATTRIBUTEID_EVALINT,
		 ATTRIBUTEID_FENCING,
		 ATTRIBUTEID_FISHING,
		 ATTRIBUTEID_FORENSICS,
		 ATTRIBUTEID_HEALING,
		 ATTRIBUTEID_HERDING,
		 ATTRIBUTEID_HIDING,
		 ATTRIBUTEID_INSCRIPTION,
		 ATTRIBUTEID_INTELLIGENCE,
		 ATTRIBUTEID_ITEMID,
		 ATTRIBUTEID_LOCKPICKING,
		 ATTRIBUTEID_LUMBERJACKING,
		 ATTRIBUTEID_MACEFIGHTING,
		 ATTRIBUTEID_MAGERY,
		 ATTRIBUTEID_MAGICRESISTANCE,
		 ATTRIBUTEID_MEDITATION,
		 ATTRIBUTEID_MINING,
		 ATTRIBUTEID_MUSICIANSHIP,
		 ATTRIBUTEID_PARRY,
		 ATTRIBUTEID_PEACEMAKING,
		 ATTRIBUTEID_POISONING,
		 ATTRIBUTEID_PROVOCATION,
		 ATTRIBUTEID_REMOVETRAP,
		 ATTRIBUTEID_SNOOPING,
		 ATTRIBUTEID_SPIRITSPEAK,
		 ATTRIBUTEID_STEALING,
		 ATTRIBUTEID_STEALTH,
		 ATTRIBUTEID_STRENGTH,
		 ATTRIBUTEID_SWORDSMANSHIP,
		 ATTRIBUTEID_TACTICS,
		 ATTRIBUTEID_TAILORING,
		 ATTRIBUTEID_TAMING,
		 ATTRIBUTEID_TASTEID,
		 ATTRIBUTEID_TINKERING,
		 ATTRIBUTEID_TRACKING,
		 ATTRIBUTEID_VETERINARY,
		 ATTRIBUTEID_WRESTLING };

endfunction

function GetAttributeNoStrIntDexIds()

	return { ATTRIBUTEID_ALCHEMY,
		 ATTRIBUTEID_ANATOMY,
		 ATTRIBUTEID_ANIMALLORE,
		 ATTRIBUTEID_ARCHERY,
		 ATTRIBUTEID_ARMSLORE,
		 ATTRIBUTEID_BEGGING,
		 ATTRIBUTEID_BLACKSMITHY,
		 ATTRIBUTEID_BOWCRAFT,
		 ATTRIBUTEID_CAMPING,
		 ATTRIBUTEID_CARPENTRY,
		 ATTRIBUTEID_CARTOGRAPHY,
		 ATTRIBUTEID_COOKING,
		 ATTRIBUTEID_DETECTINGHIDDEN,
		 ATTRIBUTEID_ENTICEMENT,
		 ATTRIBUTEID_EVALINT,
		 ATTRIBUTEID_FENCING,
		 ATTRIBUTEID_FISHING,
		 ATTRIBUTEID_FORENSICS,
		 ATTRIBUTEID_HEALING,
		 ATTRIBUTEID_HERDING,
		 ATTRIBUTEID_HIDING,
		 ATTRIBUTEID_INSCRIPTION,
		 ATTRIBUTEID_ITEMID,
		 ATTRIBUTEID_LOCKPICKING,
		 ATTRIBUTEID_LUMBERJACKING,
		 ATTRIBUTEID_MACEFIGHTING,
		 ATTRIBUTEID_MAGERY,
		 ATTRIBUTEID_MAGICRESISTANCE,
		 ATTRIBUTEID_MEDITATION,
		 ATTRIBUTEID_MINING,
		 ATTRIBUTEID_MUSICIANSHIP,
		 ATTRIBUTEID_PARRY,
		 ATTRIBUTEID_PEACEMAKING,
		 ATTRIBUTEID_POISONING,
		 ATTRIBUTEID_PROVOCATION,
		 ATTRIBUTEID_REMOVETRAP,
		 ATTRIBUTEID_SNOOPING,
		 ATTRIBUTEID_SPIRITSPEAK,
		 ATTRIBUTEID_STEALING,
		 ATTRIBUTEID_STEALTH,
		 ATTRIBUTEID_SWORDSMANSHIP,
		 ATTRIBUTEID_TACTICS,
		 ATTRIBUTEID_TAILORING,
		 ATTRIBUTEID_TAMING,
		 ATTRIBUTEID_TASTEID,
		 ATTRIBUTEID_TINKERING,
		 ATTRIBUTEID_TRACKING,
		 ATTRIBUTEID_VETERINARY,
		 ATTRIBUTEID_WRESTLING };

endfunction


/***********************************************************************************
**
** function GetBaseDexterity( who )
**
** Usage:
**	Get the base dexterity of the mobile (without mods)
**
** Parameters:
**	who: The mobile to get the dexterity from. (Mobileref)
**
** return:
**	The base dexterity of the mobile. (integer ranging [0, 6000])
**
***********************************************************************************/
function GetBaseDexterity( who )

	return Cint(GetAttributeBaseValue(who, ATTRIBUTEID_DEXTERITY) / 10);

endfunction


/***********************************************************************************
**
** function GetBaseIntelligence( who )
**
** Usage:
**	Get the base intelligence of the mobile (without mods)
**
** Parameters:
**	who: The mobile to get the intelligence from. (Mobileref)
**
** return:
**	The base intelligence of the mobile. (integer ranging [0, 6000])
**
***********************************************************************************/
function GetBaseIntelligence( who )

	return Cint(GetAttributeBaseValue(who, ATTRIBUTEID_INTELLIGENCE) / 10);

endfunction


/***********************************************************************************
**
** function GetBaseStrength( who, skillid )
**
** Usage:
**	Get the base skill of the mobile (without mods)
**
** Parameters:
**	who:	 The mobile to get the skill from. (Mobileref)
**	skillid: The skillid
**
** return:
**	The base level of the mobile in the skillid. (integer)
**	Ranging from 0 to 6000
**
***********************************************************************************/
function GetBaseSkill( who, skillid )

	return Cint(GetBaseSkillBaseValue(who, skillid) / 10);

endfunction


/***********************************************************************************
**
** function GetBaseStrength( who, skillid )
**
** Usage:
**	Get the base skill in tenth of the mobile (without mods)
**
** Parameters:
**	who:     The mobile to get the skill from. (Mobileref)
**	skillid: The skillid
**
** return:
**	The base level in tenth of the mobile in the skillid. (integer)
**	Ranging from 0 to 60000
**
***********************************************************************************/
function GetBaseSkillBaseValue( who, skillid )

	return GetAttributeBaseValue(who, GetAttributeIdBySkillId(skillid));

endfunction


/***********************************************************************************
**
** function GetBaseStrength( who )
**
** Usage:
**	Get the base strength of the mobile (without mods)
**
** Parameters:
**	who: The mobile to get the strength from. (Mobileref)
**
** return:
**	The base strength of the mobile. (integer ranging [0, 6000])
**
***********************************************************************************/
function GetBaseStrength( who )

	return Cint(GetAttributeBaseValue(who, ATTRIBUTEID_STRENGTH) / 10);

endfunction


/***********************************************************************************
**
** function GetDefaultPointsValue( skillid )
**
** Usage:
**	Get the default raw points value awarded on success use of a skill
**	as defined into CFGFILESPEC_SKILLSDEF
**
** Parameters:
**	skillid: The skillid (integer)
**
** return:
**	The default raw points value or 0 if none found. (integer)
**
***********************************************************************************/
function GetDefaultPointsValue( skillid )

        var elem := GetSkillConfigElem(skillid);
	if( !elem )
		return 0;
	else
		return Cint(elem.DefaultPoints);
	endif

endfunction


/***********************************************************************************
**
** function GetDexterity( who )
**
** Usage:
**	Get the dexterity of a mobile (including mods). Equivalent to the old
**	mobile.dexterity
**
** Parameters:
**	who: The mobile to get the dexterity from. (Mobileref)
**
** return:
**	The modified dexterity of the mobile. (integer ranging [0, 6000])
**
***********************************************************************************/
function GetDexterity( who )

	return GetAttribute(who, ATTRIBUTEID_DEXTERITY);

endfunction


/***********************************************************************************
**
** function GetDexterityMod( who )
**
** Usage:
**	Get the dexterity temporary mod of a mobile. Equivalent to the old
**	mobile.dexterity_mod
**
** Parameters:
**	who: The mobile to get the dexterity temporary mod from. (Mobileref)
**
** return:
**	The dexterity temporary mod of the mobile. (integer ranging [0, 6000])
**
***********************************************************************************/
function GetDexterityMod( who )

	return Cint(GetAttributeTemporaryMod(who, "Dexterity") / 10);

endfunction


/***********************************************************************************
**
** function GetEffectiveSkill( who, skillid )
**
** Usage:
**	Get the effective skill of a mobile. Exact equivalent of the old
**	GetSkill(who, skillid). This include the mods.
**
** Parameters:
**	who:	 The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The effective skill of the mobilein skillid. (integer ranging [0, 18000])
**
***********************************************************************************/
function GetEffectiveSkill( who, skillid )

	return GetAttribute(who, GetAttributeIdBySkillId(skillid));

endfunction


/***********************************************************************************
**
** function GetEffectiveSkill( who, skillid )
**
** Usage:
**	Get the effective skill in tenth of a mobile. This include the mods.
**
** Parameters:
**	who:	 The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The effective skill in tenth of the mobilein skillid.
**	(integer ranging [0, 180000])
**
***********************************************************************************/
function GetEffectiveSkillBaseValue( who, skillid )

	var attributeid	:= GetAttributeIdBySkillId(skillid);
	var baseskill	:= GetAttributeBaseValue(who, attributeid);
	var tempmod	:= GetAttributeTemporaryMod(who, attributeid);
	var intinsicmod	:= GetAttributeIntrinsicMod(who, attributeid);
	var effective	:= Cint(baseskill + tempmod + intinsicmod);
	
	if( effective < 0 )
		return 0;
	else
		return effective;
	endif

endfunction


/***********************************************************************************
**
** function GetHp( who )
**
** Usage:
**	Get the current hp level of a mobile. Equivalent to the old mobile.hp
**	without assignment.
**
** Parameters:
**	who: The mobile to get the hp level from. (Mobileref)
**
** return:
**	The current hp of the mobile. (integer ranging [0, 100000])
**
***********************************************************************************/
function GetHp( who )

	return Cint(GetVital(who, VITALID_LIFE) / 100);

endfunction


/***********************************************************************************
**
** function GetHpRegenRate( who )
**
** Usage:
**	Get the current hp regen rate of a mobile. Equivalent to the old
**	who.hp_regen_rate without assignment
**
** Parameters:
**	who: The mobile to get the hp regen rate from. (Mobileref)
**
** return:
**	The current hp regen rate of the mobile. (integer ranging [-30000, 30000])
**	in hundrenth per minute.
**
***********************************************************************************/
function GetHpRegenRate( who )

	return GetVitalRegenRate(who, VITALID_LIFE);

endfunction


/***********************************************************************************
**
** function GetIntelligence( who )
**
** Usage:
**	Get the intelligence of a mobile (including mods). Equivalent to the old
**	mobile.intelligence
**
** Parameters:
**	who: The mobile to get the intelligence from. (Mobileref)
**
** return:
**	The modified intelligence of the mobile. (integer ranging [0, 18000])
**
***********************************************************************************/
function GetIntelligence( who )

	return GetAttribute(who, ATTRIBUTEID_INTELLIGENCE);

endfunction


/***********************************************************************************
**
** function GetIntelligenceMod( who )
**
** Usage:
**	Get the intelligence temporary mod of a mobile. Equivalent to the old
**	mobile.intelligence_mod
**
** Parameters:
**	who: The mobile to get the intelligence temporary mod from. (Mobileref)
**
** return:
**	The intelligence temporary mod of the mobile. (integer ranging [0, 6000])
**
***********************************************************************************/
function GetIntelligenceMod( who )

	return Cint(GetAttributeTemporaryMod(who, "Intelligence") / 10);

endfunction


/***********************************************************************************
**
** function GetMana( who )
**
** Usage:
**	Get the current mana level of a mobile. Equivalent to the old mobile.mana
**	without assignment.
**
** Parameters:
**	who: The mobile to get the mana level from. (Mobileref)
**
** return:
**	The current mana of the mobile. (integer ranging [0, 100000])
**
***********************************************************************************/
function GetMana( who )

	return Cint(GetVital(who, VITALID_MANA) / 100);

endfunction


/***********************************************************************************
**
** function GetHpRegenRate( who )
**
** Usage:
**	Get the current mana regen rate of a mobile. Equivalent to the old
**	who.mana_regen_rate without assignment
**
** Parameters:
**	who: The mobile to get the mana regen rate from. (Mobileref)
**
** return:
**	The current mana regen rate of the mobile. (integer ranging [-30000, 30000])
**	in hundrenth per minute.
**
***********************************************************************************/
function GetManaRegenRate( who )

	return GetVitalRegenRate(who, VITALID_MANA);

endfunction


/***********************************************************************************
**
** function GetMaxHp( who )
**
** Usage:
**	Get the maximum hp level of a mobile. Equivalent to the old mobile.maxhp.
**
** Parameters:
**	who: The mobile to get the maximum hp level from. (Mobileref)
**
** return:
**	The maximum hp of the mobile. (integer ranging [1, 100000])
**
***********************************************************************************/
function GetMaxHp( who )

	return Cint(GetVitalMaximumValue(who, VITALID_LIFE) / 100);

endfunction


/***********************************************************************************
**
** function GetMaxMana( who )
**
** Usage:
**	Get the maximum mana level of a mobile. Equivalent to the old mobile.maxmana.
**
** Parameters:
**	who: The mobile to get the maximum mana level from. (Mobileref)
**
** return:
**	The maximum mana of the mobile. (integer ranging [1, 100000])
**
***********************************************************************************/
function GetMaxMana( who )

	return Cint(GetVitalMaximumValue(who, VITALID_MANA) / 100);

endfunction


/***********************************************************************************
**
** function GetMaxStamina( who )
**
** Usage:
**	Get the maximum stamina level of a mobile. Equivalent to the old
**	mobile.maxstamina.
**
** Parameters:
**	who: The mobile to get the maximum stamina level from. (Mobileref)
**
** return:
**	The maximum stamina of the mobile. (integer ranging [1, 100000])
**
***********************************************************************************/
function GetMaxStamina( who )

	return Cint(GetVitalMaximumValue(who, VITALID_STAMINA) / 100);

endfunction


/***********************************************************************************
**
** function GetNeededRawPointsToIncrease( basevalue )
**
** Usage:
**	Get the needed amount of raw points to increase baseskill by 1 (increasing
**	effective skill by 0.1).
**
** Parameters:
**	basevalue: The current skill value in tenth.
**
** return:
**	The amount of raw skill points needed.
**
***********************************************************************************/
function GetNeededRawPointsToIncrease( basevalue )

	if( basevalue < 200 )
		return 20.48;
	elseif( basevalue >= 2800 )
		return Cdbl(0x7fffffff);
	else
		return Cdbl(Pow(2.0, Cint(basevalue/100)) * 10.24);
	endif

endfunction


/***********************************************************************************
**
** function GetSkillConfigElem( skillid )
**
** Usage:
**	Get the config element of the received skillid from the CFGFILESPEC_SKILLSDEF
**	config file.
**
** Parameters:
**	skillid:	The skillid that must be referencied
**			by the config elem. (integer)
**
** return:
**	The config element of the skillid or an error.
**
***********************************************************************************/
function GetSkillConfigElem( skillid )

	var cfg	:= ReadConfigFile(CFGFILESPEC_SKILLSDEF);
	if( !cfg )
		return cfg;
	else
		return FindConfigElem(cfg, skillid);
	endif

endfunction


/***********************************************************************************
**
** function GetSkillIdByAttributeId( attributeid )
**
** Usage:
**	Get the skillid equivalent to the specified attibuteid.
**
** Parameters:
**	attributeid: The attributeid to get the equivalent from. (String)
**
** return:
**	The equivalent skillid if one was found or an
**	error {errortext := "No equivalent skillid found for attributeid "} if not
**
***********************************************************************************/
function GetSkillIdByAttributeId( attributeid )

// This was lower(), but the attribute ID's actually start with a cap... ??
	case( attributeid )
		ATTRIBUTEID_ALCHEMY:		return SKILLID_ALCHEMY;
		ATTRIBUTEID_ANATOMY:		return SKILLID_ANATOMY;
		ATTRIBUTEID_ANIMALLORE:		return SKILLID_ANIMALLORE;
		ATTRIBUTEID_ITEMID:		return SKILLID_ITEMID;
		ATTRIBUTEID_ARMSLORE:		return SKILLID_ARMSLORE;
		ATTRIBUTEID_PARRY:		return SKILLID_PARRY;
		ATTRIBUTEID_BEGGING:		return SKILLID_BEGGING;
		ATTRIBUTEID_BLACKSMITHY:	return SKILLID_BLACKSMITHY;
		ATTRIBUTEID_BOWCRAFT:		return SKILLID_BOWCRAFT;
		ATTRIBUTEID_PEACEMAKING:	return SKILLID_PEACEMAKING;
		ATTRIBUTEID_CAMPING:		return SKILLID_CAMPING;
		ATTRIBUTEID_CARPENTRY:		return SKILLID_CARPENTRY;
		ATTRIBUTEID_CARTOGRAPHY:	return SKILLID_CARTOGRAPHY;
		ATTRIBUTEID_COOKING:		return SKILLID_COOKING;
		ATTRIBUTEID_DETECTINGHIDDEN:	return SKILLID_DETECTINGHIDDEN;
		ATTRIBUTEID_ENTICEMENT:		return SKILLID_ENTICEMENT;
		ATTRIBUTEID_EVALINT:		return SKILLID_EVALINT;
		ATTRIBUTEID_HEALING:		return SKILLID_HEALING;
		ATTRIBUTEID_FISHING:		return SKILLID_FISHING;
		ATTRIBUTEID_FORENSICS:		return SKILLID_FORENSICS;
		ATTRIBUTEID_HERDING:		return SKILLID_HERDING;
		ATTRIBUTEID_HIDING:		return SKILLID_HIDING;
		ATTRIBUTEID_PROVOCATION:	return SKILLID_PROVOCATION;
		ATTRIBUTEID_INSCRIPTION:	return SKILLID_INSCRIPTION;
		ATTRIBUTEID_LOCKPICKING:	return SKILLID_LOCKPICKING;
		ATTRIBUTEID_MAGERY:		return SKILLID_MAGERY;
		ATTRIBUTEID_MAGICRESISTANCE:	return SKILLID_MAGICRESISTANCE;
		ATTRIBUTEID_TACTICS:		return SKILLID_TACTICS;
		ATTRIBUTEID_SNOOPING:		return SKILLID_SNOOPING;
		ATTRIBUTEID_MUSICIANSHIP:	return SKILLID_MUSICIANSHIP;
		ATTRIBUTEID_POISONING:		return SKILLID_POISONING;
		ATTRIBUTEID_ARCHERY:		return SKILLID_ARCHERY;
		ATTRIBUTEID_SPIRITSPEAK:	return SKILLID_SPIRITSPEAK;
		ATTRIBUTEID_STEALING:		return SKILLID_STEALING;
		ATTRIBUTEID_TAILORING:		return SKILLID_TAILORING;
		ATTRIBUTEID_TAMING:		return SKILLID_TAMING;
		ATTRIBUTEID_TASTEID:		return SKILLID_TASTEID;
		ATTRIBUTEID_TINKERING:		return SKILLID_TINKERING;
		ATTRIBUTEID_TRACKING:		return SKILLID_TRACKING;
		ATTRIBUTEID_VETERINARY:		return SKILLID_VETERINARY;
		ATTRIBUTEID_SWORDSMANSHIP:	return SKILLID_SWORDSMANSHIP;
		ATTRIBUTEID_MACEFIGHTING:	return SKILLID_MACEFIGHTING;
		ATTRIBUTEID_FENCING:		return SKILLID_FENCING;
		ATTRIBUTEID_WRESTLING:		return SKILLID_WRESTLING;
		ATTRIBUTEID_LUMBERJACKING:	return SKILLID_LUMBERJACKING;
		ATTRIBUTEID_MINING:		return SKILLID_MINING;
		ATTRIBUTEID_MEDITATION:		return SKILLID_MEDITATION;
		ATTRIBUTEID_STEALTH:		return SKILLID_STEALTH;
		ATTRIBUTEID_REMOVETRAP:		return SKILLID_REMOVETRAP;
	endcase
	
	var ret        := error;
	ret.+errortext := "No equivalent skillid found for attributeid " + attributeid;
	return ret;

endfunction


/***********************************************************************************
**
** function GetSkillIds()
**
** Usage:
**	Get all the known skillids.
**
** Parameters:
**	None.
**
** return:
**	An array of all known skillids.
**
***********************************************************************************/
function GetSkillIds()

	return { SKILLID_ALCHEMY,
		 SKILLID_ANATOMY,
		 SKILLID_ANIMALLORE,
		 SKILLID_ITEMID,
		 SKILLID_ARMSLORE,
		 SKILLID_PARRY,
		 SKILLID_BEGGING,
		 SKILLID_BLACKSMITHY,
		 SKILLID_BOWCRAFT,
		 SKILLID_PEACEMAKING,
		 SKILLID_CAMPING,
		 SKILLID_CARPENTRY,
		 SKILLID_CARTOGRAPHY,
		 SKILLID_COOKING,
		 SKILLID_DETECTINGHIDDEN,
		 SKILLID_ENTICEMENT,
		 SKILLID_EVALINT,
		 SKILLID_HEALING,
		 SKILLID_FISHING,
		 SKILLID_FORENSICS,
		 SKILLID_HERDING,
		 SKILLID_HIDING,
		 SKILLID_PROVOCATION,
		 SKILLID_INSCRIPTION,
		 SKILLID_LOCKPICKING,
		 SKILLID_MAGERY,
		 SKILLID_MAGICRESISTANCE,
		 SKILLID_TACTICS,
		 SKILLID_SNOOPING,
		 SKILLID_MUSICIANSHIP,
		 SKILLID_POISONING,
		 SKILLID_ARCHERY,
		 SKILLID_SPIRITSPEAK,
		 SKILLID_STEALING,
		 SKILLID_TAILORING,
		 SKILLID_TAMING,
		 SKILLID_TASTEID,
		 SKILLID_TINKERING,
		 SKILLID_TRACKING,
		 SKILLID_VETERINARY,
		 SKILLID_SWORDSMANSHIP,
		 SKILLID_MACEFIGHTING,
		 SKILLID_FENCING,
		 SKILLID_WRESTLING,
		 SKILLID_LUMBERJACKING,
		 SKILLID_MINING,
		 SKILLID_MEDITATION,
		 SKILLID_STEALTH,
		 SKILLID_REMOVETRAP };

endfunction


/***********************************************************************************
**
** function GetSkillIntrinsicMod( who, skillid )
**
** Usage:
**	Get the intrinsic mod of a mobile in skillid
**
** Parameters:
**	who:     The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The intrinsic mod of the mobile in skillid. (integer [0, 6000])
**
***********************************************************************************/
function GetSkillIntrinsicMod( who, skillid )

	return Cint(GetSkillIntrinsicModBaseValue(who, skillid) / 10);

endfunction


/***********************************************************************************
**
** function GetSkillIntrinsicModBaseValue( who, skillid )
**
** Usage:
**	Get the base value intrinsic mod of a mobile in skillid
**
** Parameters:
**	who:     The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The base value intrinsic mod of the mobile in skillid. (integer [0, 60000])
**
***********************************************************************************/
function GetSkillIntrinsicModBaseValue( who, skillid )

	return GetAttributeIntrinsicMod(who, GetAttributeIdBySkillId(skillid));

endfunction


/***********************************************************************************
**
** function GetSkillTemporaryMod( who, skillid )
**
** Usage:
**	Get the temporary mod of a mobile in skillid
**
** Parameters:
**	who:     The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The temporary mod of the mobile in skillid. (integer [0, 6000])
**
***********************************************************************************/
function GetSkillTemporaryMod( who, skillid )

	return Cint(GetSkillTemporaryModBaseValue(who, skillid) / 10);

endfunction


/***********************************************************************************
**
** function GetSkillTemporaryModBaseValue( who, skillid )
**
** Usage:
**	Get the base value temporary mod of a mobile in skillid
**
** Parameters:
**	who:     The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The base value temporary mod of the mobile in skillid. (integer [0, 60000])
**
***********************************************************************************/
function GetSkillTemporaryModBaseValue( who, skillid )

	return GetAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid));

endfunction


/***********************************************************************************
**
** function GetStamina( who )
**
** Usage:
**	Get the stamina level of a mobile. Equivalent to the old mobile.stamina
**	without assignment.
**
** Parameters:
**	who: The mobile to get the stamina level from. (Mobileref)
**
** return:
**	The current stamina of the mobile. (integer ranging [0, 100000])
**
***********************************************************************************/
function GetStamina( who )

	return Cint(GetVital(who, VITALID_STAMINA) / 100);

endfunction


/***********************************************************************************
**
** function GetStaminaRegenRate( who )
**
** Usage:
**	Get the current stamina regen rate of a mobile. Equivalent to the old
**	who.stamina_regen_rate without assignment
**
** Parameters:
**	who: The mobile to get the stamina regen rate from. (Mobileref)
**
** return:
**	The current stamina regen rate of the mobile. (integer ranging [-30000, 30000])
**	in hundrenth per minute.
**
***********************************************************************************/
function GetStaminaRegenRate( who )

	return GetVitalRegenRate(who, VITALID_STAMINA);

endfunction


/***********************************************************************************
**
** function GetStrength( who )
**
** Usage:
**	Get the strength of the mobile (including mods). Equivalent to the old
**	mobile.strength
**
** Parameters:
**	who: The mobile to get the strength from. (Mobileref)
**
** return:
**	The modified strength of the mobile. (integer ranging [0, 18000])
**
***********************************************************************************/
function GetStrength( who )

	return GetAttribute(who, "Strength");

endfunction


/***********************************************************************************
**
** function GetStrengthMod( who )
**
** Usage:
**	Get the strength temporary mod of a mobile. Equivalent to the old
**	mobile.strength_mod
**
** Parameters:
**	who: The mobile to get the strength temporary mod from. (Mobileref)
**
** return:
**	The strength temporary mod of the mobile. (integer ranging [0, 6000])
**
***********************************************************************************/
function GetStrengthMod( who )

	return Cint(GetAttributeTemporaryMod(who, "Strength") / 10);

endfunction


/***********************************************************************************
**
** function GetTrueBaseSkill( who, skillid )
**
** Usage:
**	Get the "true" base (unmodified) skill of a mobile.
**
** Parameters:
**	who:	 The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The effective skill of the mobilein skillid. (float ranging [0.0, 6000.0])
**
***********************************************************************************/
function GetTrueBaseSkill( who, skillid )

	return Cdbl(GetAttributeBaseValue(who, GetAttributeIdBySkillId(skillid)) / 10.0);

endfunction


/***********************************************************************************
**
** function GetTrueBaseSkill( who, skillid )
**
** Usage:
**	Get the "true" effective skill of a mobile.
**
** Parameters:
**	who:	 The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The effective skill of the mobilein skillid. (float ranging [0.0, 18000.0])
**
***********************************************************************************/
function GetTrueEffectiveSkill( who, skillid )

	var id   := GetAttributeIdBySkillId(skillid);
	var base := GetAttributeBaseValue(who, id);
	var mod1 := GetAttributeTemporaryMod(who, id);
	var mod2 := GetAttributeIntrinsicMod(who, id);
	var effectiveAttribute := Cdbl((base + mod1 + mod2) / 10.0);
	if( effectiveAttribute < 0.0 )
		return 0.0;
	else
		return effectiveAttribute;
	endif

endfunction


/***********************************************************************************
**
** function GetTrueSkillTemporaryMod( who, skillid )
**
** Usage:
**	Get the "true" skill temporary mod of a mobile in skillid.
**
** Parameters:
**	who:	 The mobile. (Mobileref)
**	skillid: The skillid. (integer)
**
** return:
**	The effective skill of the mobilein skillid. (float ranging [0.0, 6000.0])
**
***********************************************************************************/
function GetTrueSkillTemporaryMod( who, skillid )

	return Cdbl(GetAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid)) / 10.0);

endfunction


/***********************************************************************************
**
** function GetVitalIds()
**
** Usage:
**	Get all the known vitalids.
**
** Parameters:
**	None.
**
** return:
**	An array of all known vitalids.
**
***********************************************************************************/
function GetVitalIds()

	return { VITALID_LIFE,
		 VITALID_MANA,
		 VITALID_STAMINA };

endfunction


/***********************************************************************************
**
** function RecalcVitalsIfOnline( who )
**
** Usage:
**	Recals vitals but only if the mobile is online to prevent bugs.
**
** Parameters:
**	who: The mobile. (Mobileref)
**
** return:
**	0/1
**
***********************************************************************************/
function RecalcVitalsIfOnline( who )

	if( TypeOf(who) == "MobileRef" || TypeOf(who) == "OfflineMobileRef" ) 
		return RecalcVitals(who); 
	else
		return 0;
	endif

endfunction


/***********************************************************************************
**
** function SetBaseIntelligence( who, dexterity )
**
** Usage:
**	Set the dexterity of a mobile to the specified level. Equivalent to
**	the old mobile.setDex(dexterity)
**
** Parameters:
**	who:       The mobile to set the dexterity on. (Mobileref)
**	dexterity: The new dexterity. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetBaseDexterity( who, dexterity )

	dexterity := Cint(dexterity * 10);
	if( dexterity < 10 )
		dexterity := 10;
	elseif( dexterity > CORE_SETTINGS_MAX_ATTRIBUTE )
		dexterity := CORE_SETTINGS_MAX_ATTRIBUTE;
	endif

	var ret := SetAttributeBaseValue(who, ATTRIBUTEID_DEXTERITY, dexterity);
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function SetBaseIntelligence( who, intelligence )
**
** Usage:
**	Set the intelligence of a mobile to the specified level. Equivalent to
**	the old mobile.setInt(intelligence)
**
** Parameters:
**	who:          The mobile to set the intelligence on. (Mobileref)
**	intelligence: The new intelligence. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetBaseIntelligence( who, intelligence )

	intelligence := Cint(intelligence * 10);
	if( intelligence < 10 )
		intelligence := 10;
	elseif( intelligence > CORE_SETTINGS_MAX_ATTRIBUTE )
		intelligence := CORE_SETTINGS_MAX_ATTRIBUTE;
	endif

	var ret := SetAttributeBaseValue(who, ATTRIBUTEID_INTELLIGENCE, intelligence);
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function SetBaseSkill( who, skillid, skill )
**
** Usage:
**	Set the base skill of a mobile to the specified level.
**
** Parameters:
**	who:         The mobile to set the intelligence on. (Mobileref)
**	skillid:     The skillid. (integer)
**	skill:       The new skill level (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetBaseSkill( who, skillid, skill )

	return SetBaseSkillBaseValue(who, skillid, Cint(skill*10));

endfunction


/***********************************************************************************
**
** function SetBaseSkillBaseValue( who, skillid, skill_tenth )
**
** Usage:
**	Set the base skill of a mobile to the specified level in tenth.
**
** Parameters:
**	who:         The mobile to set the intelligence on. (Mobileref)
**	skillid:     The skillid. (integer)
**	skill_tenth: The new skill level in tenth. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetBaseSkillBaseValue( who, skillid, skill_tenth )

	if( skill_tenth < CORE_SETTINGS_MIN_ATTRIBUTE )
		skill_tenth := CORE_SETTINGS_MIN_ATTRIBUTE;
	elseif( skill_tenth > CORE_SETTINGS_MAX_ATTRIBUTE )
		skill_tenth := CORE_SETTINGS_MAX_ATTRIBUTE;
	endif

	return SetAttributeBaseValue(who, GetAttributeIdBySkillId(skillid), Cint(skill_tenth));

endfunction


/***********************************************************************************
**
** function SetBaseStrength( who, strength )
**
** Usage:
**	Set the strength of a mobile to the specified level. Equivalent to the old
**	mobile.setStr(strength)
**
** Parameters:
**	who:      The mobile to set the strength on. (Mobileref)
**	strength: The new strength. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetBaseStrength( who, strength )

	strength := Cint(strength * 10);
	if( strength < 10 )
		strength := 10;
	elseif( strength > CORE_SETTINGS_MAX_ATTRIBUTE )
		strength := CORE_SETTINGS_MAX_ATTRIBUTE;
	endif

	var ret := SetAttributeBaseValue(who, ATTRIBUTEID_STRENGTH, strength);
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function SetDexterityMod( who, mod_amt )
**
** Usage:
**	Set the dexterity temporary mod of a mobile. Equivalent to the old
**	mobile.dexterity_mod := mod_amt;
**
** Parameters:
**	who:     The mobile to set the dexterity temporary mod on. (Mobileref)
**	mod_amt: The new dexterity temporary mod. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetDexterityMod( who, mod_amt )

	var ret := SetAttributeTemporaryMod(who, ATTRIBUTEID_DEXTERITY, Cint(mod_amt*10));
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function SetHp( who, hp )
**
** Usage:
**	Set the hp level of a mobile. Equivalent to the old mobile.hp := hp;
**
** Parameters:
**	who: The mobile to set the hp level on. (Mobileref)
**	hp:  The new hp level. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetHp( who, hp )

	hp := Cint(hp * 100);
	if( hp < CORE_SETTINGS_MIN_VITAL )
		hp := CORE_SETTINGS_MIN_VITAL;
	elseif( hp > GetMaxHp(who) * 100 )
		hp := GetMaxHp(who) * 100;
	endif
	
	return SetVital(who, VITALID_LIFE, hp);

endfunction


/***********************************************************************************
**
** function SetHpRegenRate( who, hundreth_per_minute )
**
** Usage:
**	Set the hp level of a mobile. Equivalent to the old mobile.hp := hp;
**
** Parameters:
**	who:                 The mobile. (Mobileref)
**	hundreth_per_minute: The amount of hp restored every minute in hundreth. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetHpRegenRate( who, hundreth_per_minute )

	if( hundreth_per_minute < CORE_SETTINGS_MIN_REGEN_RATE )
		hundreth_per_minute := CORE_SETTINGS_MIN_REGEN_RATE;
	elseif( hundreth_per_minute > CORE_SETTINGS_MAX_REGEN_RATE )
		hundreth_per_minute := CORE_SETTINGS_MAX_REGEN_RATE;
	endif

	SetObjProperty(who, CPROP_NAME_PREFIX_REGEN_RATE+VITALID_LIFE, hundreth_per_minute);
	
	return RecalcVitalsIfOnline(who);
endfunction


/***********************************************************************************
**
** function SetIntelligenceMod( who, mod_amt )
**
** Usage:
**	Set the intelligence temporary mod of a mobile. Equivalent to the old
**	mobile.intelligence_mod := mod_amt;
**
** Parameters:
**	who:     The mobile to set the intelligence temporary mod on. (Mobileref)
**	mod_amt: The new intelligence temporary mod. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetIntelligenceMod( who, mod_amt )

	var ret := SetAttributeTemporaryMod(who, ATTRIBUTEID_INTELLIGENCE, Cint(mod_amt*10));
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function SetMana( who, mana )
**
** Usage:
**	Set the mana level of a mobile. Equivalent to the old mobile.mana := mana;
**
** Parameters:
**	who:  The mobile to set the mana level on. (Mobileref)
**	mana: The new mana level. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetMana( who, mana )

	mana := Cint(mana * 100);
	if( mana < CORE_SETTINGS_MIN_VITAL )
		mana := CORE_SETTINGS_MIN_VITAL;
	elseif( mana > GetMaxMana(who) * 100 )
		mana := GetMaxMana(who) * 100;
	endif
	
	return SetVital(who, VITALID_MANA, mana);

endfunction


/***********************************************************************************
**
** function SetManaRegenRate( who, hundreth_per_minute )
**
** Usage:
**	Set the hp level of a mobile. Equivalent to the old mobile.hp := hp;
**
** Parameters:
**	who:                 The mobile. (Mobileref)
**	hundreth_per_minute: The amount of mana restored every minute in hundreth. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetManaRegenRate( who, hundreth_per_minute )

	if( hundreth_per_minute < CORE_SETTINGS_MIN_REGEN_RATE )
		hundreth_per_minute := CORE_SETTINGS_MIN_REGEN_RATE;
	elseif( hundreth_per_minute > CORE_SETTINGS_MAX_REGEN_RATE )
		hundreth_per_minute := CORE_SETTINGS_MAX_REGEN_RATE;
	endif
	
	SetObjProperty(who, CPROP_NAME_PREFIX_REGEN_RATE+VITALID_MANA, hundreth_per_minute);
	
	return RecalcVitalsIfOnline(who);

endfunction


/***********************************************************************************
**
** function SetSkillTemporaryMod( who, skillid, mod_amt )
**
** Usage:
**	Set the skill temporary mod of a mobile in a given skillid.
**
** Parameters:
**	who:     The mobile to set skill temporary mod on. (Mobileref)
**	skillid: The skillid. (integer)
**	mod_amt: The new temporary mod. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetSkillTemporaryMod( who, skillid, mod_amt )

	return SetAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid), Cint(mod_amt*10));

endfunction


/***********************************************************************************
**
** function SetSkillTemporaryModBaseValue( who, skillid, mod_tenth )
**
** Usage:
**	Set the skill temporary mod in tenth of a mobile in a given skillid.
**
** Parameters:
**	who:       The mobile to set skill temporary mod on. (Mobileref)
**	skillid:   The skillid. (integer)
**	mod_tenth: The new temporary mod in tenth. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetSkillTemporaryModBaseValue( who, skillid, mod_tenth )

	return SetAttributeTemporaryMod(who, GetAttributeIdBySkillId(skillid), mod_tenth);

endfunction


/***********************************************************************************
**
** function SetStamina( who, stamina )
**
** Usage:
**	Set the stamina level of a mobile. Equivalent to the old
**	mobile.stamina := stamina;
**
** Parameters:
**	who:     The mobile to set the stamina level on. (Mobileref)
**	stamina: The new stamina level. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetStamina( who, stamina )

	stamina := Cint(stamina * 100);
	if( stamina < CORE_SETTINGS_MIN_VITAL )
		stamina := CORE_SETTINGS_MIN_VITAL;
	elseif( stamina > GetMaxStamina(who) * 100 )
		stamina := GetMaxStamina(who) * 100;
	endif
	
	return SetVital(who, VITALID_STAMINA, stamina);

endfunction


/***********************************************************************************
**
** function SetStaminaRegenRate( who, hundreth_per_minute )
**
** Usage:
**	Set the hp level of a mobile. Equivalent to the old mobile.hp := hp;
**
** Parameters:
**	who:                 The mobile. (Mobileref)
**	hundreth_per_minute: The amount of stamina restored every minute in hundreth. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetStaminaRegenRate( who, hundreth_per_minute )

	if( hundreth_per_minute < CORE_SETTINGS_MIN_REGEN_RATE )
		hundreth_per_minute := CORE_SETTINGS_MIN_REGEN_RATE;
	elseif( hundreth_per_minute > CORE_SETTINGS_MAX_REGEN_RATE )
		hundreth_per_minute := CORE_SETTINGS_MAX_REGEN_RATE;
	endif
	
	SetObjProperty(who, CPROP_NAME_PREFIX_REGEN_RATE+VITALID_STAMINA, hundreth_per_minute);
	
	return RecalcVitalsIfOnline(who);

endfunction


/***********************************************************************************
**
** function SetStrengthMod( who, mod_amt )
**
** Usage:
**	Set the strength temporary mod of a mobile. Equivalent to the old
**	mobile.strength_mod := mod_amt;
**
** Parameters:
**	who:     The mobile to set the strength temporary mod on. (Mobileref)
**	mod_amt: The new strength temporary mod. (integer)
**
** return:
**	0/1
**
***********************************************************************************/
function SetStrengthMod( who, mod_amt )

	var ret := SetAttributeTemporaryMod(who, ATTRIBUTEID_STRENGTH, Cint(mod_amt*10));
	if( ret )
		RecalcVitalsIfOnline(who);
	endif
	
	return ret;

endfunction


/***********************************************************************************
**
** function SetStaminaRegenRate( who, vitalid, hundreth_per_minute )
**
** Usage:
**	Set the hp level of a mobile. Equivalent to the old mobile.hp := hp;
**
** Parameters:
**	who:                 The mobile. (Mobileref)
**	vitalid:             The vital id of the wanted vital. (String)
**	hundreth_per_minute: The amount of stamina restored every minute in hundreth. (integer)
**
** return:
**	0/1
**
***********************************************************************************/

//function SetVitalRegenRate( who, vitalid, hundreth_per_minute )

//	if( hundreth_per_minute < CORE_SETTINGS_MIN_REGEN_RATE )
//		hundreth_per_minute := CORE_SETTINGS_MIN_REGEN_RATE;
//	elseif( hundreth_per_minute > CORE_SETTINGS_MAX_REGEN_RATE )
//		hundreth_per_minute := CORE_SETTINGS_MAX_REGEN_RATE;
//	endif
	
//	SetObjProperty(who, CPROP_NAME_PREFIX_REGEN_RATE+vitalid, //hundreth_per_minute);
	
//	return RecalcVitalsIfOnline(who);

//endfunction


////////////////////////////////////////////////////////////////////////////////////
//
//	Dreams
//	GetSpeed(MoveSpeed)
//	This is a custom movespeed for npcs, incase a custom movespeed isn't set it will
//	use the default dexterity based one.
//
////////////////////////////////////////////////////////////////////////////////////

function GetSpeed(me := 0, MoveSpeed)
	MoveSpeed := lower(MoveSpeed);
	var i := 0, valid := 1;
	if(MoveSpeed == "fastest")
		return 0;
	elseif(MoveSpeed == "fast")
		return 75;
	elseif(MoveSpeed == "normal")
		return 150;
	elseif(MoveSpeed == "slow")
		return 225;
	elseif(MoveSpeed == "slowest")
		return 300;
	endif

	var number := {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
	repeat
		if(MoveSpeed[i] in number)
		else
			valid := 0;
		endif
		i := i+1;
	until (i > len(MoveSpeed) || valid == 0);

	if(valid)
		if(MoveSpeed < 0)
			MoveSpeed := 0;
		endif
		return MoveSpeed;
	else
		print(me.name+" Have a wrong MoveSpeed value in npcdesc.cfg, fix it now!");
		return 300;
	endif
endfunction
