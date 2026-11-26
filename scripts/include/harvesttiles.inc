///////////////////////////////////////////////////////////////////////////
// harvesttiles.inc
//
// Which map tiles each gathering skill works on, and checks that the tile
// is really at the spot.
//
// TargetCoordinates hands scripts whatever graphic the client says was
// clicked, without checking it, so a macro can "click" a tree on bare grass.
// Never trust use_on.objtype: these functions read the real statics and land
// tile at x,y instead.
//
// The lists were picked tile by tile from the client data in November 2026.
// Each must match its regions/*.cfg list, because the engine's
// GetHarvestDifficulty checks that list too:
//   wood  IsTree                       regions/wood.cfg  ObjType
//   ore   IsOreLand / IsOreStatic      regions/ore.cfg   LandTile / ObjType
//   sand  IsSand                       regions/sand.cfg  LandTile
//   clay  IsSwamp                      regions/clay.cfg  LandTile
//   fish  IsFishLand / IsFishStatic    regions/fish.cfg  LandTile / ObjType
//   water IsWaterLand / IsWaterStatic  pitchers and bandages (no cfg)
///////////////////////////////////////////////////////////////////////////

use uo;


// A tree that is an item on the ground: Botanik planted trees and decoration
// trees. The server resolves the targeted item itself, so its graphic can be
// trusted, unlike the graphic the client reports for a map tile. Before 1.1.6
// these gave wood, and they still do; the wood comes from the region's pool.
function IsTreeItem( use_on, realm := 0 )

	var item := use_on.item;
	if( !item || item.container )
		return 0;
	elseif( realm && item.realm != realm )
		return 0;
	endif
	return IsTree( item.graphic );

endfunction


// Returns the real tile at x,y that resource ("wood", "ore", "sand", "clay"
// or "fish") can be gathered from, or 0 if there is none.
// Statics are checked before the land tile, since they sit on top of it.
function FindHarvestTile( resource, x, y, realm := 0 )

	if( !realm )
		realm := _DEFAULT_REALM;
	endif
	x := CInt( x );
	y := CInt( y );

	foreach thing in ListStaticsAtLocation( x, y, LIST_IGNORE_Z, ITEMS_IGNORE_MULTIS, realm )
		if( HarvestTileListed( resource, thing.objtype, 0 ) )
			if( HarvestTileAt( resource, x, y, thing.objtype, realm ) )
				return thing.objtype;
			endif
		endif
	endforeach

	var land := GetMapInfo( x, y, realm ).landtile;
	if( HarvestTileListed( resource, land, 1 ) )
		if( HarvestTileAt( resource, x, y, land, realm ) )
			return land;
		endif
	endif

	return 0;

endfunction


// The engine's own check: tile is in regions/<resource>.cfg and really is at x,y.
// Test .errortext, never the value itself: the difficulty roll can be 0.
// A used-up spot still counts as the tile being there, so the skill can say
// "there's none left" instead of "you can't use that".
function HarvestTileAt( resource, x, y, tile, realm )

	var difficulty := GetHarvestDifficulty( resource, x, y, tile, realm );
	if( difficulty.errortext and difficulty.errortext != "Resource is depleted" )
		return 0;
	endif
	return 1;

endfunction


function HarvestTileListed( resource, tile, is_land )

	case( resource )
		"wood":
			if( !is_land )
				return IsTree( tile );
			endif
		"ore":
			if( is_land )
				return IsOreLand( tile );
			endif
			return IsOreStatic( tile );
		"sand":
			if( is_land )
				return IsSand( tile );
			endif
		"clay":
			if( is_land )
				return IsSwamp( tile );
			endif
		"fish":
			if( is_land )
				return IsFishLand( tile );
			endif
			return IsFishStatic( tile );
	endcase

	return 0;

endfunction


// Water to fill a pitcher or clean bandages: a trough, tub, barrel or other
// water item placed in the world, a real water static, or water land at x,y.
// where is a TargetCoordinates result.
function IsWater( where, realm := 0 )

	if( !where )
		return 0;
	endif
	if( !realm )
		realm := _DEFAULT_REALM;
	endif

	if( where.item )
		if( where.item.container )
			return 0;
		elseif( IsWaterStatic( where.item.graphic ) )
			return 1;
		endif
	endif

	var x := CInt( where.x );
	var y := CInt( where.y );
	foreach thing in ListStaticsAtLocation( x, y, LIST_IGNORE_Z, ITEMS_IGNORE_MULTIS, realm )
		if( IsWaterStatic( thing.objtype ) )
			return 1;
		endif
	endforeach

	return IsWaterLand( GetMapInfo( x, y, realm ).landtile );

