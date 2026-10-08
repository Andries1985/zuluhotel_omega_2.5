// housetravel.inc - "is this spot inside somebody's house, and may this character travel there?"
// for Recall, Gate, Mark, Teleport, Earth Portal and the runebook.
//
// Patch 1.1.4, ported from ZH3.0 (68a5c45, 2026-10-02) and rebuilt on this shard's housing.
//
// Why: the travel scripts tested the destination with GetStandingHeight( x, y, runeZ ).multi. The
// engine answers that with the lowest surface AT OR ABOVE the rune's z, while MoveObjectToLocation
// drops the caster onto the nearest surface BELOW it. A rune marked in an old house at a height
// above the floor of a house placed there later therefore found "no house" and landed its holder
// inside. The same test saw nothing on a castle courtyard tile (no house piece under it), and the
// static-house plate lookup used range 0 at the rune's z, so an upper-floor rune missed it too.
//
// ZH3.0's helper reads footprint boxes off its house signs; pkg/std/housing keeps no such data, so
// here the footprint test is the engine's own ListMultisInBox (the multi's footprint extent, any
// height - the same call pkg/std/housing/utility.inc's IsLocationInThisHouse() relies on), and
// static houses are found through pkg/opt/statichousing's house plates with z ignored.

use uo;
use os;

include "include/objtype";
include ":housing:utility";					// IsCowner, IsFriend, RECALL_TO / GATE_TO / GATE_FROM
include ":statichousing:recallgatehelp";	// tile_list (via :housing:setup): the static-house plate objtypes

// A sign can sit a whole house-width from the far corner of its footprint (castles are 31 x 32).
const HOUSETRAVEL_SCAN_RANGE := 34;

// The house covering a spot, or 0. Returns a house multi, a boat multi, or a static house SIGN;
// IsCowner / IsFriend accept any of them.
//   1. a placed house whose footprint rectangle covers x,y at any height; z is ignored on purpose.
//   2. the engine's own test at this height, as the scripts always did - keeps steps, porches and
//      boats behaving exactly as before.
//   3. a static house: the plate on this tile, at any height. A plate whose sign is gone protects
//      nothing. scan_static 0 skips this step (used for NPC Teleport casts, which fire often).
function FindHouseAtSpot( x, y, z, realm, scan_static := 1 )

	// ListMultisInBox lists multis that have a piece inside the box, so a single-tile box misses
	// a courtyard tile. Scan a house-sized box around the spot and test each house's footprint
	// rectangle (the core's .footprint member) instead; a core without it falls through to the
	// old tests below.
	foreach piece_owner in ListMultisInBox( x - HOUSETRAVEL_SCAN_RANGE, y - HOUSETRAVEL_SCAN_RANGE, -128, x + HOUSETRAVEL_SCAN_RANGE, y + HOUSETRAVEL_SCAN_RANGE, 127, realm )
		if( piece_owner.isa( POLCLASS_HOUSE ) )
			var fp := piece_owner.footprint;
			if( fp )
				if( x >= fp.xmin and x <= fp.xmax and y >= fp.ymin and y <= fp.ymax )
					return piece_owner;
				endif
			endif
		endif
	endforeach

	var standing_multi := GetStandingHeight( x, y, z, realm ).multi;
	if( standing_multi )
		return standing_multi;
	endif

	if( scan_static )
		return FindStaticHouseSignAtSpot( x, y, realm );
	endif

	return 0;

endfunction

// The sign of the static house whose floor plate sits on x,y (any height), or 0.
function FindStaticHouseSignAtSpot( x, y, realm )

	foreach item_type in tile_list
		foreach plate in ListItemsNearLocationOfType( x, y, LIST_IGNORE_Z, 0, item_type, realm )
			if( GetObjProperty( plate, "HousePlate" ) and GetObjProperty( plate, "houseserial" ) )
				var sign := SystemFindObjectBySerial( CInt( GetObjProperty( plate, "houseserial" ) ) );
				if( sign )
					return sign;
				endif
			endif
		endforeach
	endforeach

	return 0;

endfunction

// 1 when "who" may recall / gate / mark at a spot covered by "house" (0 = no house there):
// owner, co-owner, a character on the account that owns a STATIC house, or the matching friend
// permission. priv is RECALL_TO / GATE_TO /
// GATE_FROM from pkg/std/housing/utility.inc.
function CanTravelIntoHouse( who, house, priv )

	if( !house )
		return 1;
	endif

	if( IsCowner( who, house ) )
		return 1;
	endif

	// A static house's sign records the buying account ("owneracct"): any character on that account
	// may travel into it, as pkg/opt/statichousing/ssign.src already treats them as owners. The
	// runebook path meant to do this but compared "OwnerAcct" (wrong case), so it never passed.
	// Placed houses carry the same property, but travel into them stays owner / co-owner / friend
	// as before: the rule is only applied when "house" is a static sign, not a multi.
	if( !house.isa( POLCLASS_MULTI ) )
		if( who.acctname and GetObjProperty( house, "owneracct" ) == who.acctname )
			return 1;
		endif
	endif

	if( IsFriend( who, house, priv ) )
		return 1;
	endif

	return 0;

endfunction

// Removes every entry of a runebook that points at this exact spot, and clears the book's
// default location when it is that spot. Returns how many entries were removed.
function RemoveRunebookEntryAt( book, x, y, z )

	set_critical(1);

	var removed := 0;
	var defs := GetObjProperty( book, "RuneDefs" );
	if( defs )
		var i;
		for( i := defs.size(); i >= 1; i := i - 1 )
			var entry := defs[i];
			if( CInt( entry.tox ) == x && CInt( entry.toy ) == y && CInt( entry.toz ) == z )
				defs.erase( i );
				removed := removed + 1;
			endif
		endfor
		if( removed )
			SetObjProperty( book, "RuneDefs", defs );
		endif
	endif

	// the book's own x/y/z are its default location - what a spell cast on the book uses
	if( CInt( GetObjProperty( book, "x" ) ) == x && CInt( GetObjProperty( book, "y" ) ) == y && CInt( GetObjProperty( book, "z" ) ) == z )
		EraseObjProperty( book, "x" );
		EraseObjProperty( book, "y" );
		EraseObjProperty( book, "z" );
		removed := removed + 1;
	endif

	if( removed )
		IncRevision( book );
	endif

	set_critical(0);
	return removed;

endfunction

// A travel spell was refused because the destination is inside a house the caster may not enter.
// As with forbidden areas, the rune does not survive: a loose rune is destroyed; a runebook loses
// that entry. A boat is not a house: nothing is destroyed for it.
function DestroyRefusedRune( caster, cast_on, house, tox, toy, toz )

	if( house.isa( POLCLASS_BOAT ) )
		return 0;
	endif

	if( cast_on.objtype == UOBJ_RUNE )
		DestroyItem( cast_on );
		SendSysMessage( caster, "The rune crumbles." );
		return 1;
	endif

	if( cast_on.objtype == UOBJ_RUNEBOOK )
		if( RemoveRunebookEntryAt( cast_on, tox, toy, toz ) )
			SendSysMessage( caster, "That location has been removed from the book." );
			return 1;
		endif
	endif

	return 0;

endfunction
