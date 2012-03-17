// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UResources.h
// © Mark Tully and TNT Software 2003 -- All Rights Reserved
// 23/2/03
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2003, Mark Tully and John Treece-Birch
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

#include "ResourceTypes.h"

class UResources
{
	public:
		static void /*e*/	ThrowResError(
								SInt32				inErr,
								ResType				inType,
								TResId				inId);
		
		static void /*e*/	ThrowResLoadFailed(
								SInt32				inErr,
								ResType				inType,
								TResId				inId);

		static void /*e*/	ThrowResAddError(
								SInt32				inErr,
								ResType				inType,
								TResId				inId);
};

// helper class for parsing resource names for the form 'resid - res name'
// eg "128 - Untitled resource"
class CResourceName
{
	protected:
		ConstStringPtr	mName;
		bool			mResIdAvailable;
		TResId			mResId;
		SInt16			mResNameIndex;		// index into mName where real res name begins. Set to 1 for beginning
		
		void			Split();		
		
	public:
						CResourceName() :
							mName(NULL),
							mResId(kNullResId),
							mResIdAvailable(false)
							{
							}
							
						CResourceName(
							const TResName inName) :
							mName(inName),
							mResId(kNullResId),
							mResIdAvailable(false)
							{
								Split();
							}
		
		void			Assign(
							const TResName	inName)
						{
							mName=inName;
							mResIdAvailable=false;
							mResId=kNullResId;
							Split();
						}
		
		bool			ResIdAvailable()			{ return mResIdAvailable; }		
		TResId			ResId()						{ return mResId; }
		StringPtr		ResName(
							TResName	outName)
						{
							outName[0]=mName[0]-(mResNameIndex-1);
							::BlockMoveData(&mName[mResNameIndex],&outName[1],outName[0]);
							return outName;
						}
		
		static void		MakeName(
							TResId			inResId,
							ConstStringPtr	inResName,
							TResName		outName);
		static SInt32	CompareNames(
							const TResName	inName1,
							const TResName	inName2);
};

class CResource;
class CResourceContainer;

class StTNTResource
{
	protected:
		TResourceReference	mResource;	
	public:
						StTNTResource(
							CResourceContainer		*inContainer,
							ResType					inType,
							TResId					inId);
						StTNTResource(
							TResourceReference		inRef=kResourceReference_Unregistered);
						~StTNTResource();
		
		CResource /*e*/	*operator->() const;
		CResource /*e*/	*Resolve() const;
		void			Assign(
							TResourceReference		inRef);		
		bool			IsNull()	{ return mResource==kResourceReference_Unregistered; }
};