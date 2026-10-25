use uo;
use os;

// Timed spell protections layered on top of a mobile's permanent ones (equipment, npcdesc CProps).
// Each is a temporary (#) cprop holding { level, owner pid }:
//  - temporary, so a restart can never leave one behind
//  - the owner pid lets the casting script check its cast is still the active one (not dispelled
//    by WipeMods, not replaced by a recast) before ending it
// The permanent PermPoisonImmunity / PhysicalProtection cprops are never touched by these spells,
// so the effective level is the higher of the two and the spells can't stack.
const SPELLPROT_POISON		:= "#SpellPoisonImmunity";	// Sanctuary, Antidote
const SPELLPROT_DRAGONSKIN	:= "#DragonSkin";		// Dragon Skin verse

// Sanctuary, Antidote and Dragon Skin all last 2 minutes at the caster's class level 1, rising
// evenly to 10 minutes at level 6.
const SPELLPROT_MIN_SECONDS	:= 120;
const SPELLPROT_MAX_SECONDS	:= 600;


// The caster's class level as 1-6. Unclassed casters, staff and NPCs count as level 1.
function SpellProtCasterLevel( level )

	level := CInt( level );
	if( level < 1 )
		return 1;
	elseif( level > 6 )
		return 6;
	endif

	return level;

endfunction


// Duration in seconds for a caster of the given class level: 120, 216, 312, 408, 504, 600.
function SpellProtDuration( level )

	level := SpellProtCasterLevel( level );
	return SPELLPROT_MIN_SECONDS + ( SPELLPROT_MAX_SECONDS - SPELLPROT_MIN_SECONDS ) * ( level - 1 ) / 5;

endfunction


function GetSpellProtLevel( who , prop )

	var data := GetObjProperty( who , prop );
	if( !data )
		return 0;
	endif

	return CInt( data[1] );

endfunction


function SetSpellProt( who , prop , level )

	SetObjProperty( who , prop , { CInt( level ) , GetPid() } );
	IncRevision( who );

endfunction


// Ends the protection only if the calling script's cast still owns it. Returns 1 if it ended it.
function EndSpellProt( who , prop )

	var data := GetObjProperty( who , prop );
	if( !data || data[2] != GetPid() )
		return 0;
	endif

	EraseObjProperty( who , prop );
	IncRevision( who );
	return 1;

endfunction


// A negative (cursed) permanent level always wins - the spells don't lift curses.
function EffectiveSpellProt( who , permprop , spellprop )

	var perm := CInt( GetObjProperty( who , permprop ) );
	if( perm < 0 )
		return perm;
	endif

	var spell := GetSpellProtLevel( who , spellprop );
	if( spell > perm )
		return spell;
	endif

	return perm;

endfunction


function GetPoisonImmunityLevel( who )

	return EffectiveSpellProt( who , "PermPoisonImmunity" , SPELLPROT_POISON );

endfunction


function GetPhysicalProtection( who )

	return EffectiveSpellProt( who , "PhysicalProtection" , SPELLPROT_DRAGONSKIN );

endfunction


// Sets a permanent protection cprop on a player to the best level granted by equipped items
// (or erases it). Used at logon to clear values left behind by the old Sanctuary / Antidote /
// Dragon Skin code, which wrote straight into these cprops.
function ResyncEquippedProt( who , prop )

	var best := 0;
	var level;
	foreach thing in ListEquippedItems( who )
		level := CInt( GetObjProperty( thing , prop ) );
		if( level > best )
			best := level;
		endif
	endforeach

	if( best )
		SetObjProperty( who , prop , best );
	else
		EraseObjProperty( who , prop );
	endif

endfunction
