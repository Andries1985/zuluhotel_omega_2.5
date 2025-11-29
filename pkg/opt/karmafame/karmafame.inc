use uo;
use os;
use math;
use cfgfile;

const MIN_KARMA_LIMIT := -20500;
const MAX_KARMA_LIMIT := 10500;

const MIN_FAME_LIMIT := 0;
const MAX_FAME_LIMIT := 10500;

const FACTOR_KARMA := 0.3;
const FACTOR_FAME  := 0.1;

const FAME_LOST_ON_DEATH := -125;

const KARMA_CRIMINAL_LIMIT := -750;
const KARMA_MURDERER_LIMIT := -3000;

const VALUE_MULTIPLIER_FOR_ALLIES := 0.5;
const VALUE_MULTIPLIER_FOR_TAMED  := 0.5;

const DELAY_TO_GET_KARMA_FAME := 300;

//include "include/client";
// Action ids used by AwardKarmaFameForAction function
/*
const ACTIONID_BEGGING       := 1;
const ACTIONID_BLESS         := 2;
const ACTIONID_CAST_NECRO    := 3;
const ACTIONID_CHOP_CORPSE   := 4;
const ACTIONID_CURE          := 5;
const ACTIONID_CURSE         := 6;
const ACTIONID_DEATH         := 7;
const ACTIONID_EXCEPTIONAL   := 8;
const ACTIONID_HEAL          := 9;
const ACTIONID_LOOTING       := 10;
const ACTIONID_POISONING     := 11;
const ACTIONID_PROTECT       := 12;
const ACTIONID_RES           := 13;
const ACTIONID_SNOOPING      := 14;
const ACTIONID_STEALING      := 15;
const ACTIONID_SUMMON_DAEMON := 16;
*/

function AdviseAttackers( who )

	var hit_list := GetObjProperty( who, "Hitlist" );

	if( hit_list )
		EraseObjProperty( who, "Hitlist" );

		foreach arr in hit_list
			if( ReadGameClock() < Cint(arr[3]) + DELAY_TO_GET_KARMA_FAME ) // what is this delay meant to do?
				var attacker := SystemFindObjectBySerial( Cint(arr[1]) );
				if( !attacker )
					attacker := SystemFindObjectBySerial( Cint(arr[1]), SYSFIND_SEARCH_OFFLINE_MOBILES );
				endif
				
				if( attacker )
					AffectKarmaAndFameForKill( attacker, who, CDbl(arr[2]) / Len(hit_list) );
				endif
			endif
		endforeach
	endif

endfunction


function AffectKarmaAndFameForKill( attacker, defender, factor := 1.0 )

	// Check if the creature was killed by his owner
	if( GetObjProperty( defender , "master" ) == attacker.serial )
		AwardFame( attacker , -200 );
		if( CInt( GetKarma( attacker ) ) > 0 )
			AwardKarma( attacker , -250 );
		endif
		return;
	endif

	if(GetObjProperty(defender, "VetResed"))
		SendSysMessage( attacker , "You will not gain any fame and karma for slaying a resurrected animal." );
		return;
	endif

	//no KF change from killing Guild Mates
	if(attacker.GuildID)
		if(attacker.GuildID == defender.GuildID)
			return;
		endif
	endif
 
	// Award the fame if you killed something more famous than you, if so award fame
	var delta_fame := GetFame(defender) - GetFame(attacker);
	if( delta_fame > 0 )
		delta_fame := CInt( delta_fame * FACTOR_FAME * factor );
		if( delta_fame <= 0 )
			delta_fame := 1;
		endif
		AwardFame( attacker , delta_fame );
	endif

	var attacker_karma := GetKarma(attacker);
	var NotoDefKarma   := GetKarma(defender);
	var delta_karma    := 0.0;
	if( attacker_karma >= 0 ) // Attacker got positive or null karma

		if( NotoDefKarma > 0 ) // Attacking positive karma

			// Lose karma
			delta_karma := ( attacker_karma - NotoDefKarma ) * FACTOR_KARMA;
			if( delta_karma > 0.0 )
				delta_karma := 0.0;
			endif
		else // Attacking negative or null karma
			// Gain karma if the defender is more evil than you're virtueous
			delta_karma := (abs(NotoDefKarma) - attacker_karma) * FACTOR_KARMA;
			if( delta_karma < 0.0 )
				delta_karma := 0.0;
			endif
		endif

	else // Attacker got negative karma

		if( NotoDefKarma >= 0 ) // Attacking positive or null karma
			// Lose karma if the defender is more virtueous than you're evil
			delta_karma := ((-attacker_karma) - NotoDefKarma) * FACTOR_KARMA;
			if( delta_karma > 0.0 )
				delta_karma := 0.0;
			endif
		elseif( attacker_karma > NotoDefKarma ) // Attacking something more evil than him
			// Gain karma
			delta_karma := (attacker_karma - NotoDefKarma) * FACTOR_KARMA;
			if( delta_karma < 0.0 )
				delta_karma := 0.0;
			endif
		endif

	endif

	delta_karma := CInt( delta_karma * factor );

	if( delta_karma <> 0.0 )
		AwardKarma( attacker , delta_karma );
	endif

