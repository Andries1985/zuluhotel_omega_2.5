use datafile;

const CLASSFIRSTS_DATAFILE := ":classfirsts:classfirsts";

function OpenClassFirstsDataFile()
	var df := OpenDataFile( CLASSFIRSTS_DATAFILE );
	if( !df )
		CreateDataFile( CLASSFIRSTS_DATAFILE, DF_KEYTYPE_STRING );
		df := OpenDataFile( CLASSFIRSTS_DATAFILE );
	endif
	return df;
endfunction

// Name of the first player to reach this level/classe key (e.g. "1m"), or 0 if unclaimed.
function GetClassFirst( key )
	var df := OpenClassFirstsDataFile();
	if( !df )
		return 0;
	endif

	var elem := df.FindElement( key );
	var name := 0;
	if( elem )
		name := elem.GetProp( "name" );
	endif

	UnloadDataFile( CLASSFIRSTS_DATAFILE );
	return name;
endfunction

function SetClassFirst( key, name )
	var df := OpenClassFirstsDataFile();
	if( !df )
		return;
	endif

	var elem := df.FindElement( key );
	if( !elem )
		elem := df.CreateElement( key );
	endif
	if( elem )
		elem.SetProp( "name", name );
	endif

	UnloadDataFile( CLASSFIRSTS_DATAFILE );
endfunction
