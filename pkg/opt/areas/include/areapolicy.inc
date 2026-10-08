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
// Script-local caches
///////////////////////////////////////////////////////////////////////////////
// Reading a GlobalProperty unpacks the whole stored value every time, and the
// parsed-area array is ~200 structs. Long-running scripts (NPC AI) resolve
// policy every tick, so each running script keeps its own copy of the last
// realm it resolved and walks that directly - no per-call unpack, no per-call
// copy. areas.cfg cannot change while the server runs, so the parsed copy is
// valid for the life of the script. Masks can change on an .areas Save, so
// the mask copy is tagged with the realm's version counter (one small int
// GlobalProperty) and refreshed only when that counter moves.
var g_areapolicy_parsed_realm := "";
var g_areapolicy_parsed := array{};
var g_areapolicy_masks_realm := "";
var g_areapolicy_masks_version := -1;
var g_areapolicy_masks := dictionary;

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
	// A stale/invalid caller-side object (e.g. a mobile that logged out before
	// a delayed member access ran) surfaces here as an error{} value rather
	// than a thrown exception. CStr() on that would stringify it into the
	// datafile descriptor and crash the core when it tries to open the
	// resulting garbage filename, so reject it before that happens.
	if(realm == error)
		return AREA_POLICY_DEFAULT_REALM;
	endif
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
// hot path. A shared GlobalProperty cache avoids that: the package start
// script parses areas.cfg once on boot and every program reads the result.
// GlobalProperties persist across saves AND server restarts, while areas.cfg
// can only change between restarts, so the boot rebuild
// (RebuildParsedAreaLinesCache) is the authoritative invalidation. Nothing on
// the hot path re-reads areas.cfg.

const AREA_POLICY_CACHE_PREFIX := "zh.areapolicy.parsed.";

// Returns a copy of the realm's parsed area array (shared cache, or a cold
// parse of areas.cfg that also populates the shared cache). Hot-path code
// should use EnsureLocalParsedAreas() and walk g_areapolicy_parsed instead,
// which avoids copying the array on every call.
function GetParsedRealmAreaLines(realm)
	realm := SanitizePolicyRealm(realm);

	var global_key := AREA_POLICY_CACHE_PREFIX + realm;
	var global_cached := GetGlobalProperty(global_key);
	if(global_cached && global_cached != error && TypeOf(global_cached) == "Struct")
		if(global_cached.parsed)
			return global_cached.parsed;
		endif
	endif

	// Cold path (start script, or cache missing): parse all lines and
	// populate the shared cache.
	var raw_lines := GetRealmAreaLines(realm);
	if(raw_lines == error)
		return raw_lines;
	endif

	var parsed := array{};
	var i;
	for(i := 1; i <= raw_lines.size(); i := i + 1)
		var area_info := ParseAreaLine(raw_lines[i], i);
		if(area_info == error)
			continue;
		endif
		parsed.append(area_info);
	endfor

	var wrapper := struct;
	wrapper.+line_count := raw_lines.size();
	wrapper.+parsed := parsed;
	SetGlobalProperty(global_key, wrapper);
	return parsed;
endfunction


// Makes sure g_areapolicy_parsed holds the given realm. Returns 1, or error
// if the realm has no areas. Only copies when the realm changes.
function EnsureLocalParsedAreas(realm)
	if(realm == g_areapolicy_parsed_realm && g_areapolicy_parsed.size())
		return 1;
	endif

	var parsed := GetParsedRealmAreaLines(realm);
	if(parsed == error)
		return parsed;
	endif

	g_areapolicy_parsed := parsed;
	g_areapolicy_parsed_realm := realm;
	return 1;
endfunction


function InvalidateParsedAreaLinesCache(realm := "")
	if(realm)
		realm := Lower(CStr(realm));
		EraseGlobalProperty(AREA_POLICY_CACHE_PREFIX + realm);
		if(realm == g_areapolicy_parsed_realm)
			g_areapolicy_parsed_realm := "";
			g_areapolicy_parsed := array{};
		endif
	endif
	return 1;
