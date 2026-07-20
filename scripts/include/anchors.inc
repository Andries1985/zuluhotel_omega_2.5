use npc;
use uo;
use cfgfile;

include "include/attributes";
include "include/constants/locations";
include "include/random";

const REGION_RESOURCE := "regions";

// Toggle to 0 to silence debug output once the perf issue is diagnosed.
const ANCHORS_DEBUG := 0;

// Per-program call counters used to rate-limit hot-path debug output.
var _dbg_regionIsCity_calls := 0;
var _dbg_initTownBounds_calls := 0;
var _dbg_isInsideHomeTown_calls := 0;
var _dbg_enforceTownBounds_calls := 0;
var _dbg_wanderWithinTown_calls := 0;
var _dbg_sendToJail_calls := 0;

function AnchorsDbg(msg)
	// Reference msg so const-off builds don't warn about unused param.
	msg := msg;
	if(ANCHORS_DEBUG)
		var me_name := "";
		var me_ref := Self();
		if(me_ref)
			me_name := CStr(me_ref.name) + "/" + CStr(me_ref.serial);
		endif
		Print("[anchors:" + me_name + "] " + CStr(msg));
	endif
	return 1;
endfunction

function RegionIsCity(region_elem)
	if(ANCHORS_DEBUG)
		_dbg_regionIsCity_calls := _dbg_regionIsCity_calls + 1;
		if((_dbg_regionIsCity_calls % 50) == 1)
			AnchorsDbg("RegionIsCity call#" + CStr(_dbg_regionIsCity_calls) + " region_elem=" + CStr(region_elem));
		endif
	endif

        if (!region_elem)
                if(ANCHORS_DEBUG)
                        AnchorsDbg("RegionIsCity NULL region_elem");
                endif
                return 0;
        endif

        var city_int := GetConfigInt(region_elem, "City");
        if (city_int == 1)
                if(ANCHORS_DEBUG and (_dbg_regionIsCity_calls % 50) == 1)
                        AnchorsDbg("RegionIsCity City=1 matched");
                endif
                return 1;
        endif

        // Support new region format that uses Type City instead of City 1.
        var type_str := GetConfigString(region_elem, "Type");
        if (type_str == "City")
                if(ANCHORS_DEBUG and (_dbg_regionIsCity_calls % 50) == 1)
                        AnchorsDbg("RegionIsCity Type=City matched");
                endif
                return 1;
        endif

        if(ANCHORS_DEBUG)
                AnchorsDbg("RegionIsCity NOT a city: City=" + CStr(city_int) + " Type=" + CStr(type_str));
        endif

        return 0;
endfunction

function drop_anchor(me)

        // assumes that var me = npc::self

        var cfg := readconfigfile("npcdesc");

        var dstart := cfg[me.npctemplate].dstart;
        var psub := cfg[me.npctemplate].psub;

        if (!psub)
                psub := 10;
        endif

        if (dstart)
                SetAnchor(me.x, me.y, dstart, psub);
        endif
endfunction

