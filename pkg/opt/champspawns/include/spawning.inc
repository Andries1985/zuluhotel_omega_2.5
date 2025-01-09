include "titles";
include "include/attributes";
include "include/random";
include ":champspawns:include/skulls";


function getSpawnTemplateName(altar)  
  var fC := GetObjProperty(altar, "FirstCheck");
  if(!fC)
    Sleep(2);
    SetObjProperty(altar, "FirstCheck", 1);
  endif
  var cfg := ReadConfigFile("spawns");

  if ( cfg.errortext )
    PrintTextAbove(altar, "Couldnt open spawns.cfg");
    return error;
  endif

  cfg := cfg[GetObjProperty(altar, "spawn")];
  if(!cfg)
    PrintTextAbove(altar, "Couldnt get the cfg elem ["+GetObjProperty(altar, "spawn")+"]");
    sleep(5);
    return error;
  endif

  var spawn_name := "level"+ getRedSkullProgressionLevel(altar);
  var spawns := GetConfigStringArray(cfg, spawn_name);

  if(!spawns)
    PrintTextAbove(altar, "Could not get ConfigStringArray(cfg, \""+spawn_name+"\"");
    return error;
  endif

  return (spawns[RandomInt(spawns.size())+1]);
endfunction

function getChampionTemplateName(altar)
  var cfg := ReadConfigFile("spawns");

  if(cfg.errortext)
    PrintTextAbove(altar, "Couldnt open spawns.cfg");
    return error;
  endif

  cfg := cfg[GetObjProperty(altar, "spawn")];

  if(!cfg)
    PrintTextAbove(altar, "Couldnt get the cfg elem ["+GetObjProperty(altar, "spawn")+"]");
    return error;
  endif

  var spawn_name := "champion";
  var spawns := GetConfigStringArray(cfg, spawn_name);

  if(!spawns)
    PrintTextAbove(altar, "Could not get ConfigStringArray(cfg, \""+spawn_name+"\"");
    SetObjProperty(altar, "active", 0);
    return error;
  endif

  return (spawns[1]);
endfunction

function SpawnChampion(altar)
  PlayStationaryEffect(altar.x, altar.y, altar.z, 0x3728, 7, 0x10, 0, altar.realm);
  var creature := CreateNPCFromTemplate(getChampionTemplateName(altar), altar.x, altar.y, altar.z+5, 0, altar.realm);
  sleepms(100);
  SetObjProperty(creature, "noprovo", 1);
  SetObjProperty(creature, "altar", altar.serial);
  SetObjProperty(creature, "players", dictionary);

  var chance := Random(100)+1;
  // Creates the champion skull.
  if(chance > 60)
	  var bp := creature.backpack;
	  var item := CreateItemInContainer(bp, 0xfffd);
	  SetObjProperty(item, "Champion", GetObjProperty(altar, "spawn"));
	  var name := "Skull of " + GetSkullType(GetObjProperty(creature, "Champion"));
	  SetName(item, name);
	  item.desc := name;  
	  if(GetObjProperty(creature, "Champion") == 2)
		  item.color := 1948;
    elseif(GetObjProperty(creature, "Champion") == 3)
		  item.color := 1183;
	  endif
  endif

  SetObjProperty(altar, "champ", creature.serial);
  return creature;
endfunction

function SpawnCreature(altar)
  var loc := getSpawnLocation(altar);
  if(!loc)
	  return 0;
  endif
  PlayStationaryEffect(loc[1], loc[2], loc[3], 0x3728, 7, 0x10, 0, altar.realm);
  var creature := CreateNPCFromTemplate(getSpawnTemplateName(altar), loc[1], loc[2], loc[3], 0, altar.realm);
  if(creature)
    if(GetObjProperty(creature, "BaseStrmod") > 50)
      SetObjProperty(creature, "BaseStrmod", (GetObjProperty(creature, "BaseStrmod")*2));
    else
      SetObjProperty(creature, "BaseStrmod", 100);
    endif
	  SetHP(creature, GetMaxHP(creature));
    SetObjProperty(creature, "iChampSpawn", 1);
    SetObjProperty(creature, "sAltarSerial", altar.serial);
    SetObjProperty(creature, "noloot", 1);
    SetObjProperty(creature, "nocut", 1);
    SetObjProperty(creature, "event", 1);
    SetObjProperty(creature, "notame", 1);
    SetObjProperty(altar, "totalspawns", CInt(GetObjProperty(altar, "totalspawns"))+1);
    return creature;
  endif
endfunction

function getMaxSpawns(altar, csLevel)
  return ((100 + csLevel) - getRedSkullProgressionLevel(altar) * 12);
endfunction

function getAmountOfConcurrentSpawns(altar)
  var cfg := ReadConfigFile("spawns");
  var element := cfg[GetObjProperty(altar, "spawn")];
  var concurrency := GetConfigString(element, "concurrency");

  if(!concurrency)
    return 50;
  else
    return CInt(concurrency);
  endif
endfunction

function getSpawnLocation(altar)
	var range := 37;
	var x,y,direction;

	direction := RandomInt(2)+1;
	case(direction)
		1: x := altar.x + (RandomInt(range)+1);
		2: x := altar.x - (RandomInt(range)+1);
	endcase	
	
	direction := RandomInt(2)+1;
	case(direction)
		1: y := altar.y + (RandomInt(range)+1);
		2: y := altar.y - (RandomInt(range)+1);
	endcase
	var z := GetMapInfo(x, y, altar.realm).z;

	if(altar.z > z + 5)
		return 0;
	endif

	return {x, y, z};
endfunction
