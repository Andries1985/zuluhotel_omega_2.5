include "include/client";
include ":Events:bag";

function RainDownMoney(corpse)
  var tile, blocked;
  var i;
  for (i := -10; i <= 10 ; i := i + 1)
    var j;
    for (j := -10; j <= 10; j := j + 1)
      tile := GetMapInfo(i, j, corpse.realm);
      if(tile)
		blocked := 0;
		var altar := ListItemsAtLocation( corpse.x+i, corpse.y+j, corpse.z, corpse.realm );
		foreach item in altar
			if(item.color == 1157)
				blocked := 1;
			endif
		endforeach
		if(!blocked)
			if(CheckLosBetween( corpse.x+i, corpse.y+j, corpse.z, corpse.x, corpse.y, corpse.z+5, corpse.realm) && CheckLosAt( corpse, corpse.x+i, corpse.y+j, corpse.z+5 ) )
				CreateItemAtLocation( corpse.x+i, corpse.y+j, corpse.z, "goldcoin", RandomInt(2000)+500);
				PlayStationaryEffect( corpse.x+i, corpse.y+j, corpse.z, 0x36b0, 7, 0, 1);
			else
				CreateItemAtLocation( corpse.x, corpse.y, corpse.z, "goldcoin", RandomInt(2000)+500);
			endif
		else
			CreateItemAtLocation( corpse.x+i, corpse.y+j, corpse.z+6, "goldcoin", RandomInt(2000)+500);
			PlayStationaryEffect( corpse.x+i, corpse.y+j, corpse.z+6, 0x36b0, 7, 0, 1);
		endif
        sleepms(50);
      endif
    endfor
  endfor
endfunction

//function GiveRewardsToPlayers(corpse)
  /*
  var altar := GetAltarByCorpse(corpse); 
  var players := GetObjProperty(altar, "players");
  
  foreach mobile in EnumerateOnlineCharacters()
    if(EligibleForLoot(mobile, players))
      PlayRewardFX(mobile);
      CreateBagWithPrize(mobile);

      var kills := players[mobile.serial];
      var event_bag := CreateItemInContainer(mobile.backpack, "backpack", 1);
      event_bag.name := "A Champion Reward Bag";
      event_bag.color := Random(3000);

      CreateItemInContainer(event_bag, "statpotion", 1);

      if(kills >= 150)
        CreateItemInContainer(event_bag, "statpotion", 1);
        CreateItemInContainer(event_bag, "transcendscroll", 1);
      endif

      if(kills >= 300)
        CreateItemInContainer(event_bag, "statpotion", 1);
        CreateItemInContainer(event_bag, "transcendscroll", 1);
      endif

      if(kills >= 500)
        CreateItemInContainer(event_bag, "powertome", 1);
      endif

      if(kills >= 1000)
        CreateItemInContainer(event_bag, "powertome", 1);
      endif
    endif
  endforeach*/
//endfunction

function EligibleForLoot(who, players)
  if(players[who.serial])
    return 1;
  else
    return 0;
  endif
endfunction

function PlayRewardFX(who)
  SendSysMessage(who, "As the champion lets out its final breath, a curious bag begins to materialize in your backpack.", FONT_NORMAL, 53);
  PlayObjectCenteredEffect(who, 0x3735, 0x1, 20);
  PlaySoundEffect(who, 0x5D0);
endfunction
