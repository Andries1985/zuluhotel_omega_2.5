/* $Id: armorZones.inc 1051 2007-04-25 05:17:47Z tekproxy $
 *
 */

use uo;
use os;
use util;
use cfgfile;

var zone_cfg := ReadConfigFile("::armrzone");
var itemdesc_cfg := ReadConfigFile(":*:itemdesc");
/*
 * CS_GetRandomArmorZone()
 *
 * Purpose
 * Reads in pol/config/armrzone.cfg and based on the 'chance' settings,
 * selects a zone that was hit.
 *
 * Parameters
 * None
 *
 * Return Value
 * String
 *
 */
function CS_GetRandomArmorZone()
	// This function assumes that the 'Chance' totals in
	// armrzone.cfg all add up to 100. You will most likely
	// get the error return value if they do not.

	//last_num stores the max roll for that zone
	var last_num := 0;
	var max_num := 0;

	//Roll determines the layer we hit.
	//In the future, we may want to have anatomy affect this?
	var roll := RandomInt(100)+1;
	
	foreach zone_name in GetConfigIntKeys(zone_cfg)
		var cfg_elem := zone_cfg[zone_name];
		last_num := max_num;
		max_num := last_num + CInt(cfg_elem.Chance);
		if ( (roll > last_num) && (roll <= max_num)  )
			return cfg_elem.Name;
		endif
		SleepMS(2);
	endforeach
	
	return error{"errortext":="Could not select a hit zone!"};
endfunction

/* CS_GetLayersInArmorZone(zone_name)
 *
 * Purpose
 * Retrieves the layer numbers an armor zone.
 *
 * Parameters
 * zone_name:	Name of the armor zone.
 *
 * Return value
 * Array
 *
 */
function CS_GetLayersInArmorZone(zone_name)
	
	var cfg_elem := zone_cfg[zone_name];
	
	return GetConfigStringArray(cfg_elem, "Layer");
endfunction
	
/*
 * CS_GetEquipmentInArmorZone(mobile, zone_name)
 *
 * Purpose
 * Returns a list of all equipment that a mobile is wearing
 * that covers an armor zone.
 *
 * An armor piece can cover layers other than its .layer property
 * which is why this function is here. A chainmail tunic for example,
 * often covers arms and body.
 *
 * Parameters
 * mobile:	Mobile to evaluate the equipment on.
 * zone_name:	Armor zone to find equipment for.
 *
 * Return Value
 * String
 *
 */
function CS_GetEquipmentInArmorZone(mobile, zone_name)
	var itemdesc_cfg := ReadConfigFile(":*:itemdesc");
	var equip_list := array{};
	
	zone_name := Lower(zone_name);
	
	foreach item in ( ListEquippedItems(mobile) )
		var item_info := itemdesc_cfg[item.objtype];
		
		foreach zone in ( GetConfigStringArray(item_info, "Coverage") )
			if ( Lower(zone) == zone_name )
				equip_list.Append(item);
				break;
			endif
			SleepMS(2);
		endforeach
		
		SleepMS(2);
	endforeach
	
	return equip_list;
endfunction


// Calculate effective item armor based on coverage
// Calculates the effective AR contribution of an armor piece, matching POL core's refresh_ar().
// Regular armor: ar * (hp/maxhp) * sum(zone_chances)
// Shields (layer 1 or 2): not zone-based in core; use shield_ar * parry * 0.5 * 0.01
// Pass who for shield parry calculation; if omitted, shields return 0.
function CS_GetEffectiveArmor(item, who := 0, parry_override := 0)
	var base_ar := CInt(item.ar * CDbl(item.hp) / CDbl(item.maxhp));
	var coverage := GetConfigStringArray(itemdesc_cfg[item.objtype], "Coverage");

	if(!coverage || coverage.size() == 0)
		var parry;
		if(parry_override)
			parry := parry_override;
		elseif(who)
			parry := GetEffectiveSkill(who, SKILLID_PARRY);
		else
			return base_ar;
		endif
		var add := 0.5 * 0.01 * base_ar * parry;
		if(add > 1.0)
			return CInt(add);
		else
			return 1;
		endif
	endif

	var effective_ar := 0.0;
	foreach area in coverage
		foreach zone in GetConfigIntKeys(zone_cfg)
			if (zone_cfg[zone].Name == area)
				effective_ar += base_ar * zone_cfg[zone].Chance/100.0;
			endif
		endforeach
	endforeach

	return CInt(effective_ar);
endfunction

			


