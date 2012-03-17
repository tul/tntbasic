// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UFileManager.cpp
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

#include		"UFileManager.h"
#include		"Marks Routines.h"
#include		"CProgram.h"
#include		<UNavServicesDialogs.h>
#include 		<stdio.h>
#include		"Root.h"

TArray<TTBInteger>					UFileManager::sFiles;
TArray<FSSpec>						UFileManager::sSpecs;
TArray<UFileManager::EDataType>		UFileManager::sFileDataFormat;
TArray<SInt32>						UFileManager::sFilePos;

FSSpec								UFileManager::sCurrentDirectory;
FSSpec								UFileManager::sGameDirectory;
CProgram							*UFileManager::sProgram;

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Power
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
float Power(
	SInt32		inValue,
	SInt32		inPower)
{
	float		value=1;
	
	if (inPower>=0)
	{
		while (inPower>0)
		{
			value*=inValue;
			inPower--;
		}
	}
	else
	{
		while (inPower<0)
		{
			value/=inValue;
			inPower++;
		}
	}
		
	return value;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Initialise
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::Initialise(
	CProgram		*inProgram)
{
	sProgram=inProgram;
	
	SetDirectoryUserPreferences();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Shutdown
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::Shutdown()
{
	ArrayIndexT		max=sFiles.GetCount();
	FSSpec			spec;
	
	spec=sCurrentDirectory;
	::CopyCAsPStr("\p",spec.name);
	
	for (ArrayIndexT n=1; n<=max; n++)
	{
		TTBInteger	fid=sFiles[n];
		if (fid!=-1)
		{
			::FSClose(fid);
			sFiles[n]=-1;
			sSpecs[n]=spec;
		}
	}
	
	sFiles.RemoveAllItemsAfter(0);
	sSpecs.RemoveAllItemsAfter(0);
	sFileDataFormat.RemoveAllItemsAfter(0);
	sFilePos.RemoveAllItemsAfter(0);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CreateFile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::CreateFile(
	CCString		&inPath,
	OSType			inType,
	EDataType		inDataType)
{
	FSSpec		spec;
	
	spec=sCurrentDirectory;
	::CopyCAsPStr((unsigned char*)inPath.mString,spec.name);
	
	if (OSErr err=::FSpCreate(&spec,sProgram->GetCreatorCode(),inType,0))
		UTBException::ThrowCouldNotCreateFile();
			
	return OpenFile(spec,true,inType,inDataType);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ OpenFile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::OpenFile(
	CCString		&inPath,
	bool			inReadWrite,
	OSType			inType,
	EDataType		inDataType)
{
	FSSpec		spec;
	
	spec=sCurrentDirectory;
	::CopyCAsPStr((unsigned char*)inPath.mString,spec.name);
	
	return OpenFile(spec,inReadWrite,inType,inDataType);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AskUserFileCreate
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::AskUserFileCreate(
	OSType		inType,
	EDataType	inDataType)
{
	FSSpec									spec;
	NavDialogOptionFlags					flags=kNavDefaultNavDlogOptions+kNavNoTypePopup-kNavAllowPreviews;
	
	bool					outReplacing;
		
	if (UNavServicesDialogs::AskSaveFile("\pUntitled",inType,spec,outReplacing,flags))
	{
		if (outReplacing)
		{
			if (OSErr err=::FSpDelete(&spec))
				UTBException::ThrowCouldNotDeleteFile();
		}
			
		if (OSErr err=::FSpCreate(&spec,sProgram->GetCreatorCode(),inType,0))
			UTBException::ThrowCouldNotCreateFile();
		
		return OpenFile(spec,true,inType,inDataType);
	}
		
	return -1;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AskUserFileOpen
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::AskUserFileOpen(
	bool			inWriteAccess,
	OSType			inType,
	EDataType		inDataType)
{
	FSSpec									spec;
	NavDialogOptionFlags					flags=kNavDefaultNavDlogOptions+kNavNoTypePopup-kNavAllowPreviews;
	
	UNavServicesDialogs::LFileChooser		chooser;
		
	// filter for the types we can open
	OSType									types[]={inType};
	LFileTypeList							fileTypes(1,types);
	NavDialogCreationOptions				*options=chooser.GetDialogOptions();

	flags &= ~kNavAllowMultipleFiles;		// Can't select multiple files
	options->optionFlags=flags;

	if (chooser.AskOpenFile(fileTypes))
	{
		chooser.GetFileSpec(1,spec);
		
		return OpenFile(spec,inWriteAccess,inType,inDataType);
	}
	
	return -1;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ OpenFile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::OpenFile(
	FSSpec		&spec,
	bool		inReadWrite,
	OSType		inType,
	EDataType	inDataType)
{
	OSType		creator,type;
	
	if (GetCreatorAndType(&spec,&creator,&type)==noErr)
	{
		if (type!=inType)
			UTBException::ThrowWrongFileType();
	}
	
	// If the file is already loaded then return that	
	if (ArrayIndexT index=IsLoaded(spec))
	{
		SetFilePos(sFiles[index],0);
		return sFiles[index];
	}
		
	// Load the file	
	ArrayIndexT		index=GetFreeSpace();
		
	if (index>sFiles.GetCount())
	{
		sFiles.InsertItemsAt(index-sFiles.GetCount(),LArray::index_Last,(TTBInteger)-1);
		
		FSSpec			invalidSpec;

		invalidSpec=sCurrentDirectory;
		::CopyCAsPStr("\p",invalidSpec.name);
	
		sSpecs.InsertItemsAt(index-sSpecs.GetCount(),LArray::index_Last,invalidSpec);
		sFileDataFormat.InsertItemsAt(index-sFileDataFormat.GetCount(),LArray::index_Last,kDataFile);
		sFilePos.InsertItemsAt(index-sFilePos.GetCount(),LArray::index_Last,(SInt32)0);
	}
	
	short		file;
	char		permission;
	
	if (inReadWrite)
		permission=fsWrPerm;
	else
		permission=fsRdPerm;
	
	if (OSErr err=::FSpOpenDF(&spec,permission,&file))
		UTBException::ThrowCouldNotOpenFile();

	sSpecs[index]=spec;
	sFiles[index]=TTBInteger(file);
	sFileDataFormat[index]=inDataType;
	sFilePos[index]=0;
	
	return file;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CloseFile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::CloseFile(
	TTBInteger		inFile)
{
	if (!IsLoaded(inFile))
		UTBException::ThrowFileIsNotLoaded();
		
	ArrayIndexT		max=sFiles.GetCount();
								
	for (ArrayIndexT n=1; n<=max; n++)
	{
		if (sFiles[n]==inFile)
		{
			OSErr	err=::FSClose(sFiles[n]);
			SignalIfOSErr_(err);
			sFiles[n]=-1;
			
			FSSpec			spec;

			spec=sCurrentDirectory;
			::CopyCAsPStr("\p",spec.name);

			sSpecs[n]=spec;
			return;
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ NewFolder
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::NewFolder(
	CCString		&inPath)
{
	FSSpec		spec;
	long		directoryId;
	
	spec=sCurrentDirectory;
	::CopyCAsPStr((unsigned char*)inPath.mString,spec.name);
	
	if (OSErr err=::FSpDirCreate(&spec,0L,&directoryId))
		UTBException::ThrowCouldNotCreateFolder();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteFile
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::DeleteFile(
	CCString		&inPath)
{
	FSSpec		spec;
	
	spec=sCurrentDirectory;
	::CopyCAsPStr((unsigned char*)inPath.mString,spec.name);
	
	if (OSErr err=::FSpDelete(&spec))
		UTBException::ThrowCouldNotDeleteFile();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileWriteInteger
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::FileWriteInteger(
	TTBInteger		inFile,
	TTBInteger		inData)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();
	
	long	ioCount;
	
	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			ioCount=sizeof(inData);
			::FSWrite(inFile,&ioCount,&inData);
		}
		break;
		
		case kTextFile:
		{
			bool	writing=false;
			
			ioCount=sizeof(char);
			
			for (SInt32 n=10; n>=0; n--)
			{
				SInt32	power=Power(10,n);
				char	letter=inData/power+'0';
				
				if ((letter-'0')>0)
					writing=true;
							
				inData-=(letter-'0')*power;
				
				if (writing)
					::FSWrite(inFile,&ioCount,&letter);
			}
			
			char	carriage='\r';
			ioCount=1;
			::FSWrite(inFile,&ioCount,&carriage);
			
			sFilePos[index]++;
		}
		break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileWriteFloat
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::FileWriteFloat(
	TTBInteger		inFile,
	TTBFloat		inData)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();
	
	long	ioCount;
	
	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			ioCount=sizeof(inData);
			::FSWrite(inFile,&ioCount,&inData);
		}
		break;
		
		case kTextFile:
		{
			CCString	string;
			
			sprintf(string,"%f",inData);
			
			SInt32	n=0;
			
			while (string[n])
			{
				ioCount=sizeof(char);
				::FSWrite(inFile,&ioCount,&string[n]);
				n++;
			}
			
			// Add a return
			char	carriage='\r';
			ioCount=1;
			::FSWrite(inFile,&ioCount,&carriage);
			
			sFilePos[index]++;
		}
		break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileWriteString
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::FileWriteString(
	TTBInteger		inFile,
	CCString		&inData)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();
	
	long	ioCount;
	
	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			ioCount=inData.GetLength()+1;
			::FSWrite(inFile,&ioCount,inData.mString);
		}
		break;
		
		case kTextFile:
		{
			char	carriage='\r';
			
			ioCount=inData.GetLength();
			::FSWrite(inFile,&ioCount,inData.mString);
			
			ioCount=1;
			::FSWrite(inFile,&ioCount,&carriage);
			
			sFilePos[index]++;
		}
		break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileReadString
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::FileReadString(
	TTBInteger		inFile,
	CCString		&outString)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();
	
	outString.mString[0]=0;	// clear the string
		
	char	letter;
	long	ioCount=sizeof(letter);
	
	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			do
			{
				OSErr	err=::FSRead(inFile,&ioCount,&letter);
				
				if (err!=noErr)
					UTBException::ThrowUnexpectedEndOfFile();
					
				outString+=letter;
				
			} while (letter);
		}
		break;
		
		case kTextFile:
		{
			OSErr	err=::FSRead(inFile,&ioCount,&letter);
			
			if (err!=noErr)
				UTBException::ThrowUnexpectedEndOfFile();
					
			while ((letter!='\n') && (letter!='\r') && (err!=eofErr))
			{
				outString+=letter;
				
				err=::FSRead(inFile,&ioCount,&letter);
				
				if ((err!=noErr) && (err!=eofErr))
					UTBException::ThrowUnexpectedEndOfFile();
			}
			
			sFilePos[index]++;
		}
		break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileReadFloat
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBFloat UFileManager::FileReadFloat(
	TTBInteger		inFile)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();
		
	TTBFloat	output=0;
	
	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			long	ioCount=sizeof(output);
			
			::FSRead(inFile,&ioCount,&output);	
		}
		break;
		
		case kTextFile:
		{
			CCString	string;
			char		letter;
			long		ioCount=sizeof(letter);
			
			OSErr	err=::FSRead(inFile,&ioCount,&letter);
			
			if (err!=noErr)
				UTBException::ThrowUnexpectedEndOfFile();
					
			while ((letter!='\n') && (letter!='\r') && (err!=eofErr))
			{
				string+=letter;
				
				err=::FSRead(inFile,&ioCount,&letter);
				
				if ((err!=noErr) && (err!=eofErr))
					UTBException::ThrowUnexpectedEndOfFile();
			}
			
			// Convert the string to a number
			SInt32		n,length=string.GetLength(),point=length;
			
			for (n=0; n<length; n++)
			{
				if (string[n]=='.')
					point=n;
			}
			
			output=0;
			
			// Read before the point
			for (n=0; n<point; n++)
				output+=(string[n]-'0')*Power(10,(point-n-1));
				
			// Read after the point
			if (point+1<length)
			{
				for (n=point+1; n<length; n++)
					output+=(string[n]-'0')*Power(10,-(n-point));
			}
			
			sFilePos[index]++;
		}
		break;
	}
	
	return output;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileReadInteger
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::FileReadInteger(
	TTBInteger		inFile)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();
		
	TTBInteger	output;
	
	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			long		ioCount=sizeof(output);
			ThrowIfOSErr_(::FSRead(inFile,&ioCount,&output));
		}
		break;
		
		case kTextFile:
		{
			CCString	string;
			char		letter;
			long		ioCount=sizeof(letter);
			
			OSErr	err=::FSRead(inFile,&ioCount,&letter);
			
			if (err!=noErr)
				UTBException::ThrowUnexpectedEndOfFile();
					
			while ((letter!='\n') && (letter!='\r') && (err!=eofErr))
			{
				string+=letter;
				
				err=::FSRead(inFile,&ioCount,&letter);
				
				if ((err!=noErr) && (err!=eofErr))
					UTBException::ThrowUnexpectedEndOfFile();
			}
			
			// Convert the string to a number
			SInt32		length=string.GetLength();
			
			output=0;
			
			for (SInt32 n=0; n<length; n++)
				output+=(string[n]-48)*Power(10,(length-n-1));
				
			sFilePos[index]++;
		}
		break;
	}
	
	return output;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileLength
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::FileLength(
	TTBInteger		inFile)
{
	if (!IsLoaded(inFile))
		UTBException::ThrowFileIsNotLoaded();

	TTBInteger	output=0;
			
	ThrowIfOSErr_(::GetEOF(inFile,&output));
	
	return output;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FilePos
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
TTBInteger UFileManager::FilePos(
	TTBInteger		inFile)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();

	TTBInteger	output=0;

	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			ThrowIfOSErr_(::GetFPos(inFile,&output));
		}
		break;
		
		case kTextFile:
		{
			output=sFilePos[index];
		}
		break;
	}
	
	return output;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetFilePos
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::SetFilePos(
	TTBInteger		inFile,
	TTBInteger		inPos)
{
	ArrayIndexT		index=IsLoaded(inFile);
	
	if (!index)
		UTBException::ThrowFileIsNotLoaded();
	
	switch (sFileDataFormat[index])
	{
		case kDataFile:
		{
			ThrowIfOSErr_(::SetFPos(inFile,fsFromStart,inPos));
		}
		break;
		
		case kTextFile:
		{
			ThrowIfOSErr_(::SetFPos(inFile,fsFromStart,0));
			
			char	letter;
			SInt32	count=0;
			SInt32	ioCount=sizeof(letter);
			
			while (count<inPos)
			{
				do
				{
					OSErr	err=::FSRead(inFile,&ioCount,&letter);
					
					if (err!=noErr)
						UTBException::ThrowUnexpectedEndOfFile();
				}
				while ((letter!='\n') && (letter!='\r'));
				
				count++;
			}
			
			sFilePos[index]=inPos;
		}
		break;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileExists
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool UFileManager::FileExists(
	CCString		&inPath)
{
	FSSpec		spec;
	
	spec=sCurrentDirectory;
	::CopyCAsPStr((unsigned char*)inPath.mString,spec.name);
	
	if (IsLoaded(spec))
		return true;
	
	short		file;
	OSErr		err;
	
	err=::FSpOpenDF(&spec,fsRdPerm,&file);
	if (err==noErr)
	{
		::FSClose(file);
	}
	
	if (err==fnfErr)
		return false;
	
	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FileType
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
OSType UFileManager::FileType(
	CCString		&inPath)
{
	FSSpec		spec;
	OSType		creator,type;
	
	spec=sCurrentDirectory;
	::CopyCAsPStr((unsigned char*)inPath.mString,spec.name);
	
	if (OSErr err=GetCreatorAndType(&spec,&creator,&type))
		UTBException::ThrowFileTypeCouldNotBeFound();
	
	return type;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDirectoryUserPreferences
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::SetDirectoryUserPreferences()
{
	short	vRefNum;
	long	parID;
	
	if (::FindFolder(kOnSystemDisk,kPreferencesFolderType,kCreateFolder,&vRefNum,&parID)==noErr)
	{
		sCurrentDirectory.vRefNum=vRefNum;
		sCurrentDirectory.parID=parID;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDirectoryTemp
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::SetDirectoryTemporary()
{
	short	vRefNum;
	long	parID;
	
	if (::FindFolder(kOnSystemDisk,kTemporaryFolderType,kCreateFolder,&vRefNum,&parID)==noErr)
	{
		sCurrentDirectory.vRefNum=vRefNum;
		sCurrentDirectory.parID=parID;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDirectorySystemPreferences
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::SetDirectorySystemPreferences()
{
	short	vRefNum;
	long	parID;
	
	if (::FindFolder(kOnSystemDisk,kSystemPreferencesFolderType,kCreateFolder,&vRefNum,&parID)==noErr)
	{
		sCurrentDirectory.vRefNum=vRefNum;
		sCurrentDirectory.parID=parID;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDirectoryHome
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UFileManager::SetDirectoryGame()
{
	sCurrentDirectory=sGameDirectory;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsLoaded
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
ArrayIndexT UFileManager::IsLoaded(
	TTBInteger		inFile)
{
	if (inFile!=-1)
	{	
		ArrayIndexT		max=sFiles.GetCount();
		
		for (ArrayIndexT n=1; n<=max; n++)
		{
			if (sFiles[n]==inFile)
				return n;
		}
	}
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ IsLoaded
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
ArrayIndexT UFileManager::IsLoaded(
	FSSpec		&inFile)
{
	ArrayIndexT		max=sFiles.GetCount();
		
	for (ArrayIndexT n=1; n<=max; n++)
	{
		if ((sSpecs[n].vRefNum==inFile.vRefNum) &&
			(sSpecs[n].parID==inFile.parID) &&
			(CmpPStr(sSpecs[n].name,inFile.name)))
		{
			return n;
		}
	}
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetFreeSpace
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
ArrayIndexT UFileManager::GetFreeSpace()
{
	ArrayIndexT		max=sFiles.GetCount();
								
	for (ArrayIndexT n=1; n<=max; n++)
	{
		if (sFiles[n]==-1)
			return n;
	}

	return max+1;
}