endfunction


// Drops and re-parses the realm's area lines from the current areas.cfg.
// Called by the package start script on boot. Returns the number of parsed
// areas, or error if the config could not be read.
function RebuildParsedAreaLinesCache(realm)
	realm := SanitizePolicyRealm(realm);
	InvalidateParsedAreaLinesCache(realm);
	var parsed := GetParsedRealmAreaLines(realm);
	if(parsed == error)
		return parsed;
	endif
	return parsed.size();
endfunction


///////////////////////////////////////////////////////////////////////////////
// Policy resolution (hot path - keep minimal)
///////////////////////////////////////////////////////////////////////////////
// One resolve = one version-counter read, zero config reads, zero large
// GlobalProperty unpacks once a script has warmed its local copies. The
// bounding-box walk lives in ResolveAreaMatchAtLocation only; the other two
// entry points are thin wrappers over it.

function ResolveAreaMatchAtLocation(x, y, realm)
	realm := SanitizePolicyRealm(realm);

	if(EnsureLocalParsedAreas(realm) == error)
		return 0;
	endif
	EnsureLocalRealmMasks(realm);

	var px := CInt(x);
	var py := CInt(y);
	var global_mask := GetGlobalBypassMaskLocal(realm);

	var i;
	var count := g_areapolicy_parsed.size();
	for(i := 1; i <= count; i := i + 1)
		if(px >= g_areapolicy_parsed[i].min_x && px <= g_areapolicy_parsed[i].max_x && py >= g_areapolicy_parsed[i].min_y && py <= g_areapolicy_parsed[i].max_y)
			var match := struct;
			match.+realm := realm;
			match.+area_index := g_areapolicy_parsed[i].area_index;
			match.+area_id := g_areapolicy_parsed[i].area_id;
			match.+name := g_areapolicy_parsed[i].name;
			match.+mask := CInt(CInt(GetPolicyMaskLocal(realm, match.area_id)) | global_mask);
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
	var match := ResolveAreaMatchAtLocation(x, y, realm);
	if(!match)
		return 0;
	endif
	return CInt(match.mask);
endfunction


// Id of the first explicit area containing the point, or 0 if none.
function ResolveAreaKeyAtLocation(x, y, realm)
	var match := ResolveAreaMatchAtLocation(x, y, realm);
	if(!match || !match.area_id)
		return 0;
	endif
	return match.area_id;
endfunction


///////////////////////////////////////////////////////////////////////////////
// Global (world-catchall) bypass mask
///////////////////////////////////////////////////////////////////////////////
// Reads the masks assigned to the well-known catch-all area ids and OR-merges
// them so any flag set on them applies world-wide. These ids are always
// present in the realm mask cache (RebuildPolicyMaskCache seeds them), so the
// 4 extra lookups per resolve are local dictionary hits.

// Assumes EnsureLocalRealmMasks(realm) has run.
function GetGlobalBypassMaskLocal(realm)
	var mask := 0;
	mask := mask | CInt(GetPolicyMaskLocal(realm, AREA_POLICY_GLOBAL_ID_WHOLEWORLD));
	mask := mask | CInt(GetPolicyMaskLocal(realm, AREA_POLICY_GLOBAL_ID_WHOLEWORLD_LEGACY));
	mask := mask | CInt(GetPolicyMaskLocal(realm, AREA_POLICY_GLOBAL_ID_BRITANNIA));
	mask := mask | CInt(GetPolicyMaskLocal(realm, AREA_POLICY_GLOBAL_ID_TRAMMEL_WHOLEWORLD));
	return CInt(mask);
endfunction


function GetGlobalBypassMask(realm)
	realm := SanitizePolicyRealm(realm);
	EnsureLocalRealmMasks(realm);
	return GetGlobalBypassMaskLocal(realm);
