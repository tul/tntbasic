// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Data Store Library
// CDataStoreSection.cpp
// © Mark Tully and TNT Software 1999
// 28/8/99
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
	A data store section defines a range in a data store. This section can have streams attached to it to read/write it's
	contents. Multiple streams can connect to the same section.
	Sections cannot overlap in the data store unless the section is a child or parent of the section.
	Sections are organised in a hierarchy with the section at the top being known as the root section. There is only one
	of these per markable data store.
*/

#include			"TNT_Debugging.h"
#include			"CDataStoreSection.h"
#include			"CMarkableDataStore.h"
#include			<TArrayIterator.h>

class CDataSectionSorter : public LAttachment
{
	protected:
		CDataStoreSection					*mRootSection;

		virtual void						ExecuteSelf(
												MessageT				inMessage,
												void					*ioParam);

		virtual EMarkerOrder				ResolveSectionAmbiguity(
												CDataStoreSectionMarker	*inMarkerA,
												CDataStoreSectionMarker	*inMarkerB);

		virtual EMarkerOrder				ResolveContainerAmbiguity(
												CDataStoreSectionMarker	*inMarkerA,
												CDataStoreSectionMarker	*inMarkerB);

	public:
											CDataSectionSorter(
												CDataStoreSection		*inRootSection)	: mRootSection(inRootSection) {}

