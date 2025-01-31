///////////////////////////////////////////////////////////
// SpawnPoint.inc
//
// Spawn Point related functions.
//
// Creator: Mercy
// Contact: mkow@sympatico.ca
//
// Created: 12-Sept-2000
// Updated: 12-Sept-2000
//
// var CProps used:
//	SpawnPoints	Array of spawn points.
//
// Item CProps used:
// 	CreateTime	Time rune was created.
//	Despawn		If this is set, all spawns are destroyed on spawn point destruction.
//	Disabled	If this prop exists, the spawn point is disabled.
// 	Expire		How long since creation (in mins) until point is destroyed?
// 	ExpireNum	How many spawns before point is destroyed?
//	Frequency	Minimum amount of time before can respawn, 0 = no time delay.
//	Group		If this is set, Spawn all NPC together, otherwise stagger 1 per cycle.
//	HrEnd		Hour to stop spawning, will kill the spawned creatures.
//	HrStart		Hour to start spawning.
//	Max		Max number to spawn.
//	NextSpawn	Time before next spawn can be created.
//	NextValidate	Time of next validation as per VALIDATE_INTERVAL.
//	Note		Notes on this spawn rune.
//	Num		Number currently spawned.
//	Range		How far can they wander (NPCs).
//	Spawns		Array of serials of objects spawned.
//	Template	NPC or object template to spawn.
//	Type		Type of spawn.
///////////////////////////////////////////////////////////

use os;
use uo;
use cfgfile;
use vitals;

include "include/clock";
include "include/random";
include "include/constants";
include ":lootlottery:include/lootlottery";

///////////////////////////////////////////////////////////
// CONSTS
///////////////////////////////////////////////////////////
const VALIDATE_INTERVAL		:= 60 * 5; // 5 minutes.

///////////////////////////////////////////////////////////
// Purpose: Add the point's serial to the var list.
// Params:  serial		Serial of the spawn point.
// Returns: N/A
///////////////////////////////////////////////////////////
function AddSpawnPoint(serial)
	set_critical(1);

	var spawnpoint := GetGlobalProperty("SpawnPoints");
	if (!spawnpoint)
		spawnpoint := array;
	endif

	spawnpoint.append(serial);
	SetGlobalProperty("SpawnPoints", spawnpoint);

	set_critical(0);
endfunction

///////////////////////////////////////////////////////////
// Purpose: Remove the point's serial from the var list.
// Params:  serial		Serial of spawn point.
// Returns: N/A
///////////////////////////////////////////////////////////
function RemoveSpawnPoint(serial)
	set_critical(1);

	var spawnpoint := GetGlobalProperty("SpawnPoints");
	if( !spawnpoint )
		return;
	endif

	var num;
	for (num := 1; num <= spawnpoint.size(); num := num + 1)
		if (spawnpoint[num] == serial)
			spawnpoint.erase(num);
			break;
		endif
	endfor

	SetGlobalProperty("SpawnPoints", spawnpoint);

	set_critical(0);
endfunction

///////////////////////////////////////////////////////////
// Purpose: Sends a death event to the manager when npc dies.
//			**Call this from death.src.
// Params:	corpse		Ref to corpse object.
// Returns: N/A
///////////////////////////////////////////////////////////
function SpawnPointDeath(corpse);
	var pointserial := GetObjProperty(corpse, "SpawnPoint");
	if (!pointserial)
		return;
	endif

	var npcserial := GetObjProperty(corpse, "AiSerial");

	var ev := struct;
	ev.+type := EVID_SPAWNPOINT_DEATH;
	ev.+npcserial := npcserial;
	ev.+pointserial := pointserial;

	var pid := GetGlobalProperty("#SpawnPointManagerPID");
	var manager := GetProcess(pid);
	if( manager )
		manager.SendEvent(ev);
	endif

endfunction

///////////////////////////////////////////////////////////
// Purpose: Removes the serial from the point's list.
// Params:  point		Ref to point object.
//			npcserial	Serial of NPC being removed.
// Returns: N/A
///////////////////////////////////////////////////////////
function RemoveSerialFromPoint(byref point, npcserial)
	var serials := GetObjProperty(point, "Serials");
	var num;
	for (num := 1; num <= serials.size(); num := num + 1)
		if (serials[num] == npcserial)
			serials.erase(num);
			break;
		endif
	endfor

	SetObjProperty(point, "Serials", serials);

	// Now set NextSpawn.
	var freq := CInt(GetObjproperty(point, "Frequency"));
	var nextspawn := ReadGameClock() + (freq * 60);
	SetObjProperty(point, "NextSpawn", nextspawn);
