use uo;
use os;

include "include/constants/propids";

function EnsureSpawnPointSettings( byref point )

	var pt_data := GetObjProperty( point, PROPID_SPAWNPOINT_SETTING );
	if( !pt_data )
		pt_data	    := {};
		pt_data[1]  := "NPC";
		pt_data[2]  := "nil";
		pt_data[3]  := 0;
		pt_data[4]  := 0;
		pt_data[5]  := 4;
		pt_data[6]  := 0;
		pt_data[7]  := 0;
		pt_data[8]  := 0;
		pt_data[9]  := 1;
		pt_data[10] := 0;
		pt_data[11] := 0;
		pt_data[12] := 0;
		pt_data[13] := 0;
		pt_data[14] := "";
		pt_data[15]	:= 0;
		SetObjProperty( point, PROPID_SPAWNPOINT_SETTING, pt_data );
	endif

	return pt_data;

endfunction


function GetSpawnedCount( byref point )

	var serials := GetObjProperty( point, PROPID_SPAWNPOINT_SPAWNED_OBJECTS );
	if( !serials )
		return 0;
	endif

	return serials.size();

endfunction


function RestartSpawnPointWithMode( byref point, spawn_to_max := 0 )

	var pt_data := EnsureSpawnPointSettings( point );

	Run_Script( ":spawnpoint:despawner", point );
	EraseObjProperty( point, PROPID_SPAWNPOINT_NEXT_VALIDATION );
	EraseObjProperty( point, "Triggered" );

	if( !spawn_to_max )
		var frequency := CInt( pt_data[6] );
		if( frequency > 0 )
			SetObjProperty( point, PROPID_SPAWNPOINT_NEXT_SPAWNING, ReadGameClock() + (frequency * 60) );
		else
			EraseObjProperty( point, PROPID_SPAWNPOINT_NEXT_SPAWNING );
		endif
		return 1;
	endif

	var target_max := CInt( pt_data[3] );
	if( target_max <= 0 )
		EraseObjProperty( point, PROPID_SPAWNPOINT_NEXT_SPAWNING );
		return 1;
	endif

	var original_group_flag := CInt( pt_data[8] );
	var temp_group_enabled := 0;
	if( !original_group_flag )
		pt_data[8] := 1;
		SetObjProperty( point, PROPID_SPAWNPOINT_SETTING, pt_data );
		temp_group_enabled := 1;
	endif

	var tries := 0;
	while( tries < (target_max + 10) )
		var current_count := GetSpawnedCount( point );
		if( current_count >= target_max )
			break;
		endif

		if( CInt(pt_data[15]) )
			SetObjProperty( point, "Triggered", 1 );
		endif

		EraseObjProperty( point, PROPID_SPAWNPOINT_NEXT_SPAWNING );
		Run_Script( ":spawnpoint:checkpoint", point );
		tries := tries + 1;
		SleepMs(5);
	endwhile

	if( temp_group_enabled )
		pt_data := EnsureSpawnPointSettings( point );
		pt_data[8] := original_group_flag;
		SetObjProperty( point, PROPID_SPAWNPOINT_SETTING, pt_data );
	endif

	EraseObjProperty( point, "Triggered" );
	return 1;

endfunction
