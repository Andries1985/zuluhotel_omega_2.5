use uo;
//use os;
//use util;
//use cfgfile;

//include ":champspawns:skulls";
//include ":champspawns:altar";
//include ":champspawns:spawning";
//include "include/client";

function wipeItemsArray(Items)
  foreach item in Items
    item := SystemFindObjectBySerial(item);

    if(!DestroyItem(item))
      DestroyItem(item);
    endif
  endforeach
endfunction

function createNextWhiteSkull(altar)
  var white_skull_level := CInt(GetObjProperty(altar, "white_skull_level"));
  var wskulls := array;
  wskulls := GetObjProperty(altar, "wskulls");
  
  if(white_skull_level == 4)
    WipeItemsArray(wskulls);
    wskulls:= {};
    SetObjProperty(altar, "wskulls", wskulls);
    EraseObjProperty(altar, "white_skull_level");
    CreateNextRedSkull(altar);
    return;
  endif

  var location := GetWhiteSkullLocation(altar, white_skull_level + 1);
  var wskull := CreateItemAtLocation(location[1], location[2], location[3], 0x1854, 1, altar.realm);

  wskull.movable := 0;

  if(!wskulls)
    wskulls := {wskull.serial};
  else
    wskulls.append(wskull.serial);
  endif

  PlaySoundEffect(wskull, 0x29);
  PlayStationaryEffect( wskull.x, wskull.y, wskull.z, 0x3728, 7, 0x10, 0, altar.realm );

  SetObjProperty(altar, "white_skull_level", white_skull_level + 1);
  SetObjProperty(altar, "wskulls", wskulls);

  return;
endfunction

function createNextRedSkull(altar)
  var lskull := CInt(GetObjProperty(altar, "wave"));
  var skulls := array;
  skulls := GetObjProperty(altar, "rskulls");

  var wskulls := array;
  wskulls := GetObjProperty(altar, "wskulls");
  WipeItemsArray(wskulls);
  wskulls:= {};
  SetObjProperty(altar, "wskulls", wskulls);
  EraseObjProperty(altar, "white_skull_level");

  if(lskull==16)
    WipeItemsArray(skulls);
    skulls:= {};
    SetObjProperty(altar, "rskulls", skulls);
    SetObjProperty(altar, "#completed", 1);
    EraseObjProperty(altar, "wave");
    return;
  endif

  var location := GetRedSkullLocation(altar, lskull+1);
  var skull := CreateItemAtLocation(location[1], location[2], location[3], 0x1854, 1, altar.realm);

  skull.color:=0x26;
  skull.movable := 0;

  if(!skulls)
    skulls := {skull.serial};
  else
    skulls.append(skull.serial);
  endif

  PlaySoundEffect(skull, 0x29);
  PlayStationaryEffect( skull.x, skull.y, skull.z, 0x3728, 7, 0x10, 0, altar.realm );

  SetObjProperty(altar, "wave", lskull+1);
  SetObjProperty(altar, "rskulls", skulls);

  return;
endfunction

function destroyLastRedSkull(altar)
  var lskull := CInt(GetObjProperty(altar, "wave"));
  var skulls := array;
  skulls := GetObjProperty(altar, "rskulls");

  if(!lskull)
    return;
  endif

  var skull := skulls[lskull];
  DestroyItem(SystemFindObjectBySerial(skull));
  skulls.erase(lskull);

  SetObjProperty(altar, "wave", lskull-1);
  SetObjProperty(altar, "rskulls", skulls);

  return;
endfunction

function getRedSkullLocation(altar, index)
  var x, y;
  if(index<5)
    x := index - 2;
    y := -2;
  elseif(index<9)
    x := 2;
    y := index-6;
  elseif(index<13)
    x := 10-index;
    y := 2;
  else
    x := -2;
    y := 14-index;
  endif

  return {altar.x+x, altar.y+y, altar.z};
endfunction

function getRedSkullProgressionLevel(altar)
  var redSkulls := CInt(GetObjProperty(altar, "wave"));

  if(redSkulls<6)
    return 0;
  elseif(redSkulls<10)
    return 1;
  elseif(redSkulls<14)
    return 2;
  else
    return 3;
  endif
endfunction

function getWhiteSkullLocation(altar, index)
  var x:=0, y:=0;

  case(index)
    default:
    0: x := -1; y := -1; break;
    1: x :=  1; y := -1; break;
    2: x :=  1; y :=  1; break;
    3: x := -1; y :=  1; break;
  endcase

  return {altar.x+x, altar.y+y, altar.z};
endfunction
