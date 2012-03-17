// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// UDataFolderManager.cpp
// John Treece-Birch
// 30/7/02
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

#include		"Marks Routines.h"
#include		"CApplication.h"
#include		"LFile.h"
#include		"UDataFolderManager.h"
#include		"UTBException.h"

TArray<CDataInfo>		UDataFolderManager::sDatas;
FSSpec					UDataFolderManager::sDataSpec;
FSSpec					UDataFolderManager::sDataTypeSpec[kDataTypes];

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Initialise
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
void UDataFolderManager::Initialise(
	FSSpec			&inDataFolderSpec)
{
	Str255			string;
	
	sDataSpec=inDataFolderSpec;
	
	// Make the music folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Music",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kMusicDataType]);
	
	// Make the input folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Input",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kInputDataType]);
	
	// Make the sound banks folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Sounds",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kSoundDataType]);
	
	// Make the sound banks folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Sound Banks",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kSoundBankDataType]);
	
	// Make the image banks folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Image Banks",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kImageBankDataType]);
	
	// Make the images folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Images",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kImageDataType]);
	
	// Make the image masks folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Image Masks",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kImageMaskDataType]);
	
	// Make the map folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Maps",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kMapDataType]);
	
	// Make the code folder spec
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataSpec.name,255);
	LString::AppendPStr(string,"\p:Code",63);
	::FSMakeFSSpec(sDataSpec.vRefNum,sDataSpec.parID,string,&sDataTypeSpec[kCodeDataType]);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ShutDown
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Release any spare file.  This should not be relied upon because it is much neater to
// release the resources yourself.
void UDataFolderManager::ShutDown()
{
	while (sDatas.GetCount())
	{
		::DisposeHandle(sDatas[1].mHandle);
		sDatas.Remove(sDatas[1]);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ MakeSpec
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
FSSpec UDataFolderManager::MakeSpec(
	CCString		&inName,
	EDataType		inType)
{
	FSSpec			spec;
	Str255			string;
	
	LString::CopyPStr("\p:",string);
	LString::AppendPStr(string,sDataTypeSpec[inType].name,255);
	LString::AppendPStr(string,"\p:",63);
	
	CCString		temp=inName;
	LString::AppendPStr(string,LString::CToPStr(temp),63);
	
	switch (inType)
	{
		case kMusicDataType:
		{
			Str255		temp;
			OSErr		err;
			short		file;
			
			LString::CopyPStr(string,temp);
			LString::AppendPStr(temp,"\p.mp3",63);
			
			::FSMakeFSSpec(sDataTypeSpec[inType].vRefNum,sDataTypeSpec[inType].parID,temp,&spec);
			if (!(err=::FSpOpenDF(&spec,fsRdPerm,&file)))
				::FSClose(file);
				
			if (err==fnfErr)
			{
				LString::CopyPStr(string,temp);
				LString::AppendPStr(temp,"\p.mov",63);
				
				::FSMakeFSSpec(sDataTypeSpec[inType].vRefNum,sDataTypeSpec[inType].parID,temp,&spec);
				if (!(err=::FSpOpenDF(&spec,fsRdPerm,&file)))
					::FSClose(file);
				
				if (err==fnfErr)
				{
					LString::CopyPStr(string,temp);
					LString::AppendPStr(temp,"\p.mod",63);
				}
			}
			
			LString::CopyPStr(temp,string);
		}
		break;
		
		case kInputDataType: LString::AppendPStr(string,"\p.ipt",63); break;
		case kSoundDataType: LString::AppendPStr(string,"\p.aif",63); break;
		case kSoundBankDataType: LString::AppendPStr(string,"\p.sbk",63); break;
		case kImageBankDataType: LString::AppendPStr(string,"\p.ibk",63); break;
		case kImageDataType: LString::AppendPStr(string,"\p.tga",63); break;
		case kImageMaskDataType: LString::AppendPStr(string,"\p.tga",63); break;
		case kMapDataType: LString::AppendPStr(string,"\p.map",63); break;
		case kCodeDataType: LString::AppendPStr(string,"\p.cde",63); break;
	}
	
	::FSMakeFSSpec(sDataTypeSpec[inType].vRefNum,sDataTypeSpec[inType].parID,string,&spec);
	
	return spec;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetData						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// If the data is currently loaded then it's owners is increased, otherwise it is loaded.
Handle UDataFolderManager::GetData(
	CCString		&inName,
	EDataType		inType)
{
	if (inType==kInvalidDataType)
		UTBException::ThrowInvalidDataType();

	// Search the currently loaded resources to see if we already have it
	ArrayIndexT		count=sDatas.GetCount();
	
	for (ArrayIndexT n=1; n<=count; n++)
	{
		if ((sDatas[n].mName==inName) && (sDatas[n].mType==inType))
		{
			sDatas[n].mOwners++;
			return sDatas[n].mHandle;
		}
	}
	
	// If not, then load it in
	FSSpec			spec=MakeSpec(inName,inType);
	
	LFile			file(spec);
			
	file.OpenDataFork(fsRdPerm);

	CDataInfo		data(file.ReadDataFork(),inType,inName);
	
	sDatas.AddItem(data);
	
	file.CloseDataFork();
	
	return data.mHandle;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReleaseData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Decreases the number of owners of a piece of data, if no more people own it then it is released
void UDataFolderManager::ReleaseData(
	Handle			inHandle)
{
	// Search the currently loaded file to see if we already have it
	ArrayIndexT		count=sDatas.GetCount();
	
	for (ArrayIndexT n=1; n<=count; n++)
	{
		if (sDatas[n].mHandle==inHandle)
		{
			sDatas[n].mOwners--;
			
			if (sDatas[n].mOwners<=0)
			{
				::DisposeHandle(sDatas[n].mHandle);
				sDatas.Remove(sDatas[n]);
			}
			
			return;
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DataExists
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Tests if a data file exists or not
bool UDataFolderManager::DataExists(
	CCString		&inName,
	EDataType		inType)
{
	if (inType==kInvalidDataType)
		UTBException::ThrowInvalidDataType();
		
	// Search the currently loaded resources to see if we already have it
	ArrayIndexT		count=sDatas.GetCount();
	
	for (ArrayIndexT n=1; n<=count; n++)
	{
		if ((sDatas[n].mName==inName) && (sDatas[n].mType==inType))	
			return true;
	}
	
	// Attempt to open the file
	FSSpec		spec=MakeSpec(inName,inType);
	short		file;
	OSErr		err;
	
	if (!(err=::FSpOpenDF(&spec,fsRdPerm,&file)))
		::FSClose(file);
		
	if (err==fnfErr)
		return false;
	
	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CountData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
SInt32 UDataFolderManager::CountData(
	EDataType		inType)
{
	if (inType==kInvalidDataType)
		UTBException::ThrowInvalidDataType();
		
	CInfoPBRec		cpb;
	short			count=1;
	OSErr			err=noErr;
	CCString		name("Untitled");
	FSSpec			spec=MakeSpec(name,inType);
	
	cpb.dirInfo.ioNamePtr=spec.name;
	cpb.dirInfo.ioVRefNum=spec.vRefNum;
	
	do
	{
		cpb.dirInfo.ioFDirIndex=count;
		cpb.dirInfo.ioDrDirID=spec.parID;
		
		err=::PBGetCatInfoSync(&cpb);
		
		if (err==noErr)
			count++;
			
	} while (err==noErr);
	
	return count-1;	
}