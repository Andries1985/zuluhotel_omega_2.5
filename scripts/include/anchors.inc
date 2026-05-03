use npc;
use uo;
use cfgfile;

include "include/attributes";
include "include/constants/locations";
include "include/random";

const REGION_RESOURCE := "regions";

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
        UnloadConfigFile("regions");

        // Try to get spawnpoint serial from property
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

        if (!home_region_name)
                SendToJailAndRespawn();
                return 0;
        endif

        // Check if the region element for home_region_name has City=1
        var regions_cfg := ReadConfigFile("::regions/regions");               // Reads regions.cfg
        var region_elem := FindConfigElem(regions_cfg, home_region_name);
        var is_city := 0;
        if (region_elem)
                is_city := GetConfigInt(region_elem, "City");
        endif
        if (is_city != 1)
                SendToJailAndRespawn();
                return 0;
        endif

        return 1;
endfunction

function IsInsideHomeTown()
        var me := Self();

        var home_realm := GetObjProperty( me, "TownHomeRealm" );
        var home_region_name := GetObjProperty( me, "TownHomeRegion" );

        if (!home_realm or !home_region_name)
                return 0;
        endif

        if (me.realm != home_realm)
                return 0;
        endif

        if (GetRegionName(me) != home_region_name)
                return 0;
        endif

        // Check if the region element for home_region_name has City=1
        var regions_cfg := ReadConfigFile("::regions/regions");               // Reads regions.cfg
        var region_elem := FindConfigElem(regions_cfg, home_region_name);
        var is_city := 0;
        if (region_elem)
                is_city := GetConfigInt(region_elem, "City");
        endif
        return is_city;
endfunction

function EnforceTownBounds(from_combat)
        var me := Self();

        if (IsInsideHomeTown())
                return 1;
        else
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

        if (!EnforceTownBounds(0))
                return;
        endif

        TryOpenNearbyNpcDoors();
        Wander();
        EnforceTownBounds(0);
endfunction

function TryOpenNearbyNpcDoors()
        var me := Self();
        start_script("::/misc/npcdoor", me);
endfunction

function SendToJailAndRespawn()
        var me := Self();

        MoveObjectToLocation( me, DEFAULT_LOCATION_JAIL_X, DEFAULT_LOCATION_JAIL_Y, DEFAULT_LOCATION_JAIL_Z,
                              "britannia", MOVEOBJECT_FORCELOCATION );
        ApplyRawDamage( me, GetMaxHP(me) + 3 );
endfunction



