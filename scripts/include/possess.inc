include "include/myutil";

function possess( me, npc )

	if ( getobjproperty(me,"possess") )
		var oldnpc := SystemFindObjectBySerial(cint(getobjproperty(me,"possess")));
		if ( oldnpc )
			unpossess(me);
		endif
	endif

	if (npc.enable( "invul" ) )
		GrantPrivilege( me, "invul" );
		me.enable( "invul" );
	else
		RevokePrivilege( me, "invul" );
	endif

	me.concealed := 1;
	var oldpack := MoveMyPack(me);

	setobjproperty(me,"oldname",me.name);
	setobjproperty(me,"oldprefix",me.title_prefix);
	setobjproperty(me,"oldsuffix",me.title_suffix);
	me.name := npc.name;
	me.title_prefix := npc.title_prefix;
	me.title_suffix := npc.title_suffix;

	clone(npc, me);
	EquipItem(npc, oldpack);
	setobjproperty(me,"possess",npc.serial);

	foreach item in ListRootItemsInContainer(npc.backpack)
		if (item.newbie) moveitemtocontainer(item, me.backpack); endif
	endforeach

endfunction

function MoveMyPack(me)

	if (!me.backpack)
		var newbackpack := CreateItemAtLocation(5123, 1779, 0, 0xe75, 1, "britannia");
		EquipItem(me, newbackpack);
	endif

	var mypack := me.backpack;
	foreach thing in listequippeditems(me)
		if ( thing.serial != mypack.serial )
			MoveItemToContainer(thing, mypack);
		endif
	endforeach

	MoveItemToLocation(mypack,5123, 1779, 0, MOVEITEM_FORCELOCATION);

	return mypack;

endfunction

function clone(npc, me)

	if (!npc.backpack)
		var newbackpack := CreateItemAtLocation(5123, 1779, 0, 0xe75, 1, "britannia");
		EquipItem(npc, newbackpack);
	endif

	if (npc.color == cint(0x4631))
		npc.color := 1002;
	endif

	me.graphic := npc.graphic;
	me.color := npc.color;
	me.facing := npc.facing;
	me.hidden := npc.hidden;

	me.setstr( npc.strength );
	me.setint( npc.intelligence );
	me.setdex( npc.dexterity );
	me.hp := npc.hp;
	me.mana := npc.mana;
	me.stamina := npc.stamina;

	foreach it in listequippeditems(npc)
		equipitem(me,it);
	endforeach

	var i;
	for (i := 0; i <= SKILLID__HIGHEST; i := i + 1)
		SetRawSkill( me, i, GetRawSkill(npc,i) );
	endfor

	MoveObjectToLocation( me, npc.x, npc.y, npc.z, "britannia", MOVEOBJECT_FORCELOCATION );	
	MoveObjectToLocation( npc, 5276, 1164, 0, "britannia", MOVEOBJECT_FORCELOCATION );
	me.concealed := 0;

	npc.hidden := 1;
	npc.frozen := 1;

endfunction

function unpossess( me )

	var npc;
	if ( getobjproperty(me,"possess") )
		npc := SystemFindObjectBySerial(cint(getobjproperty(me,"possess")));
	endif
	if (!npc)
		eraseobjproperty(me,"possess");
		if ( getobjproperty(me,"oldname") )
			me.name := getobjproperty(me,"oldname");
			me.title_prefix := getobjproperty(me,"oldprefix");
			me.title_suffix := getobjproperty(me,"oldsuffix");
		endif
		SendSysMessage(me, "NPC not found.");
		return;
	endif	


	me.concealed := 1;
	SwapClothes(me,npc);
	eraseobjproperty(me,"possess");
	if ( getobjproperty(me,"oldname") )
		me.name := getobjproperty(me,"oldname");
	endif

	me.name := getobjproperty(me,"oldname");
	me.title_prefix := getobjproperty(me,"oldprefix");
	me.title_suffix := getobjproperty(me,"oldsuffix");

	eraseobjproperty(me,"oldname");
	eraseobjproperty(me,"oldprefix");
	eraseobjproperty(me,"oldsuffix");

	npc.hidden := me.hidden;
	npc.facing := me.facing;
	npc.frozen := 0;
	npc.stamina := me.stamina;
	npc.hp := me.hp;
	npc.mana := me.mana;

	MoveObjectToLocation(npc,me.x, me.y, me.z, "britannia", MOVEOBJECT_FORCELOCATION );

	var i;
	for (i := 0; i <= SKILLID__HIGHEST; i := i + 1)
		SetRawSkill( me, i, 1000000 );
	endfor

	me.setstr( 100 );
	me.setint( 100 );
	me.setdex( 100 );
	me.hp := me.maxhp;
	me.mana := me.maxmana;
	me.stamina := me.maxstamina;

	me.graphic := me.trueobjtype;
	me.color := me.truecolor;

	var mypack := me.backpack;
	foreach thing in ListRootItemsInContainer(mypack)
		EquipItem(me, thing);
	endforeach

	var weaponone := GetEquipmentByLayer( me,  LAYER_HAND1 );
	var weapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 );
	if (weaponone)
		MoveItemToContainer(weaponone, mypack );
	endif
	if (weapontwo)
		MoveItemToContainer(weapontwo, mypack );
	endif

	foreach item in ListRootItemsInContainer(npc.backpack)
		if (item.newbie) moveitemtocontainer(item, me.backpack); endif
	endforeach

