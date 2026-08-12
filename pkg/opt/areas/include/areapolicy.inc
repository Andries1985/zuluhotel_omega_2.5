use uo;
use os;
use basic;
use cfgfile;

include ":datafile:datafile";
include "include/constants/cfgfiles";

const AREA_POLICY_DATAFILE_PREFIX := ":areas:area_policies_";
const AREA_POLICY_META_ELEMENT := "__meta__";
const AREA_POLICY_MASK_PROP := "Mask";
const AREA_POLICY_SCHEMA_VERSION := 2;
const AREA_POLICY_DEFAULT_REALM := "britannia";

const AREA_POLICY_GUARDED := 1;
const AREA_POLICY_NO_RECALL_TO := 2;
const AREA_POLICY_NO_RECALL_FROM := 4;
const AREA_POLICY_NO_MARKING := 8;
const AREA_POLICY_ANTI_MAGIC := 16;
const AREA_POLICY_FORBIDDEN := 32;
const AREA_POLICY_NO_LOOTING := 64;
const AREA_POLICY_SAFE_AREA := 128;
const AREA_POLICY_NO_PK := 256;
const AREA_POLICY_RP_AREA := 512;

// World-catchall area ids whose masks are OR-merged into every location's
// policy so that setting a flag on e.g. "feluccawholeworld" makes it apply to
// every point in that realm regardless of which specific area actually
// matches first. This preserves the pre-rewrite override behavior.
const AREA_POLICY_GLOBAL_ID_WHOLEWORLD := "feluccawholeworld";
const AREA_POLICY_GLOBAL_ID_WHOLEWORLD_LEGACY := "wholeworld";
const AREA_POLICY_GLOBAL_ID_BRITANNIA := "britannia";
const AREA_POLICY_GLOBAL_ID_TRAMMEL_WHOLEWORLD := "trammelwholeworld";

///////////////////////////////////////////////////////////////////////////////
// Area line parsing
///////////////////////////////////////////////////////////////////////////////

function ParseAreaLine(area_line, area_index := 0)
	var parts := SplitWords(area_line);
	if(!parts || parts.size() < 6)
		return error{"errortext":="Invalid area definition: " + CStr(area_line)};
	endif

	var area_id := ParseAreaIdToken(parts[5]);
	if(!area_id)
		return error{"errortext":="Missing id= in area definition: " + CStr(area_line)};
	endif

	// Optional cat=<category> token right after id=, used only to group areas
	// for display in the .areas admin gump. Absent on any line, it defaults to
	// "other" and has no effect on policy resolution (file order still wins).
	var category := "other";
	var name_start := 6;
	if(parts.size() >= 6)
		var cat_token := ParseAreaCatToken(parts[6]);
		if(cat_token)
			category := cat_token;
			name_start := 7;
		endif
	endif

	var name := "";
	var i;
	for(i := name_start; i <= parts.size(); i := i + 1)
		if(name)
			name := name + " ";
		endif
		name := name + CStr(parts[i]);
	endfor

	if(!name)
		return error{"errortext":="Invalid area name in definition: " + CStr(area_line)};
	endif

	var info := struct;
	info.+min_x := CInt(parts[1]);
	info.+max_x := CInt(parts[2]);
	info.+min_y := CInt(parts[3]);
	info.+max_y := CInt(parts[4]);
	info.+area_id := area_id;
	info.+category := category;
	info.+name := name;
	info.+area_index := CInt(area_index);
	return info;
endfunction


function ParseAreaIdToken(token)
	var raw := CStr(token);
	if(Len(raw) < 4)
		return "";
	endif
	if(Lower(SubStr(raw, 1, 3)) != "id=")
		return "";
	endif

	var raw_id := SubStr(raw, 4, Len(raw));
	return NormalizeAreaId(raw_id);
endfunction


