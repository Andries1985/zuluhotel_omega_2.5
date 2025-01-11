use uo;
use os;
use basic;
use cfgfile;
use npc;

include "include/attributes";

Const STARTHAIR := 0x2030;
Const ENDHAIR := 0x2060;

function grabloot()
	var me;
	if( GetGlobalProperty( "#NoNpcLooting" ) )
		return;
	endif
	if (GetObjProperty (me, "tmonster"))
		return;
	endif
	var numitems := 0;
	var mypack := 0;
	var izplayer;
	if (!getobjproperty(me,"serial"))
		setobjproperty(me, "serial", me.serial);
	endif
	
	mypack := me.backpack;
	if( !mypack )
		var bpack := CreateItemAtLocation( me.x , me.y , me.z , 0x0E75 , 1, me.realm );
		if( bpack )
			if( EquipItem( me , bpack ) )
				mypack := bpack;
			else
				return;
			endif
		else
			bpack := CreateItemAtLocation( me.x , me.y , me.z , 0x0E75 , 1, me.realm );
			if( bpack )
				if( EquipItem( me , bpack ) )
					mypack := bpack;
				else
					return;
				endif
			else
				return;
			endif
		endif
	endif	

	var Near_Items := ListItemsNearLocation(me.x, me.y, me.z, 6, me.realm);

     foreach item in Near_Items

        if ((GetObjProperty(item,"houseserial")) || (item.multi) || (item.movable<>0)) return; endif
	izplayer := GetObjProperty( item, "npctemplate" );
	if( CheckLineOfSight( me, item ) )

	var sh := GetStandingHeight( cint(item.x), cint(item.y), cint(item.z) );
	if (sh.multi || sh.multi.serial)
		break;
	endif
	var info := cint(GetMapInfo( cint(item.x) , cint(item.y), item.realm ).landtile);
	if ((info > cint(0x405)) && (info < cint(0x456)))
		break;
	endif
	
	if ( (item.movable) && ((item.objtype < STARTHAIR || item.objtype > ENDHAIR)) and item.objtype != 0x1f03 )
		RunToIt(me,item);
		if ( distance(me, item)<2)
			if (playeritem(item, mypack))
			say( "*yoink*" );
			sleep(2);
			return;
			endif
		else
			return;
		endif
	elseif (item.objtype == UOBJ_CORPSE and !izplayer )
	    if (!GetObjProperty(item,"#ignoreit") && !GetObjProperty( item , "nocut" ) )
		var Items := { };

		foreach thingie in EnumerateItemsIncontainer(Item)
		if ( thingie.objtype != 0x1f03 and   (thingie.objtype < STARTHAIR || thingie.objtype > ENDHAIR) 
			&& (!cint(GetObjProperty(thingie,"#ignoreit"))) )
			Items.append( thingie );
		endif
		endforeach

		if ( len(Items) )
			RunToIt(me,item);
			if (distance(me, item) < 2)
				foreach thingie in Items
				    if ( playeritem(thingie, mypack) )
				    	var looters := GetObjProperty( item , "LootedBy" );
					if( !looters )
						looters := {};
					endif
					
					if( !(me.name in looters) )
						looters.append( me.name );
						SetObjProperty( item , "LootedBy" , looters );
					endif
					
					say( "*yoink*" );
					sleep(2);
					return;
				    else
					SetObjProperty(thingie,"#ignoreit",1);
				    endif
				endforeach
			endif 
		else
			SetObjProperty(item,"#ignoreit",1);
		endif

	    endif
	endif

	endif
     endforeach

endfunction

function RunToIt(me,item)

	var numsteps;
	for (numsteps := 0; numsteps < 8; numsteps := numsteps +1)
		if ( distance(me, item) < 2 )
			return;
		else
			runtoward(item);
		endif
	endfor

endfunction

function playeritem(byref item, byref mypack)

SetObjProperty( item, "dropitem", 1 );

var usescript:=GetObjProperty( item, "old_us");
if(usescript)
	eraseObjProperty( item, "old_us");
	item.usescript := cstr(usescript);
else
	item.usescript := "";
endif

return moveitemtocontainer(item, mypack);

endfunction
