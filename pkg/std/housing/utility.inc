use uo;
use os;

include "setup";

const VIEW_SECURE := 2;
const ADD_TO_SECURE := 3;
const REMOVE_FROM_SECURE := 4;
const RECALL_TO := 5;
const GATE_TO := 6;
const GATE_FROM := 7;
const CO_OWNER := 8;

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
				if (!MoveObjectToLocation(item, container.x, container.y, container.z, container.realm))
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
function UpdateFriendFunction(house, sign)

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

	function RegisterHouseType(deed, house)

	var name := StrReplace(deed.desc, "a deed to a ", "");

	SetObjProperty(house, "MultiType", name);

endfunction