function ParseAreaCatToken(token)
	var raw := CStr(token);
	if(Len(raw) < 5)
		return "";
	endif
	if(Lower(SubStr(raw, 1, 4)) != "cat=")
		return "";
	endif

	var raw_cat := SubStr(raw, 5, Len(raw));
	return NormalizeAreaId(raw_cat);
endfunction


function NormalizeAreaId(area_id)
	// Fast path: if input is already all-lowercase alphanumeric, skip the
	// expensive per-char loop. This is called for every ParseAreaLine, so
	// even a small speedup here compounds massively.
	var raw := CStr(area_id);
	var lowered := Lower(raw);
	var lengthofid := Len(lowered);
	if(lengthofid == 0)
		return "";
	endif

	var clean := 1;
	var i;
	var ch;
	for(i := 1; i <= lengthofid; i := i + 1)
		ch := SubStr(lowered, i, 1);
		if(!((ch >= "a" && ch <= "z") || (ch >= "0" && ch <= "9")))
			clean := 0;
			break;
		endif
	endfor

	if(clean)
		return lowered;
	endif

	var normalized_id := "";
	for(i := 1; i <= lengthofid; i := i + 1)
		ch := SubStr(lowered, i, 1);
		if((ch >= "a" && ch <= "z") || (ch >= "0" && ch <= "9"))
			normalized_id := normalized_id + ch;
		endif
	endfor
	return normalized_id;
endfunction


function BuildAreaKey(area_line)
	var area_info := ParseAreaLine(area_line);
	if(area_info == error)
		return area_info;
	endif
	return area_info.area_id;
endfunction


function SanitizePolicyRealm(realm)
	realm := Lower(CStr(realm));
	if(!realm || realm == "<uninitialized object>")
		return AREA_POLICY_DEFAULT_REALM;
	endif
	return realm;
endfunction


///////////////////////////////////////////////////////////////////////////////
// Realm/config access
///////////////////////////////////////////////////////////////////////////////

function GetRealmAreaLines(realm)
	realm := SanitizePolicyRealm(realm);
	var areas_cfg := ReadConfigfile(CFGFILE_AREAS);
	if(!areas_cfg)
		return error{"errortext":="Cannot read areas config"};
	endif

	var realm_elem := areas_cfg[realm];
	if(!realm_elem)
		return error{"errortext":="Unknown areas realm: " + realm};
	endif

	var area_lines := GetConfigStringArray(realm_elem, "Area");
	if(!area_lines || area_lines.size() < 1)
		return error{"errortext":="No areas found for realm: " + realm};
	endif

	return area_lines;
endfunction


///////////////////////////////////////////////////////////////////////////////
// Parsed-line cache (shared global)
///////////////////////////////////////////////////////////////////////////////
// SplitWords/SubStr parsing every area line on every call was the sysload=100
// hot path. A shared GlobalProperty cache avoids that: any program's
// first-ever cold parse populates it, and every other program on the shard
// reads it back without paying the ~100k-instruction parse cost.
// GlobalProperties persist across saves, so we tag the cache with a "size"
// fingerprint and invalidate if the config's line count changes.

const AREA_POLICY_CACHE_PREFIX := "zh.areapolicy.parsed.";

function GetParsedRealmAreaLines(realm)
	realm := SanitizePolicyRealm(realm);

	// Must validate fingerprint against the current config's line count so a
	// config edit invalidates stale data.
	var raw_lines := GetRealmAreaLines(realm);
	if(raw_lines == error)
		return raw_lines;
	endif
	var raw_count := raw_lines.size();

	var global_key := AREA_POLICY_CACHE_PREFIX + realm;
	var global_cached := GetGlobalProperty(global_key);
	if(global_cached && global_cached != error && TypeOf(global_cached) == "Struct")
		// Struct: { fingerprint, parsed }
		if(CInt(global_cached.fingerprint) == raw_count && global_cached.parsed)
			return global_cached.parsed;
		endif
	endif

	// Cold path: parse all lines and populate the global cache.
	var parsed := array{};
	var i;
	for(i := 1; i <= raw_count; i := i + 1)
		var area_info := ParseAreaLine(raw_lines[i], i);
		if(area_info == error)
			continue;
		endif
		parsed.append(area_info);
	endfor

	var wrapper := struct;
	wrapper.+fingerprint := raw_count;
	wrapper.+parsed := parsed;
	SetGlobalProperty(global_key, wrapper);
	return parsed;
