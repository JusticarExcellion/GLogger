/*******************************************************************
* Title: Glogger
* Author: Xander Bruce
* Date: 6-4-2025
* Desc: Logging for ma games
********************************************************************/
#include <Windows.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>

#if DEBUG_SLOW
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define Kilobytes(value) ((value)*1024LL)
#define Megabytes(value) (Kilobytes(value)*1024LL)
#define Gigabytes(value) (Megabytes(value)*1024LL)
#define Terabytes(value) (Gigabytes(value)*1024LL)

#define ArrayCount(Array) ( sizeof(Array) / sizeof( ( Array )[0] ) )
#define WIN32_FILE_NAME_COUNT MAX_PATH

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

typedef int32 bool32;

#define internal static
#define global static

#define VERSION "Version Number: 00.00.01"
#define LOG_NAME "GameClock.Log"

global int64 GlobalPerfCountFrequency;

#include "StringUtils.h"

inline real32
Win32_GetSecondsElapsed( LARGE_INTEGER Start, LARGE_INTEGER End )
{
	real32 Result =  ( (real32)( End.QuadPart - Start.QuadPart ) /
				(real32)( GlobalPerfCountFrequency ) );
	return Result;
}

internal bool32
WriteBufferToFile( HANDLE *File, char* Buffer, size_t SizeOfBuffer )
{
	bool32 Valid = true;
	DWORD BytesWritten;
	DWORD BytesToWrite = StringLength( Buffer );
	#if LOGGING_INTERNAL
	char TextBuffer[256] = {};
#endif
	if( WriteFile( *File, Buffer, BytesToWrite, &BytesWritten, 0 ) )
	{
	#if LOGGING_INTERNAL
		_snprintf_s( TextBuffer, sizeof(TextBuffer), "Bytes To Write: %d\nBytes Written: %d\n", BytesToWrite, BytesWritten );
		OutputDebugStringA( TextBuffer );
#endif
	}
	else
	{
		OutputDebugStringA("ERROR: FAILED TO WRITE TO FILE, EXITING...\n");
		Valid = false;
	}

	return Valid;
}

internal bool32
GetLocalTimeAsStr( char* Buffer, size_t SizeOfBuffer )
{
	bool32 Valid = true;
	time_t Time;
	struct tm LocalTime;
	errno_t Err;

	time( &Time );
	Err = localtime_s( &LocalTime, &Time );
	Err = asctime_s( Buffer, SizeOfBuffer, &LocalTime );
	return Valid;
}

int main( int argc, char** argv )
{
	LARGE_INTEGER FileSize;
	bool32 Valid = true;
	HANDLE LogFile = CreateFileA(
		LOG_NAME,
		GENERIC_WRITE | GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		0,
		OPEN_EXISTING,0,0
	);

	if( LogFile == INVALID_HANDLE_VALUE )
	{
		Valid = false;
		DWORD ErrorCode = GetLastError();
		if( ErrorCode == ERROR_FILE_NOT_FOUND )
		{
			OutputDebugStringA("ERROR: COULD NOT OPEN LOG FILE, CREATING NEW LOG FILE\n");
			LogFile = CreateFileA(
				"GameClock.Log",
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				0,
				CREATE_NEW,0,0
			);
			Valid = (LogFile != INVALID_HANDLE_VALUE ) ? true : false;
		}
		else
		{
			OutputDebugStringA("ERROR: UNEXPECTED ERROR RECIEVED EXITING...\n");
			return 1;
		}
	}

	if( !GetFileSizeEx( LogFile, &FileSize ) )
	{
		Valid = false;
	}

	if( Valid )
	{

		//TODO: Check for asctime_s & localtime_s errors
		//		Ability to take breaks and still be reliable timing

		char UserInput[256] = {};
		char TextBuffer[512] = {};
		char TimeFmtStr[256] = {};
		LARGE_INTEGER StartTime;
		LARGE_INTEGER EndTime;
		LARGE_INTEGER PerfFrequency;

		//asctime_s
		//time_t StartBreak_DateTime;
		//time_t EndBreak_DateTime;

		QueryPerformanceFrequency( &PerfFrequency );
		GlobalPerfCountFrequency = PerfFrequency.QuadPart;

		DWORD BytesWritten = 0;
		DWORD BytesToWrite = 0;
		DWORD BytesRead = 0;
		DWORD BytesToRead = BytesToWrite;

		//NOTE: Setting File Pointer to the end of the file
		SetFilePointer( LogFile, FileSize.LowPart, NULL, FILE_BEGIN );

		OutputDebugStringA("Input Game Name: ");
		while( !UserInput[0] )
		{
			fgets( UserInput, sizeof( UserInput ), stdin );
		}
		_snprintf_s( TextBuffer, sizeof(TextBuffer), "Input: %s\n", UserInput );
		OutputDebugStringA( TextBuffer );

		GetLocalTimeAsStr( TimeFmtStr, sizeof(TimeFmtStr) );
		_snprintf_s( TextBuffer, sizeof(TextBuffer), "Game: %s\r\nStart Time: %s\r\n", UserInput, TimeFmtStr );

		WriteBufferToFile( &LogFile, (char*)TextBuffer, sizeof(TextBuffer) );
		QueryPerformanceCounter( &StartTime );

		while( 1 )
		{
			fgets( (char*)UserInput, sizeof(UserInput), stdin );
			if(*UserInput == 'b')
			{
				OutputDebugStringA("Taking a break!!!\n");
			}

			if(*UserInput == '\n')
			{
				OutputDebugStringA("End Timer\n");
				break;
			}
		}

		QueryPerformanceCounter( &EndTime );

		GetLocalTimeAsStr( TimeFmtStr, sizeof(TimeFmtStr) );
		_snprintf_s( TextBuffer, sizeof(TextBuffer), "End Time: %s\r\n", TimeFmtStr );
		WriteBufferToFile( &LogFile, (char*)TextBuffer, sizeof(TextBuffer) );

		real32 TotalSeconds = Win32_GetSecondsElapsed( StartTime, EndTime );
		real32 TotalMinutes = TotalSeconds / 60;
		real32 TotalHours   = TotalMinutes / 60;

		_snprintf_s( TextBuffer, sizeof(TextBuffer), "Total Seconds: %.05f\r\nTotal Minutes: %.05f\r\nTotal Hours: %.05f\r\n\r\n", TotalSeconds, TotalMinutes, TotalHours );
		WriteBufferToFile( &LogFile, (char*)TextBuffer, sizeof(TextBuffer) );
	}

	CloseHandle( LogFile );
	return 0;
}
