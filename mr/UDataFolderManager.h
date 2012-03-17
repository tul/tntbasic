// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// UDataFolderManager.h
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

#pragma once

#include	"CCString.h"

enum EDataType
{
	kInvalidDataType=-1,
	kMusicDataType=0,
	kInputDataType,
	kImageBankDataType,
	kSoundBankDataType,
	kImageDataType,
	kImageMaskDataType,
	kSoundDataType,
	kMapDataType,
	kCodeDataType,
	
	kDataTypes,
};

class CDataInfo
{
	public:
		Handle			mHandle;
		CCString		mName;
		EDataType		mType;
		UInt16			mOwners;
		
						CDataInfo(
							Handle			inHandle,
							EDataType		inType,
							CCString		&inName)
						{
							mOwners=1;
							mType=inType;
							mHandle=inHandle;
							mName=inName;
						}
};

class UDataFolderManager
{
	private:
		static TArray<CDataInfo>		sDatas;
		static FSSpec					sDataSpec;
		static FSSpec					sDataTypeSpec[kDataTypes];
	
	public:
		static void						Initialise(
											FSSpec			&inDataFolderSpec);
		static void						ShutDown();

		static Handle /*e*/				GetData(
											CCString		&inName,
											EDataType		inType);
		
		static void						ReleaseData(
											Handle			inHandle);
											
		static bool						DataExists(
											CCString		&inName,
											EDataType		inType);
		
		static SInt32					CountData(
											EDataType		inType);
	
		static FSSpec					MakeSpec(
											CCString		&inName,
											EDataType		inType);
};