endfunction


function InvalidateParsedAreaLinesCache(realm := "")
	if(realm)
		realm := Lower(CStr(realm));
		EraseGlobalProperty(AREA_POLICY_CACHE_PREFIX + realm);
	endif
	return 1;
endfunction


///////////////////////////////////////////////////////////////////////////////
// Policy resolution (hot path - keep minimal)
///////////////////////////////////////////////////////////////////////////////

function ResolveAreaKeyAtLocation(x, y, realm)
	var parsed := GetParsedRealmAreaLines(realm);
	if(parsed == error)
		return 0;
	endif

	var px := CInt(x);
	var py := CInt(y);

	var i;
	var area_info;
	for(i := 1; i <= parsed.size(); i := i + 1)
		area_info := parsed[i];
		if(px >= area_info.min_x && px <= area_info.max_x && py >= area_info.min_y && py <= area_info.max_y)
			return area_info.area_id;
		endif
	endfor

	return 0;
endfunction


function ResolveAreaMatchAtLocation(x, y, realm)
	realm := SanitizePolicyRealm(realm);

	var parsed := GetParsedRealmAreaLines(realm);
	if(parsed == error)
		return 0;
	endif

	var px := CInt(x);
	var py := CInt(y);
	var global_mask := GetGlobalBypassMask(realm);

	var i;
	var area_info;
	for(i := 1; i <= parsed.size(); i := i + 1)
		area_info := parsed[i];
		if(px >= area_info.min_x && px <= area_info.max_x && py >= area_info.min_y && py <= area_info.max_y)
			var match := struct;
			match.+realm := realm;
			match.+area_index := area_info.area_index;
			match.+area_id := area_info.area_id;
			match.+name := area_info.name;
			match.+mask := CInt(CInt(GetPolicyMask(realm, area_info.area_id)) | global_mask);
			return match;
		endif
	endfor

	// Location fell outside every explicit area but the world-catchall flags
	// still apply.
	if(global_mask)
		var fallback := struct;
		fallback.+realm := realm;
		fallback.+area_index := 0;
		fallback.+area_id := "";
		fallback.+name := "";
		fallback.+mask := CInt(global_mask);
		return fallback;
	endif

	return 0;
endfunction


function ResolvePolicyAtLocation(x, y, realm)
	realm := SanitizePolicyRealm(realm);

	var global_mask := GetGlobalBypassMask(realm);
	var area_key := ResolveAreaKeyAtLocation(x, y, realm);
	if(!area_key)
		return CInt(global_mask);
	endif

	return CInt(CInt(GetPolicyMask(realm, area_key)) | global_mask);
endfunction


///////////////////////////////////////////////////////////////////////////////
// Global (world-catchall) bypass mask
///////////////////////////////////////////////////////////////////////////////
// Reads the masks assigned to the well-known catch-all area ids and OR-merges
// them so any flag set on them applies world-wide. Masks are stored in the
// per-realm policy datafile which POL keeps in-memory once opened, so these
// 4 extra reads per resolve are cheap.

function GetGlobalBypassMask(realm)
	realm := SanitizePolicyRealm(realm);
	var mask := 0;
	mask := mask | CInt(GetPolicyMask(realm, AREA_POLICY_GLOBAL_ID_WHOLEWORLD));
	mask := mask | CInt(GetPolicyMask(realm, AREA_POLICY_GLOBAL_ID_WHOLEWORLD_LEGACY));
	mask := mask | CInt(GetPolicyMask(realm, AREA_POLICY_GLOBAL_ID_BRITANNIA));
	mask := mask | CInt(GetPolicyMask(realm, AREA_POLICY_GLOBAL_ID_TRAMMEL_WHOLEWORLD));
	return CInt(mask);
