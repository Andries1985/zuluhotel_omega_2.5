use polsys;
use cfgfile;

include "setup";
include "include/attributes";
include "include/time";

const VIEW_SECURE := 2;
const ADD_TO_SECURE := 3;
const REMOVE_FROM_SECURE := 4;
const RECALL_TO := 5;
const GATE_TO := 6;
const GATE_FROM := 7;
const CO_OWNER := 8;
const CONFISCATION_TICKET_OBJTYPE := 0xDF0C;

//-------------------------------------------------- New Housing Functions ----------------------------------------------------------------\\

// Returns 1 if it's the owner of co-owner.
function IsCowner( who, house )

	if (GetObjProperty(house, "ownerserial") == who.serial)
		return 1;
	endif
	
	var friends := GetObjProperty(house, "Friends");
	
	foreach friend in friends
		if (friend[1] == who.serial)
			if (friend[CO_OWNER] == 1) // [8] is co-owner tag
				return 1;
			endif
		endif
	endforeach
	
	return 0;
	
endfunction

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// IsFriend() - Can pass a priv to check
// 
// Priv List:	2=View secures	3=Add to secures	4=Take from secures	5=Recall into house
//						6=Gate to house	7=Gate from house	8=Co-owner
//
///////////////////////////////////////////////////////////////////////////////////////////////////////
function IsFriend( who, house, priv := 0 )

	var friends := GetObjProperty(house, "Friends");
	
	foreach friend in friends
		if (friend[1] == who.serial) 		// He's a friend
			if (priv != 0) 								// a priv was passed, check it equals 1
				if (friend[priv] == 1)
					return 1;									// Priv is 1, accept
				else
					return 0;									// Priv is 0, deny
				endif
			endif
			return 1; 										// No more searching for a priv, return 1
		endif
	endforeach
	
	return 0;													// DENY!!
	
endfunction


////////////////////////////////////////////////////////////////////////////////////
// Toadstool 28/04/2012
//
// UgradeSecure() - Function runs on secure open, converst old style
// secures to new. Once world wipse, remove all this.
//
///////////////////////////////////////////////////////////////////////////////////
function UpgradeSecure(who, container, boxo)
	
	// Don't run if this isnt here anymore
	if (!GetObjProperty(container, "boxserial"))
		Return 0;
	endif
	
	// Erase boxserial (the identifier of old secures) so this only runs once
	EraseObjproperty(container, "boxserial");
	
	foreach item in EnumerateItemsInContainer(container)
		if (item.serial != boxo.serial)
			if (item.container.serial == boxo.serial) // Moving top level items only
				if (!MoveItemToContainer(item, container))
					MoveObjectToLocation(item, who.x, who.y, who.z, who.realm, MOVEOBJECT_FORCELOCATION);
					SendSysMessage(who, item.desc+" placed at your feet as the secure is full.", 3, 67);
				endif
			endif
		endif
		sleepms(20);
	endforeach
	
	DestroyItem(boxo);
	
	SendSysMessage(who, "Secure container upgrade complete. Thank you.", 3, 53);
	
	return 1;
		
endfunction
	
////////////////////////////////////////////////////////////////////////////////////
// Toadstool 28/04/2012
//
// UpdateFriendFunction() - Function runs on shard start-up to convert old style
// friend list to new. Once world wipse, remove all this.
//
///////////////////////////////////////////////////////////////////////////////////
function UpdateFriendFunction(house, unused sign)

	// Friends exist, meaning new system in place
	if (GetObjProperty(house, "Friends"))
		return 0;
	endif
	
	var friends;
	var friend;

	// Mix friends/coowners into one prop, coowners getting all privs
	var i;
	for( i := 0 ; i <= MAX_FRIENDS ; i := i + 1 )
		friend := GetObjProperty( house , "friend" + i );
		if (friend)
			friends.append({friend, 1, 0, 0, 1, 0, 0, 0});
		endif
		friend := GetObjProperty( house , "Co-owner" + i );
		if (friend)
					friends.append({friend, 1, 1, 1, 1, 1, 1, 1});
		endif
		EraseObjProperty(house, "friend"+i);
		EraseObjProperty(house, "Co-owner"+i);
	endfor
	
	SetObjProperty(house, "Friends", friends);
	return( 1 );
	
endfunction
	

////////////////////////////////////////////////////////////////////////////////////
// Toadstool 27/04/2012
//
// AddFriendNew() - Adds a friend to the house
//
///////////////////////////////////////////////////////////////////////////////////
function AddFriendNew(who, house)

var friends := GetObjProperty(house, "Friends");

if (!friends)
	friends := {};
endif

var what := Target(who, TGTOPT_NOCHECK_LOS+TGTOPT_HELPFUL);

if (!what.ip)
	SendSySMessage(who, "You must select a player.");
	return 0;
