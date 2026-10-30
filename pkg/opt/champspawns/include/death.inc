include ":champspawns:rewards";

function ProcessChampSpawnCorpse(corpse)

  RegisterChampionSpawnKill(corpse);

endfunction

// An altar champion pays the reward bags from its altar's kill tally (rewards.inc), then rains its
// gold. A champion with no altar - summoned by a champion relic (SummonedByRelic) or by staff - has
// no tally, so it only rains gold.
function ProcessChampionCorpse(corpse)
  if(!GetObjProperty(corpse, "Champion"))
    return;
  endif

  // Read the altar now: the bags are paid at the moment of death, not after the corpse's 10 minutes.
  var altar := GetAltarByCorpse(corpse);
  if(altar)
    GiveRewardsToPlayers(altar);
    EraseObjProperty(altar, "players");
  endif

  RainDownMoney(corpse);
  sleep(600); // Destroy the corpse in 10 minutes.

  DestroyItem(corpse);
endfunction

// One kill for the reward bag tally, kept on the altar as players[serial] := {kills, systime of
// the last kill}. Only kills while the altar is active count, and only by players (pets and
// Warriors for Hire count for their master, see ApplyTheDamage).
function RegisterChampionSpawnKill(corpse)
  if(!GetObjProperty(corpse, "iChampSpawn"))
    return;
  endif

  var altar_serial := CInt(GetObjProperty(corpse, "sAltarSerial"));
  if(!altar_serial)
    return;
  endif
  var altar := SystemFindObjectBySerial(altar_serial);
  if(!altar or !GetObjProperty(altar, "active"))
    return;
  endif

  var killer := FindChampionSpawnKiller(corpse);
  if(!killer)
    return;
  endif

  // Many spawn corpses can be processed at once (area spells): no other script may change the
  // tally between the read and the write.
  set_critical(1);
  var players := GetObjProperty(altar, "players");
  if(TypeOf(players) != "Dictionary")
    players := dictionary;
  endif

  // An entry written before Patch 1.1.5 is the bare kill count.
  var entry := players[killer.serial];
  var kills;
  if(TypeOf(entry) == "Array")
    kills := CInt(entry[1]);
  else
    kills := CInt(entry);
  endif
  players[killer.serial] := array{kills + 1, POLCore().systime};

  SetObjProperty(altar, "players", players);
  set_critical(0);

  // Patch 1.1.5: tell the player when the bag grows. .champkills shows the count at any time.
  kills := kills + 1;
  if(killer.connected and kills in ChampionBagTiers())
    var msg := " kills at this altar: your Champion Reward Bag grows.";
    if(kills == CHAMP_BAG_MIN_KILLS)
      msg := " kills at this altar: you have earned a Champion Reward Bag.";
    elseif(kills == ChampionBagTiers()[ChampionBagTiers().size()])
      msg := " kills at this altar: your Champion Reward Bag is as full as it gets.";
    endif
    SendSysMessage(killer, kills + msg, FONT_NORMAL, CHAMP_REWARD_COLOR);
  endif
endfunction

// The player who killed a spawn creature: the corpse's KilledBySerial (set by ApplyTheDamage), or
// failing that the player who hit it last according to its hitlist. 0 when neither is a player.
function FindChampionSpawnKiller(corpse)
  var serial := CInt(GetObjProperty(corpse, "KilledBySerial"));
  if(serial)
    var killer := SystemFindObjectBySerial(serial, SYSFIND_SEARCH_OFFLINE_MOBILES);
    if(IsChampionCreditable(killer))
      return killer;
    endif
    // ".akill <range> credit" (testing altars): the staff member who ran it gets the kill.
    if(killer and killer.cmdlevel and CInt(GetObjProperty(corpse, "StaffChampCredit")) == serial)
      return killer;
    endif
  endif

  // Hitlist entries are {serial, factor, game clock of the last hit}.
  var hitlist := GetObjProperty(corpse, PROPID_MOBILE_HITLIST);
  if(TypeOf(hitlist) != "Array")
    return 0;
  endif
  var newest := 0;
  var newest_time := -1;
  foreach hit in hitlist
    if(CInt(hit[3]) > newest_time)
      var mobile := SystemFindObjectBySerial(CInt(hit[1]), SYSFIND_SEARCH_OFFLINE_MOBILES);
      if(IsChampionCreditable(mobile))
        newest := mobile;
        newest_time := CInt(hit[3]);
      endif
    endif
  endforeach
  return newest;
endfunction

function IsChampionCreditable(mobile)
  if(!mobile or !mobile.IsA(POLCLASS_MOBILE))
    return 0;
  endif
  return (!mobile.npctemplate and !mobile.cmdlevel);
endfunction

// The altar that spawned this champion, or 0 for a champion with no altar.
function GetAltarByCorpse(corpse)
  var serial := CInt(GetObjProperty(corpse, "altar"));
  if(!serial)
    return 0;
  endif
  return SystemFindObjectBySerial(serial);
endfunction
