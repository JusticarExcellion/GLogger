#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// 4 Characters compare macro
#define FourCC(string) (*(uint32*)(string))

internal bool32
New_CompareString( char* String1, int String1Count, char* String2, int String2Count )
{ //NOTE: We Could rework this to look for the Null terminator instead of
	//relying on the string count and we can say that if one string is longer
	//than the other we just return false automatically
	bool32 Result = false;
	for( int i = 0; i < String1Count && i < String2Count; i++ )
	{
		if( String1[i] != String2[i] )
		{
			break;
		}
		else if( ( i == ( String1Count - 1 ) ) && ( i == (String2Count - 1) ) )
		{
			Result = true;
		}
	}
	return Result;
}

internal bool32
CopyString( char* Dest, int DstLen, char* CopyStr, int CpStrLen )
{   //NOTE: Does not assume that either string is terminated with a null
		//character or that they are even the same length
		bool32 Valid = false;
		Valid = (DstLen >= (CpStrLen+1) && CpStrLen > -1);
		int index = 0;
		while( ( index < CpStrLen ) && ( index < DstLen ) && Valid )
		{
				*Dest++ = *CopyStr++;
				index++;
		}
		*Dest++ = 0;
		return Valid;
}

inline uint32
StringLength( char* String )
{   //NOTE: Assumes the string being given is null terminated
	uint32 Count = 0;
	while( *String++ )
	{
		Count++;
	}

	return Count;
}

internal void
CatStrings( size_t SourceACount, char* SourceA,
	   size_t SourceBCount, char* SourceB,
	   size_t DestCount,    char* Dest )
{
	for( int Index = 0; Index < SourceACount; ++Index )
	{
		*Dest++ = *SourceA++;
	}

	for( int Index = 0; Index < SourceBCount; ++Index )
	{
		*Dest++ = *SourceB++;
	}

	*Dest++ = 0;
}

#endif