endfunction


function HasPolicy(mask, policy_bit)
	mask := CInt(mask);
	policy_bit := CInt(policy_bit);
	if(policy_bit <= 0)
		return 0;
	endif

	return CInt(CInt(mask / policy_bit) % 2);
endfunction


///////////////////////////////////////////////////////////////////////////////
// Datafile access
///////////////////////////////////////////////////////////////////////////////

function GetRealmPolicyDescriptor(realm)
	return AREA_POLICY_DATAFILE_PREFIX + SanitizePolicyRealm(realm);
endfunction


function OpenAreaPolicyFile(realm, create := DF_CREATE)
	var descriptor := GetRealmPolicyDescriptor(realm);
	return DFOpenDataFile(descriptor, create);
endfunction


///////////////////////////////////////////////////////////////////////////////
// Mask-value cache (shared global)
///////////////////////////////////////////////////////////////////////////////
// Mirrors the parsed-line cache above, but for the mask VALUE itself. Every
// GetPolicyMask() call was hitting the datafile subsystem (OpenDataFile +
// FindElement + GetProp) on every single area-policy check - and since
// GetGlobalBypassMask() calls this 4 more times per resolve for the world
// catch-all ids, that's up to 5 datafile round trips per check, called every
// AI tick for every mobile shard-wide. Masks only change via SetPolicyMask
// (staff .areas edits) or PruneStaleRealmPolicies, so a cache invalidated
// only on those writes is safe. Cached per-realm as one dictionary (area_id
// -> mask) so this stays one GlobalProperty per realm, not one property per
// area.
//
// Uses .Exists() rather than truthiness to check for a cached entry, since a
// mask of 0 (the common case - most areas have no flags) is a valid cached
// value and must not be treated as "not cached".

const AREA_POLICY_MASK_CACHE_PREFIX := "zh.areapolicy.maskcache.";

function GetCachedRealmMaskDict(realm)
	var global_cached := GetGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm);
	if(global_cached && TypeOf(global_cached) == "Dictionary")
		return global_cached;
	endif

	return dictionary;
endfunction


function StoreCachedPolicyMask(realm, realm_cache, area_id, mask)
	realm_cache[area_id] := CInt(mask);
	SetGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm, realm_cache);
	return CInt(mask);
endfunction


function InvalidatePolicyMaskCache(realm, area_id := "")
	realm := SanitizePolicyRealm(realm);
	if(area_id)
		var realm_cache := GetGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm);
		if(realm_cache && TypeOf(realm_cache) == "Dictionary")
			realm_cache.Erase(CStr(area_id));
			SetGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm, realm_cache);
		endif
	else
		EraseGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm);
	endif
	return 1;
endfunction


function GetPolicyMask(realm, area_id)
	realm := SanitizePolicyRealm(realm);
	area_id := CStr(area_id);

	var realm_cache := GetCachedRealmMaskDict(realm);
	if(realm_cache.Exists(area_id))
		return realm_cache[area_id];
	endif

	// Cold path: not cached yet, fall through to the real datafile lookup.
	var mask := 0;
	var file_ref := OpenAreaPolicyFile(realm, DF_CREATE);
	if(file_ref != error)
		var elem := DFFindElement(file_ref, area_id, DF_NO_CREATE);
		if(elem && elem != error)
			var prop_val := elem.GetProp(AREA_POLICY_MASK_PROP);
			if(prop_val != error)
				mask := CInt(prop_val);
			endif
		endif
		UnloadDataFile(GetRealmPolicyDescriptor(realm));
	endif

	return StoreCachedPolicyMask(realm, realm_cache, area_id, mask);
endfunction