endfunction

///////////////////////////////////////////////////////////
// Purpose: Attempt to spawn something.
// Params:  point		Spawn point to spawn for.
// Returns: 1		Something was spawned.
//			0		Nothing was spawned.
///////////////////////////////////////////////////////////
function Spawn(byref point)

	Set_Critical( 1 );
	// NOTE: Disabled runes never expire either!
	if (GetObjProperty(point, "Disabled"))
		Set_Critical( 0 );
		return 0;
	endif

	// Done periodically.
	ValidatePointSpawns(point);

	var type := GetObjProperty(point, "Type");
	var template := GetObjProperty(point, "Template");

	var serials := GetObjProperty(point, "Serials");
	if (!serials)
		serials := array;
	endif

	var maxtime := CInt(GetObjProperty(point, "Max"));

	// Check for expiry based on time.
	var expiretime := CInt(GetObjProperty(point, "Expire")) * 60; // In mins.
	if (expiretime > 0)
		// Calculate how much time left.
		var whenexpire := CInt(GetObjProperty(point, "CreateTime")) + expiretime;
		var minsleft := (whenexpire - ReadGameClock()) / 60;
		if (minsleft <= 0)
			DeletePoint(point);
			Set_Critical( 0 );
			return 0;
		endif
		SetObjProperty(point, "Expire", minsleft);
	endif

	// Anything to spawn?
	if( template == "nil" or !template )
		Set_Critical( 0 );
		return 0;
	endif
	
	if( !CanSpawn(point) )
		Set_Critical( 0 );
		return 0;
	endif

	// How many to spawn?
	var numtospawn := maxtime - serials.size();
	if (numtospawn <= 0)
		Set_Critical( 0 );
		return 0;
	endif

	// Check for expiry based on number.
	var deleterune := 0;
	var expirenum	 := Cint( GetObjProperty(point, "ExpireNum") );
	if( expirenum )
		var expireleft := expirenum - numtospawn;
		if( expireleft <= 0 )
			numtospawn := expirenum;
			deleterune := 1;
		else
			SetObjProperty( point, "ExpireNum", expireleft );
		endif
	endif

	// Check if grouping, otherwise stagger.
	if( !GetObjProperty(point, "Group") )
		if( numtospawn > 1 )
			var freq := Cint( GetObjProperty(point, "Frequency") );
			DelaySpawning( point , freq );
		endif
		
		numtospawn := 1;
	endif

	var num;
	for( num := 0; num < numtospawn; num := num + 1 )
		case (type)
			"NPC":		if( !CreateSpawnPointNpc( point, template ) )
						Set_Critical( 0 );
						return 0;
					endif
					break;

			"Group":	if( !CreateSpawnPointNpcFromGroup( point, Cint(template) ) )
						Set_Critical( 0 );
						return 0;
					endif
					break;
					
			"Item":		if( !CreateSpawnPointItem( point, template ) )
						Set_Critical( 0 );
						return 0;
					endif
					break;

			"Container":
			
			

			default:
				syslog("Unknown SpawnPoint type: " + type);
				Set_Critical( 0 );
				return 0;
		endcase
	endfor

	if (deleterune)
		DeletePoint(point);
	endif

	Set_Critical( 0 );
	return 1;
	
endfunction

///////////////////////////////////////////////////////////
// Purpose:	Checks if it is time to spawn also despawns
//		any existing spawns if we can't spawn right now!
//
// Params:	point
// Returns:	1		Time to spawn.
//		0		Can't spawn yet.
///////////////////////////////////////////////////////////
function CanSpawn(byref point)

	var nextspawn := GetObjProperty(point, "NextSpawn");
	if (!nextspawn)
		nextspawn := 0;
	endif

	var clock := ReadGameClock();

	if( nextspawn > clock )
		DelaySpawning( point , Cint(nextspawn-clock) );
		return 0;
	endif

	// Check if it right time to spawn.
	var starthr := CInt(GetObjProperty(point, "HrStart"));
	var endhr := CInt(GetObjProperty(point, "HrEnd"));

	if ((starthr != 0 and endhr != 0))
		var hrnow := GetHourOfDay();
		if (!HourWithinRange(hrnow, starthr, endhr))
			DespawnAll(point); // Despawn all, can't spawn now.
			DelaySpawning( point , GetNumberOfSecondsBeforeStartingHour( hrnow , starthr ) );
			return 0;
		endif
	endif
	
	var freq := Cint( GetObjProperty(point, "Frequency") );
	nextspawn  := clock + (freq * 60);
	SetObjProperty( point, "NextSpawn", nextspawn );

	return 1;
	
