use uo;
use os;
use npc;
use cfgfile;

function Inncheck(character)

  /*var element, rect;
  var cfgfile := ReadConfigFile("::innlocation");
  var entries := GetConfigStringKeys(cfgfile);
  foreach listing in entries
    element :=  cfgfile[listing];
    rect := element.rect;
    rect := SplitWords(rect);
    if((character.x >= Cint(rect[1])) and (character.x <= Cint(rect[3])) and (character.y >= Cint(rect[2])) and (character.y <= Cint(rect[4])))
      return 1;
      break;
    endif
  endforeach
  return 0;
  */

	var npc_cfg := ReadConfigFile(":*:npcdesc");
  var found := 0;

	foreach mobile in ListMobilesNearLocation( character.x, character.y, character.z, 5, character.realm )
    if( mobile.IsA(POLCLASS_NPC) )
      if (GetObjProperty(mobile, "MerchantType") == "innkeep")
        found := 1;
      endif
    endif
  endforeach

  if(found == 1)
    return 1;
  else
    return 0;
  endif

endfunction  