endfunction


function HasPolicy(mask, policy_bit)
	mask := CInt(mask);
	policy_bit := CInt(policy_bit);
	if(policy_bit <= 0)
		return 0;
	endif

	if(mask & policy_bit)
		return 1;
	endif
	return 0;
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
// GetPolicyMask() call used to hit the datafile subsystem (OpenDataFile +
// FindElement + GetProp) on every single area-policy check - and since
// GetGlobalBypassMask() calls this 4 more times per resolve for the world
// catch-all ids, that's up to 5 datafile round trips per check, called every
// AI tick for every mobile shard-wide. Cached per-realm as one dictionary
// (area_id -> mask) so this stays one GlobalProperty per realm.
//
// SINGLE-WRITER RULE: readers (GetPolicyMask) never write this property.
// The only writers are RebuildPolicyMaskCache(), called from the package
// start script on boot and from PruneStaleRealmPolicies() at the end of an
// .areas Save, and InvalidatePolicyMaskCache(), which erases a whole realm.
// The old read-miss write-back was removed because it was a non-atomic
// read-modify-write of a shared property: a reader preempted between
// GetGlobalProperty and SetGlobalProperty would write its stale copy back
// over a fresh Save and resurrect every old mask. The rebuild always
// produces a complete dictionary from the datafile in one SetGlobalProperty,
// so it never merges with the existing value.
//
// Every writer also bumps a per-realm version counter. Readers compare that
// one small int against their script-local copy and only re-fetch the whole
// dictionary when it has moved.
//
// Uses .Exists() rather than truthiness to check for a cached entry, since a
// mask of 0 (the common case - most areas have no flags) is a valid cached
// value and must not be treated as "not cached".

const AREA_POLICY_MASK_CACHE_PREFIX := "zh.areapolicy.maskcache.";
const AREA_POLICY_MASK_VERSION_PREFIX := "zh.areapolicy.maskver.";

function GetCachedRealmMaskDict(realm)
	var global_cached := GetGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm);
	if(global_cached && TypeOf(global_cached) == "Dictionary")
		return global_cached;
	endif

	return dictionary;
endfunction


function GetPolicyMaskVersion(realm)
	var version := GetGlobalProperty(AREA_POLICY_MASK_VERSION_PREFIX + realm);
	if(version == error)
		return 0;
	endif
	return CInt(version);
endfunction


function BumpPolicyMaskVersion(realm)
	var version := GetPolicyMaskVersion(realm) + 1;
	SetGlobalProperty(AREA_POLICY_MASK_VERSION_PREFIX + realm, version);
	return version;
endfunction


// Makes sure g_areapolicy_masks holds the realm's current mask dictionary.
// Costs one small GlobalProperty read per call; the dictionary itself is
// only re-fetched when the realm or its version counter changes.
function EnsureLocalRealmMasks(realm)
	var version := GetPolicyMaskVersion(realm);
	if(realm == g_areapolicy_masks_realm && version == g_areapolicy_masks_version)
		return 1;
	endif

	g_areapolicy_masks := GetCachedRealmMaskDict(realm);
	g_areapolicy_masks_realm := realm;
	g_areapolicy_masks_version := version;
	return 1;
endfunction


function InvalidatePolicyMaskCache(realm)
	realm := SanitizePolicyRealm(realm);
	EraseGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm);
	BumpPolicyMaskVersion(realm);
	return 1;
endfunction


// Reads one area's mask straight from an already-open policy datafile.
// Returns error if the element or prop is unavailable so callers can tell
// "no flags" (0) apart from "could not read".
function ReadPolicyMaskFromFile(byref file_ref, area_id)
	var elem := DFFindElement(file_ref, CStr(area_id), DF_NO_CREATE);
	if(!elem || elem == error)
		return error{"errortext":="No policy element for area id: " + CStr(area_id)};
	endif

	var prop_val := elem.GetProp(AREA_POLICY_MASK_PROP);
	if(prop_val == error)
		return prop_val;
	endif

	return CInt(prop_val);
