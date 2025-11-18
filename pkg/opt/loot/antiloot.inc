use uo;
use os;

include "include/client";
include "include/time";
include "include/classes";
include "include/dotempmods";
include "include/areas";
include "include/constants/locations";


////////////////////////////////////////////////////////////////////////////////////
//
// antilooting() - The can_remove script for corpses.
//
/////////////////////////////////////////////////////////////////////////////////////
function antilooting( who, corpse, item )
	var amode ;
	var serial := GetObjProperty( corpse, PROPID_CORPSE_CORPSE_OF );
	var isrper := GetObjProperty( who, "IsRPer" );
	var rpercorpse := GetObjProperty( corpse, PROPID_CORPSE_IS_RPER );
	
	// loot delay
	var lastloot := GetObjProperty(who, "#LastLoot");
	
	if (!lastloot)
		lastloot := {ReadGameClock(), 0};
	endif
	
	// NO delay for looting own corpse, - there is now
	//if (serial != who.serial)
		// It's the same time, so player JUST looted!
		if (lastloot[1] >= ReadGameClock())
			// player has already looted 2 items in the past 1 second
			if (lastloot[2] >= 2)
				return 0;
			else
				// player hasn't looted 2 items, add to counter
				lastloot[2] := lastloot[2] + 1;
			endif
			SetObjProperty(who, "#LastLoot", lastloot);
		else
			SetObjProperty(who, "#LastLoot", {ReadGameClock(), 0});
		endif
		
	//endif
	
	// Toadstool fix. NPCS will now display who looted them. can loot npcs anywhere
	if (GetObjProperty(corpse, "npctemplate"))
		return AddLooter( who, corpse, item );
	endif
	
	if( !serial )
		return 1;
	elseif( who.cmdlevel > 0 )
		return 1;
	elseif( serial == who.serial )
		return 1;		
	elseif( GetObjProperty( corpse, PROPID_MOBILE_CANT_BE_LOOTED ) )
		return 0;
	elseif( IsInAntiLootingArea(who) )
		SendSysMessage( who, "You can't loot here.", FONT_NORMAL, 2595  );
		return 0;
	elseif(!isrper == rpercorpse)
		SendSysMessage( who, "Only RPers may loot each other.", FONT_NORMAL, 2595  );
		return 0;
	else
		var guild1 := GetObjProperty( who, "guild_id" );
		if( guild1 )
			var guild2 := GetObjProperty( who, "guild_id" );
			if( guild2 )
				if( guild1 == guild2 )
					return AddLooter( who, corpse, item );
				else
					guild1 := FindGuild( guild1 );
					guild2 := FindGuild( guild2 );
					if( guild1.IsEnemyGuild(guild2) )
						return AddLooter( who, corpse, item );
					elseif( guild1.IsAllyGuild(guild2) )
						return AddLooter( who, corpse, item );
					endif
				endif
			endif
		endif
		
		if( IsInAntiLootingArea(who) )
			SendSysMessage( who, "You can't loot here." );
			return 0;
		elseif( GetObjProperty( corpse, PROPID_CORPSE_IS_BLUE ) )
			who.setCriminal(1);
			amode := GetGlobalProperty("AntiLootMode");	
			if (amode != "OFF")
				if ( IsInAProtectedCity(who) )
					if (amode == "JAIL")
						Report( who, corpse );
						AutoJail(who);
					elseif (amode == "ON")
					SendSysMessage( who, "You have been reported for attempted illegal looting" );
					endif
					return 0;
				endif		
			endif
		endif
	endif
	var rmode := GetGlobalProperty("GlobalNoLoot");	
	if (rmode && !who.cmdlevel)
		SendSysMessage( who, "You can't loot players during an event - You have been reported for attempting to loot." );
		return 0;
	else
		Report( who, corpse );
		return AddLooter( who, corpse, item );
	endif

	return AddLooter( who, corpse, item );

endfunction


function AddLooter( looter, object, item )

	// Add looters name to looters if not already done
	var looters := GetObjProperty(object, PROPID_CORPSE_LOOTED_BY);
	var victim;
	if( !looters )
		looters := dictionary;
	endif

	if( !looters.exists(looter.serial) )
		looters[looter.serial] := looter.name;
		SetObjProperty(object, PROPID_CORPSE_LOOTED_BY, looters);
	endif

	// Record the taken item in an array. This will be HUGE
	// {desc, graphic, serial, time, looter}
	var items := GetObjProperty(object, "LootedItems");
	if (!items)
		items := {};
	endif

	items.append({item.desc, {item.graphic, item.color}, item.serial, ReadGameClock(), looter.serial});
	if(items)
		SetObjProperty(object, "LootedItems", items);
	endif
	
