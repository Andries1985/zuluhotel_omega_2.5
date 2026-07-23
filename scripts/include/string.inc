///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// string.inc
//
// Siggy's String Function Include, v1.2
//
// History:
//	Version 1.2 - 26-Apr-2000
//	- Myrathi Added SortMultiArrayByIndex
//	- Code tweaks by Myrathi
//
//	Version 1.1 - 12-Apr-2000
//	- Added ReturnIndex
//	- Code cleanup
//
//	Version 1.0 - 10-Apr-2000
//	
//
// Questions, comments, flames?   Send to Sig at -
// EMail: prostheticdelirium@worldnet.att.net
// ICQ: 10520050
//
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

use os;
const CONST_EMPTY := { };
const CONST_NULL := "";


// Function TruncateArticle(x)
// This function will remove an indefinite article ("a" or "an") 
// from a string "x", returning the rest of the string.  If there
// is no article, it returns the original string "x".

function TruncateArticle(x)

	x := CStr(x);
	var i := len(x);

	var an_articlelist := { "an ", "An ", "AN ", "aN " };
	var a_articlelist := { "a ", "A " };

	if ( x[1,3] in an_articlelist )
		x := (x[4,i-3]);
	elseif ( x[1,2] in a_articlelist )
		x := (x[3,i-2]);
	endif

	return x;

endfunction


// function AddArticle(x)
// This function will prefix an indefinite article ("a" or "an") in
// front of a string.  If the string begins with a consonant, it 
// uses "a", and if the string begins with a vowel it will use "an".
// Note, this will occasionally produce grammatically incorrect
// phrases involving strings beginning with "h" or glided "u" sounds,
// such as "a hour", "an unit", or "a eunuch".  For the curious, the
// "a" is used before consonants, including the "y" sound in some
// phoenemes of "u".  The "an" article is used before a vowel sound,
// or before an "h" in words where the accent falls on any syllable
// other than the first (like "historic").

function AddArticle(x)

	x := CStr(x);
	var vowellist := { "a", "A", "e", "E", "i", "I", "o", "O", "u", "U" };
	var i := x[1];

	if ( i in vowellist )
		x := "an " + x;
	else
		x := "a " + x;
	endif

	return x;

endfunction


// function CreateArrayIndex(MainArray)
// This function will sort an array, and create an index of the
// original positions for each element.  This index can then be used
// to sort other arrays in the same order as the first one.  For
// example, if you have the array:
//
//      var myarray := { 4, 5, 9, 0, 100, 22, 7 };
//
// the function will return the following:
// 
//	result := CreateArrayIndex(myarray);
//	SortedArray := result[1];  // { 0, 4, 5, 7, 9, 22, 100 }
//	IndexArray := result[2];   // { 4, 1, 2, 7, 3, 6, 5}
//
// This function is useful for preserving information across multiple
// arrays when you need to sort one of them.

function CreateArrayIndex(MainArray)

	var IndexArray := CONST_EMPTY;
	var ArrayLen := Len(MainArray);
	var OldArray := MainArray;
	var NewArray := MainArray.sort;
	var i, q;

	for (q := 1; q <= ArrayLen; q := q + 1)
		for (i := 1; i <= ArrayLen; i := i + 1)
			if (OldArray[q] == MainArray[i])
				IndexArray[q] := i;
				i := ArrayLen + 1;
			endif
		endfor
	endfor

	return ( { NewArray, IndexArray } );

endfunction


// function SortArrayByIndex(OldArray, IndexArray)
// This function will sort "OldArray" by the order outlined in
// "IndexArray", where "IndexArray" is an array of sequential numbers
// equal to { 1, 2, 3, ... , len(OldArray) }.  If the IndexArray is a
// different size than OldArray, or if the sequence is broken, then
// the function will return 0.

function SortArrayByIndex(OldArray, IndexArray)

	var ArrayLen := Len(OldArray);

	if (ArrayLen != Len(IndexArray) )
		return 0;
	endif

	var NewArray := CONST_EMPTY;
	var i, q;

	for (i := 1; i <= ArrayLen; i := i + 1)
		q := IndexArray[i];
		if (q > ArrayLen)
			return 0;
		endif

		NewArray[i] := OldArray[q];

	endfor

	return NewArray;

endfunction


// function SortMultiArrayByIndex(MultiArray, SubIndex)
// (Function added 26-Apr-2000 by Myrathi)
// This function will sort an array of arrays (or structures) by
// the contents of the sub-index provided. So, if you have an 
// array (zCoords) of [x,y,z] structures, calling the function 
// with 'fn(zCoords,2)' will sort by the second sub-index... that
// is, the 'y' member. Returns '0' if the SubIndex is invalid
// (only checks against the first index)

