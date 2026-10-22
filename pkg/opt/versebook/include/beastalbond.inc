use uo;

// Where the bard stood when they started playing Beastal Bond. Persistent, so logoff, reconnect
// and logon (after a restart killed the verse script) can still put them back - viewing the
// beast drags the bard along to its location every second.
const BEASTALBOND_ORIGIN := "BeastalBondOrigin";


function SaveBeastalBondOrigin( who )

	SetObjProperty( who , BEASTALBOND_ORIGIN , { who.x , who.y , who.z , who.realm } );

endfunction


// Moves the bard back to where they started playing, if they're not already there.
function ReturnToBeastalBondOrigin( who )

	var origin := GetObjProperty( who , BEASTALBOND_ORIGIN );
	if( !origin )
		return 0;
	endif

	EraseObjProperty( who , BEASTALBOND_ORIGIN );
	if( who.x != origin[1] || who.y != origin[2] || who.realm != origin[4] )
		MoveObjectToLocation( who , origin[1] , origin[2] , origin[3] , origin[4] , MOVEOBJECT_FORCELOCATION );
	endif

	return 1;

endfunction


// Logoff / reconnect / logon: the bond doesn't survive the bard leaving (matches ZH3.0). Clearing
// BeastalBond also makes the wisp AI (ai_beastal_bond.src) shut itself down. Only touches
// frozen / concealed when bond state is present, so concealed staff are left alone.
function EndBeastalBond( who )

	if( !GetObjProperty( who , "BeastalBond" ) && !GetObjProperty( who , "BeastalBondTimer" ) &&
	    !GetObjProperty( who , "#StopPlaying" ) && !GetObjProperty( who , "#ViewingBeast" ) &&
	    !GetObjProperty( who , BEASTALBOND_ORIGIN ) )
		return 0;
	endif

	who.frozen := 0;
	who.concealed := 0;
	EraseObjProperty( who , "BeastalBond" );
	EraseObjProperty( who , "BeastalBondTimer" );
	EraseObjProperty( who , "#StopPlaying" );
	EraseObjProperty( who , "#ViewingBeast" );
	ReturnToBeastalBondOrigin( who );

	return 1;

endfunction
