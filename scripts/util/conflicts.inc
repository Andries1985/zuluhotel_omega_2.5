////////////////////////////////////////////////////////////
//
//  conflicts.inc
//
//  Author: rje
//
//  Date: 12/31/98
//
//  This module includes functions for targetting and
//  untargetting another creature, and general functions
//  for handling conflicts and eating!
//
////////////////////////////////////////////////////////////
use os;
use cfgfile;
use npc;

include "include/attributes";

var BadGuy         := 0;
var Hunger         := 0;

var conf           := ReadConfigFile( "corpses" );

Const UOBJECT_CORPSE  := 0x2006;  // for the carnivore in all of us
Const UOBJECT_BLOOD   := 0x122d;
Const UOBJECT_MEAT    := 0x09f1;
Const UOBJECT_HUMAN   := 0x190;

Const IN_RANGE        := 5;

//////////////////////////////////////////////////////////////////////////////
//
//  co_human()
//
//  Returns 1 if the object passed in has a human objid.
//
//////////////////////////////////////////////////////////////////////////////
function co_human(theobj)

   if ( theobj.objtype == UOBJECT_HUMAN
     or theobj.objtype == UOBJECT_HUMAN+1 )
      return 1;
   else
      return 0;
   endif

endfunction

/////////////////////////////////////////////////////
//
//  Target dude for termination.
//
/////////////////////////////////////////////////////
function co_target( dude )

   var msg := GetObjProperty( me, "targetText" );

   if (msg)
      say ( msg );
   endif

   SetObjProperty( me, "BadGuy", dude );
   SetOpponent( dude );
   SetWarMode(1);
   BadGuy := dude;
   return dude;

endfunction

///////////////////////////////////////////////////
//
//  co_untarget
//
//  Undo the target lock.
//
///////////////////////////////////////////////////
function co_untarget()

   SetObjProperty( me, "BadGuy", "" );
   SetWarMode( 0 );
   set_critical( 0 );
   BadGuy := 0;

endfunction



////////////////////////////////////////////////////////////////
//
//  eatTheDead()
//
//  Search for a corpse in the immediate vicinity, and eat it.
//  Returns the amount of food derived from eating a corpse,
//  or 0 if no food was gotten.
//
//  hunger is updated accordingly.
//
////////////////////////////////////////////////////////////////
function co_eatTheDead()

   Hunger := Hunger + 1;

   foreach body in ListItemsNearLocation( me.x, me.y, me.z, IN_RANGE )

      if ( ( body.objtype == UOBJECT_CORPSE ) 
       and ( GetObjProperty( body, "cut" ) != "1") )

         while ( Distance( me, body ) >= 1 )
            WalkToward( body );
         endwhile

         var c_type     := body.corpsetype;
         var elem       := FindConfigElem(conf, c_type);
         var item1      := GetConfigint( elem, "item1"   );
         var amount1    := GetConfigint( elem, "amount1" );
         var item2      := GetConfigint( elem, "item2"   );
         var amount2    := GetConfigint( elem, "amount2" );

         SetObjProperty( body, "cut","1" );

           // item decay handled by core, v069+
           // var parms array;  // used to also have a local var for "theblood"
           // parms[1] := body;
           // parms[2] := CreateItemAtLocation( body.x, body.y, body.z, UOBJECT_BLOOD, 1, body.realm);// theblood
           // start_script("decay",parms);

         if ( ( item1 == UOBJECT_MEAT ) or  ( item2 == UOBJECT_MEAT ) )
            say( "* munch munch *" );
            Hunger := Hunger - ( amount1 * 500 );
            return amount1 * 500;
         endif
      endif

   endforeach
   return 0;

endfunction

////////////////////////////////////////////////////////
//
//  co_fight()
//
//  A basic fighting routine plagiarized from Dan's
//  code.
//
//  * if the opponent is dead/gone/far away, return
//    to normal.
//
//  * if we're badly wounded, run!
//
//  * otherwise, close distance.
//
////////////////////////////////////////////////////////
function co_fight()
  
   var d  := Distance( me, BadGuy );

   if ((BadGuy.dead) or (!BadGuy)  or (d > 10) )
      co_untarget();
      return;
   elseif( GetHP(me) <= Cint( GetMaxHP(me) / 5 ) and d < 16)
      RunAwayFrom( BadGuy );
      TurnToward( BadGuy );
   elseif (d > 2)        
      RunToward(   BadGuy );
   elseif (d > 1)
      WalkToward(  BadGuy );
   endif        

endfunction