// Rewritten (2026) from an O(n^2) nested-loop selection sort to an iterative
// bottom-up merge sort - O(n log n). The old version was heavy enough at a
// few hundred elements to risk tripping the runaway-script watchdog for any
// caller sorting a shard-wide collection (houses, boats, etc). Same ascending
// order by SubIndex as before.
function SortMultiArrayByIndex(MultiArray, SubIndex)

	var ArrayLen := Len(MultiArray);
	if ( ArrayLen < 2 )
		return MultiArray;
	endif

	if ( SubIndex > len(MultiArray[1]) )
		return 0;
	endif

	var width := 1;
	while ( width < ArrayLen )
		var i := 1;
		while ( i <= ArrayLen )
			var mid := i + width - 1;
			if ( mid > ArrayLen )
				mid := ArrayLen;
			endif
			var high := i + (2 * width) - 1;
			if ( high > ArrayLen )
				high := ArrayLen;
			endif
			MultiArray := MergeRunsByIndex(MultiArray, i, mid, high, SubIndex);
			i := i + (2 * width);
		endwhile
		width := width * 2;
	endwhile

	return MultiArray;

endfunction

function MergeRunsByIndex(MultiArray, low, mid, high, SubIndex)
	if ( mid >= high )
		return MultiArray;
	endif

	var left := array;
	var i;
	for(i := low; i <= mid; i := i + 1)
		left.append(MultiArray[i]);
	endfor

	var right := array;
	for(i := mid + 1; i <= high; i := i + 1)
		right.append(MultiArray[i]);
	endfor

	var li := 1;
	var ri := 1;
	var write_idx := low;
	while ( li <= left.size() && ri <= right.size() )
		if ( right[ri][SubIndex] < left[li][SubIndex] )
			MultiArray[write_idx] := right[ri];
			ri := ri + 1;
		else
			MultiArray[write_idx] := left[li];
			li := li + 1;
		endif
		write_idx := write_idx + 1;
	endwhile

	while ( li <= left.size() )
		MultiArray[write_idx] := left[li];
		li := li + 1;
		write_idx := write_idx + 1;
	endwhile

	while ( ri <= right.size() )
		MultiArray[write_idx] := right[ri];
		ri := ri + 1;
		write_idx := write_idx + 1;
	endwhile

	return MultiArray;
endfunction


// function PadString(x, stringlength)
// This function will pad a string "x" with enough spaces to reach
// a length indicated by "stringlength".  If the string is already
// longer than stringlength, the function will return 0.

function PadString(x, stringlength)

	var padding := stringlength - len(x);
	if (padding < 0)
		return 0;
	endif

	var padstr := "                    ";     // 20 spaces

	while(padding > 20)
		x := x + padstr;
		padding := padding - 20;
	endwhile

	if (padding > 0)
		x := x + padstr[1, padding];
	endif

	return x;

endfunction



// function PadArray(x, arraylength)
// This function will pad an array "x" with enough 0s to reach length
// specified in "arraylength".  If the length of the array is already
// greater than arraylength, the function will return 0.

function PadArray(x, arraylength)

	var padding := arraylength - len(x);

	if (padding < 0)
		return 0;
	elseif (padding == 0)
		return x;
	endif

	var q := len(x);
	var i;

	for (i := 1; i <= padding; i := i + 1)
		q := q + 1;
		x[q] := 0;
	endfor

	return x;

endfunction


// function ReturnIndex(myarray, elementvalue)
// This function will return the index (location within an
// array) of an element.  If the element is not found, then
// the function will return 0.

function ReturnIndex(myarray, elementvalue)

	var i;
	for (i := 1; i <= len(myarray); i := i + 1)
		if (myarray[i] == elementvalue)
			return i;
		endif
	endfor

	return 0;

endfunction

function Check_ValidChar( text, allow_space:=0 )

         //Make sure you check a text without spaces or it will fail.
         var valid_char := array{ "a", "b", "c", "d", "e", "f", "g",
                                  "h", "i", "j", "k", "l", "m", "n",
                                  "o", "p", "q", "r", "s", "t", "u",
                                  "v", "w", "x", "y", "z", "A", "B",
                                  "C", "D", "E", "F", "G", "H", "I",
                                  "P", "O", "N", "M", "L", "K", "J",
                                  "Q", "R", "S", "T", "U", "V", "W",
                                  "X", "Y", "Z", "1", "2", "3", "4",
                                  "5", "6", "7", "8", "9", "0" };

         var len_text := Len( text );

         var i;
         for( i:=1; i<=len_text; i+=1 )
                  SleepMS(5);
                  var letter := CStr( text[i] );
                  if( letter in valid_char )
                           continue;
                  elseif( allow_space && ( text[i] == " " || CAsc( text[i] ) == 0 ))
                           continue;
                  endif
                  
                  return 0;
         endfor
         
         return 1;
endfunction