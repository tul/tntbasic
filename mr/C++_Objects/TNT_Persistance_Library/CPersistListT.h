// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CPersistListT.h
// TNT Library
// Copyright Mark Tully and TNT Software 2000
// 18/8/00
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
#include "UPersistanceMgr.h"
#include "Endian.h"
#include <LStream.h>

template <class T>
class CPersistListT : public CPersistor, public CLinkedListT<T>
{
	protected:
	    virtual void ClearPersistList()
							{	     
							    DeleteAllLinks();
							}

	    virtual void   FlattenToStreamSelf(
						    LStream &outStream)
						    {
						        SInt32 count=CountElements();

						        /* HToBE32(count); Should be done automatically now */

								outStream << count;

								CListIndexerT<T> indexer(this);

								while (T *p=indexer.GetNextData())
								    p->FlattenToStream(outStream);
						    }

	    virtual void   AssignFromStreamSelf(
							LStream &inStream)
							{
							    ClearPersistList();

							    SInt32 count;

							    inStream >> count;

							    /* BEToH32(count); Should be done automatically now */

							    while (count--)
							    {
									T *p=dynamic_cast<T*>(UPersistanceMgr::ReanimateObjectFromStream(0L,inStream));
									AppendElement(p);									
							    }
							}

public:
    enum { kPersistorId = FOUR_CHAR_CODE('List') };

    CPersistListT(
		CPersistor *inParent,
		TPersistorId inId=kPersistorId) :
		CPersistor(inParent,inId)
		{}
		
	virtual ~CPersistListT() { ClearPersistList(); }
};