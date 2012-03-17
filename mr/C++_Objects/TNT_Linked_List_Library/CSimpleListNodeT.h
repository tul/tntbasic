// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// CSimpleListNodeT.h
// © Mark Tully 2000
// 3/1/00
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
	A much less complex version of the CLinkedListT class. This one is single linked only. It's particularly useful for
	parsers.
	
		+	Simpler, doesn't need an "overlord" object to manage the nodes
		-	Doesn't support automatic list repair when objects are deleted
		+	Follows "classic" single linked list coding practise
*/

#pragma once

typedef unsigned long TListIndex;

template <class T>
class CSimpleListNodeT
{
	protected:
		CSimpleListNodeT<T>		*mTail;
		T						*mData;
	
	public:
								CSimpleListNodeT(
									T					*inData,
									CSimpleListNodeT<T>	*inTail=0L)
								{
									mTail=inTail;
									mData=inData;
								}
		virtual					~CSimpleListNodeT()	{}
		
		void					SetTail(
									CSimpleListNodeT<T> *inNode) { mTail=inNode; }	// be careful with this, you may want to use append instead
		
		void					Append(
									CSimpleListNodeT<T>	*inNode)
								{
									if (inNode)
									{
										CSimpleListNodeT<T>	*index=this;
										
										while (!index->IsEnd())
											index=index->Tail();
											
										index->mTail=inNode;
									}
								}
		
		TListIndex				Length()
								{
									CSimpleListNodeT<T>	*index=this;
									TListIndex			count=1;
									
									while (!index->IsEnd())
									{
										index=index->Tail();
										count++;
									}
										
									return count;
								}
		
		
		CSimpleListNodeT<T>		*Tail()
								{
									return mTail;
								}
		
		T						*TailData()
								{
									return mTail ? mTail->Head() : 0L;
								}
								
		T						*Head()
								{
									return mData;
								}
		
		bool					IsEnd()
								{
									return mTail==0;
								}
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Cons
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The class hugs style list building constructor. This is used in may places where x* needs to be parsed
// Takes the head and appends tail onto it. Head may be nil in which case the result is tail.
// Tail may also be nil in which case the result is head.
// If both are nil then the result is too nil.
template <class T>
CSimpleListNodeT<T> *Cons(
	CSimpleListNodeT<T>	*inX,
	CSimpleListNodeT<T>	*inXs)
{
	if (inX)
	{
		inX->Append(inXs);
		return inX;
	}
	else
		return inXs;		/* Note : inXs can be nil as well */
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ConsData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Variation of the above which returns the mData of the resultant list instead of the node itself
template <class T>
T *ConsData(
	CSimpleListNodeT<T>	*inX,
	CSimpleListNodeT<T>	*inXs)
{
	CSimpleListNodeT<T>	*node=Cons<T>(inX,inXs);
	
	return node ? node->Head() : 0L;
}