elseif (what == who)
	SendSysMessage(who, "You are the owner!");
	return 0;
endif

foreach person in friends
	if (what.serial == person[1])
		SendSysMessage(who, "That person is already a friend.");
		return 0;
	endif
endforeach

friends.append({what.serial, 1, 0, 0, 1, 0, 0, 0});
SetObjProperty(house, "Friends", friends);
SendSysMessage(what, who.name+" has made you a friend of their house.", 3, 53);
SendSysMessage(who, what.name+" is now a friend of your house.", 3, 53);

endfunction


//////////////////////////////////////////////////////////////////////////////////
// Toadstool 27/04/2012
//
// RemoveFriendNew() - Removes a friend from the house
// 
//
//
//////////////////////////////////////////////////////////////////////////////////
function RemoveFriendNew(who, house, friends, data)

var allfriends := GetObjProperty(house, "Friends");
var removed := SystemFindObjectBySerial(CInt(friends[data.keys[2]][1]), SYSFIND_SEARCH_OFFLINE_MOBILES);

var i := 1;
foreach person in allfriends
	if (person[1] == removed.serial)
		allfriends.erase(i);
	endif
	i := i + 1;;
endforeach

SetObjProperty(house, "Friends", allfriends);

SendSysMessage(who, ""+removed.name+" has been removed from the house friends list.", 3, 43);
SendSysMessage(removed, "You have been removed from "+who.name+" house friends list.", 3, 43); 


endfunction

function RemoveSpecificFriendNew(who, house)

var allfriends := GetObjProperty(house, "Friends");
var allfriends2 := allfriends;

foreach friend in allfriends
	if (friend[1] == who.serial)
		allfriends2.erase(_friend_iter);
	endif
endforeach

SetObjProperty(house, "Friends", allfriends2);

endfunction
///////////////////////////////////////////////////////////////////////////////
// Toadstool 27/04/2012
//
// SaveData() - Saves all checkbox data.
//
// Todo: Look into saving co-owner
//
//////////////////////////////////////////////////////////////////////////////
function SaveData(house, thekeys, friends)

// Using a sent friends, look at how pages work etc to figure out why

// Get rid of the 0
thekeys.erase(1);
// get rid of the last entry, button selection
thekeys.erase(thekeys.size());
// If a radio button pressed (player name) get rid of that value
if (thekeys[1] < 100)
	thekeys.erase(1);
endif

var i := 1; // iterator to cycle through each person in friends - not reset
var b := 1; // iterator used to cycle through each bit in friends privs - is reset
var store := {};
var val;

// Cycle through each friend in friends

foreach person in friends

	// End of page, letting it go would write 0's to rest.
	if (i > 11 || i > friends.size())
		break;
	endif

	val := i*100;

	// cycle through entire key range and get persons numbers
	foreach thing in thekeys	
		if (thing-val <= person.size())
			store.append(thing-val);
		endif
	endforeach

	// cycle through persons numbers, updating them		
	foreach num in person
		num := num;
		if (b != 1)
			// if co-owner, set all privs to 1
			if (b in store || (person[8] == 1 && !b in store))
				friends[i][b] := 1;
			else
				friends[i][b] := 0;
			endif
		endif

		b := b + 1;
	endforeach

	store := {};
	b := 1;
	i := i + 1;

endforeach

// friends may have been a cut down version. We update ALL friends
i := 1;
b := 1;
var allfriends := GetObjProperty(house, "Friends");

foreach person in allfriends

	foreach man in friends

		if (man[1] == person[1])
			allfriends[i] := friends[b];
		endif
		b := b + 1;
	endforeach
b := 1;
i := i + 1;

endforeach

SetObjProperty(house, "Friends", allfriends);

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// We add all the players current houses in an array on the player.
// The only logging before was a deed that people threw away anyway.
//
/////////////////////////////////////////////////////////////////////////////////////////////
function AddHouseToCharacter(who, hserial)

var houses := GetObjProperty(who, "Houses");

if (!houses)
	houses := {};
endif

houses.append(hserial);

SetObjProperty(who, "Houses", houses);

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Toadstool
//
// We add all the players current houses in an array on the player.
// The only logging before was a deed that people threw away anyway.
//
/////////////////////////////////////////////////////////////////////////////////////////////
function RemoveHouseFromCharacter(who, hserial)

var houses := GetObjProperty(who, "Houses");
var houses2 := houses;

if (!houses)
	syslog("Error, player SHOULD have a house to remove, but doesn't");
	return 0;
endif

foreach house in houses
	if (house == hserial)
		houses2.erase(_house_iter);
	endif
endforeach

if (houses2.size() < 1)
	EraseObjProperty(who, "Houses");
else
	SetObjProperty(who, "Houses", houses2);
