use boat;
use storage;
use os;
use uo;

include "include/objtype";

Const KEYSTART := 0x100E;
Const KEYEND := 0x1013;
Const KEYRING := 0x1011;

function IsExtended( plank )
  return plank.graphic in { GID_PLANK_EXTENDED_FACING_EAST, GID_PLANK_EXTENDED_FACING_WEST, GID_PLANK_EXTENDED_FACING_NORTH, GID_PLANK_EXTENDED_FACING_SOUTH };
endfunction                           

function IsPlankOccupied( plank )
  if (!plank or !plank.realm)
    return 0;
  endif
  return ListMobilesNearLocation( plank.x, plank.y, plank.z, 0, plank.realm ).size();
endfunction

function Extend( plank )
  if (!IsExtended(plank))
    TogglePlank(plank);
  endif
endfunction

function Retract( plank )
  if (IsExtended(plank))
    TogglePlank(plank);
  endif
endfunction

function TogglePlank( plank )
  var partnertype := PlankPartner( plank.graphic );
  if (partnertype)
    plank.graphic := partnertype;
  endif
endfunction

function PlankPartner( planktype )
  case (planktype)
    GID_PLANK_EXTENDED_FACING_EAST:  return GID_PLANK_RETRACTED_FACING_EAST;   // 0x3ed5: return 0x3eb1;
    GID_PLANK_RETRACTED_FACING_EAST: return GID_PLANK_EXTENDED_FACING_EAST;    // 0x3eb1: return 0x3ed5;
    GID_PLANK_EXTENDED_FACING_WEST:  return GID_PLANK_RETRACTED_FACING_WEST;   // 0x3ed4: return 0x3eb2;
    GID_PLANK_RETRACTED_FACING_WEST: return GID_PLANK_EXTENDED_FACING_WEST;    // 0x3eb2: return 0x3ed4;
    GID_PLANK_EXTENDED_FACING_NORTH:  return GID_PLANK_RETRACTED_FACING_NORTH; // 0x3e84: return 0x3e85;
    GID_PLANK_RETRACTED_FACING_NORTH: return GID_PLANK_EXTENDED_FACING_NORTH;  // 0x3e85: return 0x3e84;
    GID_PLANK_EXTENDED_FACING_SOUTH:  return GID_PLANK_RETRACTED_FACING_SOUTH; // 0x3e89: return 0x3e8a;
    GID_PLANK_RETRACTED_FACING_SOUTH: return GID_PLANK_EXTENDED_FACING_SOUTH;  // 0x3e8a: return 0x3e89;
    default: syslog( "Unknown plank type 0x" + Hex( planktype ) );
             return error;
  endcase
endfunction

function FK(me, plank)
  var doorlockid := GetObjProperty( plank, "lockid" );
  var keylockid;
  var insidering;
  var MyKeys := EnumerateItemsInContainer(me.backpack);
  foreach key in MyKeys
	if ( (key.objtype >= KEYSTART ) and (key.objtype <= KEYEND) and (key.objtype != KEYRING ) )
	  keylockid := GetObjProperty( key, "lockid" );
	  if ( (keylockid) and (keylockid == doorlockid) )
		return 1;
	  endif
	elseif (key.objtype == KEYRING)
	  insidering := FindMyPack(key.serial);
	  foreach otherkey in EnumerateItemsInContainer(insidering)
		keylockid := GetObjProperty( otherkey, "lockid" );
		if (keylockid == doorlockid)
		  return 1;
		endif
	  endforeach
	endif
  endforeach				
  return 0;
endfunction

function OpenTamedStorageAreas()
  var bank := FindStorageArea( "Tamed Storage" );
  if (!bank)
    bank := CreateStorageArea( "Tamed Storage" );
  endif
  return bank;
endfunction

function FindMyPack(myserial)
  var mybank := OpenTamedStorageAreas();
  var bank_obj_name := "Bankbox  " + hex(myserial);
  var bankbox := FindRootItemInStorageArea( mybank, bank_obj_name );
  if (!bankbox)
    bankbox := CreateRootItemInStorageArea(mybank, bank_obj_name, UOBJ_BANKBOX  );
  endif
  return bankbox;
endfunction
