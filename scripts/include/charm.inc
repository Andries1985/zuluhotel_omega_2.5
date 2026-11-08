/////////////////////////////////////////////////////////////////////////////
//
//  Control Undead and Spell Bind: a creature held by a spell for a while.
//
//  The state is kept on the creature so it survives a reboot. The spell
//  sleeps for the duration and then ends the charm, but a reboot kills that
//  sleeping script, so the tamed AI (scripts/ai/tamed.src) also checks the
//  end time when it starts and on every loop and ends the charm itself.
//
//  CProp "Charm": struct
//		spell       "controlundead" or "spellbind"
//		ends_at     ReadGameClock() when the charm ends
//		master      serial of the caster
//		script      the creature's own AI script, put back at the end
//		name        the creature's own name, put back at the end
//		prev_master serial of the pet's owner if Spell Bind took someone's pet
//		            (0 for a wild creature); the pet goes back to them
//  CProp "charmed" is also set to 1: other scripts read it to treat the
//  creature as a magic pet (no transfer, stabling or mounting).
//
//  Before Patch 1.1.6 both spells kept the end time only in the sleeping
//  script; Control Undead also never cleared the core master, so the freed
//  creature's attacks were reported (and flagged) as the caster's. Those
//  left "charmed" (1, or Spell Bind's old script name) and no "Charm"; a
//  creature like that is ended at once.
//
/////////////////////////////////////////////////////////////////////////////

use uo;
use os;
use cfgfile;

const PROPID_CHARM := "Charm";


// Puts mob under caster's control for duration seconds.
// Returns the end time, which the spell keeps to recognise its own charm.
function StartCharm( mob, caster, duration, spellname )

	var charm_ends := ReadGameClock() + CInt( duration );

	var charm_prev := 0;
	if( mob.script == "tamed" )
		charm_prev := CInt( GetObjProperty( mob, "master" ) );
	endif

	var charm_data := struct;
	charm_data.+spell       := spellname;
	charm_data.+ends_at     := charm_ends;
	charm_data.+master      := caster.serial;
	charm_data.+script      := mob.script;
	charm_data.+name        := mob.name;
	charm_data.+prev_master := charm_prev;

	set_critical( 1 );
	SetObjProperty( mob, PROPID_CHARM, charm_data );
	SetObjProperty( mob, "charmed", 1 );
	SetObjProperty( mob, "master", caster.serial );
	mob.setmaster( caster );
	mob.script := "tamed";
	set_critical( 0 );

	RestartScript( mob );

	return charm_ends;

endfunction


// 1 if mob is held by Control Undead or Spell Bind (current or pre-1.1.6 leftover).
function IsSpellCharmed( mob )

	if( GetObjProperty( mob, PROPID_CHARM ) or GetObjProperty( mob, "charmed" ) )
		return 1;
	endif

	return 0;

endfunction


// A real pet can carry the charmed == 1 a pre-1.1.6 Control Undead left on a creature it
// freed (tameable undead: undead horses, mares, dracoliches...). Taming sets PreviouslyTamed,
// which Control Undead never did, so such a flag is junk: drop it instead of freeing the pet.
function ClearStaleCharmFlag( mob )

	if( GetObjProperty( mob, PROPID_CHARM ) )
		return 0;
	endif
	if( GetObjProperty( mob, "charmed" ) == 1 and GetObjProperty( mob, "PreviouslyTamed" ) )
		EraseObjProperty( mob, "charmed" );
		return 1;
	endif

	return 0;

endfunction


// 1 once mob's charm should end.
function CharmExpired( mob )

	if( !IsSpellCharmed( mob ) )
		return 0;
	endif

	var charm_data := GetObjProperty( mob, PROPID_CHARM );
	if( !charm_data )
		return 1;	// pre-1.1.6 leftover: no end time was ever stored
	endif

	return ( ReadGameClock() >= CInt( charm_data.ends_at ) );

endfunction


// The owner a Spell Bound pet goes back to, or 0.
function CharmPrevMaster( mob )

	var charm_data := GetObjProperty( mob, PROPID_CHARM );
	if( !charm_data or !charm_data.prev_master )
		return 0;
	endif

	var charm_owner := SystemFindObjectBySerial( charm_data.prev_master );
	if( !charm_owner )
		charm_owner := SystemFindObjectBySerial( charm_data.prev_master, SYSFIND_SEARCH_OFFLINE_MOBILES );
	endif
	if( !charm_owner )
		return 0;
	endif

	return charm_owner;

endfunction


// Ends the charm: a taken pet goes back to its owner; anything else goes back
// to its own AI with no master (cprop or core). Returns the owner it went back
// to, or 0 if it is wild again.
function EndCharm( mob )

	var charm_data   := GetObjProperty( mob, PROPID_CHARM );
	var charm_script := "";
	var charm_name   := "";
	if( charm_data )
		charm_script := charm_data.script;
		charm_name   := charm_data.name;
	else
		// Pre-1.1.6 Spell Bind kept the old script name in "charmed"
		var charm_old := GetObjProperty( mob, "charmed" );
		if( TypeOf( charm_old ) == "String" )
			charm_script := charm_old;
		endif
	endif

	var charm_owner := CharmPrevMaster( mob );
	if( !charm_owner and ( !charm_script or charm_script == "tamed" ) )
		var charm_cfg := ReadConfigFile( "::npcdesc" );
		if( charm_cfg )
			charm_script := charm_cfg[mob.npctemplate].script;
		endif
	endif

	var charm_caster := 0;
	var charm_caster_serial := CInt( GetObjProperty( mob, "master" ) );
	if( charm_caster_serial )
		charm_caster := SystemFindObjectBySerial( charm_caster_serial );
	endif

	set_critical( 1 );
	EraseObjProperty( mob, PROPID_CHARM );
	EraseObjProperty( mob, "charmed" );
	if( charm_owner )
		SetObjProperty( mob, "master", charm_owner.serial );
		mob.setmaster( charm_owner );
		mob.script := "tamed";
	else
		EraseObjProperty( mob, "master" );
		mob.setmaster( 0 );
		if( charm_script )
			mob.script := charm_script;
		endif
	endif
	if( charm_name )
		mob.name := charm_name;
	endif
	set_critical( 0 );

	if( charm_caster and charm_caster != charm_owner )
		SendSysMessage( charm_caster, mob.name + " is no longer under your control." );
	endif
	if( charm_owner )
		SendSysMessage( charm_owner, mob.name + " is yours again." );
	endif
	PrintTextAbove( mob, "*breaks free*" );

	RestartScript( mob );

	return charm_owner;

endfunction
