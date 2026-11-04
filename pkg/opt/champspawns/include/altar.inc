//Edited September 20, 2024

use uo;
use os;
use util;
use cfgfile;

const INAC_WAIT := 600; // 10 mins to sleep when inactive
const BIG_CHECK_WAIT:=5;  //  5 secs, checks for % of dead monsters  - ideal would be 1 here, but its a bit... heavy loop, so im using 5
const TINY_WAIT := 10;    // 10 msec to sleep between each spawn
const HOUR := 3600; //in seconds
const DAY := 86400; //in seconds
const ACTIVE_EACH := 5 * DAY;  // active every 5 + Random(2) days. So it can spawn at any moment between 5 and 7 days from completed
const ACTIVE_EACH_RANDOM := 2 * DAY;

const LVLDOWNCHECK := 600;  // checks for level down each 10 mins

function createAltar(loc, spawn_id)
  if(!loc)
    return;
  endif

  var item := CreateItemAtLocation(loc.x, loc.y, loc.z, "champaltar", 1, loc.realm);
  sleepms(500);

  if(item)
    var items := ListItemsNearLocationOfType( loc.x, loc.y, loc.z, 2, 0x9101, loc.realm);
    foreach pent in items
      SetObjProperty(pent, "spawn", CInt(spawn_id));
    endforeach;
  else
    Print("Error creating item: " + item);
  endif
endfunction

function ActivateAltar(altar)
  var pentagram := GetObjProperty(altar, "pentagram");

  foreach piece in pentagram
    piece:=SystemFindObjectBySerial(piece);
    piece.invisible:=0;
    piece.color:=0;
  endforeach

  SetObjProperty(altar, "active", 1);
  SetObjProperty(altar, "players", dictionary);
  EraseObjProperty(altar, "ChampionAt");
endfunction

function DeactivateAltar(altar)
  var pentagram := GetObjProperty(altar, "pentagram");

  foreach piece in pentagram
    piece:=SystemFindObjectBySerial(piece);
    piece.color:=980;
    piece.invisible:=1;
  endforeach

  var activationTime := POLCORE().systime + ACTIVE_EACH + RandomInt(ACTIVE_EACH_RANDOM);

  SetObjProperty(altar, "NextActiv", activationTime);
  EraseObjProperty(altar, "#completed");
  EraseObjProperty(altar, "active");
  EraseObjProperty(altar, "champ");
  EraseObjProperty(altar, "ChampionAt");
endfunction