endfunction


// Rebuilds the whole realm mask dictionary from the datafile in one pass and
// stores it with a single SetGlobalProperty. Every parsed area id plus the
// world catch-all ids get an entry (0 when absent) so the hot path never
// misses on a known id. If the config or datafile cannot be read the realm
// cache is erased instead, so readers fall back to the datafile rather than
// serving a dictionary that may be stale. Returns the entry count, or error.
function RebuildPolicyMaskCache(realm)
	realm := SanitizePolicyRealm(realm);

	var parsed := GetParsedRealmAreaLines(realm);
	if(parsed == error)
		InvalidatePolicyMaskCache(realm);
		return parsed;
	endif

	var file_ref := OpenAreaPolicyFile(realm, DF_CREATE);
	if(file_ref == error)
		InvalidatePolicyMaskCache(realm);
		return file_ref;
	endif

	var realm_cache := dictionary;
	var i;
	for(i := 1; i <= parsed.size(); i := i + 1)
		var area_id := CStr(parsed[i].area_id);
		var mask := ReadPolicyMaskFromFile(file_ref, area_id);
		if(mask == error)
			mask := 0;
		endif
		realm_cache[area_id] := CInt(mask);
	endfor

	var global_ids := array{
		AREA_POLICY_GLOBAL_ID_WHOLEWORLD,
		AREA_POLICY_GLOBAL_ID_WHOLEWORLD_LEGACY,
		AREA_POLICY_GLOBAL_ID_BRITANNIA,
		AREA_POLICY_GLOBAL_ID_TRAMMEL_WHOLEWORLD
	};
	foreach global_id in global_ids
		if(!realm_cache.Exists(global_id))
			var global_mask := ReadPolicyMaskFromFile(file_ref, global_id);
			if(global_mask == error)
				global_mask := 0;
			endif
			realm_cache[global_id] := CInt(global_mask);
		endif
	endforeach
	UnloadDataFile(GetRealmPolicyDescriptor(realm));

	SetGlobalProperty(AREA_POLICY_MASK_CACHE_PREFIX + realm, realm_cache);
	BumpPolicyMaskVersion(realm);
	return realm_cache.size();
endfunction


// Uncached datafile read for an id that is not in the mask cache (cache not
// built yet, or unknown id). Never stores the result: readers never write
// the shared cache (see SINGLE-WRITER RULE above), and a failed open/read
// must not be remembered as "mask 0" or a transient hiccup during a world
// save would leave e.g. Britain unguarded until the next staff Save.
function ReadPolicyMaskUncached(realm, area_id)
	var mask := 0;
	var file_ref := OpenAreaPolicyFile(realm, DF_CREATE);
	if(file_ref != error)
		var file_mask := ReadPolicyMaskFromFile(file_ref, area_id);
		if(file_mask != error)
			mask := CInt(file_mask);
		endif
		UnloadDataFile(GetRealmPolicyDescriptor(realm));
	endif
	return CInt(mask);
endfunction


// Assumes EnsureLocalRealmMasks(realm) has run and area_id is a string.
function GetPolicyMaskLocal(realm, area_id)
	if(g_areapolicy_masks.Exists(area_id))
		return CInt(g_areapolicy_masks[area_id]);
	endif
	return ReadPolicyMaskUncached(realm, area_id);
endfunction


function GetPolicyMask(realm, area_id)
	realm := SanitizePolicyRealm(realm);
	area_id := CStr(area_id);
	EnsureLocalRealmMasks(realm);
	return GetPolicyMaskLocal(realm, area_id);
endfunction