function InitTownBounds()
        var me := Self();
        if(ANCHORS_DEBUG)
                _dbg_initTownBounds_calls := _dbg_initTownBounds_calls + 1;
                AnchorsDbg("InitTownBounds ENTER call#" + CStr(_dbg_initTownBounds_calls) + " template=" + CStr(me.npctemplate));
        endif
        UnloadConfigFile("regions");
        var spawnpoint_serial := GetObjProperty(me, PROPID_MOBILE_SPAWNPOINT_SERIAL);
        var home_x;
        var home_y;
        var home_z;
        var home_realm;
        var spawnpoint := SystemFindObjectBySerial(CInt(spawnpoint_serial));
         var home_region_name;

        if (spawnpoint_serial)
                var spawnpoint := SystemFindObjectBySerial(CInt(spawnpoint_serial));
                if (spawnpoint)
                        home_x := spawnpoint.x;
                        home_y := spawnpoint.y;
                        home_z := spawnpoint.z;
                        home_realm := spawnpoint.realm;
                        home_region_name := GetRegionName(spawnpoint);
                else
                        home_x := me.x;
                        home_y := me.y;
                        home_z := me.z;
                        home_realm := me.realm;
                        home_region_name := GetRegionName(me);
                endif
        else
                home_x := me.x;
                home_y := me.y;
                home_z := me.z;
                home_realm := me.realm;
                home_region_name := GetRegionName(me);
        endif

        SetObjProperty(me, "TownHomeX", home_x);
        SetObjProperty(me, "TownHomeY", home_y);
        SetObjProperty(me, "TownHomeZ", home_z);
        SetObjProperty(me, "TownHomeRealm", home_realm);
        SetObjProperty(me, "TownHomeRegion", home_region_name);

        if(ANCHORS_DEBUG)
                AnchorsDbg("InitTownBounds home_region=" + CStr(home_region_name) + " home_realm=" + CStr(home_realm) + " home_xy=" + CStr(home_x) + "," + CStr(home_y));
        endif

        if (!home_region_name)
                if(ANCHORS_DEBUG)
                        AnchorsDbg("InitTownBounds NO region_name -> jail");
                endif
                SendToJailAndRespawn();
                return 0;
        endif

        // Check if the region element for home_region_name is a city region.
        var regions_cfg := ReadConfigFile("::regions/regions");               // Reads regions.cfg
        var region_elem := FindConfigElem(regions_cfg, home_region_name);
        var is_city := RegionIsCity(region_elem);
        if(ANCHORS_DEBUG)
                AnchorsDbg("InitTownBounds region=" + CStr(home_region_name) + " is_city=" + CStr(is_city));
        endif
        if (is_city != 1)
                if(ANCHORS_DEBUG)
                        AnchorsDbg("InitTownBounds region NOT city -> jail");
                endif
                SendToJailAndRespawn();
                return 0;
        endif

        return 1;
endfunction

function IsInsideHomeTown()
        var me := Self();
        if(ANCHORS_DEBUG)
                _dbg_isInsideHomeTown_calls := _dbg_isInsideHomeTown_calls + 1;
                if((_dbg_isInsideHomeTown_calls % 50) == 1)
                        AnchorsDbg("IsInsideHomeTown call#" + CStr(_dbg_isInsideHomeTown_calls) + " template=" + CStr(me.npctemplate));
                endif
        endif

        var home_realm := GetObjProperty( me, "TownHomeRealm" );
        var home_region_name := GetObjProperty( me, "TownHomeRegion" );

        if (!home_realm or !home_region_name)
                if(ANCHORS_DEBUG)
                        AnchorsDbg("IsInsideHomeTown MISSING home props home_realm=" + CStr(home_realm) + " home_region=" + CStr(home_region_name));
                endif
                return 0;
        endif

        if (me.realm != home_realm)
                if(ANCHORS_DEBUG)
                        AnchorsDbg("IsInsideHomeTown REALM MISMATCH me=" + CStr(me.realm) + " home=" + CStr(home_realm));
                endif
                return 0;
        endif

        var cur_region := GetRegionName(me);
        if (cur_region != home_region_name)
                if(ANCHORS_DEBUG)
                        AnchorsDbg("IsInsideHomeTown REGION MISMATCH me=" + CStr(cur_region) + " home=" + CStr(home_region_name));
                endif
                return 0;
        endif

        // Check if the region element for home_region_name is a city region.
        var regions_cfg := ReadConfigFile("::regions/regions");               // Reads regions.cfg
        var region_elem := FindConfigElem(regions_cfg, home_region_name);
        var result := RegionIsCity(region_elem);
        if(ANCHORS_DEBUG and !result)
                AnchorsDbg("IsInsideHomeTown FAILED city check region=" + CStr(home_region_name));
        endif
        return result;
endfunction