		static CDataSectionSorter 			*GetSectionSorter(
												CMarkableDataStore		*inStore);
};	

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ResolveSectionAmbiguity
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Takes two markers which are in the same section and decides what order they should be in.
// Default ordering is if they're the same class (including custom classes) then treat them as equal.
// Then other markers are ordered by start <= custom marker classes <= end.
// If both are custom markers then the result is "unknown".
EMarkerOrder CDataSectionSorter::ResolveSectionAmbiguity(
	CDataStoreSectionMarker			*inMarkerA,
	CDataStoreSectionMarker			*inMarkerB)
{
	EMarkerOrder		result=kUnknown;

	if (inMarkerA->GetMarkerClass()==inMarkerB->GetMarkerClass())
		result=kEqual;
	else if (inMarkerA->GetMarkerClass()==CDataStoreSectionMarker::kStartMarkerClass)
		result=kABeforeB;
	else if (inMarkerA->GetMarkerClass()==CDataStoreSectionMarker::kEndMarkerClass)
		result=kAAfterB;
	else	// inMarkerA is a custom marker
	{
		switch (inMarkerB->GetMarkerClass())
		{
			case CDataStoreSectionMarker::kStartMarkerClass:
				result=kAAfterB;
				break;
			
			case CDataStoreSectionMarker::kEndMarkerClass:
				result=kABeforeB;
				break;
		}
	}
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ResolveContainerAmbiguity
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Each marker belongs to a different section. One section is inside the other section in the hierarchy. This means that that
// sections markers should ordered to be inside the container sections markers.
// It gets the sections from the markers and by using the depths figures out which one must be inside which then orders
// the markers appropiately.
EMarkerOrder CDataSectionSorter::ResolveContainerAmbiguity(
	CDataStoreSectionMarker	*inMarkerA,
	CDataStoreSectionMarker	*inMarkerB)
{
	CDataStoreSection	*sectionA=inMarkerA->GetSection();
	CDataStoreSection	*sectionB=inMarkerB->GetSection();
	EMarkerOrder		result=kUnknown;

	// The code after this orders markerA to lie inside of markerB's range. If that's incorrect then swap the marker ptrs
	if (sectionA->GetHierarchicalDepth()<sectionB->GetHierarchicalDepth())
	{
		CDataStoreSectionMarker		*temp=inMarkerA;
		
		inMarkerA=inMarkerB;
		inMarkerB=temp;
	}

	// Make markerA lie inside of markerB's section.
	switch (inMarkerB->GetMarkerClass())
	{
		case CDataStoreSectionMarker::kStartMarkerClass:
			result=kAAfterB;
			break;
			
		case CDataStoreSectionMarker::kEndMarkerClass:
			result=kABeforeB;
			break;

		default:
			// inMarkerB is a custom marker in parent section. Put it before a start marker in the child section or after
			// an end marker. This is because it is likely to be referencing data outside of the child sections range.
			switch (inMarkerA->GetMarkerClass())
			{
				case CDataStoreSectionMarker::kStartMarkerClass:
					result=kAAfterB;
					break;
					
				case CDataStoreSectionMarker::kEndMarkerClass:
					result=kABeforeB;
					break;

				default:
					// Both markers are custom markers, we can apply no ordering to them as we don't understand them.
					// Return unknown and another marker sorting attachment might take up the challenge.
					result=kUnknown;
					break;
			}
			break;
	}
	
	return result;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ExecuteSelf
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This is an implementation of a marker sorting attachment for resolving ambiguities when several section markers point to
// the same address. It figures out what type of ambiguity it is and calls the appropiate resolve method.
void CDataSectionSorter::ExecuteSelf(
	MessageT		inMessage,
	void*			ioParam)
{
	SMarkerSortRequest	*request=(SMarkerSortRequest*)ioParam;
	
	if (request->result==kUnknown)
	{
		// Cast the two markers to section markers if possible
		CDataStoreSectionMarker		*markerA=dynamic_cast<CDataStoreSectionMarker*>(request->markerA);
		CDataStoreSectionMarker		*markerB=dynamic_cast<CDataStoreSectionMarker*>(request->markerB);
		
		if (!markerA || !markerB)
			return;		// Can't solve ambiguity
			
		CDataStoreSection		*sectionA=markerA->GetSection();
		CDataStoreSection		*sectionB=markerB->GetSection();

		// Marking the same sections?
		if (sectionA==sectionB)
		{
			// Resolve using standard section marker disambiguator
			request->result=ResolveSectionAmbiguity(markerA,markerB);
		}
		else
		{	
			// We must find the common parent section of these two sections, the junction in their relative hierarchical
			// positions. That junction section can then resolve the ambiguity of it's child sections.
			UInt16					aDepth=sectionA->GetHierarchicalDepth();
			UInt16					bDepth=sectionB->GetHierarchicalDepth();
			CDataStoreSection		*aParent=sectionA;
			CDataStoreSection		*bParent=sectionB;
			
			while (aDepth>bDepth)
			{
				aParent=aParent->GetParentSection();
				aDepth--;
			}

			while (bDepth>aDepth)
			{
				bParent=bParent->GetParentSection();
				bDepth--;
			}

			// Same depth in the hierarchy now, move both up the hierarchy together until they have a common parent
			while (bParent->GetParentSection()!=aParent->GetParentSection())
			{
				aParent=aParent->GetParentSection();
				bParent=bParent->GetParentSection();
			}
			
			// The junction of the two elements is in the parent. First check to see if one section was a sub section of the other.
			if (aParent==bParent)		// The two elements became equal when the depths were balanced, that means one was inside
			{							// the other, but which way? Deeper one is the sub section.
				request->result=ResolveContainerAmbiguity(markerA,markerB);
			}
			else
			{
				if (!aParent->GetParentSection())	// Parents are equal but the sections are diffrent. If they both have a
				{									// null parent then there's multiple root sections which isn't allowed.
					SignalPStr_("\pMultiple root sections detected in marker disambiguator.\nUse only one root section per data store.");
				}
				else								// Get the common parent to resolve the ambiguity
					request->result=aParent->OrderSubSections(aParent,bParent);
			}
		}
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetSectionSorter									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Searches the passed data store for a section sorter attachment. If there's none it returns 0L.
CDataSectionSorter *CDataSectionSorter::GetSectionSorter(
	CMarkableDataStore		*inStore)
{
	TArray<LAttachment*>*			attachments=inStore->GetAttachmentsList();
	CDataSectionSorter			*sorter=0L;

	if (attachments)
	{
		TArrayIterator<LAttachment*>	iterator(*attachments);
		LAttachment						*data;
	
		while (iterator.Next(data))
		{
			if (sorter=dynamic_cast<CDataSectionSorter*>(data))
				return sorter;
		}
	}
	
	return sorter;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// The section marker doesn't take a reference to the section or the data store. You must set it after construction is
// complete using Set. If it was done from the constructor then the marker sorting routine would try and sort an half
// constructed marker and fail.
CDataStoreSectionMarker::CDataStoreSectionMarker(
	EMarkerClass					inClass)
{
	mMarkerClass=inClass;
	mSection=0L;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Set
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// A replacement of the standard marker set function which takes a section instead of a data store. Null sections are
// acceptable.
void CDataStoreSectionMarker::Set(
	CDataStoreSection				*inSection,
	CDataStore::TDataOffset			inOffset)
{
	mSection=inSection;			// Assign section first so that when connecting the marker the section it belongs to can be found
	Set(inSection ? inSection->GetDataStore() : 0L,inOffset);
}
										
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor												/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This constructor makes this the root section for the store.
// This is the only way of setting the data store so it would be silly to pass a null value.
// If an accessor was added to set the data store it would have to index though all the sub sections and change their
// data stores too, as this level of abstraction doesn't record the section sections that is not possible.
// Alternatively it could delete all the sub sections and then change stream, but again, it doesn't know about sub sections.
CDataStoreSection::CDataStoreSection(
	CMarkableDataStore		*inStore,
	CDataStore::TDataOffset	inStartOffset,
	CDataStore::TDataOffset	inLength) : mStart(CDataStoreSectionMarker::kStartMarkerClass), mEnd(CDataStoreSectionMarker::kEndMarkerClass)
{
	mDataStore=inStore;
	mLocked=false;

	// Set the start marker and end marker
	mStart.Set(this,inStartOffset);
	mEnd.Set(this,inStartOffset+inLength);

	// Add the section sorted attachment ot the store
	if (CDataSectionSorter::GetSectionSorter(inStore)==0L)
	{
		CDataSectionSorter		*sorter=new CDataSectionSorter(this);
		ThrowIfMemFull_(sorter);
		
		inStore->AddAttachment(sorter,0L,false);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// This constructor takes a section in which this sub section should exist. The inStartOffset is measured from the beginning
// of the data store NOT the beginning of the parent section. The range of this section should lie within the parent section's
// range otherwise it breaks the concepts behind the hierarchy.
CDataStoreSection::CDataStoreSection(
	CDataStoreSection		*inParent,
	CDataStore::TDataOffset	inStartOffset,
	CDataStore::TDataOffset	inLength) : mStart(CDataStoreSectionMarker::kStartMarkerClass), mEnd(CDataStoreSectionMarker::kEndMarkerClass)
{
	mLocked=false;

	if (inParent)
		mDataStore=inParent->GetDataStore();
	else
		mDataStore=0L;
		
	mParentSection=inParent;

	// Set the start marker and end marker
	mStart.Set(this,inStartOffset);
	mEnd.Set(this,inStartOffset+inLength);	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Deletes the section sorter if this was the root section.
CDataStoreSection::~CDataStoreSection()
{
	if (!GetParentSection())
	{
		CDataSectionSorter		*sorter=CDataSectionSorter::GetSectionSorter(GetDataStore());
	
		if (sorter)
			GetDataStore()->RemoveAttachment(sorter);
		
		delete sorter;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetHierarchicalDepth
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the distance from this section to the top of the hierarchy. If this is the root section then the result is 0.
// If it's inside the root section it's depth is 1, and so on.
UInt32 CDataStoreSection::GetHierarchicalDepth()
{
	UInt32				depth=0;

	for (CDataStoreSection *parent=GetParentSection(); parent; parent=parent->GetParentSection())
		depth++;

	return depth;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ OrderSubSections
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Called to order two immediate sub sections of this section. You should override this to order your sub sections if you're
// to support them. Failiure to sort sub sections properly can result in markers losing track of their data when sections
// are resized. If a marker loses track of it's data it means that a write to that marker will corrupt your data.
EMarkerOrder CDataStoreSection::OrderSubSections(
	CDataStoreSection		*inSectionA,
	CDataStoreSection		*inSectionB)
{
	// The default implementation has a go at ordering the sections sensibly, but it fails if both sections are 0 in length.
	// If you can guarantee that you will never have two empty sections next to each other then this implementation will do
	// you just fine.
	if (inSectionA->GetStartOffset()<(inSectionB->GetStartOffset()+inSectionB->GetLength()))
		return kABeforeB;
	else if ((inSectionA->GetStartOffset()+inSectionA->GetLength())>(inSectionB->GetStartOffset()))
		return kAAfterB;
	else
	{
		// Both of the sections have a null length - ambiguous result.
		return kUnknown;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetStartOffset								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Return the offset from the beginning of the data store of this stream.
CDataStore::TDataOffset CDataStoreSection::GetStartOffset()
{
	return mStart;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetLength
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CDataStore::TDataOffset CDataStoreSection::GetLength() const
{
	return mEnd.GetMarker()-mStart.GetMarker();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetLength										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resizes the section.
void CDataStoreSection::SetLength(
	CDataStore::TDataOffset	inLength)
{
	mStart.ResizeFromMarker(GetLength(),inLength);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ NoMoreUsers
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Only deletes itself if it's not locked
void CDataStoreSection::NoMoreUsers()
{
	if (mLocked==false)
		LSharable::NoMoreUsers();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetLock
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// When locked, the data section doens't get deleted when it's users reach zero. This is useful to keep the data sections
// around if they're frequently accessed forinstance. If you create the data store on the stack, make sure you lock it
// otherwise it may autodelete which would be bad.
// Setting it to false can cause auto deletion!
void CDataStoreSection::SetLock(
	bool					inLocked)
{
	if (inLocked!=mLocked)
	{
		mLocked=inLocked;
		if ((!inLocked) && (GetUseCount()==0))
			NoMoreUsers();
	}
}
