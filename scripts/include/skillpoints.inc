use uo;
use os;
use util;
use basic;
use cfgfile;
use attributes;

include "include/attributes";
include "include/classes";

function get_default_points(skillid)
        var cfg := readconfigfile(":shilhook:attributes");

        var default_points := FindConfigElem( cfg, GetAttributeIdBySkillId( skillid ) ).DefaultPoints;
        
        if (!default_points)
                return 0;
        else
                return default_points;

        endif

endfunction


function AwardSkillPoints( who, skillid, points )

  // NPCS don't gain skill
	if( who.IsA(POLCLASS_NPC) )
		return 0;
	// staff don't gain skill
	elseif( who.cmdlevel )
		return 0;
	endif

  	// Get lock/drop/raise status of the skills
	var skills_state_array   := GetObjProperty( who, "SkillsState" );
	if( !skills_state_array )
		skills_state_array := array;
	endif
	
	// Get the lock/drop/raise status of THIS skill
	var skill_state := skills_state_array[skillid+1];
	
	// Skill is set to drop
	if( skill_state == "d" )
		var baseskill := GetBaseSkill( who, skillid );
		var amount   := 1;
		//amount to drop is more than the skill he has, set skill to 0 and LOCK the skill
		if( amount > baseskill )
			amount := baseskill;
			skills_state_array[skillid+1] := "l";
			SetObjProperty( who, "SkillsState", skills_state_array );
		endif
		// Reduce skill points
		SetBaseSkill( who, skillid, Cint(baseskill - amount) );
		return 0;
		
		// Skill is set to locked, just return 0
	elseif( skill_state == "l" )
		return 0;
	endif
	
	//-------- Skill is set to RAISE, do some gain ---------\\
	
	// Check if the player has any skill modifier set, set by admins using .setplayermultipliers. Should never happen...
	points := Cint( points * GetSkillPointsMultiplier( who , skillid ) );
	var special_multipliers := GetObjProperty( who, "SkillMultipliers" );
	if( len(special_multipliers) )
		if( special_multipliers[skillid+1] )
			if( Cint(special_multipliers[skillid+1]) == 9999 )
				points := 0;
			else
				var skill_mult := Cdbl( special_multipliers[skillid+1] );
				points := Cint( points * skill_mult );
			endif
		endif
	endif
	
	// Check if any global mulitpliers are set for this skill, using .setglobalmultipliers. power hour anyone? :P
	var global_multipliers := GetGlobalProperty( "SkillPointsMultiplier" );
	if( len(global_multipliers) )
		if( global_multipliers[skillid+1] )
			if( Cint(global_multipliers[skillid+1]) == 9999 )
				points := 0;
			else
				var mult := Cdbl( global_multipliers[skillid+1] );
				points := Cint( points * mult );
			endif
		endif
	endif
	
	//Bonus for everyone
	points := Cint( points * 1.4 );

	//Bonus for first gen players
	if(who.acct.GetProp("FirstGen"))
		points := Cint( points * 1.1 );
	endif	

	//RPers receive increased skill gain due to no macroing rules.
	if(GetObjProperty(who, "IsRPer"))
		points := Cint( points * 1.5 );
	endif	

	// Patch 1.1.4: the Powerplayer's class bonus is the smaller curve on every skill (1.15 at
	// level 1 up to 1.9 at level 6) instead of the per-skill specialist bonus and second roll,
	// which the class lookup in classes.inc never grants a Powerplayer. Replaces the old
	// 1.1 / 1.2 / 1.3 table that only applied at levels 3 to 5.
	var pplevel := CInt( GetObjProperty( who, CLASSEID_POWERPLAYER ) );
	if( pplevel )
		points := Cint( points * ClasseSmallBonusByLevel( pplevel ) );
	endif

	// Set the skill points
	AwardPoints(who, skillid, points);
	
endfunction

function GetPlayerSkillCap( who, skillid )
	var skillcap := 130;
	var matrix := GetObjProperty(who, "pScrollMatrix");

	if(matrix)
		skillcap := 130 + CInt(matrix[skillid]);
		if(skillid == 0)
			skillcap := 130 + CInt(matrix[49]);
		endif
		if(skillcap > 150)
			skillcap := 150;
		endif
	endif

	return skillcap;
endfunction

function ApplySkillHardCap( who, skillid )
	var skillcap := GetPlayerSkillCap(who, skillid);
	if(GetBaseSkill(who, skillid) > skillcap)
		SetBaseSkill(who, skillid, skillcap);
	endif
endfunction

/***********************************************************************************
// **
// ** function AwardPoints( who, skillid, skillpoints )
// **
// ** Usage:
// **	Awards the given raw skillpoints in the skillid and the stats
// **	increase specified into CFGFILESPEC_SKILLSDEF for this skillid.
// **
// ** Parameters:
// **	who:	     The mobile to whom the skillpoints are awarded. (Mobileref)
// **	skillid:     The skillid to add the points to. (integer)
// **	skillpoints: The amount of raw skill points to be awarded. (integer)
// **
// ** return:
// **	0/1		Weither if the points could be awarded or not.
// **
//***********************************************************************************/

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
			AwardRawAttributePoints(who, ATTRIBUTEID_STRENGTH, ClassStatGainAmount( who, "str", RandomDiceRoll(adv[2]) ));
		endif
	endif
	
	// Check the dexterity advancement
	adv := elem.DexAdv;
	if( adv )
		adv := SplitWords(adv);
		if( RandomInt(1000) < Cdbl(adv[1])*10 )
			AwardRawAttributePoints(who, ATTRIBUTEID_DEXTERITY, ClassStatGainAmount( who, "dex", RandomDiceRoll(adv[2]) ));
		endif
	endif
	
	// Check the intelligence advancement
	adv := elem.IntAdv;
	if( adv )
		adv := SplitWords(adv);
		if( RandomInt(1000) < Cdbl(adv[1])*10 )
			AwardRawAttributePoints(who, ATTRIBUTEID_INTELLIGENCE, ClassStatGainAmount( who, "int", RandomDiceRoll(adv[2]) ));
		endif
	endif
	
	return 1;

endfunction

/***********************************************************************************
// **
// ** function AwardRawSkillPoints( who, skillid, rawpoints )
// **
// ** Usage:
// **	Awards the given raw points in the skillid.
// **
// ** Parameters:
// **	who:	   The mobile to whom the skillpoints are awarded. (Mobileref)
// **	skillid:   The skillid to add the points to. (integer)
// **	rawpoints: The amount of raw skill points to be awarded. (integer)
// **
// ** return:
// **	0/1		Weither if the points could be awarded or not.
// **
// ***********************************************************************************/
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

	//Powerhour
	if(GetGlobalProperty("PHS") || GetObjProperty(who, "#PPHS"))
		rawpoints := Cint( rawpoints * 2 );
	endif

	return AwardRawAttributePoints(who, GetAttributeIdBySkillId(skillid), rawpoints);

endfunction


// Patch 1.1.4: stat affinity. The dice amount of a stat advancement is multiplied by the class
// bonus for a class's strong stat and divided by it for its weak stat (tables in classes.inc),
// rounded half up and never below 1, so a roll that fired always awards something. Because raw
// stat points are applied against the tenth's cost as a probability, this scales the chance of
// gaining 0.1 by the same factor at every stat level.
function ClassStatGainAmount( who, stat, amount )

	var mult := GetStatPointsMultiplier( who, stat );
	if( mult == 1.0 )
		return amount;
	endif

	var scaled := Cint( CDbl( amount ) * mult + 0.5 );
	if( scaled < 1 )
		scaled := 1;
	endif

	return scaled;

endfunction