endfunction


// Tree statics you can chop or carve kindling from: wood.cfg ObjType.
function IsTree( tile )

	if( ((tile >= 0x0C95) and (tile <= 0x0C96)) or
	    ((tile >= 0x0C99) and (tile <= 0x0C9E)) or
	    (tile == 0x0CA8) or
	    ((tile >= 0x0CAA) and (tile <= 0x0CAB)) or
	    ((tile >= 0x0CC8) and (tile <= 0x0CEA)) or
	    ((tile >= 0x0CF3) and (tile <= 0x0D03)) or
	    ((tile >= 0x0D41) and (tile <= 0x0DAB)) or
	    ((tile >= 0x12B6) and (tile <= 0x12C7)) or
	    ((tile >= 0x1B1F) and (tile <= 0x1B26)) or
	    (tile == 0x1B7E) )
		return 1;
	endif
	return 0;

endfunction


// Rock, mountain, cave and dirt land tiles that give ore: ore.cfg LandTile.
function IsOreLand( tile )

	if( ((tile >= 0x00DE) and (tile <= 0x00DF)) or
	    ((tile >= 0x00E2) and (tile <= 0x00E7)) or
	    ((tile >= 0x00F4) and (tile <= 0x00F7)) or
	    ((tile >= 0x0104) and (tile <= 0x0107)) or
	    ((tile >= 0x0110) and (tile <= 0x0113)) or
	    ((tile >= 0x0122) and (tile <= 0x0125)) or
	    ((tile >= 0x01D3) and (tile <= 0x01DA)) or
	    ((tile >= 0x021F) and (tile <= 0x0230)) or
	    ((tile >= 0x0232) and (tile <= 0x023B)) or
	    ((tile >= 0x0245) and (tile <= 0x026D)) or
	    ((tile >= 0x02BC) and (tile <= 0x02CB)) or
	    ((tile >= 0x063B) and (tile <= 0x063E)) or
	    ((tile >= 0x06CD) and (tile <= 0x06D1)) or
	    ((tile >= 0x06DA) and (tile <= 0x06DD)) or
	    ((tile >= 0x06EB) and (tile <= 0x06FE)) or
	    ((tile >= 0x070D) and (tile <= 0x0714)) or
	    ((tile >= 0x071D) and (tile <= 0x0720)) or
	    ((tile >= 0x072B) and (tile <= 0x0732)) or
	    ((tile >= 0x073B) and (tile <= 0x073E)) or
	    ((tile >= 0x0749) and (tile <= 0x0750)) or
	    ((tile >= 0x0759) and (tile <= 0x075C)) )
		return 1;
	endif
	return 0;

endfunction


// Cave floor statics that give ore: ore.cfg ObjType.
function IsOreStatic( tile )

	if( ((tile >= 0x053B) and (tile <= 0x054F)) or
	    ((tile >= 0x0551) and (tile <= 0x0553)) )
		return 1;
	endif
	return 0;

endfunction


// Sand land tiles: sand.cfg LandTile.
function IsSand( tile )

	if( ((tile >= 0x0016) and (tile <= 0x003A)) or
	    ((tile >= 0x0044) and (tile <= 0x004B)) or
	    ((tile >= 0x011E) and (tile <= 0x0121)) or
	    ((tile >= 0x0126) and (tile <= 0x012D)) or
	    (tile == 0x0192) or
	    ((tile >= 0x01A8) and (tile <= 0x01AB)) or
	    ((tile >= 0x01B9) and (tile <= 0x01D1)) or
	    ((tile >= 0x0282) and (tile <= 0x0285)) or
	    ((tile >= 0x028A) and (tile <= 0x0291)) or
	    ((tile >= 0x0335) and (tile <= 0x033C)) or
	    ((tile >= 0x0342) and (tile <= 0x0344)) or
	    (tile == 0x034D) or
	    ((tile >= 0x034F) and (tile <= 0x0354)) or
	    ((tile >= 0x0359) and (tile <= 0x035C)) or
	    ((tile >= 0x03B7) and (tile <= 0x03BE)) or
	    ((tile >= 0x03C7) and (tile <= 0x03CA)) or
	    ((tile >= 0x064B) and (tile <= 0x0652)) or
	    ((tile >= 0x0657) and (tile <= 0x065A)) or
	    ((tile >= 0x0663) and (tile <= 0x066A)) or
	    ((tile >= 0x066F) and (tile <= 0x0672)) )
		return 1;
	endif
	return 0;

endfunction


// Swamp land tiles that give clay: clay.cfg LandTile.
function IsSwamp( tile )

	if( ((tile >= 0x3D65) and (tile <= 0x3EF0)) )
		return 1;
	endif
	return 0;