endfunction

///////////////////////////////////////////////////////////
// Purpose: Spawns all points.
// Params:  none
// Returns: N/A
///////////////////////////////////////////////////////////
function SpawnAllPoints()
	var point;
	var spawnpoints := GetGlobalProperty("SpawnPoints");

	// This also does cleanup if there are problems.
	var num := 1;
	foreach serial in spawnpoints
		point := SystemFindObjectBySerial(cint(serial));
		if (!point)
			spawnpoints.erase(num);
			continue;
		endif

		Spawn(point);
		num := num + 1;
		Sleepms(500);
	endforeach

	SetGlobalProperty("SpawnPoints", spawnpoints);
endfunction

///////////////////////////////////////////////////////////
// Purpose: Kills/destroys all spawns.
// Params:  point		Ref to spawn point object.
// Returns: N/A
///////////////////////////////////////////////////////////
function DespawnAll(byref point)
	var ptobj;
	var spawno;
	var type := GetObjProperty(point, "Type");

	foreach spawnsr in GetObjProperty(point, "Serials")
		spawno := SystemFindObjectBySerial(cint(spawnsr));
		case (type)
			"NPC":
			"Group":	// Kill all NPCs.
					RevokePrivilege( spawno , "invul" );
					SetObjProperty(  spawno , "guardkill" , 1 );
					ApplyRawDamage(  spawno , Cint(spawno.maxhp + 3) );
					break;

			"Container":
				// Destroy the container, all within will be destroyed too.

			"Item":		// Destroy all spawned items even if not on the ground.
					DestroyItem( spawno );
					break;

		endcase
	endforeach
endfunction

///////////////////////////////////////////////////////////
// Purpose: Destroys the spawn point.
// Params:  point		Ref tospawn point obj.
// Returns: N/A
///////////////////////////////////////////////////////////
function DeletePoint(byref point)
	//var type := GetObjProperty(point, "Type");
	//var template := GetObjProperty(point, "Template");
	//var max := CInt(GetObjProperty(point, "Max"));
	var despawn := GetObjProperty(point, "Despawn");
	//var dtxt := "";
	if (despawn)
		//dtxt := "Despawning all spawns.";
		DespawnAll(point);
	endif
	//syslog("SpawnPoint (" + type + ": " + max + " " + template + ") expired. " + dtxt);
	DestroyItem(point);
endfunction

///////////////////////////////////////////////////////////
// Purpose: Validates a point's serials list, and removes
//			invalid entries.
// Params:  point		Ref to point to validate.
// Returns: N/A
///////////////////////////////////////////////////////////
function ValidatePointSpawns(byref point)
	var nextval := CInt(GetObjProperty(point, "NextValidate"));
	if (!nextval)
		nextval := 0;
	endif

	// Check if time to validate again.
	var clock := ReadGameClock();
	if (nextval > clock)
		return 0;
	endif
	nextval := clock + VALIDATE_INTERVAL;
	SetObjProperty(point, "NextValidate", nextval);

	var serials := GetObjProperty(point, "Serials");
	if (serials.size() <= 0)
		return 0;
	endif

	var obj;
	var idx;
	for (idx := 1; idx <= serials.size(); idx := idx + 1)
		obj := SystemFindObjectBySerial(cint(serials[idx]));
		if (!obj)
			serials.erase(idx);
		endif
	endfor

	SetObjProperty(point, "Serials", serials);
endfunction