function SetPolicyMask(realm, area_id, mask, updated_by := "")
	Print("[areas.persist] set begin realm=" + CStr(realm) + " area_id=" + CStr(area_id) + " mask=" + CStr(mask) + " updated_by=" + CStr(updated_by));
	var file_ref := OpenAreaPolicyFile(realm, DF_CREATE);
	if(file_ref == error)
		Print("[areas.persist] set open failed realm=" + CStr(realm) + " area_id=" + CStr(area_id));
		return file_ref;
	endif

	var elem := DFFindElement(file_ref, CStr(area_id), DF_CREATE);
	if(elem == error)
		Print("[areas.persist] set element failed realm=" + CStr(realm) + " area_id=" + CStr(area_id));
		UnloadDataFile(GetRealmPolicyDescriptor(realm));
		return elem;
	endif

	elem.SetProp(AREA_POLICY_MASK_PROP, CInt(mask));
	elem.SetProp("UpdatedAt", ReadGameClock());
	elem.SetProp("UpdatedBy", CStr(updated_by));
	Print("[areas.persist] set wrote realm=" + CStr(realm) + " area_id=" + CStr(area_id) + " mask=" + CStr(mask));
	InvalidatePolicyMaskCache(realm, area_id);

	var meta_elem := DFFindElement(file_ref, AREA_POLICY_META_ELEMENT, DF_CREATE);
	if(meta_elem != error)
		meta_elem.SetProp("SchemaVersion", AREA_POLICY_SCHEMA_VERSION);
		meta_elem.SetProp("UpdatedAt", ReadGameClock());
		meta_elem.SetProp("UpdatedBy", CStr(updated_by));
		Print("[areas.persist] set meta updated realm=" + CStr(realm));
	else
		Print("[areas.persist] set meta missing realm=" + CStr(realm));
	endif

	Print("[areas.persist] set end realm=" + CStr(realm) + " area_id=" + CStr(area_id));
	UnloadDataFile(GetRealmPolicyDescriptor(realm));
	return CInt(mask);
endfunction


function PruneStaleRealmPolicies(realm)
	Print("[areas.persist] prune begin realm=" + CStr(realm));
	var file_ref := OpenAreaPolicyFile(realm, DF_CREATE);
	if(file_ref == error)
		Print("[areas.persist] prune open failed realm=" + CStr(realm));
		return 0;
	endif

	var area_lines := GetRealmAreaLines(realm);
	if(area_lines == error)
		Print("[areas.persist] prune load areas failed realm=" + CStr(realm));
		UnloadDataFile(GetRealmPolicyDescriptor(realm));
		return 0;
	endif

	var valid_ids := dictionary;
	var i;
	for(i := 1; i <= area_lines.size(); i := i + 1)
		var area_info := ParseAreaLine(area_lines[i], i);
		if(area_info != error)
			valid_ids.Insert(CStr(area_info.area_id), 1);
		endif
	endfor

	var keys := file_ref.Keys();
	if(keys == error)
		Print("[areas.persist] prune keys failed realm=" + CStr(realm));
		UnloadDataFile(GetRealmPolicyDescriptor(realm));
		return 0;
	endif
	Print("[areas.persist] prune keys realm=" + CStr(realm) + " count=" + CStr(keys.size()));

	var removed := 0;
	var key;
	foreach key in keys
		var key_str := CStr(key);
		if(key_str == AREA_POLICY_META_ELEMENT)
			continue;
		endif
		if(!(key_str in valid_ids.Keys()))
			Print("[areas.persist] prune delete realm=" + CStr(realm) + " area_id=" + key_str);
			file_ref.DeleteElement(key_str);
			removed := removed + 1;
		endif
	endforeach

	// Bulk removal - simplest correct move is to drop the whole realm's cached
	// mask dict rather than track exactly which keys were pruned.
	InvalidatePolicyMaskCache(realm);

	Print("[areas.persist] prune end realm=" + CStr(realm) + " removed=" + CStr(removed));
	UnloadDataFile(GetRealmPolicyDescriptor(realm));
	return removed;
endfunction
