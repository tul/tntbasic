// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// UPersistanceMgr.cpp
// TNT Library
// Copyright Mark Tully and TNT Software 2000
// 7/8/00
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

#include	"UPersistanceMgr.h"
#include	"LStream.h"
#include	"Endian.h"
#include	"TNT Errors.h"
#include	<cstring>

CDeletingLinkedListT<SPersistorCreatorEntry>	UPersistanceMgr::sCreators;

// Note about endian-ness
// ----------------------
// Normally a different implementation of LStream is used on a LE machine which converts the stream's contents from BE to LE on the fly.
// (This is done based on the assumption that the LStream's contents is all in BE)
// Because we know the id is in BE format and we want it in BE format (even if this machine is LE) we read the raw data rather than
// using a >> operator which would automatically convert the BE to LE. To do this we use ReadBlock and WriteBlock rather than >> and <<


// the parent is used only for directly constructed persistors, ie members of other objects.
// It allows a parent object to be aware what it's direct members are and write them out
// when it's flattened.
// In general don't specify a parent for objects created via "new" as they can't be reanimated
// or saved automatically (unless you use the CPersistPtrT class which I ain't wrote yet but
// just thought of....)
CPersistor::CPersistor(
    CPersistor *inParent,
    TPersistorId inId) :
    mPersistLink(this),
    mPersistorId(inId)
{
    if (inParent)
		inParent->mSubPersists.AppendElement(&mPersistLink);
}

/*e*/
void CPersistor::FlattenToStream(
    LStream &outStream)
{
//    outStream << mPersistorId;
  	outStream.WriteBlock(&mPersistorId,sizeof(mPersistorId));
   
    CListIndexerT<CPersistor>	indexer(&mSubPersists);
    
    while (CPersistor *per=indexer.GetNextData())
	per->FlattenToStream(outStream);

    FlattenToStreamSelf(outStream);
}

/*e*/
void CPersistor::AssignFromStream(
    LStream	&inStream)
{
//    inStream >> mPersistorId;
	inStream.ReadBlock(&mPersistorId,sizeof(mPersistorId));

    CListIndexerT<CPersistor> indexer(&mSubPersists);

    while (CPersistor *p=indexer.GetNextData())
	p->AssignFromStream(inStream);

    AssignFromStreamSelf(inStream);
}

/* Static e */
CPersistor *UPersistanceMgr::ReanimateObjectFromStream(
    CPersistor *inParent,
    LStream &inStream)
{
    TPersistorId	id=0;

//	inStream >> id;		
	inStream.ReadBlock(&id,sizeof(id));
    
    inStream.SetMarker(-sizeof(TPersistorId),streamFrom_Marker);
    
    TPersistorCreator	creator=LookupCreator(id);
    
    if (!creator)
    {
		char string[50];

		std::strcpy(string,"Reanimation failed for class 'xxxx'");

		std::memcpy(&string[30],&id,sizeof(id));

		SignalString_(string);
		Throw_(kTNTErr_UnregisteredPersistorClass);
    }

    CPersistor *p=creator(inParent);
    ThrowIfMemFull_(p);

    p->AssignFromStream(inStream);

    return p;
}

/* Static */
TPersistorCreator UPersistanceMgr::LookupCreator(
    TPersistorId	inId)
{
    CListIndexerT<SPersistorCreatorEntry>	indexer(&sCreators);
    
    while (SPersistorCreatorEntry *pe=indexer.GetNextData())
    {
		if (pe->id==inId)
		    return pe->creator;
    }
    
    return 0;
}

/* Static e */
void UPersistanceMgr::RegisterCreator(
    TPersistorId		inId,
    TPersistorCreator	inCreator,
    bool				inAllowOverride)
{
	if ((!inAllowOverride) && LookupCreator(inId))
	{
		char string[50];

		std::strcpy(string,"Duplicate reanimator with id 'xxxx'");

		std::memcpy(&string[30],&inId,sizeof(inId));

		SignalString_(string);

		Throw_(kTNTErr_DuplicateReanimator);
	}
	
    SPersistorCreatorEntry	*pe=new SPersistorCreatorEntry(inId,inCreator);
    ThrowIfMemFull_(pe);
    
    sCreators.LinkElement(pe);
}