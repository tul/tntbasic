// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CResource.h
// © Mark Tully and TNT Software 2003 -- All Rights Reserved
// 16/2/03
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

#include "CListElementT.h"
#include "CCString.h"
#include "ResourceTypes.h"

class CResourceContainer;
class CMacResFile;

class CResource : public CListElementT<CResource>
{
	friend class CResourceContainer;
	friend class CMacResFile;
	friend class CBundleResFile;
	
	protected:
		enum
		{
			kRFlag_NeedsFlush	=	(1<<0),		// resource has changes awaiting flush to disk
			kRFlag_ReadOnly		=	(1<<1)		// resource is read only
		};
		CResourceContainer			*mContainer;// parent container
		SInt32						mUsers;		// reference count of num users - when it hits 0 the res is unloaded
		ResType						mType;		// resource type - classic 4 byte id, shorter and easier to manage than a folder name and just as unique
		TResId						mId;		// resource id
		TResName					mName;		// name of loaded resource - unique within resource type
		TResourceReference			mRef;		// unique opaque reference valid for this session only.
		UInt32						mFlags;		// kRFlag_*

		virtual OSStatus			PutResourceDataSelf(
										UInt32		inWriteOffset,
										const void	*inData,
										UInt32		&ioByteCount) = 0;
		virtual OSStatus			GetResourceDataSelf(
										UInt32		inReadOffset,
										void		*outBuffer,
										UInt32		&ioByteCount) = 0;
		virtual void /*e*/			SetSizeSelf(
										UInt32		inNewSize) = 0;
		virtual void /*e*/			FlushSelf() = 0;		
		virtual void /*e*/			Flush();		// generally flush the entire res container rather than an individual resource. flushing of indiviual resources without flushing the entire container may cause odd results.
		
		virtual void /*e*/			UpdateInfo(
										ResType				inNewType,
										TResId				inNewId,
										const TResName		inNewName);

	public:
									CResource(
										CResourceContainer	*inContainer,
										ResType				inType,
										TResId				inId,
										const TResName		inName);
		virtual						~CResource();
				
		inline const TResName		&GetName()		{ return mName; }
		inline ResType				GetType()		{ return mType; }
		inline TResId				GetId()			{ return mId; }
		inline TResourceReference	GetRef()		{ return mRef; }
		virtual UInt32				GetSize() = 0;
		virtual void /*e*/			SetSize(
										UInt32		inSize);
		inline bool					NeedsFlush()	{ return (mFlags&kRFlag_NeedsFlush)!=0; }
		inline bool					IsReadOnly()	{ return (mFlags&kRFlag_ReadOnly)!=0; }
		
		inline void					Retain()		{ mUsers++; }
		inline SInt32				GetUsage()		{ return mUsers; }
		virtual bool				Release();
		
		virtual OSStatus			PutResourceData(
										UInt32		inWriteOffset,
										const void	*inData,
										UInt32		&ioByteCount);
		virtual OSStatus			GetResourceData(
										UInt32		inReadOffset,
										void		*outBuffer,
										UInt32		&ioByteCount);
		virtual Handle				GetReadOnlyDataHandle() = 0;		// returns a handled owned by this resource containing the resource data
};