return 1;

endfunction

function Report( looter, object )
	var looters := GetObjProperty(object, PROPID_CORPSE_LOOTED_BY);
	var victim;
	if( !looters )
		looters := dictionary;
	endif

	if( !looters.exists(looter.serial) )
		//Report Code
		foreach character in EnumerateOnlineCharacters()
			if (character.serial == object.serial)
				victim := character.name;
			endif
		endforeach
		if (!victim)
			victim := object.name;
		endif
//		foreach character in EnumerateOnlineCharacters()
//			if( character.cmdlevel > 0 )
//SendSysmessage( character, looter.name +" looted "+victim+" at //"+object.x+","+object.y+","+object.z, 3 ,130  );
//			endif
//		endforeach
//		looters[looter.serial] := looter.name;
//		return SetObjProperty(object, PROPID_CORPSE_LOOTED_BY, looters);
	else
		return 0;
	endif
endfunction

function AutoJail (looter)
	var days;
	var acc	:= looter.acct;
	var note := acc.getprop("Notes");
	var offence := GetObjProperty(looter,"autojail");
	if (!offence)
		offence := 0;
	endif
	offence := offence + 1;
	SetObjproperty(looter, "autojail",offence);
	if (offence > 4)
		offence := 4;
	endif
	case (offence)
		1:	days := 3;
		2:	days := 7;
		3:	days := 14;
		4: 	days := 28;
	endcase
	var jaildays := (days * 86400);
	var secs := polcore().systime;
	var year;
	var month;
	var day;
	var hour;
	var minutes;
	CalculateDate( secs, year, month, day );
	CalculateTime( secs, hour, minutes );
	if (minutes < 10)
	note := note + " - "+month+"/"+day+"/"+year+" ("+hour+":0"+minutes+") town looting : "+days+" days";
	else
	note := note + " - "+month+"/"+day+"/"+year+" ("+hour+":"+minutes+") town looting : "+days+" days";
	endif
	acc.setprop("Release", (ReadGameClock() + jaildays));
	acc.setprop("Notes", note);
	looter.frozen := 1;
	MoveObjectToLocation( looter, DEFAULT_LOCATION_JAIL_X, DEFAULT_LOCATION_JAIL_Y, DEFAULT_LOCATION_JAIL_Z, DEFAULT_LOCATION_JAIL_R, MOVEOBJECT_FORCELOCATION);
	SendSysMessage( looter, "You have been jailed for attempted town looting.");
	DisconnectClient( looter );
endfunction

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// insertintocorpse() - A can_remove script for corpses, passed from main container script.
// 											This is to check all items added to the corpse.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
function insertintocorpse(who, corpse, item);

	var looteditems := GetObjProperty(corpse, "LootedItems");

	if (!looteditems)
	//	return 1;
	endif

	var looteditemsmod := looteditems;

	var i := 1;
	var lootnum := 0;
	foreach thing in looteditems
		if (thing[5] == who.serial)
			if (thing[3] == item.serial)
				// Erase item from looted items since it was put back
				looteditemsmod.erase(i);
			endif
			lootnum := lootnum + 1;
		endif
		i := i + 1;
	endforeach
	

	if (looteditemsmod.size() < 1)
		EraseObjProperty(corpse, "LootedItems");
		EraseObjProperty(corpse, PROPID_CORPSE_LOOTED_BY);

	else

	if(looteditemsmod)
		SetObjProperty(corpse, "LootedItems", looteditemsmod);
	endif

		// All items put back, remove looter tag

		if (lootnum <= 1)

			// Taken from antiloot.src, I've never accessed dictionary like that...
			var looters := GetObjProperty(corpse, PROPID_CORPSE_LOOTED_BY);
			if( looters.exists(who.serial) )
				looters.erase(who.serial);	
				SetObjProperty(corpse, PROPID_CORPSE_LOOTED_BY, looters);
			endif
		endif	

	endif

	return 1;

endfunction

