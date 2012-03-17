// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TLVDragNDropTrait.h
// Mark Tully
// 10/4/98
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1998, Mark Tully and John Treece-Birch
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

#include										"Utility Objects.h"

template <class T>
class TLVDragNDropTrait : public T
{
	private:
		Boolean									hilightOn;
		Boolean									dragAcceptable;

		static DragTrackingHandlerUPP			trackingHandlerUPP;
		static DragReceiveHandlerUPP			receiveHandlerUPP;
	
		void /*e*/								Init();
		static pascal OSErr						TrackingHandler(DragTrackingMessage theMessage,WindowPtr theWindow,void *refCon,DragReference theDrag);
		static pascal OSErr						ReceiveHandler(WindowPtr theWindow, void *refCon,DragReference theDrag);
	
	protected:
		/*e*/									TLVDragNDropTrait(WindowPtr windowPtr,const Rect &listViewRect,TLinkedList *listToUse,void *refCon=0L) : T(windowPtr,listViewRect,listToUse,refCon) { Init(); }
		virtual Boolean							IsDragAcceptable(DragReference drag);
		virtual Boolean							IsItemAcceptable(DragReference drag,ItemReference itemRef)=0L;
		virtual RgnHandle						GetHilightRgn();
		virtual Boolean							IsPointInDropRgn(Point localPoint);
		virtual void /*e*/						ReceiveDragItem(DragReference drag,ItemReference theItem)=0L;
	
		virtual void							DragReceiptPostProcess()					{}
		virtual void							DragReceiptPreProcess(short maxItems)		{}
		virtual void							StartDrag(EventRecord &event);
		virtual void /*e*/						AddItemToDrag(DragReference drag,TListViewCellPtr cellPtr)=0L;
	
	public:
		/*e*/									TLVDragNDropTrait(WindowPtr windowPtr,const Rect &listViewRect,void *refCon=0L) : T(windowPtr,listViewRect,refCon) { Init(); }
		virtual									~TLVDragNDropTrait();

		virtual T::TClickResult					HandleMouseClick(EventRecord &event);

};

template <class T> DragTrackingHandlerUPP		TLVDragNDropTrait<T>::trackingHandlerUPP=0L;
template <class T> DragReceiveHandlerUPP		TLVDragNDropTrait<T>::receiveHandlerUPP=0L;

/*e*/
template <class T>
void TLVDragNDropTrait<T>::Init()
{
	hilightOn=false;
	dragAcceptable=false;
	
	// if the upps aren't allocated then allocate them now. They can't get disposed of but the system will free them at
	// ExitToShell. I think this is better than having either seperate copies of the UPPs for all d&d traits or having to
	// use a KillGlobals call.
	if (!trackingHandlerUPP)
		ThrowIfMemFull_(trackingHandlerUPP=NewDragTrackingHandlerUPP(TrackingHandler));
	if (!receiveHandlerUPP)
		ThrowIfMemFull_(receiveHandlerUPP=NewDragReceiveHandlerUPP(ReceiveHandler));

	// Now add the handlers to this window	
	ThrowIfOSErr_(InstallTrackingHandler(trackingHandlerUPP,GetDrawPort(),this));
	ThrowIfOSErr_(InstallReceiveHandler(receiveHandlerUPP,GetDrawPort(),this));
}

template <class T>
TLVDragNDropTrait<T>::~TLVDragNDropTrait()
{
	RemoveTrackingHandler(trackingHandlerUPP,GetDrawPort());
	RemoveReceiveHandler(receiveHandlerUPP,GetDrawPort());
}

// ---- Static
template <class T>
pascal OSErr TLVDragNDropTrait<T>::TrackingHandler(DragTrackingMessage theMessage,WindowPtr theWindow,void *refCon,DragReference theDrag)
{
	UPortSaver				port(theWindow);
	UForeColourSaver		safe1(TColourPresets::kBlack);
	UBackColourSaver		safe2(TColourPresets::kWhite);
	TLVDragNDropTrait<T>	*trait=(TLVDragNDropTrait<T>*)refCon;
	DragAttributes			attributes;
	
	GetDragAttributes(theDrag,&attributes);

	switch (theMessage)
	{
		case kDragTrackingEnterWindow:
			// This is the first time that the drag has entered the window, see if it's acceptable
			trait->dragAcceptable=trait->IsDragAcceptable(theDrag);
			trait->hilightOn=false;
			break;
		
		case kDragTrackingInWindow:
			// The user is holding the drag in our window, see if we need to hilight or anything
			if (!trait->dragAcceptable)
				break;
						
			if (attributes & kDragHasLeftSenderWindow)
			{
				Point		localMouse;

				GetDragMouse(theDrag,&localMouse,0L);
				GlobalToLocal(&localMouse);

				if (trait->IsPointInDropRgn(localMouse))	// Is the mouse currently in the area of the list which we should hilight?
				{
					if (!trait->hilightOn)			// Already hilighted?
					{
						Rect						tempRect=trait->GetListViewContentRect();
					
						//RgnHandle	hilightRgn=trait->GetHilightRgn();
						RgnHandle	hilightRgn=NewRgn();
						RectRgn(hilightRgn,&tempRect);
						
						if (hilightRgn)
						{						
							ShowDragHilite(theDrag,hilightRgn,true);
							DisposeRgn(hilightRgn);
							trait->hilightOn=true;
						}
					}
				}
				else
				{
					if (trait->hilightOn) // just left, unhilight
					{
						HideDragHilite(theDrag);
						trait->hilightOn=false;
					}
				}
			}
			break;
	
		case kDragTrackingLeaveWindow:
			if (!trait->dragAcceptable)
				break;
			if (trait->hilightOn)
			{
				HideDragHilite(theDrag);
				trait->hilightOn=false;
			}
			break;
	}

	return noErr;
}

