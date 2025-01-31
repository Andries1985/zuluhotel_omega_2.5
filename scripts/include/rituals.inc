use uo;
use os;

include "include/namingbyenchant";
include "include/spelldata";
include "include/client";
include "include/eventid";
include "include/classes";
include "include/itemutil";
include "include/random";
include ":gumps:yesno";
include ":combat:hitscriptinc";

//Used objects
const RITUAL_ROBE        := 0xBA29;
const OAK_STAFF          := 0xBA28;
const NORTH_RUNE         := 0xBA24;
const MANA_CRYSTAL       := 0xBA2C;
const MANA_PILE          := 0x3789;
const WORDS_CAPTOR       := 0xBA2B;
const MANA_FLUX          := 0x3778;

//Switchs for processRitual
const UNDO_CIRCLE        := 0x000;
const KEEP_CIRCLE        := 0x001;

//Switchs for targetting
const CAN_BE_EQUIPPED    := 0x000;
const IS_A_WEAPON        := 0x002;
const IS_AN_ARMOR        := 0x004;
const HAVE_HP            := 0x006;
const CAN_BE_CURSED      := 0x000;
const CANNOT_BE_CURSED   := 0x001;


var captor;
var cfg;

function TryToDrawCircle( who )

	var x := who.x;
	var y := who.y;
	var z := who.z;
	var r := who.realm;
	var center_y := y + 5;

	if( len(ListItemsNearLocation( x , center_y , z , 6 )) >= 1 )
		SendSysmessage( who , "You don't have the necessary space to draw a circle there." );
		EraseObjProperty( who , "#Casting" );
		return 0;
	endif


	cfg := ReadConfigFile( "rituals" );

	
	var elem := FindConfigElem( cfg , "DrawCircle" );
	var current := 1;


	var starting_hp := who.hp;
	var magic_circle := {};
	var loop;

	var speechs_delay := cfg["General"].DelayBetweenSpeechs;
	var base_light    := cfg["General"].BaseLightLevel;


	//Create the speech captor
	captor := CreateItemAtLocation( x , center_y , z , WORDS_CAPTOR , 1, r );
	RegisterForSpeechEvents( captor , 6 );
	EnableEvents( SYSEVENT_SPEECH , 6 );

	SendSysmessage( who , "You can start to draw the circle" );


	while( GetConfigString( elem , "Speech" + current ) )

		var speech := GetConfigString( elem , "Speech" + current );
		var text := GetConfigString( elem , "Print"  + current );
		var x_mod := GetConfigInt(    elem , "Xmod"   + current );
		var y_mod := GetConfigInt(    elem , "Ymod"   + current );
		var facing := GetConfigInt(    elem , "Facing" + current );
		var action := Cint( GetConfigString( elem , "Action" + current ) );
		var appear := Cint( GetConfigString( elem , "Appear" + current ) );
		var anim := Cint( GetConfigString( elem , "Anim"   + current ) );
		var sfx := Cint( GetConfigString( elem , "Sfx"    + current ) );
		var consume := GetConfigStringArray( elem , "Consume" + current );

		//debugging happening here

		Sleep( speechs_delay );
		loop := 1;
		var ev;

		while( loop )

			if( who.x != (x + x_mod) or who.y != (y + y_mod) or who.hp < starting_hp )
				DestroyDone( who , magic_circle );
				return 0;
			endif

			ev := os::wait_for_event( 5 );

			if( ev and ev.source == who )
				set_critical(1);
				if( ev.type == SYSEVENT_SPEECH )
					if( ev.text[speech] )
						loop := 0;
					else
						DisableEvents( SYSEVENT_SPEECH );
						DestroyDone( who , magic_circle );
						return 0;
					endif
				endif
				set_critical(0);
			endif

		endwhile

		if( Len(consume) )
			set_critical(1);
			if(consume)
			foreach substance in consume
				var params	:= SplitWords( substance );
				var objtype	:= Cint( params[1] );
				if( !objtype )
					objtype := GetObjtypeByName( params[1] );
				endif

				var amount	:= 1;
				if( params[2] )
					amount	:= Cint( params[2] );
				endif

				if( !ConsumeObjType( who , objtype , amount ) )
					SendSysmessage( who , "You miss some substances to pursue the ritual any further." );
					DisableEvents( SYSEVENT_SPEECH );
					DestroyDone( who , magic_circle );
					EraseObjProperty( who , "#Casting" );
					return 0;
				endif
			endforeach
			endif
			set_critical(0);
		endif

		if( text )
			PrintTextAbove( who , text );
		endif

		if( action )
			PerformAction( who , action );
		endif

		if( anim )
			PlayObjectCenteredEffect( who , anim , 0xa , 0xa );
		endif

		if( sfx )
			PlaySoundEffect( who , sfx );
		endif

		if( appear )
			var itemX_mod := GetConfigInt( elem , "Xmod" + (current + 1) );
			var itemY_mod	:= GetConfigInt( elem , "Ymod" + (current + 1) );
			magic_circle.append( CreateItemAtLocation( (x + itemX_mod) , (y + itemY_mod) , z , appear , 1, r ) );
		endif

		foreach mobile in ListMobilesNearLocation( who.x , who.y , who.z , 10, who.realm );
			mobile.setlightlevel( base_light * current , 600 );
		endforeach


		current := current + 1;

	endwhile

	PerformAction( who , ANIM_CAST_AREA );
	PlaySoundEffect( who , 0x01fe );
	CreateMagicCircle( x , y , z , r, magic_circle );

	Sleep( 1 );

	CreateCandles( x , y , z , r, magic_circle );
	MakeTheCircleUnDecayable( magic_circle );

	return magic_circle;

