use uo;
use os;

include "include/namingbyenchant";
include "include/client";

function createRPerRangerBag(backpack)

    var bow := CreateItemInContainer(backpack, "SBow");

    SetObjProperty(bow, "IsRPer", 1);
    bow.dmg_mod := 20;
    SetNameByEnchant(bow);
    
    //Misc
    CreateItemInContainer(backpack, "arrow", 500);
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "rawfishsteak", 100);
    CreateItemInContainer(backpack, "fishingpole", 1);
    CreateItemInContainer(backpack, "kindling", 100);
    CreateItemInContainer(backpack, "magicfish1", 10);
    CreateItemInContainer(backpack, "magicfish2", 10);
    CreateItemInContainer(backpack, "magicfish4", 10);
    CreateItemInContainer(backpack, "magicfish5", 10);
    CreateItemInContainer(backpack, "magicfish6", 10);
    CreateItemInContainer(backpack, "goldcoin", 2000);

    //Leather Armor
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d4);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d5);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d6);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13da);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13db);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1DB9);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x170B);

endfunction


function createRPerWarriorBag(backpack)

    var spear := CreateItemInContainer(backpack, "SSpear");
    var katana := CreateItemInContainer(backpack, "SKatana");
    var kryss := CreateItemInContainer(backpack, "SKryss");
    var mace := CreateItemInContainer(backpack, "SMace");

    SetObjProperty(spear, "IsRPer", 1);
    SetObjProperty(katana, "IsRPer", 1);
    SetObjProperty(kryss, "IsRPer", 1);
    SetObjProperty(mace, "IsRPer", 1);

    spear.dmg_mod := 20;
    katana.dmg_mod := 20;
    kryss.dmg_mod := 20;
    mace.dmg_mod := 20;
    SetNameByEnchant(spear);
    SetNameByEnchant(katana);
    SetNameByEnchant(kryss);
    SetNameByEnchant(mace);

    //Misc
    CreateItemInContainer(backpack, "bandages", 200);
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "greaterhealpotion", 10);
    CreateItemInContainer(backpack, "greatercurepotion", 5);
    CreateItemInContainer(backpack, "goldcoin", 2000);

    //Plate Armor
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1410, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1411, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1412, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1413, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1414, 1);
	armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1415, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1B74, 1);

endfunction


function createRPerMageBag(backpack)

    var armor := {};
    var robe := CreateItemInContainer(backpack, "stygianrobe");
    var staff := CreateItemInContainer(backpack, "gnarledstaff");

    SetObjProperty(robe, "IsRPer", 1);
    SetObjProperty(staff, "IsRPer", 1);
    staff.dmg_mod := 25;
    SetNameByEnchant(staff);
    SetNameByEnchant(robe);

    armor[len(armor)+1] := robe;
    armor[len(armor)+1] := CreateItemInContainer(backpack, "wizardhat");
    
    //Misc
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "greaterhealpotion", 10);
    CreateItemInContainer(backpack, "blankscroll", 100);
    CreateItemInContainer(backpack, "emptybottle", 100);
    CreateItemInContainer(backpack, "BlackPearl", 120);
    CreateItemInContainer(backpack, "SulphurousAsh", 120);
    CreateItemInContainer(backpack, "SpiderSilk", 120);
    CreateItemInContainer(backpack, "Nightshade", 120);
    CreateItemInContainer(backpack, "MandrakeRoot", 120);
    CreateItemInContainer(backpack, "Ginseng", 120);
    CreateItemInContainer(backpack, "Garlic", 120);
    CreateItemInContainer(backpack, "BloodMoss", 120);
    CreateItemInContainer(backpack, "goldcoin", 3000);
    
endfunction


function createRPerThiefBag(backpack)

    var bow := CreateItemInContainer(backpack, "SBow");
    var spear := CreateItemInContainer(backpack, "SSpear");
    var kryss := CreateItemInContainer(backpack, "SKryss");

    bow.dmg_mod := 20;
    spear.dmg_mod := 20;
    kryss.dmg_mod := 20;
    SetObjProperty(bow, "IsRPer", 1);
    SetObjProperty(spear, "IsRPer", 1);
    SetObjProperty(kryss, "IsRPer", 1);
    SetNameByEnchant(bow);
    SetNameByEnchant(spear);
    SetNameByEnchant(kryss);
    
    //Misc
    CreateItemInContainer(backpack, "arrow", 200);
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "bandages", 100);
    CreateItemInContainer(backpack, "poisonpotion", 50);
    CreateItemInContainer(backpack, "greaterpoisonpotion", 20);
    CreateItemInContainer(backpack, "deadlypoisonpotion", 5);
    CreateItemInContainer(backpack, "greaterhealpotion", 10);
    CreateItemInContainer(backpack, "greatercurepotion", 5);
    CreateItemInContainer(backpack, "goldcoin", 2000);

    //Leather Armor
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d4);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d5);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d6);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13da);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13db);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1DB9);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x170B);

