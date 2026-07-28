use uo;
use polsys;

include ":housing:utility";

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

///////////////////////////////////////////////////////////////////////////////////////
// Omega
//
// SetNoDamageZoneProperties()/RemoveNoDamageZoneProperties() - No Damage Zone doesn't
// grant invul like Safe Area does. Combat is blocked from ever starting (packethook,
// Fight(), damage-calc backstops), so there's nothing for invul to protect against -
// this property just flags "who is currently standing in one" for those hot-path checks.
//
///////////////////////////////////////////////////////////////////////////////////////
function SetNoDamageZoneProperties(who)
	if (who.cmdlevel)
		return 0;
	endif
	SetObjProperty(who, "InNoDamageZone", 1);
endfunction

function RemoveNoDamageZoneProperties(who)
	EraseObjProperty(who, "InNoDamageZone");
endfunction

///////////////////////////////////////////////////////////////////////////////////////
// Omega
//
// StoreDonatorMountForNoDamageZone() - Same net effect as manually using a mount
// stone (see pkg/opt/vanityshop/mountstone.src StoreMount()), but driven off the
// live mount NPC directly rather than the rider+stone-item pair that command
// expects, since our callers (Fight(), tamed.src, dismount.inc) only ever have the
// mount mobile in hand.
//
///////////////////////////////////////////////////////////////////////////////////////
function StoreDonatorMountForNoDamageZone(mount)
	if (!mount)
		return 0;
	endif

	var stone_serial := GetObjProperty(mount, "DMountStoneSerial");
	var stone := SystemFindObjectBySerial(stone_serial);
	if (!stone)
		return 0;
	endif

	PlayStationaryEffectEX(mount.x, mount.y, mount.z, mount.realm, 0x3728, 0xa, 0xa, mount.color, 4);
	MoveObjectToLocation(mount, 5288, 1176, 0, "britannia", MOVEOBJECT_FORCELOCATION);

	SetObjProperty(stone, "MountHP", mount.hp);
	SetObjProperty(stone, "MountMana", mount.mana);
	SetObjProperty(stone, "MountStamina", mount.stamina);
	SetObjProperty(stone, "petname", mount.name);
	SetObjProperty(stone, "MountStored", 1);
	SetObjProperty(mount, "guardkill", 1);
	ApplyRawDamage(mount, CInt(GetHP(mount) + 3));

	var rider_serial := CInt(GetObjProperty(mount, "master"));
	var rider := SystemFindObjectBySerial(rider_serial);
	if (rider)
		EraseObjProperty(rider, "DonatorMounted");
	endif

	return 1;
endfunction

///////////////////////////////////////////////////////////////////////////////////////
// Omega
//
// ConfiscateTamedPetForNoDamageZone() - Shared confiscation helper for a tamed pet
// ordered to attack a defender inside a No Damage Zone from outside it: donator
// mounts get stored to their stone (non-destructive, recoverable normally);
// everything else gets ticketed to its master the same way
// ConfiscatePetForRestrictedRelease() (scripts/ai/tamed.src) already does for
// NOPK/Guarded/Safe release restrictions, then removed via KillConfiscatedPet().
// Called from scripts/ai/combat/fight.inc's Fight() and tamed.src's own Fight().
//
// Deliberately NOT triggered just by a pet being present/released/dismounted in
// the zone - zoo owners need to be able to walk pets in and .release/GoWild them
// to populate the exhibit. Once released they can't target or attack anyone
// (blocked the same way everything else in the zone is), so they're harmless
// left alone; only an explicit outside-in attack order gets punished.
//
///////////////////////////////////////////////////////////////////////////////////////
function ConfiscateTamedPetForNoDamageZone(pet)
	if (!pet)
		return 0;
	endif

	if (GetObjProperty(pet, "DonatorMount"))
		return StoreDonatorMountForNoDamageZone(pet);
	endif

	var master_serial := CInt(GetObjProperty(pet, "master"));
	var master := SystemFindObjectBySerial(master_serial);
	if (!master)
		master := SystemFindObjectBySerial(master_serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
	endif

	if (master)
		var ticket := CreateItemInBackpack(master, CONFISCATION_TICKET_OBJTYPE, 1);
		if (ticket)
			PopulateConfiscatedPetTicket(ticket, master.serial, pet);
			SendSysMessage(master, "Your pet " + pet.name + " has been confiscated (No Damage Zone).", 3, 2595);
		endif
	endif

	KillConfiscatedPet(pet);
	return 1;
endfunction