endfunction



function CreateMagicCircle( x , y , z , r, byref magic_circle )

	magic_circle.append( CreateItemAtLocation( (x + 2) , (y + 7) , z , 0xBA01 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 1) , (y + 7) , z , 0xBA02 , 1, r ) );
	magic_circle.append( CreateItemAtLocation(  x      , (y + 7) , z , 0xBA03 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 1) , (y + 7) , z , 0xBA04 , 1, r) );
	magic_circle.append( CreateItemAtLocation( (x - 2) , (y + 7) , z , 0xBA05 , 1, r) );
	magic_circle.append( CreateItemAtLocation( (x - 2) , (y + 6) , z , 0xBA06 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 2) , (y + 5) , z , 0xBA07 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 2) , (y + 4) , z , 0xBA08 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 2) , (y + 3) , z , 0xBA09 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 1) , (y + 3) , z , 0xBA0A , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 1) , (y + 2) , z , 0xBA0B , 1, r ) );
	magic_circle.append( CreateItemAtLocation(  x      , (y + 2) , z , 0xBA0C , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 1) , (y + 2) , z , 0xBA0D , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 2) , (y + 2) , z , 0xBA0E , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 3) , (y + 2) , z , 0xBA0F , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 3) , (y + 3) , z , 0xBA10 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 3) , (y + 4) , z , 0xBA11 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 3) , (y + 5) , z , 0xBA12 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 3) , (y + 6) , z , 0xBA13 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 2) , (y + 6) , z , 0xBA14 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 1) , (y + 6) , z , 0xBA15 , 1, r ) );
	magic_circle.append( CreateItemAtLocation(  x      , (y + 6) , z , 0xBA16 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 1) , (y + 6) , z , 0xBA17 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 1) , (y + 5) , z , 0xBA18 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x - 1) , (y + 4) , z , 0xBA19 , 1, r ) );
	magic_circle.append( CreateItemAtLocation(  x      , (y + 3) , z , 0xBA1A , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 1) , (y + 3) , z , 0xBA1B , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 2) , (y + 3) , z , 0xBA1C , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 2) , (y + 4) , z , 0xBA1D , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 2) , (y + 5) , z , 0xBA1E , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 1) , (y + 5) , z , 0xBA1F , 1, r ) );
	magic_circle.append( CreateItemAtLocation(  x      , (y + 5) , z , 0xBA20 , 1, r ) );
	magic_circle.append( CreateItemAtLocation(  x      , (y + 4) , z , 0xBA21 , 1, r ) );
	magic_circle.append( CreateItemAtLocation( (x + 1) , (y + 4) , z , 0xBA22 , 1, r ) );

endfunction



function CreateCandles( x , y , z , r, byref magic_circle )

	magic_circle.append( CreateItemAtLocation(  x      , (y + 2) , z , 0x1854 , 1, r ) );
	magic_circle[len(magic_circle)].movable := 0;
	SetName( magic_circle[len(magic_circle)] , "Magic Candle" );
	PlaySoundEffect( magic_circle[len(magic_circle)] , 0x022b );
	Sleep( 1 );

	magic_circle.append( CreateItemAtLocation( (x + 3) , (y + 3) , z , 0x1854 , 1, r ) );
	magic_circle[len(magic_circle)].movable := 0;
	SetName( magic_circle[len(magic_circle)] , "Magic Candle" );
	PlaySoundEffect( magic_circle[len(magic_circle)] , 0x022b );
	Sleep( 1 );

	magic_circle.append( CreateItemAtLocation( (x + 2) , (y + 6) , z , 0x1854 , 1, r ) );
	magic_circle[len(magic_circle)].movable := 0;
	SetName( magic_circle[len(magic_circle)] , "Magic Candle" );
	PlaySoundEffect( magic_circle[len(magic_circle)] , 0x022b );
	Sleep( 1 );

	magic_circle.append( CreateItemAtLocation( (x - 1) , (y + 7) , z , 0x1854 , 1, r ) );
	magic_circle[len(magic_circle)].movable := 0;
	SetName( magic_circle[len(magic_circle)] , "Magic Candle" );
	PlaySoundEffect( magic_circle[len(magic_circle)] , 0x022b );
	Sleep( 1 );

	magic_circle.append( CreateItemAtLocation( (x - 2) , (y + 4) , z , 0x1854 , 1, r ) );
	magic_circle[len(magic_circle)].movable := 0;
	SetName( magic_circle[len(magic_circle)] , "Magic Candle" );
	PlaySoundEffect( magic_circle[len(magic_circle)] , 0x022b );
	Sleep( 1 );

