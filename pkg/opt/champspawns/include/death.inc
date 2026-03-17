include ":champspawns:rewards";

function ProcessChampSpawnCorpse(corpse)

  RegisterChampionSpawnKill(corpse);

endfunction

function ProcessChampionCorpse(corpse)
  if(!GetObjProperty(corpse, "Champion"))
    return;
  endif

  RainDownMoney(corpse);
  sleep(600); // Destroy the corpse in 10 minutes.

  var altar := GetAltarByCorpse(corpse);
  EraseObjProperty(altar, "players");
  DestroyItem(corpse);
endfunction

function RegisterChampionSpawnKill(corpse)
  if(!GetObjProperty(corpse, "iChampSpawn"))
    return;
  endif

  var killer := SystemFindObjectBySerial(GetObjProperty(corpse, "KilledBySerial"));
  var altar := SystemFindObjectBySerial(GetObjProperty(corpse, "sAltarSerial"));
  var players := GetObjProperty(altar, "players");

  if(players[killer.serial])
    players[killer.serial] := CInt(players[killer.serial]) + 1;
  else
    players[killer.serial] := 1;
  endif

  SetObjProperty(altar, "players", players);
endfunction

function GetAltarByCorpse(corpse)
  return SystemFindObjectBySerial(GetObjProperty(corpse, "altar"));
endfunction