///////////////////////////////////////////////////////////
// Purpose:	Create an npc for a spawn point
//
// Params:	point		The rune
//		template	The npctemplate
//
// Returns:	npc		Ref to mobile object if successful, 0 if not
///////////////////////////////////////////////////////////
function CreateSpawnPointNpc( byref point, template )

	var forbidden := GetGlobalProperty( "ForbiddenTemplates" );
	if( !forbidden )
		forbidden := array;
	endif
	
	if( template in forbidden )
		return 0;
	endif
	
	var x := point.x;
	var y := point.y;
	var z := point.z;
	var realm := point.realm;
	var range	:= Cint( GetObjProperty( point , "Range" ) );
	var critter	:= CreateNpcFromTemplate( template, 5283, 1184, 0, 0, "britannia" );
	if( !critter )
		return 0;
	endif
	
	var serials := GetObjProperty( point , "Serials" );
	if( !serials )
		serials := array;
	endif
	
	if( range )
		var anchor	:= struct;
		anchor.+x	:= point.x;
		anchor.+y	:= point.y;
		anchor.+realm := point.realm;
		anchor.+range	:= range;
		anchor.+psub	:= Cint(100/range);
		SetObjProperty(critter, "Anchor", anchor);
	endif
	
	SetObjProperty( critter, "SpawnPoint", point.serial );
	SetObjProperty( critter, "AiSerial", critter.serial );
	if(Getobjproperty(point, "LottoDrops"))
		CopyLottery(point, critter);
	endif

	if( !MoveObjectToLocation( critter, x, y, z, realm, MOVEOBJECT_FORCELOCATION ) )
		RevokePrivilege( critter , "invul" );
		SetObjProperty(  critter , "guardkill" , 1 );
		ApplyRawDamage(  critter , Cint(critter.maxhp + 3) );
		return 0;
	endif
	
	RestartScript( critter );
	serials.append(critter.serial);
	SetObjProperty(point, "Serials", serials);
	
	return critter;
	
endfunction

///////////////////////////////////////////////////////////
// Purpose: 	Create a npc from goups.cfg
// Params: 	point		The spawn rune
//		group		The npc group
// Returns:	npc		The npc ref if successful, 0 if not
///////////////////////////////////////////////////////////
function CreateSpawnPointNpcFromGroup( byref point, group )
	
	var cfg := ReadConfigFile( ":spawnpoint:groups" );
	if( !cfg )
		return 0;
	endif
	
	var elem	:= cfg[group];
	var possibles	:= GetConfigStringArray( elem , "spawn" );
	if( !possibles.size() )
		return 0;
	endif
	
	var template	:= possibles[ Random( possibles.size() ) + 1 ];
	if( !template )
		return 0;
	endif
	
	var critter	:= CreateSpawnPointNpc( point, template );
	if( !critter )
		return 0;
	endif
	
	return critter;
	
endfunction

///////////////////////////////////////////////////////////
// Purpose: 	Create a npc from goups.cfg
// Params: 	point		Item reference of the point
//		delay		Delay before the next spawn
// Returns:	nothing
///////////////////////////////////////////////////////////
function DelaySpawning( point , delay )

	var parms  := struct;
	parms.+point := point;
	parms.+delay := Cint(delay + 5); // +5 to prevent lagging issues to alter spawning
	
	Start_Script( ":spawnpoint:delayedspawning" , parms );
	
endfunction

///////////////////////////////////////////////////////////
// Purpose:	Create an item for a spawn point
//
// Params:	point		The rune
//		data		item name and amount
//
// Returns:	npc		Ref to mobile object if successful, 0 if not
///////////////////////////////////////////////////////////
function CreateSpawnPointItem( byref point, data )

	var amount;
	var objtype;	
	var template;
	
	data	 := SplitWords( data );
	objtype	 := Cint( data[1] );
	template := data[1];
	
	if( !objtype )
		objtype := GetObjtypeByName(template);
	endif
	
	if( !objtype )
		return 0;
	endif
	
	var forbidden := GetGlobalProperty( "ForbiddenTemplates" );
	if( !forbidden )
		forbidden := array;
	endif
	
	if( (template in forbidden) or (Cstr(objtype) in forbidden) )
		return 0;
	endif
	
	if( RandomDiceStr( data[2] ) )
		amount := RandomDiceStr( data[2] );
    	else
        	amount := 1;
    	endif
	
	var x;
	var y;
	var z;
	var realm;
	var item;
	var try	:= 1;
	var range	:= Cint( GetObjProperty( point , "Range" ) );
	
	while( !item and try <= 3 )
		
		if( range )
			x := point.x + Cint( Random((range*2)+1) - range );
			y := point.y + Cint( Random((range*2)+1) - range );
			z := Cint( GetWorldHeight( x, y, realm ) );
			realm := point.realm;
		else
			x := point.x;
			y := point.y;
			z := point.z;
			realm := point.realm;
		endif
		
		item := CreateItemAtLocation( x, y, z, objtype, amount, realm );
	
	endwhile

	if( !item )
		return 0;
	endif

	var serials := GetObjProperty( point , "Serials" );
	if( !serials )
		serials := array;
	endif

	SetObjProperty( item, "SpawnPoint", point.serial );
	serials.append( item.serial );
	SetObjProperty( point, "Serials", serials );
	
	return item;
	
endfunction