template <class T>
RgnHandle TLVDragNDropTrait<T>::GetHilightRgn()
{
	RgnHandle	rgn=NewRgn();
	
	if (rgn)		
	{
		Rect		rect=GetListViewContentRect();
		::RectRgn(rgn,&rect);
	}
	
	return rgn;
}

template <class T>
Boolean TLVDragNDropTrait<T>::IsPointInDropRgn(Point localPoint)
{
	Rect		rect=GetListViewContentRect();

	return ::PtInRect(localPoint,&rect);
}

// ---- Static
template <class T>
pascal OSErr TLVDragNDropTrait<T>::ReceiveHandler(WindowPtr theWindow, void *refCon,DragReference drag)
{
	TLVDragNDropTrait<T>			*trait=(TLVDragNDropTrait<T>*)refCon;

	if (trait->hilightOn)
	{
		UPortSaver				port(theWindow);
		UForeColourSaver		safe1(TColourPresets::kBlack);
		UBackColourSaver		safe2(TColourPresets::kWhite);
		
		HideDragHilite(drag);
		trait->hilightOn=false;
	}
	else
		return dragNotAcceptedErr;

	// Pass all the dropped types onto the handle item receipt proc
	unsigned short		maxItems,itemIndex;
	ItemReference		theItem;

	// Loop through all items in the drag
	CountDragItems(drag,&maxItems);
	
	trait->DragReceiptPreProcess(maxItems);
	
	for (short itemIndex=1; itemIndex<=maxItems; itemIndex++)
	{
		GetDragItemReferenceNumber(drag,itemIndex,&theItem);

		Try_
		{
			trait->ReceiveDragItem(drag,theItem);
		}
		Catch_(err)
		{
			trait->DragReceiptPostProcess();
			return ErrCode_(err);
		}
	}

	trait->DragReceiptPostProcess();

	return noErr;
}

// ---- Static
template <class T>
Boolean TLVDragNDropTrait<T>::IsDragAcceptable(DragReference drag)
{
	unsigned short		maxItems,itemIndex;
	ItemReference		theItem;

	// Loop through all items in the drag
	CountDragItems(drag,&maxItems);
	
	for (short itemIndex=1; itemIndex<=maxItems; itemIndex++)
	{
		GetDragItemReferenceNumber(drag,itemIndex,&theItem);
		
		if (!IsItemAcceptable(drag,theItem))
			return false;
	}

	return true;
}

template <class T>
typename T::TClickResult TLVDragNDropTrait<T>::HandleMouseClick(EventRecord &event)
{
	T::TClickResult		clickRes=T::HandleMouseClick(event);
	Boolean				wasDouble=(clickRes==kDoubleClick);

	UPortSaver			safe(GetDrawPort());
	Point				localPoint=event.where;
	
	GlobalToLocal(&localPoint);

	switch (clickRes)
	{
		case kWasInCells:
			if (PointToCell(localPoint))		// in a cell?
			{
				// could well be start drag time
				if (WaitMouseMoved(event.where))
				{
					// drag started
					StartDrag(event);
					clickRes=kWasInCells;		// doesn't count as a double click if they dragged
				}
			}
			break;
	}
	
	return clickRes;
}

template <class T>
void TLVDragNDropTrait<T>::StartDrag(EventRecord &event)
{
	DragReference			dragRef=0L;
	RgnHandle				dragRgn=0L,imageRgn=0L;
	UPortSaver				port(GetDrawPort());
	TCellIndex				max=CountSelectedCells();

	if (!max)
		return;		// Start drag shouldn't have been called as there's nothing to drag!

	Try_
	{
		// create the drag
		ThrowIfOSErr_(NewDrag(&dragRef));

		CListIndexerT<void>		indexer(cellsList);
		
		while (TListViewCellPtr cellPtr=(TListViewCell*)indexer.GetNextData())
		{
			if (cellPtr->IsCellSelected())
				AddItemToDrag(dragRef,cellPtr);
		}
		
		// now build ourselves a drag rgn
		imageRgn=GetSelectedCellsRgn(true);

		Point		imageOffsetPt={0,0};
		
		SetDragImage(dragRef,GetPortPixMap(GetWindowPort(GetDrawPort())),imageRgn,imageOffsetPt,0L);

		// translate the rgn to global coords
		LocalToGlobalRgn(imageRgn);
				
		dragRgn=NewRgn();
		
		if (dragRgn)
		{
			CopyRgn(imageRgn,dragRgn);
			OutlineRgn(dragRgn);
		}
		
		// commence the drag
		OSErr err=TrackDrag(dragRef,&event,dragRgn);
		if (err!=userCanceledErr)
			ThrowIfOSErr_(err);
	}
	Catch_(err)
	{
		SysBeep(0);
	}
	
	// dispose of the drag
	if (dragRgn)
		DisposeRgn(dragRgn);
	if (dragRef)
		DisposeDrag(dragRef);
	if (imageRgn)
		DisposeRgn(imageRgn);
}