endif

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Returns 1 if world location x,y,z falls inside this specific house's footprint,
// and 0 if it's outside every house or inside a DIFFERENT house/boat multi. Used to
// validate teleporter placement instead of the unreliable "is my new item already
// showing up in house.items" check, which doesn't actually confirm the item landed
// inside this house.
/////////////////////////////////////////////////////////////////////////////////////////////
function IsLocationInThisHouse(house, x, y, z, realm)

	var found := ListMultisInBox(x, y, z, x, y, z, realm);

	foreach m in (found)
		if (m.serial == house.serial)
			return 1;
		endif
	endforeach

	return 0;

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Destroys every teleporter this house has ever placed (tracked via the "TeleporterSerials"
// cprop) and clears the list. Call this before DestroyMulti() on ANY house teardown path
// (player demolish, admin .destroymulti, decay) so teleporters can never survive their
// house being torn down.
//
// Also sweeps house.items for teleporters placed before serial-tracking existed (or
// otherwise untracked) that still happen to sit within this house's own footprint, so
// upgrading to serial-tracking doesn't strand pre-existing teleporters at demolish time.
/////////////////////////////////////////////////////////////////////////////////////////////
function CleanupHouseTeleporters(house)

	var teleserials := GetObjProperty(house, "TeleporterSerials");
	if (!teleserials)
		teleserials := {};
	endif

	foreach item in (house.items)
		if (item.objtype == 0xA3CE && GetObjProperty(item, "teleportserial") == house.serial && !(item.serial in teleserials))
			teleserials.append(CInt(item.serial));
		endif
	endforeach

	foreach teleserial in (teleserials)
		var teleitem := SystemFindObjectBySerial(CInt(teleserial));
		if (teleitem)
			DestroyItem(teleitem);
		endif
	endforeach

	EraseObjProperty(house, "TeleporterSerials");

endfunction

/////////////////////////////////////////////////////////////////////////////////////////////
// Per-account house limit. Houses are tracked per-CHARACTER (the "Houses" cprop
// above), but an account gets a fixed 5 character slots (account.GetCharacter(1..5),
// including offline ones), so the account-wide total is a live sum across those
// slots rather than a separately maintained counter.
/////////////////////////////////////////////////////////////////////////////////////////////
const MAX_HOUSES_PER_ACCOUNT := 5;

function GetAccountHouseCount(who)
	if (!who.acctname)
		return 0;
	endif

	var account := FindAccount(who.acctname);
	if (!account)
		return 0;
	endif

	var total := 0;
	var i;
	for (i := 1; i <= 5; i := i + 1)
		var chr := account.GetCharacter(i);
		if (chr)
			var houses := GetObjProperty(chr, "Houses");
			var slot_count := 0;
			if (houses)
				slot_count := houses.size();
				total := total + slot_count;
			endif
			Print("[houselimit][debug] account '" + CStr(who.acctname) + "' slot " + CStr(i) + " -> character '" + CStr(chr.name) + "' has " + CStr(slot_count) + " house(s)");
		endif
		SleepMS(2);
	endfor

	Print("[houselimit][debug] account '" + CStr(who.acctname) + "' total house count = " + CStr(total));
	return total;
endfunction

// Staff (cmdlevel>=4) are exempt, matching Buildhouse()'s existing
// city/dungeon/shrine/graveyard cmdlevel<4 gate in housedeed.src.
function IsAccountAtHouseLimit(who)
	if (who.cmdlevel >= 4)
		Print("[houselimit][debug] who '" + CStr(who.name) + "' cmdlevel>=4, exempt from house limit");
		return 0;
	endif

	var count := GetAccountHouseCount(who);
	if (count >= MAX_HOUSES_PER_ACCOUNT)
		Print("[houselimit][debug] who '" + CStr(who.name) + "' at limit (" + CStr(count) + "/" + CStr(MAX_HOUSES_PER_ACCOUNT) + ") - DENY");
		return 1;
	endif

	Print("[houselimit][debug] who '" + CStr(who.name) + "' (" + CStr(count) + "/" + CStr(MAX_HOUSES_PER_ACCOUNT) + ") - allow");
	return 0;
endfunction

function RegisterHouseType(deed, house)

var name := StrReplace(deed.desc, "a deed to a ", "");

SetObjProperty(house, "MultiType", name);

endfunction

function PopulateConfiscatedPetTicket(ticket, owner_serial, pet)
	if(!ticket or !pet)
		return 0;
	endif

	var pet_name := pet.name;
	var max_hp := CInt(GetObjProperty(pet, "CustomHitsLevel"));
	if(!max_hp)
		max_hp := GetMaxHP(pet);
	endif
	var fine := CInt(max_hp / 40);

	ticket.name := "Confiscated pet ticket - " + pet_name + " - Fine: " + fine + "gp";
	SetObjProperty(ticket, "petname", pet.name);
	SetObjProperty(ticket, "pettemplate", pet.npctemplate);
	SetObjProperty(ticket, "pethp", GetHP(pet));
	SetObjProperty(ticket, "petmana", GetMana(pet));
	SetObjProperty(ticket, "petcolor", pet.color);
	SetObjProperty(ticket, "owner_serial", owner_serial);
	SetObjProperty(ticket, "confiscation_fine", fine);

	return fine;
endfunction

function KillConfiscatedPet(pet)
	if(!pet)
		return;
	endif

	MoveObjectToLocation(pet, 5288, 1176, 0, "britannia", MOVEOBJECT_FORCELOCATION);
	SetObjProperty(pet, "guardkill", 1);
	pet.kill();
endfunction

function ConfiscateBossPet(mobile, locationnoun := "house")
	var master_serial := GetObjProperty(mobile, "master");
	if(master_serial == error)
		master_serial := 0;
	endif
	if(!master_serial or mobile.script != "tamed")
		// Wild boss — kill outright
		KillConfiscatedPet(mobile);
		return;
	endif

	var pet_name := mobile.name;

	var master := SystemFindObjectBySerial(master_serial);
	var master_online := 1;
	if(!master)
		master := SystemFindObjectBySerial(master_serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
		master_online := 0;
	endif

	if(!master)
		KillConfiscatedPet(mobile);
		return;
	endif

	var ticket := 0;
	var ticket_location := "";
	if(master_online)
		ticket := CreateItemInBackpack(master, CONFISCATION_TICKET_OBJTYPE);
		if(ticket)
			ticket_location := "backpack";
		endif
	endif

	if(!ticket)
		var bankbox := FindBankBox(master);
		if(bankbox)
			ticket := CreateItemInContainer(bankbox, CONFISCATION_TICKET_OBJTYPE);
			if(ticket)
				ticket_location := "bank";
			endif
		endif
	endif

	if(!ticket)
		KillConfiscatedPet(mobile);
		if(master_online)
			SendSysMessage(master, "Your pet " + pet_name + " was destroyed for entering a " + locationnoun + ". A claim ticket could not be created — your backpack and bank are full.", 3, 43);
		endif
		return;
	endif

	var fine := PopulateConfiscatedPetTicket(ticket, master_serial, mobile);

	KillConfiscatedPet(mobile);

	if(master_online)
		SendSysMessage(master, "Your pet " + pet_name + " has been confiscated for entering a " + locationnoun + ".");
		SendSysMessage(master, "A claim ticket has been placed in your " + ticket_location + ". Visit an Animal Trainer and pay " + fine + "gp to reclaim it.", 3, 43);
	endif
endfunction

////////////////////////////////////////////////////////////////////////////////////
// House sign "Info" tab support: owner last-login lookup, "at risk of escrow"
// determination, and a pretty house-type display name.
///////////////////////////////////////////////////////////////////////////////////
const FOUR_YEARS_SECONDS := 4 * 365 * 86400;

function ResolveHouseOwnerLastLogin(owner_acct)
	if(!owner_acct)
		return -1;
	endif

	var account := FindAccount(owner_acct);
	if(!account)
		return -1;
	endif

	var last_login := CInt(account.GetProp("LastLogin"));
	if(!last_login)
		return 0;
	endif

	return last_login;
endfunction

function FormatHouseOwnerLastLoginDate(last_login)
	if(last_login < 0)
		return "Unknown";
	elseif(!last_login)
		return "Never";
	endif

	return GetDateString(CInt(last_login));
endfunction

// Accounts with no recorded login (unknown or never) are treated as "at risk" --
// conservative, since there's no evidence of recent activity either way.
function IsOwnerAccountAtRisk(last_login)
	if(last_login <= 0)
		return 1;
	endif

	return ((POLCore().systime - last_login) > FOUR_YEARS_SECONDS);
endfunction

var _house_itemdesc_cfg := 0;

function ResolveHouseTypeDisplayName(objtype)
	if(!objtype)
		return "Unknown";
	endif

	if(!_house_itemdesc_cfg)
		_house_itemdesc_cfg := ReadConfigFile(":housing:itemdesc");
	endif

	if(_house_itemdesc_cfg)
		var house_elem := _house_itemdesc_cfg[objtype];
		if(house_elem and !house_elem.errortext)
			var house_name := GetConfigString(house_elem, "Name");
			if(house_name and !house_name.errortext and house_name != "")
				if(Len(house_name) > 5 && house_name[Len(house_name)-4, Len(house_name)] == "house")
					house_name := house_name[1, Len(house_name)-5];
				endif
				return house_name;
			endif
		endif
	endif

	return CStr(Hex(objtype));
endfunction