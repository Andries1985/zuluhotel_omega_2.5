use storage;
use uo;
use os;

function find_or_create_storage ( areaname )

	var area := FindStorageArea ( areaname );
	if (!area)
		area := CreateStorageArea ( areaname );
		if (area)
			print ( "Storage Area '" + areaname + "' created." );
		endif
	endif
	return area;

endfunction

function find_or_create_item ( storage, name, objtype )

	var item := FindRootItemInStorageArea ( storage, name );
	if (item)
		return item;
	endif

	//didn't find, so create
	item := CreateRootItemInStorageArea ( storage, name, objtype );
	return item;

endfunction

// function IsLocationAccessible ( character, x, y, z )
//     var xd, yd, zd;
    
//     xd := character.x - x;
//     if (xd < -1 or xd > 1) 
//         return 0;
//     endif
    
//     yd := character.y - y;
//     if (yd < -1 or yd > 1) 
//         return 0;
//     endif
    
//     zd := character.z - z;
//     if (zd < -10 or zd > 10)
//         return 0;
//     endif

//     return CheckLosAt( character, x, y, z );

// endfunction

function coordist ( x1 , y1 , x2 , y2 )

	var xd := x1 - x2;
	var yd := y1 - y2;
	if (xd < 0)
		xd := -xd;
	endif
	if (yd < 0)
		yd := -yd;
	endif
	if (xd > yd)
		return xd;
	else
		return yd;
	endif

endfunction

function AllocLockId()

	set_critical (1);
	var lockid := GetGlobalProperty( "nextlockid" );
	if (!lockid)
		lockid := 1;
	endif
	SetGlobalProperty ( "nextlockid", lockid+1 );

	set_critical (0);

	return lockid;
endfunction

function GetDirection (character, x, y)

	var ns_desc := "";
	var ew_desc := "";

	if (y < character.y)
        	ns_desc := "north";
	elseif  (y > character.y)
		ns_desc := "south";
	endif

	if (x < character.x)
		ew_desc := "west";
	elseif  (x > character.x)
		ew_desc := "east";
	endif

	var total_desc := "";
	if (ns_desc == "" and ew_desc == "")
		total_desc := "nearby";
	else 
		total_desc :=ns_desc + ew_desc;
	endif

	return total_desc;

endfunction

// ///////////////////
// //  opens the bank storage area
// ///////////////////

// function OpenWorldBank()

// 	var bank := FindStorageArea( "World Bank" );
// 	if (!bank)
// 		bank := CreateStorageArea( "World Bank" );
// 	endif

// 	// this should not happen. FIXME, make sure and document that fact.
// 	if (!bank)
// 		syslog( "Unable to open or create world bank!" );
// 	endif

// 	return bank;

// endfunction



// ///////////////////
// // finds the bankbox for the given character
// ///////////////////

// function FindBankBox( character )

// 	var worldbank := OpenWorldBank();
// 	var bank_obj_name := "Bankbox of " + character.serial;
// 	var bankbox := FindRootItemInStorageArea( worldbank, bank_obj_name );

// 	if (!bankbox)
// 		bankbox := CreateRootItemInStorageArea( worldbank, bank_obj_name, UOBJ_BANKBOX );
// 	endif

// 	// should never happen.  FIXME make sure and document that fact
// 	if (!bankbox)
// 		SysLog( "Unable to find or create bankbox for " + character.serial );
// 	endif

// 	return bankbox;

// endfunction