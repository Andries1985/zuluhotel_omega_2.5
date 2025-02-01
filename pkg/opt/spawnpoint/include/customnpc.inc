use uo;
use os;
use cfgfile;
use vitals;
use attributes;

include "include/attributes";
include "include/client";
include "include/constants/gumpids";

 
include ":gumps:old-gumps";

function SelectCustomNPC( who, point )

	if (!ConfirmCustomNPCSelection(who))
		SendSysMessage(who, "Cancelled.");
		return 0;
	endif

	SendSysMessage(who, "Select a custom NPC you want to spawn from this spawnpoint.");
	var npc := Target(who);

	if (npc && !npc.isa(POLCLASS_NPC))
		SendSysMessage(who, "Not an NPC, cancelled.");
		return 0;
	endif

	if (!npc)
		SendSysMessage(who, "Cancelled.");
		return 0;
	endif

	SaveDataToPoint(npc,point, who);
	
	return npc;
	
endfunction

function ConfirmCustomNPCSelection(who)

GFInitGump(20, 20);
GFNoClose();
GFNoDispose();
GFResizePic(20, 20, GUMPID_BKGND_STONESLAB , 470, 120);
GFTextMid(20, 30, 490,53, "Are you ready to create a custom NPC spawnpoint?");
GFTextLine(30, 60, CInt(COLOR_BLUE_LIGHT), "- Make sure you have already created the NPC you want to spawn here.");
GFTextLine(30, 80, CInt(COLOR_BLUE_LIGHT), "- You will be prompted to target the specific npc.");
GFTextMid(20, 110, 490, CInt(COLOR_BLUE_LIGHT), "CONTINUE?");
var okay := GFButtonID( 100, 110, 2130, 2129, 1 );
var cancel := GFButtonID( 350, 110, 2121, 2120, 1 );

var data := GFSendGump(who);

if(data[okay.keyid])
	return 1;
elseif(data[cancel.keyid])
	return 0;
endif

return 0;

endfunction

function EraseCustomPropsandItems(who, point)

	// Erase all props and items in npc spawn if custom spawn unselected
	EraseObjProperty(point,	"NPCSkills");
	EraseObjProperty(point,	"NPCProps");
	EraseObjProperty(point,	"NPCVits");
	EraseObjProperty(point,	"overw");
	EraseObjProperty(point,	"CustomLoot");
	EraseObjProperty(point,	"CustomPoint");

	// destroy all items in current spawnpoint
	foreach item in EnumerateItemsInContainer(point)
		DestroyItem(item);
	endforeach

endfunction

function SaveDataToPoint(npc, point, who)

// erase previous customnpc stuff
EraseCustomPropsandItems(who, point);

var o := struct, vits := struct, cprops := {}, skillids := {};
var allprops := GetObjPropertyNames( npc );
var maxhitsnpc;

o.+Name := npc.name;
o.+Graphic := npc.graphic;
o.+script := npc.script;
o.+criminal := npc.criminal;
o.+murderer := npc.murderer;

o.+objtype :=  GetStrength(npc);
o.+trueobjtype := npc.objtype;
o.+color := npc.color;
o.+truecolor := npc.color;
o.+Gender := npc.gender;
o.+STR := GetStrength(npc);
o.+INT := GetIntelligence(npc);
o.+DEX := GetDexterity(npc);
o.+runspeed := npc.run_speed;
o.+ar	:= npc.ar;
o.+titleprefix := npc.title_prefix;
o.+titlesuffix := npc.title_suffix;
o.+titleguild := npc.title_guild;
o.+titlerace := npc.title_race;
o.+facing := npc.facing;

if (GetObjProperty(npc, "CustomHitsLevel"))
	maxhitsnpc := GetObjProperty(npc, "CustomHitsLevel");
	//print($"Max hits for NPC is " + maxhitsnpc);
	vits.+HITS := maxhitsnpc;
	//print($"Vits 1 is " + vits);
else
	vits.+HITS := GetMaxHp(npc);
	//print($"Vits 2 is " + vits);
endif
vits.+STAM := GetMaxStamina(npc);
vits.+MANA := GetMaxMana(npc);

//print($"Vits 3 is " + vits);

foreach prop in allprops
	cprops.append({prop, GetObjProperty(npc, prop)});
endforeach

foreach item in ListEquippedItems(npc)
	var copy;
	if (item != npc.backpack)
		copy := CreateItemCopyAtLocation(point.x, point.y, point.z, item, point.realm);
	endif
	MoveItemToContainer(copy, point);
	copy.movable := 0;
endforeach

