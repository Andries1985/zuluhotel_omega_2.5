use uo;
use os;
use basic;
use cfgfile;
use npc;

include "include/attributes";

function grabblocker()
    var me;
	if(!GetObjProperty (me, "Boss") && !GetObjProperty (me, "SuperBoss") && !GetObjProperty (me, "Champion"))
		return;
	endif
    
	var mypack := 0;
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
			bpack := CreateItemAtLocation( me.x , me.y , me.z , 0x0E75 , 1, me.realm);
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

	var Near_Items := ListItemsNearLocation(me.x, me.y, me.z, 2, me.realm);
    var notOk := 0;
    foreach item in Near_Items
        if(GetObjProperty(item,"houseserial") || item.multi) 
            notOk := 1;
        endif
    
	    if(CheckLineOfSight( me, item ))
            var sh := GetStandingHeight( cint(item.x), cint(item.y), cint(item.z), item.realm );
            if(sh.multi || sh.multi.serial)
                notOk := 1;
            endif

            var izplayer := GetObjProperty( item, "npctemplate" );
            if (item.objtype == UOBJ_CORPSE and !izplayer )
                notOk := 1;
            endif

            var info := cint(GetMapInfo( cint(item.x) , cint(item.y), item.realm ).landtile);
            if((info > cint(0x405)) && (info < cint(0x456)))
                notOk := 1;
            endif
        
            if(item.movable && !notOk)
                RunToItem(me,item);
                if( distance(me, item)<2)
                    if (playeritems(item, mypack))
                        say( "*yoink*" );
                        sleep(1);
                        return;
                    endif
                else
                    return;
                endif
            endif
            notOk := 0;
        endif
    endforeach
endfunction

function RunToItem(me,item)
	var numsteps;
	for (numsteps := 0; numsteps < 8; numsteps := numsteps +1)
		if ( distance(me, item) < 3 )
			return;
		else
			runtoward(item);
		endif
	endfor

endfunction

function playeritems(byref item, byref mypack)
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