endfunction


// Water land tiles you can fish in: fish.cfg LandTile.
function IsFishLand( tile )

	if( ((tile >= 0x004D) and (tile <= 0x004F)) or
	    ((tile >= 0x0051) and (tile <= 0x0052)) or
	    ((tile >= 0x0055) and (tile <= 0x006C)) or
	    ((tile >= 0x006E) and (tile <= 0x006F)) or
	    ((tile >= 0x00A8) and (tile <= 0x00AB)) or
	    ((tile >= 0x0136) and (tile <= 0x0137)) or
	    (tile == 0x3FF0) )
		return 1;
	endif
	return 0;

endfunction


// Water statics you can fish in: fish.cfg ObjType.
function IsFishStatic( tile )

	if( ((tile >= 0x1796) and (tile <= 0x179C)) or
	    ((tile >= 0x1FA4) and (tile <= 0x1FAC)) or
	    ((tile >= 0x1FAE) and (tile <= 0x1FBB)) or
	    ((tile >= 0x1FBD) and (tile <= 0x1FC0)) or
	    ((tile >= 0x1FC2) and (tile <= 0x1FC5)) or
	    ((tile >= 0x1FC7) and (tile <= 0x1FCA)) or
	    ((tile >= 0x2453) and (tile <= 0x245D)) or
	    ((tile >= 0x346E) and (tile <= 0x3485)) or
	    ((tile >= 0x3494) and (tile <= 0x3498)) or
	    ((tile >= 0x349A) and (tile <= 0x34A4)) or
	    ((tile >= 0x34A6) and (tile <= 0x34AB)) or
	    ((tile >= 0x34B8) and (tile <= 0x34BB)) or
	    ((tile >= 0x34BD) and (tile <= 0x34C0)) or
	    ((tile >= 0x34C2) and (tile <= 0x34C5)) or
	    ((tile >= 0x34C7) and (tile <= 0x34CA)) )
		return 1;
	endif
	return 0;

endfunction


// Water land tiles that fill pitchers and clean bandages.
function IsWaterLand( tile )

	if( ((tile >= 0x004C) and (tile <= 0x006F)) or
	    ((tile >= 0x00A8) and (tile <= 0x00AB)) or
	    ((tile >= 0x0136) and (tile <= 0x0137)) or
	    (tile == 0x3FF0) )
		return 1;
	endif
	return 0;

endfunction


// Water statics that fill pitchers and clean bandages, troughs, tubs, barrels and fountains included.
function IsWaterStatic( tile )

	if( ((tile >= 0x090D) and (tile <= 0x0910)) or
	    ((tile >= 0x0B41) and (tile <= 0x0B44)) or
	    (tile == 0x0E7B) or
	    (tile == 0x0FFA) or
	    (tile == 0x154D) or
	    (tile == 0x1559) or
	    ((tile >= 0x1731) and (tile <= 0x175A)) or
	    ((tile >= 0x1796) and (tile <= 0x17B2)) or
	    ((tile >= 0x19C3) and (tile <= 0x19EC)) or
	    ((tile >= 0x1FA3) and (tile <= 0x1FCA)) or
	    ((tile >= 0x2453) and (tile <= 0x245D)) or
	    ((tile >= 0x346E) and (tile <= 0x3485)) or
	    ((tile >= 0x3494) and (tile <= 0x3498)) or
	    ((tile >= 0x349A) and (tile <= 0x34A4)) or
	    ((tile >= 0x34A6) and (tile <= 0x34AB)) or
	    ((tile >= 0x34B8) and (tile <= 0x34BB)) or
	    ((tile >= 0x34BD) and (tile <= 0x34C0)) or
	    ((tile >= 0x34C2) and (tile <= 0x34C5)) or
	    ((tile >= 0x34C7) and (tile <= 0x34CA)) or
	    ((tile >= 0x34D1) and (tile <= 0x34D5)) or
	    (tile == 0x34ED) or
	    (tile == 0x34F3) or
	    (tile == 0x34F9) or
	    ((tile >= 0x34FB) and (tile <= 0x34FC)) or
	    ((tile >= 0x34FE) and (tile <= 0x34FF)) or
	    (tile == 0x3505) or
	    ((tile >= 0x350B) and (tile <= 0x350D)) or
	    (tile == 0x3511) or
	    (tile == 0x3513) or
	    (tile == 0x3517) or
	    (tile == 0x351D) or
	    (tile == 0x3523) or
	    (tile == 0x3526) )
		return 1;
	endif
	return 0;

endfunction
