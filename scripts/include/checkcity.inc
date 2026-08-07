use os;
use uo;
use cfgfile;

///////////////////////////////////////////////////////////////////////////////
// Shared regions.cfg Type= scanning, used by CheckCity() below and reused by
// pkg/std/housing/housedeed.src and pkg/opt/alryc/textcmd/test/housezoneaudit.src
// for their own region-type checks (city/dungeon/shrine/graveyard/suburb/...).
///////////////////////////////////////////////////////////////////////////////

Function NormalizeRegionType(region_type)
    case(Lower(CStr(region_type)))
        "poi":
            return "poi";
        "city":
            return "city";
        "suburb":
            return "suburb";
        "shrine":
            return "shrine";
        "dungeon":
            return "dungeon";
        "graveyard":
            return "graveyard";
        "jail":
            return "jail";
        "none":
            return "none";
    endcase

    return Lower(CStr(region_type));
EndFunction

// Returns struct{name, range:={x1,y1,x2,y2}} for the first regions.cfg region
// of the given Type whose Range overlaps the given box (realm-scoped), or 0
// if none match / regions.cfg can't be read (fail-open, same as callers'
// prior behavior when the config was unavailable).
Function IsBoxOverlappingRegionType(x1, y1, x2, y2, realm, region_type)
    var regions_cfg := ReadConfigFile("::regions/regions");
    if(!regions_cfg)
        return 0;
    endif

    var wanted_region_type := NormalizeRegionType(region_type);
    var keys := GetConfigStringKeys(regions_cfg);
    if(!keys)
        return 0;
    endif

    foreach key in keys
        var region := FindConfigElem(regions_cfg, key);
        if(!region)
            continue;
        endif

        if(NormalizeRegionType(GetConfigString(region, "Type")) != wanted_region_type)
            continue;
        endif

        var region_realm := Lower(CStr(GetConfigString(region, "Realm")));
        if(region_realm and (region_realm != Lower(CStr(realm))))
            continue;
        endif

        var range := SplitWords(CStr(GetConfigString(region, "Range")));
        if(!range or range.size() < 4)
            continue;
        endif

        var rx1 := CInt(range[1]);
        var ry1 := CInt(range[2]);
        var rx2 := CInt(range[3]);
        var ry2 := CInt(range[4]);

        if((x1 <= rx2) and (x2 >= rx1) and (y1 <= ry2) and (y2 >= ry1))
            return struct{ name := key, range := {rx1, ry1, rx2, ry2} };
        endif
    endforeach

    return 0;
EndFunction

// Point-in-region convenience wrapper - a zero-size box degenerates correctly
// into a point-in-rect test.
Function IsPointInRegionType(x, y, realm, region_type)
    return IsBoxOverlappingRegionType(x, y, x, y, realm, region_type);
EndFunction

///////////////////////////////////////////////////////////////////////////////
// CheckCity(item) - is this item/mobile standing in a Type=City region?
///////////////////////////////////////////////////////////////////////////////

Function CheckCity(item)
    if(IsPointInRegionType(item.x, item.y, item.realm, "city"))
        return 1;
    endif

    return 0;
EndFunction
