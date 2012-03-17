// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Linked List Library
// ULinkedListUtils.h
// Mark Tully
// 9/12/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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
	Contains common utility functions - most of which can't be directly implemented into the CLinkedListT class as it would
	cause restrictions eg comparision of lists REQUIRES an operator == overload for *T.
*/

#include "CLinkedListT.h"
#include "CBitArray.h"

namespace ULinkedListUtils
{

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CompareByDataContentOrder						/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns true if the lists are exactly the same by a content comparison of each element. Assumes the lists are sorted.
// Comparison is a lot faster when lists are sorted. Marked as throwing to reserve the right of the == to throw but it
// probably won't.
template <class T>
bool CompareByDataContentOrder(
	const CLinkedListT<T>			*inListA,
	const CLinkedListT<T>			*inListB)
{
	if (inListA==inListB)
		return true;
	
	if (inListA->CountElements()!=inListB->CountElements())
		return false;
		
	CListIndexerT<T>				indexer1((CLinkedListT<T>*)inListA),indexer2((CLinkedListT<T>*)inListB);
	T								*dataA=0,*dataB=0;
	
	while ((dataA=indexer1.GetNextData()) && (dataB=indexer2.GetNextData()))
	{
		if (!(*dataA==*dataB))
			return false;
	}
	
	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CompareByDataContentNoOrder					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns true if lists contain the same elements. Relies on the existance of an == operator for element's data and also
// a small amount of working memory. As the lists are unordered and can contain duplicate elements this working memory is
// used to track which elements have been taken.
// Empty lists are trivially equal.
template <class T>
bool CompareByDataContentNoOrder(
	const CLinkedListT<T>			*inListA,
	const CLinkedListT<T>			*inListB)
{
	if (inListA==inListB)
		return true;
	
	TListIndex						count=inListA->CountElements();
	
	if (inListB->CountElements()!=count)
		return false;
	
	if (count)
	{
		CBitArray					matchedElements(count);
		
		CListIndexerT<T>			indexer((CLinkedListT<T>*)inListA);
		
		while (T *data=indexer.GetNextData())				// for each data - look for an unmatched equivalent in the other list
		{
			CListIndexerT<T>		indexer2((CLinkedListT<T>*)inListB);
			TListIndex				matchIndex=0;
			
			// look for a match which is untaken
			while (T *data2=indexer2.GetNextData())
			{
				if (*data==*data2)							// match?
				{
					if (!matchedElements.Test(matchIndex))
					{
						// not taken
						matchedElements.Set(matchIndex);		// make it taken
						break;								// break search for match
					}
				}
				
				matchIndex++;
			}
			
			if (matchIndex==count)		// search was exhaustive and uncessful
				return false;			// lists therefore don't match
		}
	}

	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CompareByDataAddressNoOrder					/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns true if lists contain the same elements. As the lists are unordered and can contain duplicate elements this
// working memory is used to track which elements have been taken.
// Empty lists are trivially equal.
template <class T>
bool CompareByDataAddressNoOrder(
	const CLinkedListT<T>			*inListA,
	const CLinkedListT<T>			*inListB)
{
	if (inListA==inListB)
		return true;
	
	TListIndex						count=inListA->CountElements();
	
	if (inListB->CountElements()!=count)
		return false;
	
	if (count)
	{
		CBitArray					matchedElements(count);
		
		CListIndexerT<T>			indexer((CLinkedListT<T>*)inListA);
		
		while (T *data=indexer.GetNextData())				// for each data - look for an unmatched equivalent in the other list
		{
			CListIndexerT<T>		indexer2((CLinkedListT<T>*)inListB);
			TListIndex				matchIndex=0;
			
			// look for a match which is untaken
			while (T *data2=indexer2.GetNextData())
			{
				if (data==data2)							// match?
				{
					if (!matchedElements.Test(matchIndex))
					{
						// not taken
						matchedElements.Set(matchIndex);		// make it taken
						break;								// break search for match
					}
				}
				
				matchIndex++;
			}
			
			if (matchIndex==count)		// search was exhaustive and uncessful
				return false;			// lists therefore don't match
		}
	}

	return true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CompareByDataAddressOrder
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns true if the lists are exactly the same by an address comparison of each element. Assumes the lists are sorted.
// Comparison is a lot faster when lists are sorted.
template <class T>
bool CompareByDataAddressOrder(
	const CLinkedListT<T>			*inListA,
	const CLinkedListT<T>			*inListB)
{
	if (inListA==inListB)
		return true;
	
	if (inListA->CountElements()!=inListB->CountElements())
		return false;
		
	CListIndexerT<T>				indexer1((CLinkedListT<T>*)inListA),indexer2((CLinkedListT<T>*)inListB);
	T								*dataA=0,*dataB=0;
	
	while ((dataA=indexer1.GetNextData()) && (dataB=indexer2.GetNextData()))
	{
		if (dataA!=dataB)
			return false;
	}
	
	return true;
}

}