endfunction


function createRPerBardBag(backpack)

    var crook := CreateItemInContainer(backpack, "SShepherdscrook");
    
    SetObjProperty(crook, "IsRPer", 1);
    crook.dmg_mod := 25;
    SetNameByEnchant(crook);
    
    //Misc
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "greaterhealpotion", 20);
    CreateItemInContainer(backpack, "lute", 1);
    CreateItemInContainer(backpack, "goldcoin", 4000);

    //Leather Armor
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d4);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d5);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d6);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13da);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13db);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1DB9);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x170B);

endfunction


function createRPerPaladinBag(backpack)

    var sword := CreateItemInContainer(backpack, "SKatana");
    var mace := CreateItemInContainer(backpack, "SMace");

    SetObjProperty(sword, "IsRPer", 1);
    SetObjProperty(mace, "IsRPer", 1);
    sword.dmg_mod := 20;
    mace.dmg_mod := 20;
    SetNameByEnchant(sword);
    SetNameByEnchant(mace);

    //Misc
    CreateItemInContainer(backpack, "bandages", 200);
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "greaterhealpotion", 10);
    CreateItemInContainer(backpack, "greatercurepotion", 5);
    CreateItemInContainer(backpack, "goldcoin", 2000);

    //Plate Armor
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1410, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1411, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1412, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1413, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1414, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1415, 1);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1B74, 1);

endfunction


function createRPerMysticArcherBag(backpack)

    var bow := CreateItemInContainer(backpack, "SBow");

    SetObjProperty(bow, "IsRPer", 1);
    bow.dmg_mod := 20;
    SetNameByEnchant(bow);

    //Misc
    CreateItemInContainer(backpack, "arrow", 500);
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "rawfishsteak", 100);
    CreateItemInContainer(backpack, "fishingpole", 1);
    CreateItemInContainer(backpack, "kindling", 100);
    CreateItemInContainer(backpack, "magicfish1", 10);
    CreateItemInContainer(backpack, "magicfish2", 10);
    CreateItemInContainer(backpack, "magicfish4", 10);
    CreateItemInContainer(backpack, "magicfish5", 10);
    CreateItemInContainer(backpack, "magicfish6", 10);
    CreateItemInContainer(backpack, "BlackPearl", 60);
    CreateItemInContainer(backpack, "SulphurousAsh", 60);
    CreateItemInContainer(backpack, "SpiderSilk", 60);
    CreateItemInContainer(backpack, "Nightshade", 60);
    CreateItemInContainer(backpack, "MandrakeRoot", 60);
    CreateItemInContainer(backpack, "Ginseng", 60);
    CreateItemInContainer(backpack, "Garlic", 60);
    CreateItemInContainer(backpack, "BloodMoss", 60);
    CreateItemInContainer(backpack, "goldcoin", 2000);

    //Leather Armor
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d4);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d5);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d6);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13da);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13db);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1DB9);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x170B);

endfunction


function createRPerBladesingerBag(backpack)

    var daisho := CreateItemInContainer(backpack, "SDaisho");

    SetObjProperty(daisho, "IsRPer", 1);
    daisho.dmg_mod := 20;
    SetNameByEnchant(daisho);

    //Misc
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "greaterhealpotion", 20);
    CreateItemInContainer(backpack, "lute", 1);
    CreateItemInContainer(backpack, "goldcoin", 4000);

    //Chainmail Armor + Plate for missing pieces
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13BB);  // Chainmail Coif
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13BE);  // Chainmail Leggings
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13BF);  // Chainmail Tunic
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1410);  // Platemail Arms
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1413);  // Platemail Gorget
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1414);  // Platemail Gloves
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x170B);  // Leather Boots

endfunction


function createRPerCrafterBag(backpack)
    var smithyhammer := CreateItemInContainer(backpack, "smithyhammer");
    smithyhammer.SpecMult := 3;
    smithyhammer.skilladv := 7;
    smithyhammer.skilladvamount := 3;
    SetObjProperty(smithyhammer, "IsRPer", 1);
    SetNameByEnchant(smithyhammer);
    
    //Misc
    CreateItemInContainer(backpack, "shovel", 5);
    CreateItemInContainer(backpack, "pickaxe", 1);
    CreateItemInContainer(backpack, "saw", 1);
    CreateItemInContainer(backpack, "tinkerstools", 1);
    CreateItemInContainer(backpack, "axe", 2);
    CreateItemInContainer(backpack, "fishsteak", 20);
    CreateItemInContainer(backpack, "goldcoin", 3000);

    //Leather Armor
    var armor := {};
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d4);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d5);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13d6);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13da);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x13db);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x1DB9);
    armor[len(armor)+1] := CreateItemInContainer(backpack, 0x170B);

endfunction
