use uo;
use cfgfile;

include ":gumps:yesno";
include "include/client";
include "include/classes";
include "include/math";

const GOLD_COLOR	:= 0x0033;

function ApplyMaterialProperties(byref product_desc, material, is_exceptional, character, skill_id)

	// Support both physical items and ResourceRequest structs
	var mat_objtype;
	var mat_color;
	var mat_quality;
	if(material.preferredSourceOrder)
		// ResourceRequest struct — lowercase fields
		mat_objtype := material.objtype;
		mat_color := material.color;
		mat_quality := material.quality;
	else
		// Physical item — uppercase POL properties
		mat_objtype := material.objtype;
		mat_color := material.Color;
		mat_quality := CDbl(material.quality);
	endif

	var material_cfg;
	case(skill_id)
		SKILLID_BLACKSMITHY:
			material_cfg := ReadConfigFile( ":blacksmithy:blacksmithy" );
		SKILLID_TAILORING:
			material_cfg := ReadConfigFile( ":tailoring:tailoring" );
		SKILLID_CARPENTRY:
			material_cfg := ReadConfigFile( ":carpentry:carpentry" );
		SKILLID_BOWCRAFT:
			material_cfg := ReadConfigFile( ":carpentry:carpentry" );
		default:
			return 0;
	endcase

	var material_desc := material_cfg[mat_objtype];

	product_desc.Desc := material_desc.Name +" "+product_desc.Desc;
	// Use stored quality if present, else fall back to config
	if(mat_quality)
		product_desc.Quality := mat_quality;
	else
		product_desc.Quality := material_desc.Quality;
	endif

	if( mat_objtype == UOBJ_GOLD_INGOT )
		product_desc.Color := GOLD_COLOR;
	else
		product_desc.Color := mat_color;
	endif

	//Specific Exceptional properties
	if(is_exceptional)
		if(GetObjProperty(character, "ToggleBuildMark"))
			if( YesNo( character , "Add Builder Mark?" ))
				product_desc.Cprops.insert( "CraftedBy", character.name );
			endif
		else
			product_desc.Cprops.insert( "CraftedBy", character.name );
		endif
		product_desc.Cprops.insert( "IsExceptional", 1 );
		product_desc.Desc := "Exceptional " + product_desc.Desc;
	
		var multiplier := 5 + Cint(GetEffectiveSkill(character, SKILLID_ARMSLORE) / 10 );
		
		if(GetObjProperty(character , CLASSEID_CRAFTER))
			multiplier := Cint(multiplier * ClasseBonus(character, CLASSEID_CRAFTER));
		endif

		multiplier := (100 + multiplier);
        product_desc.Quality := Trunc((product_desc.Quality * multiplier) / 100.0, 2);

	endif

	
	product_desc.Cprops.insert("CraftedWith", mat_objtype);
	product_desc.Cprops.insert("BaseName", product_desc.Desc);

	if(product_desc.hitscript)// only weapons have hitscripts, right??
		if (material_desc.ElemDamage)
			product_desc.Cprops.insert("ElementalDamage",material_desc.ElemDamage);
		endif
		if (material_desc.SlayType)
			product_desc.Cprops.insert("SlayType",material_desc.SlayType);
			product_desc.Hitscript := ":combat:slayerscript";
		endif
	elseif(product_desc.ar)
		foreach cprop in GetConfigStringArray( material_desc , "CProp" );
			var split_prop := SplitWords( cprop );
			var prop_name := split_prop[1];
			var prop_mod  := Cint(split_prop[2]);
			
			var presently := Cint(product_desc.Cprops[prop_name]);
			if( !presently )
				presently := 0;
			endif

			if( presently && (abs(prop_mod) > abs(presently) ))
				prop_mod := 0 - presently;
			endif

			var resulting := presently + prop_mod;
			if( resulting > 0 and prop_name == "DefaultDex" )
				resulting := 0;
			endif
			product_desc.Cprops[prop_name] := resulting;
		endforeach
	endif

endfunction