endfunction



function DestroyDone( who , magic_circle , item := 0 )

	set_critical(1);
	var amount := len(magic_circle);

	var mana := who.mana;

	if( GetObjProperty( magic_circle[1] , "PermMagicCircle" ))
		var i;
		for( i := 5 ; i <= amount ; i := i + 1 )
			DestroyItem( magic_circle[i] );
		endfor
	else
		foreach part in magic_circle
			DestroyItem( part );
		endforeach
	endif

	if( item )
		DestroyItem( item );
	endif

	PrintTextAbove( who , "*The mystic energy go out of control and unleash itself upon " + who.name + "!*" );
	PlaySoundEffect( who , SFX_SPELL_LIGHTNING );
	who.mana := 0;
	ApplyRawDamage( who , amount * 2 + mana );
	EraseObjProperty( who , "#Casting" );
	DestroyItem( captor );
	set_critical(0);

endfunction



function ProcessRitual( who , item , byref magic_circle , the_ritual , keep_circ := UNDO_CIRCLE )

	var x  := magic_circle[1].x;
	var y  := magic_circle[1].y;
	var z  := magic_circle[1].z;
	var r  := magic_circle[1].realm;
	var cx := x;
	var cy := y + 5;
	var cz := z;

	var wizards := array( who );
	var manapool := 0;

	if( item )
		MoveObjectToLocation( item , cx , cy , cz , r, MOVEOBJECT_FORCELOCATION );
		item.movable := 0;
	endif

	SendSysMessage( who , "Do you wish to access some mana crystals' power?" );
	if( YesNo( who , "Use crystals?" ) )
		while( 1 )
			SendSysMessage( who , "Select a mana crystal to access, Hit Esc to cancel." );
			var crystal := Target( who , TGTOPT_CHECK_LOS );

			if( !crystal )
				break;
			elseif( !Accessible( who , crystal ) )
				SendSysMessage( who , "You can't reach that" );
			elseif( crystal.objtype != MANA_CRYSTAL )
				SendSysMessage( who , "You can only use mana crystal" );
			elseif( !GetObjProperty( crystal , "ManaLevel" ) )
				SendSysMessage( who , "This crystal have no mystic energy left to use." );
			else
				manapool := manapool + GetObjProperty( crystal , "ManaLevel" );
				EraseObjProperty( crystal , "ManaLevel" );
			endif
		endwhile
	endif

	SendSysMessage( who , "Do you wish to invite others wizards to your ritual?" );
	if( YesNo( who , "Invite wizards?" ) )
		while( 1 )
			SendSysMessage( who , "Select the wizard you want to invite. Hit Esc to cancel." );
			var wizard := Target( who );

			if( !wizard )
				break;
			elseif( !wizard.acctname )
				SendSysMessage( who , "That isn't a wizard." );
			elseif( !YesNo( wizard , "Agree to join?" ) )
				SendSysMessage( who , "He don't want to join the ritual." );
			elseif( !CheckRitualEquip( wizard ) )
				SendSysMessage( who , "That wizard isn't wearing the required equipment to perform a ritual." );
				SendSysMessage( wizard , "You aren't wearing the required equipment to perform a ritual." );
			else
				wizards.append( wizard );
				SendSysmessage( who, wizard.name + " agreed to join the ritual" );
				SendSysmessage( wizard, "You can go in the Circle" );
			endif
		endwhile
	endif

	Sleep( len(wizards) );

	PrintTextAbove( item , "*Every wizard gather in the circle as the magical ritual start*" );

	var elem := FindConfigElem( cfg , the_ritual );
	var amount := 1;
	while( GetConfigString( elem , "Speech" + amount ) )
		amount := amount + 1;
	endwhile

	amount := amount - 1;

	var manapile	:= CreateItemAtLocation( cx , cy , cz+(4*amount) , MANA_PILE , 1, r );
	SetName( manapile , "Mana Cloud" );
	manapile.movable := 0;

	SetObjProperty( captor , "RitualRunning" , 1 );

	var parms := {};
	parms[1] := wizards;
	parms[2] := magic_circle[1];
	parms[3] := captor;

	Start_Script( ":artificier:wizardpozcheck" , parms );



	var current := 1;
	var loop;

	while( GetObjProperty( captor , "RitualRunning" ) and GetConfigString( elem , "Speech" + current ) )

		var speech	:= GetConfigString( elem , "Speech" + current );
		var text	:= GetConfigString( elem , "Print"  + current );
		var appears	:= GetConfigString( elem , "Appear" + current );
		var action	:= Cint( GetConfigString( elem , "Action" + current ) );
		var anim	:= Cint( GetConfigString( elem , "Anim"   + current ) );
		var sfx	:= Cint( GetConfigString( elem , "Sfx"    + current ) );
		var x_mod	:= GetConfigInt( elem , "Xmod" + current );
		var y_mod	:= GetConfigInt( elem , "Ymod" + current );
		var facing	:= GetConfigInt( elem , "Facing" + current );
		var consume	:= GetConfigStringArray( elem , "Consume" + current );

		loop := 1;
		var ev;
		while( loop and GetObjProperty( captor , "RitualRunning" ) )

			ev := os::wait_for_event( 5 );
			if( ev )
				if( ev.source in wizards )
					if( ev.type == SYSEVENT_SPEECH )
						if( ev.text[speech] and !GetObjProperty( ev.source , "SaidFormula" + current ))
							if( x_mod )
								if( ev.source.x != x + x_mod )
									DisableEvents( SYSEVENT_SPEECH );
									DestroyItem( manapile );
									foreach wizard in wizards
										EraseObjProperty( wizard , "SaidFormula" + current );
									endforeach

									EnergyDiruption( wizards , item , magic_circle , manapool );
									EraseObjProperty( who , "#Casting" );
									return 0;
								endif
							endif

							if( y_mod )
								if( ev.source.y != y + y_mod )
									DisableEvents( SYSEVENT_SPEECH );
									DestroyItem( manapile );
									foreach wizard in wizards
										EraseObjProperty( wizard , "SaidFormula" + current );
									endforeach

									EnergyDiruption( wizards , item , magic_circle , manapool );
									EraseObjProperty( who , "#Casting" );
									return 0;
								endif
							endif

							SetObjProperty( ev.source , "SaidFormula" + current , 1 );
							var mana_amount := Cint( ev.source.mana / 4 );
							ev.source.mana := ev.source.mana - mana_amount;
							manapool := manapool + mana_amount;
							PlayMovingEffect( ev.source , manapile , MANA_FLUX , 0xa , 0xa );

							if( action )
								PerformAction( ev.source , action );
							endif

							if( text )
								PrintTextAbove( ev.source , text );
							endif

							if( anim )
								PlayObjectCenteredEffect( ev.source , anim , 0xa , 0xa );
							endif

							if( sfx )
								PlaySoundEffect( ev.source , sfx );
							endif

						else

							DisableEvents( SYSEVENT_SPEECH );
							DestroyItem( manapile );
							foreach wizard in wizards
								EraseObjProperty( wizard , "SaidFormula" + current );
							endforeach

							EnergyDiruption( wizards , item , magic_circle , manapool );
							EraseObjProperty( who , "#Casting" );
							return 0;
						endif
					endif
				endif
			endif

			var total := 0;
			foreach wizard in wizards
				if( GetObjProperty( wizard , "SaidFormula" + current ) )
					total := total + 1;
				endif
			endforeach

			if( total == len( wizards ) )

				foreach wizard in wizards
					EraseObjProperty( wizard , "SaidFormula" + current );
					var mana_amount := Cint( wizard.mana / 4 );
					wizard.mana := wizard.mana - mana_amount;
					manapool := manapool + mana_amount;
					PlayMovingEffect( wizard , manapile , MANA_FLUX , 0xa , 0xa );
					PerformAction( wizard , ANIM_CAST_DIR );
				endforeach

				if( item )
					PrintTextAbove( item , "*As the power words sequence is finished," );
					PrintTextAbove( item , "The mystic energy channels above the item.*" );
				endif

				if( len(consume) )
					set_critical(1);
					foreach substance in consume
						var params	:= SplitWords( substance );
						var objtype	:= Cint( params[1] );
						if( !objtype )
							objtype := GetObjtypeByName( params[1] );
						endif

						var amount	:= 1;
						if( params[2] )
							amount	:= Cint( params[2] );
						endif

						if( !ConsumeObjType( who , objtype , amount ) )
							SendSysmessage( who , "You miss some substances to pursue the ritual any further." );
							DisableEvents( SYSEVENT_SPEECH );
							DestroyDone( who , magic_circle );
							EraseObjProperty( who , "#Casting" );
							return 0;
						endif
					endforeach
					set_critical(0);
				endif

				if( appears )
					set_critical(1);
					var the_items   := SplitWords( appears );
					var item_x_mods := SplitWords( GetConfigString( elem , "IXmod" + current ) );
					var item_y_mods := SplitWords( GetConfigString( elem , "IYmod" + current ) );

					if( len(the_items) == len(item_x_mods) and len(the_items) == len(item_y_mods) )
						var count;
						for( count := 1 ; count <= len(the_items) ; count := count + 1 )
							var to_be_created := Cint( the_items[count] );
							var item_x_mod := Cint( item_x_mods[count] );
							var item_y_mod := Cint( item_y_mods[count] );
							if( !to_be_created )
								to_be_created := GetObjtypeByName( the_items[count] );
							endif

							magic_circle.append( CreateItemAtLocation( x + item_x_mod , y + item_y_mod , z , to_be_created , 1, r ) );
							magic_circle[len(magic_circle)].movable := 0;
						endfor
					endif
					set_critical(0);
				endif

				if( item )
					item.movable := 1;
					MoveObjectToLocation( item , item.x , item.y , item.z+2 , r, MOVEOBJECT_FORCELOCATION );
					item.movable := 0;
				endif

				manapile.movable := 1;
				MoveObjectToLocation( manapile , manapile.x , manapile.y , manapile.z-2 , r, MOVEOBJECT_FORCELOCATION );
				manapile.movable := 0;

				loop	:= 0;
				current := current + 1;
				Sleep( 2 );

			endif
		endwhile
	endwhile

	if( !GetObjProperty( captor , "RitualRunning" ) )
		DestroyItem( manapile );
		EnergyDiruption( wizards , item , magic_circle , manapool );
		EraseObjProperty( who , "#Casting" );
		return 0;
	endif

	Sleep( 5 );

	foreach wizard in wizards
		manapool := manapool + wizard.mana;
		wizard.mana := 0;
		PlayMovingEffect( wizard , manapile , MANA_FLUX , 0xa , 0xa );
		PerformAction( wizard , ANIM_CAST_DIR );
	endforeach

	PrintTextAbove( who , "*All the mystic energy goes through " + who.name + " as the final ritual process begins*" );
	PlaySoundEffect( who , 0x01fe );

	var times;
	var mana_part := Cint( manapool / 10 );
	for( times := 1 ; times <= 10 ; times := times + 1 )
		PlayMovingEffect( manapile , who , MANA_FLUX , 0xa , 0xa );
		who.mana := who.mana + mana_part;
		manapool := manapool - mana_part;
		Sleepms(500);
	endfor

	DestroyItem( manapile );
	EraseObjProperty( captor , "RitualRunning" );

	//Try to enchant the item
	if( !CheckRitualSkillCheck( wizards , item , magic_circle , the_ritual ) )
		EraseObjProperty( who , "#Casting" );
		return 0;
	endif

	//Try to undo the Circle
	if( len(magic_circle) > 4 )
		if( keep_circ == UNDO_CIRCLE )
			if( !TryToUndoTheCircle( who , magic_circle , item ) )
				EraseObjProperty( who , "#Casting" );
				return 0;
			endif
		else
			var to_be_destroyed := {};
			var index;
			for( index := 5 ; index <= len( magic_circle ) ; index := index + 1 )
				to_be_destroyed.append( magic_circle[index] );
			endfor
			DestroyCircle( to_be_destroyed );
		endif
	endif

	foreach mobile in ListMobilesNearLocation( who.x , who.y , who.z , 10 );
		mobile.setlightlevel( 0 , 600 );
	endforeach

	if( item )
		ReturnItemToCaster( who , item );
	endif

	foreach wizard in wizards
		SendSysMessage( wizard , "You're exhausted from the ritual" );
		wizard.mana := 0;
		wizard.stamina := 0;
		wizard.hp := 1;
	endforeach

	EndRitual( who , captor );

	return GetRitualPower( wizards , the_ritual );

