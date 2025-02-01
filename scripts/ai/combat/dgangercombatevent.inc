function process_combat_event(ev)

	if (spellcast)
		spellattack(ev.source);
	endif

        if (ev.type == SYSEVENT_DAMAGED)
			if ( (spellcast) and ( GetHP(me) < (GetStrength(me)/2)) and !(GetObjProperty( me , "Type" ) == "Undead" ) )
				CastSpell(me,me,"gheal");
				if( me.poisoned )
					CastSpell(me,me,"cure");
					CastSpell(me,ev.source,"poison");
				endif
			endif
                        if ((GetHP(me) * 100/GetmaxHP(me)) < flee_point)
				EraseObjProperty(me,"#flees");
                                flee(ev.source);
                        endif
        endif

endfunction

function in_combat_event_loop(opponent)

	me.name := opponent.name;
	var myweaponone := GetEquipmentByLayer( me,  LAYER_HAND1 );
	var myweapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 );
	var yourweaponone := GetEquipmentByLayer( opponent,  LAYER_HAND1 );
	var yourweapontwo := GetEquipmentByLayer( opponent,  LAYER_HAND2 );

	if ( 	(myweaponone.objtype != yourweaponone.objtype) or
		(myweapontwo.objtype != yourweapontwo.objtype) )
			DestroyItem(myweaponone);
			DestroyItem(myweapontwo);
			myweaponone := CreateItemInContainer(me.backpack, cint(yourweaponone.objtype),1);
			myweapontwo := CreateItemInContainer(me.backpack, cint(yourweapontwo.objtype),1);
			EquipItem(me, myweaponone);
			EquipItem(me, myweapontwo);
			myweaponone.dmg_mod := 10;
			myweapontwo.dmg_mod := 10;
			setobjproperty(myweaponone,"noloot",1);
			setobjproperty(myweapontwo,"noloot",1);

		if ( (myweaponone.objtype == 0x13B2) or (myweapontwo.objtype == 0x13B2) )
			var it := CreateItemInInventory(me.backpack,0xf3f,50);
			setobjproperty(it,"noloot",1);
		elseif ( (myweaponone.objtype == 0x0f4f) or (myweapontwo.objtype == 0x0f4f) or 
			(myweaponone.objtype == 0x13fd) or (myweapontwo.objtype == 0x13fd) )
			var it :=CreateItemInInventory(me.backpack,0x1bfb,50);
			setobjproperty(it,"noloot",1);
		elseif ( (myweaponone.objtype == 0x6050) or (myweapontwo.objtype == 0x6050) )
			var it :=CreateItemInInventory(me.backpack,0x6051,25);
			setobjproperty(it,"noloot",1);
		endif

	endif

	if (spellcast)
		spellattack(opponent);
	endif

endfunction

function spellattack(opponent)

	if (!GetObjProperty(me,"#lastbreath"))
		SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);
	endif

	if (GetObjProperty(me,"#lastbreath") > ReadGameClock() )
		return;
	endif

	if (Random(2)==1)
		sleep(1);
		cast_offensive_spell(me,opponent);
	endif

	SetObjProperty(me,"#lastbreath",ReadGameClock() + 5);

endfunction

function post_combat()

	DisableCombatEvents();
	EnableMainEvents();

	SetWarMode( 0 );
	SetOpponent( 0 );

	if ( (spellcast) and ( GetHP(me) < (GetStrength(me)/2)) )
		CastSpell(me,me,"gheal");
	endif

	me.graphic := me.objtype;
	me.color := 0;

	sleep(1);
	look_around();

endfunction

function prepare_for_fight(opponent)

	DisableMainEvents();
	me.concealed := 1;
	set_critical(1);
	var it;
 	foreach it in listequippeditems(me)
		destroyitem(it);
	endforeach
	var p := createitematlocation(5288 , 1176 , 0, 0x0e75, 1, "britannia");
	if (!equipitem(me, p)) destroyitem(p); endif
	me.criminal := 0;
	me.graphic := opponent.graphic;
	me.color:= opponent.color;
	me.name := opponent.name;

	foreach it in listequippeditems(opponent)
		var what := createitemincontainer(me.backpack, cint(it.objtype), 1);
		what.color := it.color;
		what.graphic := it.graphic;
		if (it.name)
			what.name := it.name;
		endif
		equipitem(me,what);
		setobjproperty(what,"noloot",1);
	endforeach
	me.criminal := 0;
	var oldprio := set_priority(50);

	var weaponone := GetEquipmentByLayer( me,  LAYER_HAND1 );
	weaponone.dmg_mod := 10;
	var weapontwo := GetEquipmentByLayer( me,  LAYER_HAND2 );
	weapontwo.dmg_mod := 10;
	if ( (weaponone.objtype == 0x13B2) or (weapontwo.objtype == 0x13B2) )
		var it := CreateItemInInventory(me.backpack,0xf3f,50);
		setobjproperty(it,"noloot",1);
	elseif ( (weaponone.objtype == 0x0f4f) or (weapontwo.objtype == 0x0f4f) or 
		(weaponone.objtype == 0x13fd) or (weapontwo.objtype == 0x13fd) )
		var it := CreateItemInInventory(me.backpack,0x1bfb,50);
		setobjproperty(it,"noloot",1);
	elseif ( (weaponone.objtype == 0x6050) or (weapontwo.objtype == 0x6050) )
		var it := CreateItemInInventory(me.backpack,0x6051,25);
		setobjproperty(it,"noloot",1);
	endif

	me.setmaster(opponent);
	if (opponent.title_prefix ) me.title_prefix := opponent.title_prefix; endif
	if (opponent.title_suffix ) me.title_suffix := opponent.title_suffix; endif
	var i;
	for (i := 0; i <= 48; i := i + 1)
		SetBaseSkill( me, i, (GetBaseSkill(opponent,i)+50) );
	endfor

	if (	(GetEffectiveSkill(me,SKILLID_MAGERY)>100) or (GetEffectiveSkill(me,SKILLID_MEDITATION)>100) or 
		(GetEffectiveSkill(me,SKILLID_SPIRITSPEAK)>100) or (GetEffectiveSkill(me,SKILLID_EVALINT)>100) )
			spellcast := 1;
	endif

	set_critical(0);
	me.concealed := 0;
	EnableCombatEvents();
	me.setmaster(0);

endfunction