endfunction

function SwapClothes(me, npc)

	if (!me.backpack)
		var newbackpack := CreateItemAtLocation(5123, 1779, 0, 0xe75, 1, "britannia");
		EquipItem(me, newbackpack);
	endif

	var hispack := npc.backpack;
	var mypack := me.backpack;

	if (hispack)
		MoveItemToLocation(hispack, 5123, 1779, 0, MOVEITEM_FORCELOCATION);
	else
		hispack := CreateItemAtLocation(5123, 1779, 0, 0xe75, 1, "britannia");
	endif

	foreach thing in listequippeditems(me)
		EquipItem(npc, thing);
	endforeach

	EquipItem(me, hispack);

	foreach item in ListRootItemsInContainer(npc.backpack)
		if (item.newbie) moveitemtocontainer(item, me.backpack); endif
	endforeach

endfunction

function unpossesscorpse( me, corpse )

	var npc;
	if ( getobjproperty(me,"possess") )
		npc := SystemFindObjectBySerial(cint(getobjproperty(me,"possess")));
	endif
	if (!npc)
		eraseobjproperty(me,"possess");
		if ( getobjproperty(me,"oldname") )
			me.name := getobjproperty(me,"oldname");
			me.title_prefix := getobjproperty(me,"oldprefix");
			me.title_suffix := getobjproperty(me,"oldsuffix");
		endif
		return;
	endif	

	me.concealed := 1;

	Resurrect( me );
	me.graphic := me.trueobjtype;
	me.color := me.truecolor;

	SwapClothes(me,npc);

	eraseobjproperty(me,"possess");

	me.name := getobjproperty(me,"oldname");
	me.title_prefix := getobjproperty(me,"oldprefix");
	me.title_suffix := getobjproperty(me,"oldsuffix");
	eraseobjproperty(me,"oldname");
	eraseobjproperty(me,"oldprefix");
	eraseobjproperty(me,"oldsuffix");

	MoveObjectToLocation( npc, 5288 , 1176 , 0, "britannia", MOVEOBJECT_FORCELOCATION );
	setobjproperty(npc, "guardkill", 1);
	RevokePrivilege( npc, "invul" );
	Damage( npc, npc.hp );

	var i;
	for (i := 0; i <= SKILLID__HIGHEST; i := i + 1)
		SetRawSkill( me, i, 1000000 );
	endfor

	me.setstr( 100 );
	me.setint( 100 );
	me.setdex( 100 );
	me.hp := me.maxhp;
	me.mana := me.maxmana;
	me.stamina := me.maxstamina;

	me.graphic := me.trueobjtype;
	me.color := me.truecolor;

	var mypack := me.backpack;
	foreach thing in ListRootItemsInContainer(mypack)
		EquipItem(me, thing);
	endforeach

	var weaponone := GetEquipmentByLayer( me,  LAYER_HAND1 );
	var weapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 );
	if (weaponone)
		MoveItemToContainer(weaponone, mypack );
	endif
	if (weapontwo)
		MoveItemToContainer(weapontwo, mypack );
	endif

endfunction
