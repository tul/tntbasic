// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// UPersistanceMgr.h
// TNT Library
// © Mark Tully and TNT Software 2000
// 14/8/00
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

#pragma once

#include "CLinkedListT.h"
class LStream;

typedef SInt32 TPersistorId;

class CPersistor
{
	friend class UPersistanceMgr;

	protected:
		CListElementT<CPersistor>	mPersistLink;	// These are used for maintaining a direct member hierarchy
		CLinkedListT<CPersistor>	mSubPersists;	// you shouldn't use them for anything else or mess with them!
		TPersistorId				mPersistorId;

		virtual void /*e*/			FlattenToStreamSelf(
										LStream &outStream) {}
		virtual void /*e*/			AssignFromStreamSelf(
										LStream &inStream) {}	
	
	public:
		/*e*/						CPersistor(
										CPersistor *inParent,
										TPersistorId inid);

		TPersistorId				GetPersistorId() const		{ return mPersistorId; }
										
		virtual void /*e*/			FlattenToStream(
										LStream &outStream);
		virtual void /*e*/			AssignFromStream(
										LStream &inStream);
};

typedef CPersistor *(*TPersistorCreator)(CPersistor *inParent);

struct SPersistorCreatorEntry : public CListElementT<SPersistorCreatorEntry>
{
	SPersistorCreatorEntry(
		TPersistorId		inId,
		TPersistorCreator	inCreator) :
		id(inId),
		creator(inCreator),
		CListElementT<SPersistorCreatorEntry>(this)
		{}

	TPersistorId		id;
	TPersistorCreator	creator;
};

template <class T>
class CPersistorCreatorT
{
	public:
		static CPersistor *Create(
								CPersistor *inParent)
								{
									CPersistor *p=new T(inParent);
									ThrowIfMemFull_(p);
									
									return p;
								}
};

#define RegisterReanimator_(x) UPersistanceMgr::RegisterCreator(x::kPersistorId,CPersistorCreatorT<x>::Create)

class UPersistanceMgr
{
	protected:
		static CDeletingLinkedListT<SPersistorCreatorEntry>	sCreators;


		static TPersistorCreator	LookupCreator(
									    TPersistorId	inId);

	public:
		static CPersistor /*e*/		*ReanimateObjectFromStream(
				    					CPersistor			*inParent,
									    LStream				&inStream);
		static void /*e*/			RegisterCreator(
									    TPersistorId		inId,
									    TPersistorCreator	inCreator,
									    bool				inAllowOverride=false);
					    
};