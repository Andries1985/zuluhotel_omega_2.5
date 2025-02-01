// All that's needed is for the mob to have the "LottoDrops" prop
// You can specify how many times to run the lottery, what chance there'll be of a drop on each run
// and  what % of total HP the player will need to reach, but we AT LEAST try award one player
// use the following props:
// LottoTrigger		% of health
// LottoChance		% of chance of drop
// LottoQuantity	How many items to try to drop
// LottoItem		What item to try to drop


use uo;
use os;
use cfgfile;

include "include/client";
include "include/random";
include "include/starteqp";

const DEFAULT_LOTTO_TRIGGER := 50;
const DEFAULT_LOTTO_CHANCE := 25;
const DEFAULT_LOTTO_QUANTITY := 1;
const DEFAULT_LOTTO_ITEM := "questtoken";

function LootLottery(who)
	
	var attacker_list := GetObjProperty(who, "AttackerList");
	if (!attacker_list)
		return 0;
	endif
	
	//convert trigger % to HP value
	var lotto_trigger := GetObjProperty(who, "LottoTrigger");
	if (!lotto_trigger)
		lotto_trigger := DEFAULT_LOTTO_TRIGGER;
	endif
	var max_hp := GetObjProperty(who, "MyMaxHP");
	lotto_trigger *= CDbl(max_hp)/100.0;
	
	var serial_list := attacker_list.Keys();
	//first let's remove those that didn't reach the trigger damage
	foreach attacker in serial_list
		//keep at least the last one, even if no one reached the trigger
		if (attacker_list.Size() == 1)
			break;
		endif
		//remove from list if under the trigger
		if (attacker_list[attacker] < lotto_trigger)
			attacker_list.Erase(attacker);
		endif
	endforeach
	serial_list := attacker_list.Keys();
	// Gets the Token drop parameters or default
	var lotto_chance := GetObjProperty(who, "LottoChance");
	if (!lotto_chance)
		lotto_chance := DEFAULT_LOTTO_CHANCE;
	endif

	var lotto_quantity := GetObjProperty(who, "LottoQuantity");
	if (!lotto_quantity)
		lotto_quantity := DEFAULT_LOTTO_QUANTITY;
	endif
	// If it's a dice type quantity, check the result now
	var is_dice := Lower(lotto_quantity).find("d");
	if (is_dice)
		lotto_quantity := RandomDiceRoll(lotto_quantity);
	endif

	var lotto_item := GetObjProperty(who, "LottoItem");
	if (!lotto_item)
		lotto_item := DEFAULT_LOTTO_ITEM;
	endif

	//how many should win?
	if(GetObjProperty(who, "LottoSingle"))
		serial_list := {serial_list.randomentry()};
	endif

	if(DEBUG_MODE)
		PrintTextAbove( who , "running the lottery");
	endif
	
	// Runs for each item
	var i := 0;
	while (i < Cint(lotto_quantity))
		var chance := Random(100);
		if (chance < lotto_chance)
			//pick an index on the candidate list and find him
			var lucky_who := SystemFindObjectBySerial(serial_list.randomentry());
			if (lucky_who)
				var prize := CreateItemInBackpack(lucky_who, lotto_item);
				if (!prize)
					SendSysmessage(lucky_who, "Couldn't create your prize!", FONT_NORMAL, 2595 );
				else
					PrintTextAbove( lucky_who, "I got a "+prize.name+"!", FONT_NORMAL, 114);
				endif
			endif
		endif
		i += 1;
	endwhile
	// Drop all loot in a bag?
	var lotto_lootbag := GetObjProperty(who, "LottoLootBag");
	if (!lotto_lootbag)
		return 0;
	endif
	//create the self destructing bag
	var lootbag := Createitematlocation(5288 , 1176 , 0, "lootbag", 1, "britannia");
	//get the original loot parameters
	var npcdesc := ReadConfigFile( "::npcdesc" );
	var elem := FindConfigElem( npcdesc, GetObjProperty( who, "npctemplate" ) );
	var magic_chance := GetConfigInt( elem, "Magicitemchance");
	var magic_level := GetConfigInt( elem, "Magicitemlevel");
	var lootgroup := Cint(GetConfigInt( elem, "lootgroup" ));
	var mobname := GetConfigString(elem, "name");
	lootbag.name := mobname +" Loot Bag";
	lootbag.color := GetConfigInt( elem, "Color");

	MakeLoot( lootbag , lootgroup, magic_chance, magic_level);
	//give it to one of the winners
	var luckyloot_who := SystemFindObjectBySerial(serial_list.randomentry());
	if (luckyloot_who)
		var gotloot := MoveItemToContainer(lootbag, luckyloot_who.backpack);
		if (!gotloot)
			SendSysmessage(luckyloot_who, "Couldn't create your loot bag!", FONT_NORMAL, 2595 );
		else
			PrintTextAbove( luckyloot_who, "I got the creature's loot bag!", FONT_NORMAL, 114);
		endif
	endif

endfunction

/*use like
if(Getobjproperty(who, "LottoDrops"))
	CopyLottery(who, target);
endif
*/
function CopyLottery(source, destination)
	
	if(!Getobjproperty(source, "LottoDrops"))
		return 0;
	endif

	var lotto_trigger := Getobjproperty(source, "LottoTrigger");
	var lotto_quantity := Getobjproperty(source, "LottoQuantity");
	var lotto_chance := Getobjproperty(source, "LottoChance");
	var lotto_item := Getobjproperty(source, "LottoItem");
	SetObjProperty(destination, "LottoDrops", 1);
	SetObjProperty(destination, "LottoTrigger", lotto_trigger);
	SetObjProperty(destination, "LottoQuantity", lotto_quantity);
	SetObjProperty(destination, "LottoChance", lotto_chance);	
	SetObjProperty(destination, "LottoItem", lotto_item);
	

endfunction