endfunction


function AwardFame( who, amt )

	var fame := GetFame( who );

	if( !fame )
		fame := 0;
	endif

	if( amt > 0 ) // Gaining fame
		amt := CInt(amt/50);
		if(amt < 1)
			amt := 1;
		elseif(amt > 25)
			amt := 25;
		endif

		if( amt <= 1 )
			SendSysMessage( who , "You have gained a tiny amount of fame!" );
		elseif( amt <= 5 )
			SendSysMessage( who , "You have gained a very small amount of fame!" );
		elseif( amt <= 10 )
			SendSysMessage( who , "You have gained a small amount of fame!" );
		elseif( amt <= 15 )
			SendSysMessage( who , "You have gained a good amount of fame!" );
		elseif( amt <= 20 )
			SendSysMessage( who , "You have gained a large amount of fame!" );
		else
			SendSysMessage( who , "You have gained a great amount of fame!" );
		endif
	elseif( amt < 0 ) // Losing fame
		if( amt >= -50 )
			SendSysMessage( who , "You have lost a tiny amount of fame!" );
		elseif( amt >= -100 )
			SendSysMessage( who , "You have lost a very small amount of fame!" );
		elseif( amt >= -300 )
			SendSysMessage( who , "You have lost a small amount of fame!" );
		elseif( amt >= -600 )
			SendSysMessage( who , "You have lost a good amount of fame!" );
		elseif( amt >= -1000 )
			SendSysMessage( who , "You have lost a large amount of fame!" );
		else
			SendSysMessage( who , "You have lost a great amount of fame!" );
		endif
	endif

	SetFame( who , fame + amt );

endfunction


function AwardKarma( who , amt )

	var karma := GetKarma( who );

	if( !karma )
		karma := 0;
	endif

	if( amt > 0 ) // Gaining karma
		amt := CInt(amt/50);
		if(amt < 1)
			amt := 1;
		elseif(amt > 25)
			amt := 25;
		endif

		if( amt <= 1 )
			SendSysMessage( who , "You have gained a tiny amount of karma!" );
		elseif( amt <= 5 )
			SendSysMessage( who , "You have gained a very small amount of karma!" );
		elseif( amt <= 10 )
			SendSysMessage( who , "You have gained a small amount of karma!" );
		elseif( amt <= 15 )
			SendSysMessage( who , "You have gained a good amount of karma!" );
		elseif( amt <= 20 )
			SendSysMessage( who , "You have gained a large amount of karma!" );
		else
			SendSysMessage( who , "You have gained a great amount of karma!" );
		endif
	elseif( amt < 0 ) // Losing karma
		if( amt >= -50 )
			SendSysMessage( who , "You have lost a tiny amount of karma!" );
		elseif( amt >= -100 )
			SendSysMessage( who , "You have lost a very small amount of karma!" );
		elseif( amt >= -300 )
			SendSysMessage( who , "You have lost a small amount of karma!" );
		elseif( amt >= -600 )
			SendSysMessage( who , "You have lost a good amount of karma!" );
		elseif( amt >= -1000 )
			SendSysMessage( who , "You have lost a large amount of karma!" );
		else
			SendSysMessage( who , "You have lost a great amount of karma!" );
		endif
	endif

	SetKarma( who , karma + amt );

