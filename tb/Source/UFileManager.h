// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UFileManager.h
// © John Treece-Birch 2001
// 8/5/01
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2001, Mark Tully and John Treece-Birch
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

#pragma once

#include	"TNTBasic_Types.h"
#include	"CCString.h"

class CProgram;

class UFileManager
{
	public:
		enum EDataType
		{
			kDataFile=0,
			kTextFile,
		};
		
	private:
		static TArray<TTBInteger>		sFiles;
		static TArray<FSSpec>			sSpecs;
		static TArray<EDataType>		sFileDataFormat;
		static TArray<SInt32>			sFilePos;
		
		static FSSpec					sCurrentDirectory;
		static FSSpec					sGameDirectory;
		static CProgram					*sProgram;
		
		static TTBInteger				OpenFile(
											FSSpec		&spec,
											bool		inReadWrite,
											OSType		inType,
											EDataType	inDataType);
		
		static ArrayIndexT				IsLoaded(
											TTBInteger		inFile);
		static ArrayIndexT				IsLoaded(
											FSSpec			&inFile);
		static ArrayIndexT				GetFreeSpace();
		
	public:
		static void						Initialise(
											CProgram		*inProgram);
		static void						Shutdown();
		
		static void						SetGameDirectory(
											FSSpec			&inSpec)
										{
											sGameDirectory=inSpec;
										}
										
		static TTBInteger				CreateFile(
											CCString		&inPath,
											OSType			inType,
											EDataType		inDataType);
		static TTBInteger				OpenFile(
											CCString		&inPath,
											bool			inReadWrite,
											OSType			inType,
											EDataType		inDataType);
		static TTBInteger				AskUserFileCreate(
											OSType			inType,
											EDataType		inDataType);
		static TTBInteger				AskUserFileOpen(
											bool			inWriteAccess,
											OSType			inType,
											EDataType		inDataType);
		
		static void						CloseFile(
											TTBInteger		inFile);
		
		static void						NewFolder(
											CCString		&inPath);
		
		static void						DeleteFile(
											CCString		&inPath);
		
		static void						FileWriteInteger(
											TTBInteger		inFile,
											TTBInteger		inData);
		static void						FileWriteFloat(
											TTBInteger		inFile,
											TTBFloat		inData);
		static void						FileWriteString(
											TTBInteger		inFile,
											CCString		&inData);
		
		static TTBInteger				FileLength(
											TTBInteger		inFile);
		static TTBInteger				FilePos(
											TTBInteger		inFile);
		static void						SetFilePos(
											TTBInteger		inFile,
											TTBInteger		inPos);
											
		static bool						FileExists(
											CCString		&inPath);
		static OSType					FileType(
											CCString		&inPath);
	
		static void						FileReadString(
											TTBInteger		inFile,
											CCString		&outString);
		static TTBFloat					FileReadFloat(
											TTBInteger		inFile);
		static TTBInteger				FileReadInteger(
											TTBInteger		inFile);
											
		static void						SetDirectoryUserPreferences();
		static void						SetDirectoryTemporary();
		static void						SetDirectorySystemPreferences();
		static void						SetDirectoryGame();
};