// Writes several areas' masks in ONE datafile session: open once, write
// each element, stamp the meta element once, unload once. changes is a
// dictionary of area_id -> mask. Does NOT touch the mask cache: callers
// finish with PruneStaleRealmPolicies() (or RebuildPolicyMaskCache()) which
// rebuilds the whole realm cache once. Returns the number written, or error
// if the datafile could not be opened.
function SetPolicyMasks(realm, byref changes, updated_by := "")
	realm := SanitizePolicyRealm(realm);
	if(!changes || !changes.size())
		return 0;
	endif

	var file_ref := OpenAreaPolicyFile(realm, DF_CREATE);
	if(file_ref == error)
		Print("[areas.persist] set open failed realm=" + CStr(realm));
		return file_ref;
	endif

	var now := ReadGameClock();
	var written := 0;
	foreach area_id in (changes.Keys())
		var elem := DFFindElement(file_ref, CStr(area_id), DF_CREATE);
		if(elem == error)
			Print("[areas.persist] set element failed realm=" + CStr(realm) + " area_id=" + CStr(area_id));
			continue;
		endif
		elem.SetProp(AREA_POLICY_MASK_PROP, CInt(changes[area_id]));
		elem.SetProp("UpdatedAt", now);
		elem.SetProp("UpdatedBy", CStr(updated_by));
		written := written + 1;
	endforeach

	if(written)
		var meta_elem := DFFindElement(file_ref, AREA_POLICY_META_ELEMENT, DF_CREATE);
		if(meta_elem != error)
			meta_elem.SetProp("SchemaVersion", AREA_POLICY_SCHEMA_VERSION);
			meta_elem.SetProp("UpdatedAt", now);
			meta_elem.SetProp("UpdatedBy", CStr(updated_by));
		else
			Print("[areas.persist] set meta missing realm=" + CStr(realm));
		endif
	endif

	UnloadDataFile(GetRealmPolicyDescriptor(realm));
	return written;
endfunction


// Single-area convenience wrapper over SetPolicyMasks(). Same cache rule.
function SetPolicyMask(realm, area_id, mask, updated_by := "")
	var changes := dictionary;
	changes[CStr(area_id)] := CInt(mask);
	var written := SetPolicyMasks(realm, changes, updated_by);
	if(written == error)
		return written;
	endif
	if(!written)
		return error{"errortext":="Could not write policy mask for area id: " + CStr(area_id)};
	endif
	return CInt(mask);
endfunction


// Deletes datafile elements whose id no longer exists in areas.cfg, then
// rebuilds the realm mask cache. Runs last in an .areas Save, so this is the
// single point where the cache is rebuilt from the freshly written datafile.
// Returns the number of elements removed (0 on failure).
function PruneStaleRealmPolicies(realm)
	realm := SanitizePolicyRealm(realm);

	var parsed := GetParsedRealmAreaLines(realm);
	if(parsed == error)
		Print("[areas.persist] prune load areas failed realm=" + CStr(realm));
		return 0;
	endif

	var valid_ids := dictionary;
	var i;
	for(i := 1; i <= parsed.size(); i := i + 1)
		valid_ids.Insert(CStr(parsed[i].area_id), 1);
	endfor

	var file_ref := OpenAreaPolicyFile(realm, DF_CREATE);
	if(file_ref == error)
		Print("[areas.persist] prune open failed realm=" + CStr(realm));
		return 0;
	endif

	var keys := file_ref.Keys();
	if(keys == error)
		Print("[areas.persist] prune keys failed realm=" + CStr(realm));
		UnloadDataFile(GetRealmPolicyDescriptor(realm));
		return 0;
	endif

	var removed := 0;
	foreach key in keys
		var key_str := CStr(key);
		if(key_str == AREA_POLICY_META_ELEMENT)
			continue;
		endif
		if(!valid_ids.Exists(key_str))
			file_ref.DeleteElement(key_str);
			removed := removed + 1;
		endif
	endforeach

	UnloadDataFile(GetRealmPolicyDescriptor(realm));

	RebuildPolicyMaskCache(realm);
	return removed;
endfunction