endfunction


function CheckRitualSkillCheck( wizards , item , magic_circle , spell )

	//Variables declaration and initialisation
	var who	 := wizards[1];
	var conf	 := ReadConfigFile("::spells");
	var elem	 := FindConfigElem(conf, spell);
	var circ	 := GetConfigInt(elem, "Circle");
	conf	 	 := ReadConfigFile("::circles");
	var value	 := conf[circ].PointValue;
	var manacost	 := conf[circ].Mana;
	var difficulty := GetSpellDifficulty( spell );
	var min_diff	 := 100 + Cint((circ - 29) * 15);

	if( spell == SPELLID_RITUAL_OF_CONSECRATION or spell == SPELLID_RITUAL_OF_CHANNEL_MANA )
		who.mana := who.mana * 2;
	endif

	set_critical(1);
	foreach wizard in wizards
		var magery := Cint( GetSkill( wizard , SKILLID_MAGERY ) );
		if( wizard != who )
			magery := Cint( magery / 2 );
			if( GetObjProperty( wizard , CLASSEID_MAGE ) )
				magery := Cint( magery * ClasseBonus( wizard , CLASSEID_MAGE ) );
			endif

		elseif( Cint(GetObjProperty( wizard , CLASSEID_MAGE )) >= 5 )
			magery := Cint( magery * ClasseBonus( wizard , CLASSEID_MAGE ) - Cint( magery * 2) );
		endif

		difficulty := difficulty - magery;
	endforeach

	set_critical(0);

	if( difficulty < min_diff )
		difficulty := min_diff;
	endif

	if( !ConsumeReagents( who , spell ) )
		PlayObjectCenteredEffect(who, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE , LOOP_SPELL_FIZZLE );
		PlaySoundEffect(who, SFX_SPELL_FIZZLE);
		SendSysMessage(who, "Insufficient Reagents.");
		DestroyDone( who , magic_circle , item );
		EraseObjProperty( who , "#Casting" );
		EndRitual( who , captor );
		return 0;
	endif

	if( !ConsumeMana( who , spell ) )
		PlayObjectCenteredEffect(who, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE);
		PlaySoundEffect(who, SFX_SPELL_FIZZLE);
		SendSysMessage(who, "Insufficient Mana.");
		DestroyDone( who , magic_circle , item );
		EraseObjProperty( who , "#Casting" );
		EndRitual( who , captor );
		return 0;
	endif

	if( !CheckSkill( who , SKILLID_MAGERY , difficulty , value ) )
		if( GetObjProperty( who , CLASSEID_MAGE ) )
			if( !CheckSkill( who , SKILLID_MAGERY , difficulty , value ) )
				PlayObjectCenteredEffect(who, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
				PlaySoundEffect(who, SFX_SPELL_FIZZLE);
				SendSysmessage(who, "The spell fizzles");
				DestroyDone( who , magic_circle , item );
				EraseObjProperty( who , "#Casting" );
				EndRitual( who , captor );
				return 0;
			endif
		else
			PlayObjectCenteredEffect(who, FX_SPELL_FIZZLE, SPEED_SPELL_FIZZLE, LOOP_SPELL_FIZZLE );
			PlaySoundEffect(who, SFX_SPELL_FIZZLE);
			SendSysmessage(who, "The spell fizzles");
			DestroyDone( who , magic_circle , item );
			EraseObjProperty( who , "#Casting" );
			EndRitual( who , captor );
			return 0;
		endif
	endif

	Sleep( 5 );

	PrintTextAbove( item , "The item absorbed the enchantment successfully!" );
	PlayObjectCenteredEffect( item , FX_SPELL_MARK , SPEED_SPELL_MARK , LOOP_SPELL_MARK );
	PlaySoundEffect( who , SFX_SPELL_MARK );

	foreach wizard in wizards
		AwardFame( wizard , Cint( (difficulty - 90) * 50 ) );
	endforeach

	return 1;

endfunction


function TryToUndoTheCircle( who , magic_circle , item := 0 )

	var cfg	:= ReadConfigFile( "rituals" );
	var elem	:= FindConfigElem( cfg , "UndoCircle" );
	var current	:= 1;

	var x := magic_circle[1].x;
	var y := magic_circle[1].y;
	var z := magic_circle[1].z;
	var starting_hp := who.hp;

	var loop;

	var speechs_delay := cfg["General"].DelayBetweenSpeechs;


	SendSysMessage( who , "You must now undo the magic circle to successfully finish the ritual." );

	while( GetConfigString( elem , "Speech" + current ) )

		var speech	:= GetConfigString( elem , "Speech" + current );
		var text	:= GetConfigString( elem , "Print" + current );
		var x_mod	:= GetConfigInt( elem , "Xmod" + current );
		var y_mod	:= GetConfigInt( elem , "Ymod" + current );
		var facing	:= GetConfigInt( elem , "Facing" + current );
		var appear	:= GetConfigInt( elem , "Appear" + current );


		Sleep( speechs_delay );
		loop := 1;
		var ev;

		while( loop )

			if( who.x != (x + x_mod) or who.y != (y + y_mod) or who.hp < starting_hp )
				DisableEvents( SYSEVENT_SPEECH );
				DestroyDone( who , magic_circle , item );
				EraseObjProperty( who , "#Casting" );
				EndRitual( who , captor );
				return 0;
			endif

			ev := os::wait_for_event( 5 );

			if( ev and ev.source == who )
				if( ev.type == SYSEVENT_SPEECH )
					if(ev.text[speech] )
						loop := 0;
					else
						DisableEvents( SYSEVENT_SPEECH );
						DestroyDone( who , magic_circle , item );
						EraseObjProperty( who , "#Casting" );
						EndRitual( who , captor );
						return 0;
					endif
				endif
			endif

		endwhile

		if( text )
			PrintTextAbove( who , text );
		endif
		current := current + 1;

	endwhile

	DisableEvents( SYSEVENT_SPEECH );

	DestroyCircle( magic_circle );
	PlaySoundEffect( who , 0x01fe );
	EndRitual( who , captor );

	return 1;

endfunction



function DestroyCircle( magic_circle )

	foreach part in magic_circle
		DestroyItem( part );
	endforeach

endfunction



function CheckRitualEquip( who )

	var staff := GetEquipmentByLayer( who, LAYER_HAND2 );
	var robe  := GetEquipmentByLayer( who, LAYER_ROBE );
	var mount := GetEquipmentByLayer( who, LAYER_MOUNT );

	if( staff.objtype != OAK_STAFF or robe.objtype != RITUAL_ROBE )
		SendSysMessage( who, "You must wear a consecrated ritual robe and young oak staff to perform a ritual." );
		return 0;
	endif

	if( GetObjProperty( staff, "Consecrated" ) != who.serial or GetObjProperty( robe, "Consecrated" ) != who.serial )
		SendSysMessage( who, "Your ritual equipment isn't consecrated" );
		return 0;
	endif

	if( mount )
		SendSysMessage( who, "You can't perform a ritual while mounted." );
		return 0;
	endif

	return 1;

endfunction


function EnergyDiruption( wizards, item, magic_circle, manapool )

	var amount := len( magic_circle );

	foreach wizard in wizards
		manapool := manapool + wizard.mana;
		wizard.mana := 0;
	endforeach

	DestroyItem( item );

	var dmg := Cint(manapool+(amount*2) / len(wizards) );

	foreach wizard in wizards
		PrintTextAbove( wizard , "*The mystic energy goes out of control and unleashes itself " + wizard.name + "!*" );
		PlaySoundEffect( wizard , SFX_SPELL_LIGHTNING );
		ApplyRawDamage( wizard , dmg );
	endforeach

	if( GetObjProperty( magic_circle[1] , "PermMagicCircle" ) )
		var i;
		for( i := 5 ; i <= amount ; i := i + 1 )
			DestroyItem( magic_circle[i] );
		endfor
	else
		foreach part in magic_circle
			DestroyItem( part );
		endforeach
	endif

	EraseObjProperty( wizards[1] , "#Casting" );

endfunction


function ReturnItemToCaster( who , item )

	item.movable := 1;
	MoveItemToContainer( item , who.backpack );

endfunction


function MakeTheCircleUnDecayable( magic_circle )

	foreach part in magic_circle
		part.decayat := 0;
	endforeach

endfunction


function LookForTheCircle( who )

	var is_permanent;
	var magic_circle := {};

	//Check if the caster want to use his personnal circle
	foreach thing in ListItemsAtLocation( who.x , who.y , who.z );
		if( thing.objtype == NORTH_RUNE and GetObjProperty( thing , "PermMagicCircle" ) )
			is_permanent := thing;
			break;
		endif
	endforeach

	//If he have no permanent circle, then he must draw a new one
	if( is_permanent )
		if( GetObjProperty( is_permanent , "PermMagicCircle" ) != who.serial )
			SendSysMessage( who , "You can't perform a ritual by using the circle of someone else." );
			return;
		endif

		magic_circle.append( is_permanent );
		var runes_serials := GetObjProperty( is_permanent , "RunesSerials" );
		if( !runes_serials or len(runes_serials) < 3 )
			SendSysMessage( who , "Your circle was corrupted!" );
			return;
		endif

		foreach serial in runes_serials
			var rune := SystemFindObjectBySerial( cint(serial) );
			if( !rune )
				SendSysMessage( who , "Your circle was corrupted!" );
				return;
			endif
			magic_circle.append( rune );
		endforeach

		cfg := ReadConfigFile( "rituals" );

		//Create the speech captor
		captor := CreateItemAtLocation( who.x , who.y + 5 , who.z , WORDS_CAPTOR , 1 );
		RegisterForSpeechEvents( captor , 6 );
		EnableEvents( SYSEVENT_SPEECH , 6 );

		PrintTextAbove( who , "*The sky darken*" );
		foreach mobile in ListMobilesNearLocation( who.x , who.y , who.z , 10 );
			mobile.setlightlevel( 24 , 600 );
		endforeach

		CreateMagicCircle( is_permanent.x , is_permanent.y , is_permanent.z , magic_circle );
		CreateCandles( is_permanent.x , is_permanent.y , is_permanent.z , magic_circle );
		MakeTheCircleUnDecayable( magic_circle );

		return magic_circle;
	else
		var circle := TryToDrawCircle( who );
		if( !circle )
			EndRitual( who , captor );
		endif
		return circle;
	endif

endfunction


function TargetItemToEnchant( who, magic_circle, options, msg )

	while( 1 )
		SendSysMessage( who, msg );
		var item := Target( who, TGTOPT_CHECK_LOS );

		if( !item )
			SendSysmessage( who, "The ritual is aborted" );
			EraseObjProperty( who, "#Casting" );

			if( GetObjProperty( magic_circle[1] , "PermMagicCircle" ))
				var i;
				for( i := 5 ; i <= len(magic_circle) ; i := i + 1 )
					DestroyItem( magic_circle[i] );
				endfor
			else
				TryToUndoTheCircle( who, magic_circle );
			endif
			return 0;

		elseif( !Accessible( who, item ) )
			SendSysMessage( who, "You can't reach that" );

		elseif( !ReserveItem( item ) )

		elseif( item in ListEquippedItems( who ) )
			SendSysMessage( who, "You must unequip it first." );

		elseif( GetObjProperty( item, "UnEnchantable" ) )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.hitscript == ":combat:voidscipt" )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.hitscript == ":combat:banishscript" )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.hitscript == ":combat:blackrockscript" )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.hitscript == ":combat:lifedrainscript" )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.hitscript == ":combat:manadrainscript" )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.hitscript == ":combat:piercingscript" )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.hitscript == ":combat:spellstrikescript" )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( GetObjProperty( item, "IsGMItem" ) )
			SendSysMessage( who, "You can't enchant this item." );

		elseif( item.newbie )
			SendSysMessage( who, "You can't enchant newbied items." );

		elseif( item.equipscript == "::skilladvancerequip" and item.unequipscript == "::skilladvancerunequip" )

			case( options )

				0x000:
					return item;
					break;


				0x001:
					var curse := GetObjProperty( item , "Cursed" );
					if( curse == 2 or curse == 3 )
						SendSysmessage( who , "You can't target cursed items." );
					else
						return item;
					endif
					break;


				0x002:
					if( item.skillid )
						return item;
					else
						SendSysMessage( who , "You must target a weapon." );
					endif
					break;


				0x003:
					if( item.skillid )
						var curse := GetObjProperty( item , "Cursed" );
						if( curse == 2 or curse == 3 )
							SendSysmessage( who , "You can't target cursed items." );
						else
							return item;
						endif
					else
						SendSysMessage( who , "You must target a weapon." );
					endif
					break;


				0x004:
					if( item.ar )
						return item;
					else
						SendSysMessage( who , "You must target an armor." );
					endif
					break;


				0x005:
					if( item.ar )
						var curse := GetObjProperty( item , "Cursed" );
						if( curse == 2 or curse == 3 )
							SendSysmessage( who , "You can't target cursed items." );
						else
							return item;
						endif
					else
						SendSysMessage( who , "You must target an armor." );
					endif
					break;


				0x006:
					if( item.hp )
						return item;
					else
						SendSysMessage( who , "You must target an item that have hp." );
					endif
					break;


				0x007:
					if( item.hp )
						var curse := GetObjProperty( item , "Cursed" );
						if( curse == 2 or curse == 3 )
							SendSysmessage( who , "You can't target cursed items." );
						else
							return item;
						endif
					else
						SendSysMessage( who , "You must target an item that have hp." );
					endif
					break;

			endcase

		else
			SendSysMessage( who , "The item must be equippable." );
		endif

	endwhile