endfunction

/*
function AwardKarmaFameForAction( who , action , on_who := 0 )
*/

function CheckForCriminalsNearMobile( who , range := 5 )

	if( who.criminal or who.murderer )
		return;
	endif

	var criminals := ListMobilesInLineOfSight( who , range );
	var guarr;
	foreach crim in criminals
		if( crim.murderer )
			PrintTextAbove( who , "A murderer is here!!!" );
			Sleep( 2 );
			PrintTextAbove( who , "No..." );
			Sleepms( 500 );
			PrintTextAbove( who , "Please..." );
			Sleepms( 500 );
			PrintTextAbove( who , "Don't hurt me..." );
			Sleepms( 500 );
			PrintTextAbove( who , "Guards please help me!" );
			guarr := GetGlobalProperty( "GuardsCalled" );
			if (!guarr)
				guarr:={};
			endif
			guarr.append( who.serial );
			SetGlobalProperty( "GuardsCalled" , guarr );
		elseif( crim.criminal )
			PrintTextAbove( who , "Guards!!! There is a criminal here!!!! Come Here!!!!" );
			guarr := GetGlobalProperty( "GuardsCalled" );
			if (!guarr)
				guarr:={};
			endif
			guarr.append( who.serial );
			SetGlobalProperty( "GuardsCalled" , guarr );
		endif
	endforeach

endfunction


function GetFame( who )

	var fame := Cint( GetObjProperty( who, "Fame" ) );
	if( !who.npctemplate )
		if( !who.cmdlevel )
			if( fame > MAX_FAME_LIMIT )
				fame := MAX_FAME_LIMIT;
				SetObjProperty( who, "Fame", MAX_FAME_LIMIT );
			elseif( fame < MIN_FAME_LIMIT )
				fame := MIN_FAME_LIMIT;
				SetObjProperty( who, "Fame", MIN_FAME_LIMIT );
			endif
		endif
	endif

	return fame;

endfunction


function GetFameID( fame )

	var FameArray  := {0, 1250, 2500, 5000, 10000};
	var index := Len(FameArray);
	while ((index > 1) && (fame < FameArray[index]))
		index := index - 1;
	endwhile

	return index;

endfunction


function GetKarma( who )

	var karma := Cint( GetObjProperty( who, "Karma" ) );
	if( !who.npctemplate )
		if( !who.cmdlevel )
			if( karma > MAX_KARMA_LIMIT )
				karma := MAX_KARMA_LIMIT;
				SetObjProperty( who, "Karma", MAX_KARMA_LIMIT );
			elseif( karma < MIN_KARMA_LIMIT )
				karma := MIN_KARMA_LIMIT;
				SetObjProperty( who, "Karma", MIN_KARMA_LIMIT );
			endif
		endif
	endif

	return karma;

endfunction


function GetKarmaID( karma )

	var KarmaArray := {-20000, -9999, -4999, -2499, -1249, -625, +625, +1250, +2500, +5000, +10000}; /*ROWS*/
	var index := Len(KarmaArray);
	while ((index > 1) && (karma < KarmaArray[index]))
		index := index - 1;
	endwhile

	return index;

endfunction


