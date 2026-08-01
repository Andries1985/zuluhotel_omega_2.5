use uo;
use polsys;

///////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// SetSafeAreaProperties() - Set invul, and in future skill restrictions or whatever
// we want. 
//
// Note: InSafeArea prop doesn't get removed on server restart. Only way this can
//       be exploited, ie keeping invuln status outside of a safe zone. Is if a staff
//		   member erases the property. This IS recorded though :)
///////////////////////////////////////////////////////////////////////////////////////
function SetSafeAreaProperties(who)

	// Staff don't need protection!
	if (who.cmdlevel)
		return 0;
	endif

	// Have to set this prop to get around leavearea restriction
	SetObjProperty(who, "InSafeArea", 1);
	SetObjProperty(who, "c", 1);
	// Set player to invuln
	GrantPrivilege( who, "invul" );
	who.enable("invul");
	IncRevision(who);

	// Enter Message
	PrintTextAbovePrivate(who, "*You are under the magical protection of the safe zone*", who, 3, 53);
		
	// Pretty Animation
	PlayObjectCenteredEffectEx( who, 0x376A, 0, 15, 53, 0, 0, 0 );

	//Sound Effect
	PlaySoundEffectPrivate(who, 0x0F8, who);

endfunction

///////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// RemoveSafeAreaProperties() - Removes any things the Set function does
//
///////////////////////////////////////////////////////////////////////////////////////
function RemoveSafeAreaProperties(who, nomsg := 0)

	// Staff don't need protection!
	if (who.cmdlevel)
		return 0;
	endif

	// Remove Safe area
	EraseObjProperty(who, "InSafeArea");
	EraseObjProperty(who, "c");
	// Removed invul priv
	RevokePrivilege( who, "invul" );
	who.disable("invul");
	IncRevision(who);
	if (nomsg == 1)
		return 1;
	endif

	// Leave MEssage
	PrintTextAbovePrivate(who, "*You are no longer protected by the safe zone*", who, 3, 43);

	// Pretty animation
	PlayObjectCenteredEffectEx( who, 0x376A, 0, 15, 43, 0, 0, 0 );

	// Sound effect
	PlaySoundEffectPrivate(who, 0x0F7, who);

endfunction

function SetNOPK(who)
	SetObjProperty(who, "NOPKAREA", 1);
endfunction

function RemoveNOPK(who)
	EraseObjProperty(who, "NOPKAREA");
endfunction