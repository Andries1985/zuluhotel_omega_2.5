include "include/client";
include ":Events:bag";

// The gold rain: a champion's gold is dropped in GOLD_RAIN_PILES piles on random tiles within
// GOLD_RAIN_RANGE of its body, only where a player can walk to the pile and pick it up. A pile's
// size is worked out from GOLD_RAIN_TOTAL, so the average total stays the same whatever the pile
// count, and if fewer good tiles exist the piles are simply bigger.
//
// Patch 1.1.4: it used to try every tile of the 21 x 21 square, but tested each with
// GetMapInfo( i, j ) - the offsets, not map coordinates - so only the 121 tiles south-east of the
// body got gold, and a pile could land inside a wall, under a static, on a roof or across water.
const GOLD_RAIN_TOTAL := 250000;	// average gold per champion (the old rain paid about 180,000)
const GOLD_RAIN_PILES := 20;
const GOLD_RAIN_RANGE := 10;
const GOLD_RAIN_MAX_RISE := 10;		// a pile's floor may be at most this far above or below the body's
const CHAMP_ALTAR_HUE := 1157;		// the altar pieces (champspawns scripts/oncreate.src); no gold on them

function RainDownMoney(corpse)

	var spots := FindGoldRainSpots(corpse);
	var pile := CInt(GOLD_RAIN_TOTAL / spots.size());
	foreach spot in spots
		// pile/2 to 3*pile/2, so the average stays at pile
		var amount := RandomInt(pile) + CInt(pile / 2) + 1;
		CreateItemAtLocation(spot[1], spot[2], spot[3], "goldcoin", amount, corpse.realm);
		PlayStationaryEffect(spot[1], spot[2], spot[3], 0x36b0, 7, 0, 1);
		sleepms(100);
	endforeach

endfunction

// Up to GOLD_RAIN_PILES {x, y, z} spots, tried in random order. A tile counts only if a player can
// stand on it near the body's floor level, outside any house or boat, with no altar piece on it,
// in line of sight of the body, and on a walkable path from it. The body's own tile is the
// fallback when nothing else qualifies.
function FindGoldRainSpots(corpse)

	var offsets := array{};
	var i, j;
	for (i := -GOLD_RAIN_RANGE; i <= GOLD_RAIN_RANGE; i := i + 1)
		for (j := -GOLD_RAIN_RANGE; j <= GOLD_RAIN_RANGE; j := j + 1)
			offsets.append({i, j});
		endfor
	endfor

	// Fisher-Yates shuffle, so the piles scatter instead of filling the square row by row.
	var n := offsets.size();
	while (n > 1)
		var k := RandomInt(n) + 1;
		var tmp := offsets[n];
		offsets[n] := offsets[k];
		offsets[k] := tmp;
		n := n - 1;
	endwhile

	var spots := array{};
	foreach offset in offsets
		var z := GoldRainSpotZ(corpse, corpse.x + offset[1], corpse.y + offset[2]);
		if (z != error)
			spots.append({corpse.x + offset[1], corpse.y + offset[2], z});
			if (spots.size() >= GOLD_RAIN_PILES)
				break;
			endif
		endif
	endforeach

	if (!spots.size())
		spots.append({corpse.x, corpse.y, corpse.z});
	endif
	return spots;

endfunction

// The z a pile can be dropped at on x, y, or error if a player could not reach it there.
function GoldRainSpotZ(corpse, x, y)

	if (!GetMapInfo(x, y, corpse.realm))
		return error;
	endif

	foreach item in ListItemsAtLocation(x, y, LIST_IGNORE_Z, corpse.realm)
		if (item.color == CHAMP_ALTAR_HUE)
			return error;
		endif
	endforeach

	// Standable floor at the body's level, or the ground just below it on a slope.
	var stand := GetStandingHeight(x, y, corpse.z, corpse.realm);
	if (!stand or stand.errortext)
		stand := GetStandingHeight(x, y, corpse.z - GOLD_RAIN_MAX_RISE, corpse.realm);
	endif
	if (!stand or stand.errortext or stand.multi)
		return error;
	endif
	var z := CInt(stand.z);
	if (z > corpse.z + GOLD_RAIN_MAX_RISE or z < corpse.z - GOLD_RAIN_MAX_RISE)
		return error;
	endif

	if (x == corpse.x and y == corpse.y)
		return z;
	endif
	if (!CheckLosBetween(x, y, z + 1, corpse.x, corpse.y, corpse.z + 5, corpse.realm))
		return error;
	endif
	// Players, monsters and doors don't count as obstacles: they move or open.
	var path := FindPath(corpse.x, corpse.y, corpse.z, x, y, z, corpse.realm, FP_IGNORE_MOBILES + FP_IGNORE_DOORS, 2);
	if (!path or path.errortext)
		return error;
	endif
	return z;

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
