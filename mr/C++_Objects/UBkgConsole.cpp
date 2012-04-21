// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UBkgConsole.cpp
// © Mark Tully 2000
// 28/12/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* * Redistributions of source code must retain the above copyright notice, this list
*   of conditions and the following disclaimer.
* * Redistributions in binary form must reproduce the above copyright notice, this
*   list of conditions and the following disclaimer in the documentation and/or other
*   materials provided with the distribution.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
* ***** END LICENSE BLOCK ***** */

/*
	Handy routines for using with the BkgConsole kit.
	As with BkgConsole you must have a Root.h file
*/

#include "Root.h"
#include "UBkgConsole.h"
#include "Version.h"
#include <iostream>
#include <fstream>
#include <syslimits.h>


// default the background console output stream to stdout
// can be overriden by assignment at any time, or by calling OpenLogFile
std::ostream			*UBkgConsole::gLogFile=&std::cout;
static std::fstream		*s_logFile=NULL;

// --------------------------------------------------------------------------------------------------------------------------
//		* DebugHeader
// --------------------------------------------------------------------------------------------------------------------------
// Echos a time stamp into the file
void UBkgConsole::DebugHeader()
{
	try
	{
		UInt32		time;
		Str255		timeStr,dateStr;
		
		::GetDateTime(&time);
		::DateString(time,abbrevDate,dateStr,0);
		::TimeString(time,true,timeStr,0);
		
		ECHO("---------------------------------------------------------------------------");
		ECHO( APP_NAME << " " << verShortText << " debug log started on " << dateStr << " " << timeStr);
		ECHO("---------------------------------------------------------------------------");
	}
	catch(...)
	{
	}	
}

// --------------------------------------------------------------------------------------------------------------------------
//		* OpenDefaultLogFile
// --------------------------------------------------------------------------------------------------------------------------
// Opens an automatically named log file. name is based on the name of the app from Root.h
void UBkgConsole::OpenDefaultLogFile()
{
	const char		*prefix=NULL;
#if TARGET_API_MAC_OSX
	FSRef			ref;
	OSErr			err=FSFindFolder(kUserDomain,kLogsFolderType,true,&ref);

	if (err==noErr)
	{
		// create file in directory??
		char		path[PATH_MAX];
		err=FSRefMakePath(&ref,(UInt8*)path,sizeof(path));
		if (err==noErr)
		{
			// add file name
			int		len=strlen(path);
			snprintf(path+len,sizeof(path)-len,"/%s.log",APP_NAME);
			OpenLogFile(path);
		}
	}
#else
	OpenLogFile(APP_NAME ".log");
#endif
}

// --------------------------------------------------------------------------------------------------------------------------
//		* OpenLogFile
// --------------------------------------------------------------------------------------------------------------------------
// Opens a log file at the path specified and sets it as the log output file
void UBkgConsole::OpenLogFile(const char *inPath)
{
	if (s_logFile)
	{
		CloseLogFile();
	}

	try
	{
		s_logFile=new std::fstream;
		s_logFile->open(inPath,std::fstream::out);
		UBkgConsole::gLogFile=s_logFile;
	}
	catch(...)
	{
	}
}

// --------------------------------------------------------------------------------------------------------------------------
//		* CloseLogFile
// --------------------------------------------------------------------------------------------------------------------------
// Closes the log file, and sets the log output back to std::cout
void UBkgConsole::CloseLogFile()
{
	if (s_logFile==UBkgConsole::gLogFile)
	{
		UBkgConsole::gLogFile=&std::cout;
	}

	try
	{
		if (s_logFile)
		{
			s_logFile->close();
			delete s_logFile;
			s_logFile=NULL;
		}
	}
	catch(...)
	{
	}
}

std::ostream &operator<<(std::ostream &stream, unsigned char *inPascalString)
{
	if (inPascalString && *inPascalString)
	{
		stream.write((const char*)inPascalString+1,inPascalString[0]);
	}
	return stream;
}

std::ostream &operator<<(std::ostream &stream, CFURLRef inRef)
{
	UInt8	str[300];
	CFURLGetFileSystemRepresentation(inRef,false,str,sizeof(str));
	stream << ((char*)str);
	return stream;
}
