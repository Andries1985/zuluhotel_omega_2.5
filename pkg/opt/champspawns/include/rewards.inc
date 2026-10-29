include "include/client";
include "include/random";
include "util/bank";	// FindBankBox - the reward bag of an offline player or a full backpack

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

// Champion reward bags - Patch 1.1.5. When an altar's champion dies, every character with at least
// CHAMP_BAG_MIN_KILLS kills on the altar's tally (death.inc) gets a bag, provided their last kill
// came no more than CHAMP_CREDIT_WINDOW seconds before the newest kill on the tally: kills made at
// an altar that was then left to decay don't pay when someone else finishes it. The newest kill,
// not the champion's death, is the reference, because the champion fight itself adds no kills.
// The tiers add up, so a 1000-kill bag holds everything below it. Part of the tomes, scrolls and
// stat potions, and all the presents and vanity tokens, moved here from the champion's corpse
// (nlootgroup.cfg lootgroups 150 and 151).
const CHAMP_BAG_MIN_KILLS := 150;
const CHAMP_CREDIT_WINDOW := 7200;
const CHAMP_BAG_GRAPHIC := 0x0E76;
const CHAMP_REWARD_COLOR := 53;

function GiveRewardsToPlayers(altar)

	var players := GetObjProperty(altar, "players");
	if(TypeOf(players) != "Dictionary")
		return;
	endif

	// An offline player's bag goes to their bank box.
	set_script_option(SCRIPTOPT_CAN_ACCESS_OFFLINE_MOBILES, 1);

	// Entries are {kills, systime of the last kill}; one from before Patch 1.1.5 has no time and
	// can't show a recent kill.
	var newest := 0;
	foreach entry in players
		if(TypeOf(entry) == "Array" and CInt(entry[2]) > newest)
			newest := CInt(entry[2]);
		endif
	endforeach

	foreach serial in (players.keys())
		var entry := players[serial];
		if(TypeOf(entry) == "Array")
			if(newest - CInt(entry[2]) <= CHAMP_CREDIT_WINDOW)
				var who := SystemFindObjectBySerial(CInt(serial), SYSFIND_SEARCH_OFFLINE_MOBILES);
				if(who)
					var kills := CInt(entry[1]);
					if(kills >= CHAMP_BAG_MIN_KILLS)
						GiveChampionBag(who, kills);
					elseif(who.connected)
						SendSysMessage(who, "You slew " + kills + " of the champion's minions. It takes " + CHAMP_BAG_MIN_KILLS + " to earn a Champion Reward Bag.", FONT_NORMAL, CHAMP_REWARD_COLOR);
					endif
				endif
			endif
		endif
	endforeach

endfunction

// Builds the bag for this many kills, in the backpack of an online player or else in the bank box.
function GiveChampionBag(who, kills)

	var tomes := 0, scrolls := 0, stats := 0, tokens := 0, presents := 0;
	if(kills >= 150)
		scrolls := scrolls + 1;
		tokens := tokens + 1;
		presents := presents + 1;
	endif
	if(kills >= 300)
		scrolls := scrolls + 1;
		tokens := tokens + 1;
	endif
	if(kills >= 500)
		stats := stats + 1;
	endif
	if(kills >= 750)
		tomes := tomes + 1;
	endif
	if(kills >= 1000)
		tomes := tomes + 1;
		stats := stats + 1;
		tokens := tokens + 2;
		presents := presents + 1;
	endif

	var bag := 0;
	var in_bank := 0;
	if(who.connected)
		bag := CreateItemInContainer(who.backpack, CHAMP_BAG_GRAPHIC, 1);
	endif
	if(!bag)
		var bankbox := FindBankBox(who);
		if(bankbox)
			bag := CreateItemInContainer(bankbox, CHAMP_BAG_GRAPHIC, 1);
			in_bank := 1;
		endif
	endif
	if(!bag)
		SysLog("Champion reward bag for " + who.name + " (" + Hex(who.serial) + ", " + kills + " kills) could not be created: " + bag.errortext);
		return 0;
	endif
	bag.name := "A Champion Reward Bag";
	bag.color := RandomInt(3000);

	AddChampionReward(bag, "PowerTome", tomes);
	AddChampionReward(bag, "TranscendScroll", scrolls);
	AddChampionReward(bag, "StatPotion", stats);
	AddChampionReward(bag, "vanitytoken", tokens);
	var i;
	for(i := 1; i <= presents; i := i + 1)
		// The champions' old ChampionPresents group: gift1-3, equally likely.
		CreateItemInContainer(bag, "gift" + (RandomInt(3) + 1), 1);
	endfor

	if(who.connected)
		if(in_bank)
			SendSysMessage(who, "Your backpack is full, so your Champion Reward Bag was placed in your bank box.", FONT_NORMAL, CHAMP_REWARD_COLOR);
		else
			PlayRewardFX(who);
		endif
		SendSysMessage(who, "You slew " + kills + " of the champion's minions.", FONT_NORMAL, CHAMP_REWARD_COLOR);
	endif
	return bag;

endfunction

// One at a time, as the loot code does: these items don't stack.
function AddChampionReward(bag, objname, amount)

	var i;
	for(i := 1; i <= amount; i := i + 1)
		CreateItemInContainer(bag, objname, 1);
	endfor

endfunction

function PlayRewardFX(who)
  SendSysMessage(who, "As the champion lets out its final breath, a curious bag begins to materialize in your backpack.", FONT_NORMAL, 53);
  PlayObjectCenteredEffect(who, 0x3735, 0x1, 20);
  PlaySoundEffect(who, 0x5D0);
endfunction
