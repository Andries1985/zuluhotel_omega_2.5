///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////
//  MOONGATES.INC - Places moongates into the world.
//
//  This file is to be included into start.src and the function
//  CreateTeleporters() called from there.
//
//  Author: Ignatius 1999/05/27
//
//  Notes: System moongates are not saved, they are created upon
//  each system startup.
//
//  Modified by Madman Across the Water, 2000/03/14
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////

use uo;
use os;

// This should be a var variable since other scripts need it
// to cycle the moongates via the index variable.

function CreateMoongates();

    // since the language don't have truely var variables
    // (cross-script) we have to put this into the systemmoongate.src
    // aswell.
        var moongates := { 
        {4467,	1283,	5,		"britannia"},		// moonglow
    	{1336,	1997,	5,		"britannia"},		// britain
    	{1499,	3771,	5,		"britannia"},		// jhelom
        {771,	753,	5,		"britannia"},		// yew
    	{2701,	693,	5,		"britannia"},		// minoc
    	{1828,	2948,	-20,	"britannia"},		// trinsic
    	{643,	2067,	5,		"britannia"},		// skara
    	{3564,	2140,	34,		"britannia"},		// magincia
		{802,	1204,	25,		"tokuno"},			// Zento City
		{1169,	998,	41,		"tokuno"},			// Isamu-Jima Island
		{270,	628,	15,		"tokuno"},			// Homare-Jima Island
		{2264, 	1574,	-28,	"ilshenar"},		//Vitures Gate System
		{1015,	527,	-65,	"malas"},			//Luna
		{1997,	1386,	-85,	"malas"},			//Umbra
		{851,	3525,	-38,	"termur"}			//Royal City
    };

	var moongate;

    syslog( "Creating moongates." );

	foreach gate in moongates
		moongate := CreateItemAtLocation( CInt(gate[1]) , CInt(gate[2]) , CInt(gate[3]) , 0x6201 , 1, gate[4] );

      	moongate.facing := 1;
      	moongate.decayat := 0;
      	moongate.movable := 0;
 
   endforeach
endfunction