if (npc.backpack)
	foreach item in EnumerateItemsInContainer(npc.backpack)
		if (GetObjProperty(item, "LFucker"))
			var copy := CreateItemCopyAtLocation(point.x, point.y, point.z, item, point.realm);
			SetObjProperty(copy, "LFucker", 1);
			MoveItemToContainer(copy, point);
			SetObjProperty(copy, "packitem", 1);
			copy.movable := 0;
		endif
	endforeach
endif

var i;
for (i := 0; i <= 48; i := i + 1)
	skillids.append(GetBaseSkill(npc, i));
endfor

SetObjProperty(point, "CustomPoint", npc.npctemplate);
SetObjProperty(point, "NPCSkills", skillids);
SetObjProperty(point, "NPCProps", cprops);
SetObjProperty(point, "NPCVits", vits);
SetObjProperty(point, "overw", o);

CustomNPCGump(who, point, o, vits, cprops, npc);

endfunction

function DressNPCCustom(me)

var point := SystemFindObjectBySerial(GetObjProperty(me, "SpawnPoint"));

foreach item in EnumerateItemsInContainer(point)
	var copy := CreateItemCopyAtLocation(5273, 1181, 0, item, "britannia");
	copy.movable := 1;
	if (GetObjProperty(copy, "packitem"))
		EraseObjProperty(copy, "packitem");
		MoveItemToContainer(copy, me.backpack);
	else
		EquipItem(me, copy);
	endif
	copy.movable := 1;
endforeach

endfunction

function CustomNPCGump(who, point, o, vits, cprops, npc, help := 0)

var mprop, mlevel, lootgroup;

if (!GetObjProperty(npc, "CustomLoot"))
	var npcdesc := ReadConfigFile( "::npcdesc" );
	var elem := FindConfigElem( npcdesc,npc.npctemplate );
	mprop  := GetConfigInt( elem, "MagicItemChance");
	mlevel := GetConfigInt( elem, "MagicItemLevel");
	lootgroup := GetConfigInt( elem, "lootgroup" );
else
	var cloot := GetObjProperty(npc, "CustomLoot");
	mprop  := cloot[1];
	mlevel := cloot[2];
	lootgroup := cloot[3];
endif

if (!mprop)
	mprop := 0;
endif

if (!mlevel)
	mlevel := 0;
endif

if (!lootgroup)
	lootgroup := 0;
endif

if (!help)
help := "<u><center>Final Options</center></u>
	These are options that are easier to set via a gump,
	then by editing props.  Click the corresponing button
	under help to find out more about that setting.";
endif

var layout :=  {"nodispose",
		"noclose",
		"page 0",
		"resizepic 30 30 9250 400 400",

		"gumppic 150 85 2445",
		"gumppic 150 115 2443",
		"text 50 45 53 0",

		"text 50 85 92 1",
		"text 380 65 92 2",
		"text 50 115 92 4",
		"text 50 150 92 5",
		"text 50 170 92 6",
	
		"text 230 150 995 7",
		"text 230 170 995 7",

		"CheckBox 195 150 210 211 1 11",
		"CheckBox 195 170 210 211 0 12",

		"text 50 200 92 8",
		"text 100 225 91 9",
		"text 100 250 91 10",
		"text 100 275 91 11",

		"gumppic 230 225 2443",
		"gumppic 230 250 2443",
		"gumppic 230 275 2443",

		"button 385 90 2361 2362 0 2 21",
		"button 385 120 2361 2362 0 3 22",
		"button 385 155 2361 2362 0 4 23",
		"button 385 175 2361 2362 0 5 24",
		"button 385 230 2361 2362 0 6 25",
		"button 385 255 2361 2362 0 7 26",
		"button 385 280 2361 2362 0 8 27",

		"TextEntry 155 87 80 20 63 31 20", 
		"TextEntry 155 117 40 20 63 32 21",

		"TextEntry 235 227 40 20 63 33 22",
		"TextEntry 235 252 40 20 63 34 23",
		"TextEntry 235 277 40 20 63 35 24",

		"button 330 350 2311 2312 1 0 1",

		"page 1",
		"HtmlGump 45 300 250 117 3 1 2",

		"page 2",
		"HtmlGump 45 300 250 117 12 1 2",

		"page 3",
		"HtmlGump 45 300 250 117 13 1 2",

		"page 4",
		"HtmlGump 45 300 250 117 14 1 2",

		"page 5",
		"HtmlGump 45 300 250 117 15 1 2",

		"page 6",
		"HtmlGump 45 300 250 117 16 1 2",

		"page 7",
		"HtmlGump 45 300 250 117 17 1 2",

		"page 8",
		"HtmlGump 45 300 250 117 18 1 2"
};