endfunction


function GetRitualPower( wizards , ritualid )

	var who    := wizards[1];
	var rand   := Cint( GetSkill( who, SKILLID_MAGERY ) / 40 );
	var number := len( wizards );
	var circle := GetCircle ( ritualid );
	var power  := RandomInt( rand ) + 1;

	if( number >= 5 )
		power := power + 2;
	elseif( number >= 3 )
		power := power + 1;
	endif

	foreach wizard in wizards
		if( GetObjProperty( wizard, CLASSEID_MAGE ) )
			power := power + Cint( ClasseBonus( wizard, CLASSEID_MAGE ) );
		endif
	endforeach

	power := power + 29 - circle;

	if( power > 6 )
		power := 6;
	elseif( power < 1 )
		power := 1;
	endif

	return power;

endfunction


function TryToPerformRitual( who,
		spellid,
		opt 	:= CAN_BE_EQUIPPED + CAN_BE_CURSED,
		msg 	:= "Choose the item you wish to enchant.",
		keep_circ 	:= UNDO_CIRCLE )


	//Check if the caster have some unwanted equipment
	if( !CheckRitualEquip( who ) )
		EraseObjProperty( who , "#Casting" );
		return 0;
	endif


	//Look for the magic circle
	var magic_circle := LookForTheCircle( who );
	if( !magic_circle )
		EndRitual( who , captor );
		return 0;
	endif

	if( GetObjProperty( magic_circle[1] , "PermMagicCircle" ))
		keep_circ := KEEP_CIRCLE;
	endif

	//Get the item to enchant
	var item;
	if( msg != "" )
		item := TargetItemToEnchant( who , magic_circle , opt , msg );
		if( !item )
			EndRitual( who , captor );
			return 0;
		endif
	endif

	if( !item )
		item := 0;
	endif

	//Try to perform the ritual
	var power := ProcessRitual( who , item , magic_circle , spellid , keep_circ );
	if( !power )
		EndRitual( who , captor );
		return 0;
	endif


	EndRitual( who , captor );
	return { item , power };


endfunction



function EndRitual( who , captor )

	EraseObjProperty( who , "#Casting" );
	DisableEvents( SYSEVENT_SPEECH );
	DestroyItem( captor );

endfunction