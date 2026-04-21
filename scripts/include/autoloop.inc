// AutoLoop.inc by Lapo 'Aspox' Luchini <lapo@lapo.it>

/*
** AutoLoop_init asks character how many time it wants to iterate
** [if "autoloop" CProp is set it doesn't ask but uses that value, unless is 0]
** AutoLoop_more retunrs the number of loops remaining
**
** Use like this (for a crafter-like skill):
**
** AutoLoop_init(who);
** while(AutoLoop_more()>0 && <material_not_finished>)
**   <create_item>;
** endwhile
** AutoLoop_finish();
*/

use uo;
use os;

include "include/random";

const AL_consecutive := 10; // number of consecutive items before a anti-macroer gump appears to ask to continue
const AL_timeout := 15; // number of seconds the player has to press "continue"

var AL_number	:= 0;
var AL_remain	:= 0;
var AL_char;
var AL_delay;
var AL_charX;
var AL_charY;
var AL_setting;

// init the autoloop variables
function AutoLoop_init( character, maxx := 9999, delay := 5 )

	AL_setting := CInt( GetObjProperty( character, "autoloop" ) );
	if( !AL_setting )
		SetObjProperty ( character, "autoloop", 0 );
		AL_setting := 0;
	endif

	if( AL_setting == 0 )
		AL_number := CInt( SendTextEntryGump(character, "How many loops? [0-"+maxx+"]", TE_CANCEL_ENABLE, TE_STYLE_NORMAL, 4));
	else
		AL_number := AL_setting;
	endif

	if( !AL_number )
		AL_number := 0;
	elseif( AL_number > maxx )
		AL_number := maxx;
	endif

	AL_remain := AL_number;
	AL_char   := character;
	AL_charX  := AL_char.x;
	AL_charY  := AL_char.y;
	AL_delay  := delay;

endfunction


// he numer returned is the number of things STILL to do, use BEFORE creating, not after
function AutoLoop_more()

	if( (AL_charX == AL_char.x) && (AL_charY == AL_char.y) )
		if( AL_remain > 0 )
			if( AL_remain != AL_number )
				Sleep( AL_delay );
				//if( (AL_number - AL_remain) % AL_consecutive = 0)
					//if( !TimedYesNo(AL_char) )
					//	AL_remain := -1;
					//	AL_number := 0; // to print "looping aborted"
					//endif
				//endif
			endif
			if( AL_remain > 0 )
				SendSysMessage( AL_char , "Looping [" + AL_remain + " more to go].", 3 , 89 );
				AL_remain := CInt( AL_remain ) - 1;
			endif
		else
			AL_remain := -1;
		endif
	else
		SendSysMessage( AL_char , "You moved." , 3 , 89 );
		AL_remain := -1;
		AL_number := 0; // to print "looping aborted"
	endif

	return( AL_remain+1 );

endfunction


// to be used after all, to clean up the variables and print the finishing text
function AutoLoop_finish()

	if( (AL_remain < 0) && AL_number )
		SendSysMessage( AL_char, "You finished looping.", 3, 89);
	else
		SendSysMessage( AL_char, "Looping aborted.", 3, 89);
	endif

	AL_number := 0;
	AL_remain := 0;

endfunction


// anti-macroers jumping gump
function TimedYesNo( who )

	var x := 220 + Random( 200 );
	var y := 140 + Random( 200 );
	var AL_layout := {
		"page 0",
		"gumppic " + x + " " + y + " 2070",
		"button " + (x+30) + " " + (y+75) + " 2130 2129 1 0 1",
		"button " + (x+90) + " " + (y+75) + " 2119 2120 1 0 0",
		"text " + (x+40) + " " + (y+30) + " 1355 0"
	};
	var AL_data := { "Continue?" };

	var timeover := ReadGameClock();
	var res:=SendDialogGump( who, AL_layout, AL_data );

	if( ReadGameClock() - timeover > AL_timeout)
		SendSysMessage( who, "You waited too long to decide.", 3, 89 );
		return( 0 );
	elseif( res )
		return( 1 );
	else
		return( 0 );
	endif

endfunction