var data := {"Finalise Custom NPC Properties - "+o.name,
		"Script:",
		"Help",
		help,

		"Run Speed:",
		"NPC Items (worn):",
		"NPC Items (backpack):",

		"Toggle newbied",

		"Loot Level:",
		"Lootgroup:",
		"Magic Item Level:",
		"Magic Item Chance:",
	
		GetHelp(21),
		GetHelp(22),
		GetHelp(23),
		GetHelp(24),
		GetHelp(25),
		GetHelp(26),
		GetHelp(27),
		GetHelp(28),

		o.script,
		o.runspeed,
		lootgroup,
		mlevel,
		mprop
};

var result := SendDialogGump(who, layout, data);

o.script := (result[31][5, len(result[31])]);
o.runspeed := (result[32][5, len(result[32])]);

var lootgroup2 := CInt(result[33][5, len(result[33])]);
var mlevel2 := CInt(result[34][5, len(result[34])]);
var mprop2 := CInt(result[35][5, len(result[35])]);

if (result[11] || result[12])
	foreach item in EnumerateItemsInContainer(point)
		if (GetObjProperty(item, "packitem"))
			if (result[12])
				SetObjProperty(item, "noloot", 1);
			endif
		else
			if (result[11])
				SetObjProperty(item, "noloot", 1);
			endif
		endif
	endforeach
endif

if (who.cmdlevel > 3)
	if (lootgroup2 != lootgroup || mlevel2 != mlevel || mprop2 != mprop)
		SetObjProperty(point, "CustomLoot", {mprop2, mlevel2, lootgroup2});
	endif
else
	SendSysMessage(who, "Loot group unchanged, need dev+ rights for that.");
endif

SetObjProperty(point, "overw", o);

SendSysMessage(who, "Data Saved.", 3, 73);

endfunction


function GetHelp(num)

case (num)
21: return "<u><center>Script</center></u>
	Sets the AI script the NPC will use. Some scripts are...<br>
	<b>killpcs</b>: The melee ai, used by monsters such as <i>behemoths/skeletons/zombies</i>.<br>
	<b>firebreather</b>: Much the same as killpcs, except the npcs will occasionall breathe fire.  Use by <i>dragon/hells stallion/nightmare</i>.<br>
	<b>spellkillpcs</b>: Used for monsters that cast magic, such as <i>balrons/liche/black wisp</i>.<br>
	<b>archerkillpcs</b>: Ranged archery attack.  Used by <i>brigand archer/orc archer</i>.<br>
	<b>explosionkillpcs</b>: AI that keeps a 4 tile distance between the opponent.  Won't seek out target, used by <i>tainted ranger</i>.";

22: return "<u><center>Run Speed</center></u>
	Controls how fast the NPC will run. Slowest being 1 and fastest 250.<br>
	An NPC on a firebreather ai script will ALWAYS run, while a killpcs will not.<br>
	However, a mounted NPC on any script WILL run.  A behemoth has a run speed of 250, but because it's on the killpcs script it walks.";

23: return "<u><center>Worn NPC Items</center></u>
	Toggling this on (on by default) will make sure any items the NPC wears will NOT be in it's loot.<br>
	If you toggle this off then any item, unless specified, will be in the NPCs loot.";

24: return "<u><center>Backpack NPC Items</center></u>
	Not normally needed unless you place a secondary weapon, or arrows bolts you don't want to appear in loot.<br>
	This is toggled off by default.";

25: return "<u><center>Lootgroup</center></u>
	Only Developers can edit this.<br>
	The lootgroup number found in nlootgroup.cfg. Some examples...
	<b>9</b>: <i>Level 5 chest/balron/behemoth</i> 8 is level 4 chest, etc.<br>
	<b>35</b>: <i>Undead Flayer/black wisp/frost wisp</i><br>
	<b>37</b>: <i>dragon</i>";

26: return "<u><center>Magic Item Level</center></u>
	Only Developers can edit this.<br>
	How good the magic items will be.
	<b>8</b>: <i>balron/golden dragon</i><br>
	<b>7</b>: <i>behemothr</i><br>
	<b>6</b>: <i>undead flayer</i><br>
	<b>5</b>: <i>black wisp/frost wisp/evil spellbook/dragon</i>";

27: return "<u><center>Magic Item Chance</center></u>
	Only Developers can edit this.<br>
	Chance of a good item spawning.
	<b>60</b>: <i>dragon/behemoth</i><br>
	<b>65</b>: <i>undead flayer</i><br>
	<b>99</b>: <i>golden dragon</i><br>
	<b>95</b>: <i>balron</i>";
default: return "lol";
endcase

endfunction