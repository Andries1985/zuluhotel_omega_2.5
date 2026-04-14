// canstack.inc — Shared stacking identity check
//
// Mirrors POL core's Item::can_add_to_self() with additional
// script property checks (stricter than core).
// Used by: packethook (stack merging), omegacache (item identity)

use uo;

// Returns 1 if two items are identical for stacking purposes, 0 otherwise.
// Checks all properties that define item identity:
//   objtype, stackable, graphic, color, flags, quality,
//   weight_multiplier_mod, scripts, and all CProps.
//
// NOTE: Stricter than POL core — also checks usescript, equipscript,
// snoopscript which core's can_add_to_self() does not.
function CanStack(item1, item2)
	if(item1.objtype != item2.objtype)
		return 0;
	endif
	if(!item1.stackable)
		return 0;
	endif
	if(item1.graphic != item2.graphic)
		return 0;
	endif
	if(item1.color != item2.color)
		return 0;
	endif
	if(item1.newbie != item2.newbie)
		return 0;
	endif
	if(item1.insured != item2.insured)
		return 0;
	endif
	if(item1.cursed != item2.cursed)
		return 0;
	endif
	if(item1.quality != item2.quality)
		return 0;
	endif
	if(item1.weight_multiplier_mod != item2.weight_multiplier_mod)
		return 0;
	endif

	// In-use check (matches POL core's !inuse())
	if(item1.inuse or item2.inuse)
		return 0;
	endif

	// Script checks (stricter than POL core)
	if(item1.usescript != item2.usescript)
		return 0;
	endif
	if(item1.equipscript != item2.equipscript)
		return 0;
	endif
	if(item1.snoopscript != item2.snoopscript)
		return 0;
	endif

	// Compare CProps (skip globally ignored CProps from stacking.cfg)
	// Note: POL core reads config/stacking.cfg IgnoreCprops for this.
	// We read it here to match core behaviour from eScript.
	var ignore_list := dictionary;
	var stacking_cfg := ReadConfigFile("stacking");
	if(stacking_cfg)
		var stacking_elem := FindConfigElem(stacking_cfg, "Stacking");
		if(stacking_elem)
			foreach propname in (ListConfigElemProps(stacking_elem))
				if(propname == "IgnoreCprops")
					var ignored := SplitWords(GetConfigString(stacking_elem, "IgnoreCprops"));
					foreach ig in ignored
						ignore_list[ig] := 1;
					endforeach
				endif
			endforeach
		endif
	endif

	var props1 := GetObjPropertyNames(item1);
	var props2 := GetObjPropertyNames(item2);

	// Filter out ignored props before comparing
	var filtered1 := array{};
	foreach propname in props1
		if(!ignore_list[propname])
			filtered1.append(propname);
		endif
	endforeach
	var filtered2 := array{};
	foreach propname in props2
		if(!ignore_list[propname])
			filtered2.append(propname);
		endif
	endforeach

	if(filtered1.size() != filtered2.size())
		return 0;
	endif
	foreach propname in filtered1
		if(GetObjProperty(item1, propname) != GetObjProperty(item2, propname))
			return 0;
		endif
	endforeach

	return 1;
endfunction