function GetKarmaFameName( who )

	var KFMatrix   := {
	/* Karma\Fame  0              1250            2500         5000             10000                   */
	/* -30000 */ {"Outcast",     "Wretched",     "Nefarious", "Dread",         "Dread Lord/Lady"        },
	/*  -9999 */ {"Despicable",  "Dastardly",    "Wicked",    "Evil",          "Evil Lord/Lady"         },
	/*  -4999 */ {"Scoundrel",   "Malicious",    "Vile",      "Villainous",    "Dark Lord/Lady"         },
	/*  -2499 */ {"Unsavory",    "Dishonorable", "Ignoble",   "Sinister",      "Sinister Lord/Lady"     },
	/*  -1249 */ {"Rude",        "Disreputable", "Notorious", "Infamous",      "Dishonored Lord/Lady"   },
	/*   -624 */ {"Unfair",      "Notable",      "Prominent", "Renowned",      "Lord/Lady"              },
	/*   +625 */ {"Fair",        "Upstanding",   "Reputable", "Distinguished", "Distinguished Lord/Lady"},
	/*  +1250 */ {"Kind",        "Respectable",  "Proper",    "Eminent",       "Eminent Lord/Lady"      },
	/*  +2500 */ {"Good",        "Honorable",    "Admirable", "Noble",         "Noble Lord/Lady"        },
	/*  +5000 */ {"Honest",      "Commendable",  "Famed",     "Illustrious",   "Illustrious Lord/Lady"  },
	/* +10000 */ {"Trustworthy", "Estimable",    "Great",     "Glorious",      "Glorious Lord/Lady"     }
};
	var karma := GetKarma( who );
	var fame  := GetFame( who );
	var title := KFMatrix[ GetKarmaID( karma ) ][ GetFameID( fame )];

	if( karma < KARMA_MURDERER_LIMIT )
		who.setCriminal(0);
		who.setMurderer(1);
	elseif( karma < KARMA_CRIMINAL_LIMIT )
		who.setCriminal(1);
		who.setMurderer(0);
	else
		who.setMurderer(0);
	endif

	if( title["Lord/Lady"] )
		if( who.gender == 0 )
			title["Lord/Lady"] := "Lord";
		else
			title["Lord/Lady"] := "Lady";
		endif
	endif

	return title;

endfunction


function SetFame( who , fame )

	if( !fame )
		fame := 0;
	else
		fame := CInt( fame );
	endif

	if( !who.npctemplate && !who.cmdlevel )
		if( fame > MAX_FAME_LIMIT )
			fame := MAX_FAME_LIMIT;
		elseif( fame < MIN_FAME_LIMIT )
			fame := MIN_FAME_LIMIT;
		endif
	endif

	SetObjProperty( who , "Fame" , fame );
	SetNewNotoName( who );

endfunction


function SetKarma( who, karma )

	if( !karma )
		karma := 0;
	else
		karma := Cint(karma);
	endif

	if( !who.npctemplate && !who.cmdlevel )
		if( karma > MAX_KARMA_LIMIT )
			karma:=MAX_KARMA_LIMIT;
		elseif( karma < MIN_KARMA_LIMIT )
			karma:=MIN_KARMA_LIMIT;
		endif

		var old := GetKarma( who );

		if( karma < KARMA_MURDERER_LIMIT )
			if( old >= KARMA_MURDERER_LIMIT )
				SendSysMessage( who , "For all yours sins, you're now considered as a murderer..." );
			endif
		elseif( karma < KARMA_CRIMINAL_LIMIT )
			if( old >= KARMA_CRIMINAL_LIMIT )
				SendSysMessage( who , "From now, you're considered as a criminal..." );
			elseif( old < KARMA_MURDERER_LIMIT )
				SendSysMessage( who , "You slowly purge yours sins, you're now considered only as a criminal..." );
			endif
		else
			if( old < KARMA_CRIMINAL_LIMIT )
				SendSysMessage( who , "You are no longer a criminal!" );
			endif
		endif
	endif

	SetObjProperty( who, "Karma", karma );
	SetNewNotoName( who );

endfunction


function SetNewNotoName( who )

	if( who.npctemplate || who.cmdlevel )
		return;
	endif

	var prefix := GetKarmaFameName( who );

	if( Len(prefix) > 0 )
		who.title_prefix := "The " + prefix + "";
	else
		who.title_prefix := "";
	endif

endfunction