function EnforceTownBounds(from_combat)
        var me := Self();
        if(ANCHORS_DEBUG)
                _dbg_enforceTownBounds_calls := _dbg_enforceTownBounds_calls + 1;
                if((_dbg_enforceTownBounds_calls % 25) == 1)
                        AnchorsDbg("EnforceTownBounds call#" + CStr(_dbg_enforceTownBounds_calls) + " from_combat=" + CStr(from_combat));
                endif
        endif

        if (IsInsideHomeTown())
                return 1;
        else
                if(ANCHORS_DEBUG)
                        AnchorsDbg("EnforceTownBounds NOT inside home town -> jail");
                endif
                SendToJailAndRespawn();
                return 0;
        endif

        if (from_combat)
                SendToJailAndRespawn();
                return 0;
        endif

        var home_x := CInt(GetObjProperty( me, "TownHomeX" ));
        var home_y := CInt(GetObjProperty( me, "TownHomeY" ));
        var home_z := CInt(GetObjProperty( me, "TownHomeZ" ));
        var home_realm := GetObjProperty( me, "TownHomeRealm" );
        var try_x;
        var try_y;
        var try_z;
        var moved := 0;
        var i;

        if (!home_realm)
                SendToJailAndRespawn();
                return 0;
        endif

        // Resolve z from standing height at home x/y before trying to move.
        try_z := CInt(GetStandingHeight( home_x, home_y, home_z, home_realm ));
        moved := MoveObjectToLocation( me, home_x, home_y, try_z, home_realm, MOVEOBJECT_NORMAL );

        for (i := 1; i <= 10; i := i + 1)
                if (moved)
                        break;
                endif

                try_x := home_x + (Random(41) - 20);
                try_y := home_y + (Random(41) - 20);
                // Resolve the proper standing z for each random x/y candidate.
                try_z := CInt(GetStandingHeight( try_x, try_y, home_z, home_realm ));
                moved := MoveObjectToLocation( me, try_x, try_y, try_z, home_realm, MOVEOBJECT_NORMAL );
        endfor

        if (!moved)
                // Last resort so this AI cannot remain outside town forever.
                SendToJailAndRespawn();
                return 0;
        endif

        return 1;
endfunction

function WanderWithinTown()
        var me := Self();
        if(ANCHORS_DEBUG)
                _dbg_wanderWithinTown_calls := _dbg_wanderWithinTown_calls + 1;
                if((_dbg_wanderWithinTown_calls % 10) == 1)
                        AnchorsDbg("WanderWithinTown call#" + CStr(_dbg_wanderWithinTown_calls));
                endif
        endif

        if (!EnforceTownBounds(0))
                return;
        endif

        TryOpenNearbyNpcDoors();
        StepCivilianThroughDoorIfNeeded();
        Wander();
        EnforceTownBounds(0);
endfunction

function IsDoorPushCivilianTemplate(me)
        if (!me or !me.npctemplate)
                return 0;
        endif

        if (me.npctemplate == "townperson" or me.npctemplate == "person" or me.npctemplate == "noble")
                return 1;
        endif

        return 0;
endfunction

function FacingToMoveDir(facing)
        case (CInt(facing))
                0:
                        return "N";
                1:
                        return "NE";
                2:
                        return "E";
                3:
                        return "SE";
                4:
                        return "S";
                5:
                        return "SW";
                6:
                        return "W";
                7:
                        return "NW";
                default:
                        return "";
        endcase
endfunction

function StepCivilianThroughDoorIfNeeded()
        var me := Self();

        if (!IsDoorPushCivilianTemplate(me))
                return;
        endif

        var move_dir := FacingToMoveDir(me.facing);
        if (!move_dir)
                return;
        endif

        var stepthrudoor;
        for (stepthrudoor := 1; stepthrudoor <= 2; stepthrudoor := stepthrudoor + 1)
                if (!CanMove(move_dir))
                        break;
                endif

                if (!Move(move_dir))
                        break;
                endif
        endfor
endfunction

function TryOpenNearbyNpcDoors()
        var me := Self();
        start_script("::/misc/npcdoor", me);
endfunction

function SendToJailAndRespawn()
        var me := Self();
        if(ANCHORS_DEBUG)
                _dbg_sendToJail_calls := _dbg_sendToJail_calls + 1;
                AnchorsDbg("SendToJailAndRespawn call#" + CStr(_dbg_sendToJail_calls) + " template=" + CStr(me.npctemplate) + " xy=" + CStr(me.x) + "," + CStr(me.y) + " realm=" + CStr(me.realm));
        endif

        MoveObjectToLocation( me, DEFAULT_LOCATION_JAIL_X, DEFAULT_LOCATION_JAIL_Y, DEFAULT_LOCATION_JAIL_Z,
                              "britannia", MOVEOBJECT_FORCELOCATION );
        ApplyRawDamage( me, GetMaxHP(me) + 3 );
endfunction



