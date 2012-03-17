// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TListView.cpp
// Mark Tully
// 27/3/98
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

#include		"MR Debug.h"
#include		"TListView.h"
#include		"Utility Objects.h"
#include		"UListViewUpdateBlocker.h"
#include		"TListViewScroller.h"
#include		"TListIndexer.h"

/*e*/
TListView::TListView(WindowPtr windowPtr,const Rect &listRect,TLinkedList *listToUse,void *refCon)
{
	// if listToUse is 0L you better had specify the list before using it using SpecifyLinkedList		
	Init(windowPtr,listRect,listToUse,refCon);
}

/*e*/
TListView::TListView(WindowPtr windowPtr,const Rect &listRect,void *refCon)
{
	TLinkedListPtr		listPtr=new TLinkedList;

	ThrowIfMemFull_(listPtr);

	Init(windowPtr,listRect,listPtr,refCon);
}

/*e*/
void TListView::Init(WindowPtr windowPtr,const Rect &listRect,TLinkedList *listToUse,void *refCon)
{
	ControlHandle			scrollBar=0L;

	this->windowPtr=windowPtr;
	this->listViewRect=listRect;
	this->refCon=refCon;
	enabled=true;
	cellsList=0L;
	invalledRgn=0;
	vScroller=0L;
	recentlySelectedCell=0L;
	clickFlags=kUserCanDragSelect|kDeselectIfClickInEmpty;
	lastClickTime=0L;
	lastClickChangedSelection=false;
	listForeColour=TColourPresets::kBlack;
	listBackColour=TColourPresets::kWhite;
	
	Try_
	{
		cellsList=listToUse;

		invalledRgn=NewRgn();
		ThrowIfMemFull_(invalledRgn);
		drawCounter=0;
		
		// Create the scroller bar
		Rect					controlRect=GetScrollBarRect();
		
		scrollBar=NewControl(windowPtr,&controlRect,"\p",true,0,0,0,scrollBarProc,(long)this);
		ThrowIfMemFull_(scrollBar);
		
		vScroller=new TListViewScroller(this,scrollBar);
		ThrowIfMemFull_(vScroller);

		ValidateAll();
	}
	Catch_(err)
	{
		if (cellsList)
			delete cellsList;
			
		if (vScroller)
			delete vScroller;
		else if (scrollBar)
			DisposeControl(scrollBar);
		if (invalledRgn)
			DisposeRgn(invalledRgn);
	}	
}

TListView::~TListView()
{
	if (vScroller)
		delete vScroller;
	if (invalledRgn)
		DisposeRgn(invalledRgn);
	if (cellsList)
		delete cellsList;
}

Rect TListView::GetScrollBarRect()
{
	Rect		tempRect=listViewRect;
	
	tempRect.left=tempRect.right-kScrollBarWidth-1;

	return tempRect;
}

void TListView::DrawListFrame()
{
	UPortSaver		safe(windowPtr);
	UPenStateSaver	penSafe(1,1);
	
	FrameRect(&listViewRect);
}

// This proc assumes the rect being passed has been clipped to the content rect. It also assumes that the list is allowed
// to draw.
void TListView::DrawCellsInRect(const Rect &updateRect)
{
	Point					point;
	TListViewCellPtr		cell;
	
	point.h=updateRect.left;
	point.v=updateRect.top;
	cell=PointToCell(point);
	
	Rect					cellRect,clippedRect;
	UPortSaver				safePort(GetDrawPort());
	UForeColourSaver		safeCol(listForeColour);
	UBackColourSaver		safeCol2(listBackColour);
		
	DrawCellsPreProcess();
	
	{
		UClipSaver				clipSave;
		RgnHandle				clipRgn=NewRgn();
		
		if (clipRgn)
		{	
			cellRect.bottom=updateRect.top;
			
			while (cell)
			{
				cellRect=cell->GetCellRect();
				
				clippedRect=cellRect;
				if (::NewClipRect(&clippedRect,&updateRect)!=csClippedOff)
				{			
					RectRgn(clipRgn,&clippedRect);
					SectRgn(clipRgn,clipSave.GetSavedRgn(),clipRgn);
					SetClip(clipRgn);
					
					cell->DrawCell(cellRect);
				}
				else
					break;
				
				// Move onto the next cell	
				cell=(TListViewCellPtr) (cell->link.GetNextElement() ? cell->link.GetNextElement()->GetData() : 0);
			}
			
			DisposeRgn(clipRgn);
		}
	}
	
	// Now all the cells are drawn, we erase all the space between the last drawn cell and the bottom of the update rect
	Rect		tempRect=updateRect;
	
	tempRect.top=cellRect.bottom;
	::EraseRect(&tempRect);
	
	DrawCellsPostProcess();
}

Rect TListView::GetCellsRect(TListViewCellPtr cell)
{
	Rect		rect;
	
	rect=GetListViewContentRect();
	rect.top+=cell->GetDistanceToCellTop()-vScroller->GetDistanceToTop();
	rect.bottom=rect.top+cell->GetCellHeight();
	
	return rect;
}

void TListView::InvalRect(const Rect &invalRect)
{
	Rect		clippedRect=invalRect;
	Rect		cellContentRect=GetListViewContentRect();

	// Clip the invalled rect to the cells content rect
	if (::NewClipRect(&clippedRect,&cellContentRect)==csClippedOff)
		return;

	if (IsListViewUpdating())
	{
		// Draw now
		DrawCellsInRect(clippedRect);
	}
	else
	{
		// Defer until later
		RgnHandle		tempRgn=NewRgn();
		
		if (tempRgn)
		{
			RectRgn(tempRgn,&clippedRect);
			UnionRgn(tempRgn,invalledRgn,invalledRgn);
			DisposeRgn(tempRgn);
		}
	}
}

void TListView::SetListViewUpdating(Boolean updateInstantly)
{
	if (updateInstantly)
	{
		if (drawCounter>0)
		{
			drawCounter--;
			if (!drawCounter)	// Allowed to draw again?
			{
				vScroller->UpdateScrollBar(false);
				DrawInvalledRgn();
				ReactToUpdateStateChanged(true);
			}
		}
	}
	else
	{
		if (!drawCounter)
			ReactToUpdateStateChanged(false);
		drawCounter++;
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊGetScrollOffset
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Retrieves how far into the list (in scroll units) the view currently is.
unsigned short TListView::GetScrollOffset()
{
	return vScroller->GetControlValue();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ÊSetScrollOffset
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets how far into the list (in scroll units) the view currently is.
void TListView::SetScrollOffset(unsigned short inNewValue)
{
	vScroller->SetControlValue(inNewValue);
}

// Distance to scroll is in scroll units rather than pixels
// To convert from pixels to scroll units divide by the scroll unit scale.
void TListView::ScrollList(signed short distanceToScroll)
{
	vScroller->SetControlValue(vScroller->GetScrollBarValue()+distanceToScroll);
}

void TListView::ScrollIntoView(TListViewCellPtr cellPtr)
{
	// If the currentCell is above the view area then put the top of the cell into view
	// If the currentCell is below the view area then put the bottom of the cell into view
	Rect			contentRect=GetListViewContentRect();
	TCellHeight		currentDistanceTop=vScroller->GetDistanceToTop();
	TCellHeight		currentDistanceBottom=currentDistanceTop+FRectHeight(contentRect);
	
	if (cellPtr->GetDistanceToCellTop()<currentDistanceTop)
		ScrollIntoView(cellPtr,0);
	else if ((cellPtr->GetDistanceToCellTop()+cellPtr->GetCellHeight())>=currentDistanceBottom)
		ScrollIntoView(cellPtr,cellPtr->GetCellHeight());
	// else it already in view so do nothing
}

// Does the minimum necessary to put a cell in view, if it's already in view it does nothing
void TListView::ScrollIntoView(TListViewCellPtr cellPtr,TCellHeight distanceIntoCell)
{
	TCellHeight		distance=cellPtr->GetDistanceToCellTop()+distanceIntoCell;

	// Work out what section of the cells are being viewed
	Rect			contentRect=GetListViewContentRect();
	TCellHeight		currentDistanceTop=vScroller->GetDistanceToTop();
	TCellHeight		currentDistanceBottom=currentDistanceTop+FRectHeight(contentRect);
	
	if (InRange(distance,currentDistanceTop,currentDistanceBottom))
		return;	// Already in the current view
	
	// If the distance which is supposed to be being viewed is above the current view section then scroll up a bit
	if (distance<currentDistanceTop)
		PutRowInView(distance,0);
	else
		PutRowInView(distance,FRectHeight(contentRect));
}

void TListView::CentreInView(TListViewCellPtr cellPtr)
{
	Rect		tempRect=cellPtr->GetCellRect();

	CentreInView(cellPtr,FRectHeight(tempRect)/2);
}

void TListView::CentreInView(TListViewCellPtr cellPtr,TCellHeight distanceIntoCell)
{
	// Get the distanceInto the list which should be put in the middle of the list
	TCellHeight		distance=cellPtr->GetDistanceToCellTop()+distanceIntoCell;
	Rect			contentRect=GetListViewContentRect();
	
	PutRowInView(distance,FRectHeight(contentRect)/2);
}

// This proc is used by the ScrollIntoView and CentreInView procs to place a specified row into the content rect in a
// certain position
void TListView::PutRowInView(TCellHeight distanceIntoCells,unsigned short distanceIntoContentRect)
{
	signed short	delta=(distanceIntoCells-(vScroller->GetDistanceToTop()+distanceIntoContentRect))/vScroller->GetScrollScale();
	
	if (delta)
		ScrollList(delta);
}

Rect TListView::GetListViewContentRect()
{
	Rect		tempRect=listViewRect;
	
	tempRect.right-=kScrollBarWidth;
	
	FInsetRect(tempRect,1,1);
	
	return tempRect;
}

void TListView::ChangeListViewRect(const Rect &newRect)
{
	UListViewUpdateBlocker			block(this);

	// Change the rect
	listViewRect=newRect;

	InvalRect(listViewRect);

	// Move the scroll bar
	vScroller->SetScrollBarPosition(GetScrollBarRect());

	// Reflow the data in all the cells
	TListIndexer	indexer(cellsList);
	Rect			contentRect=GetListViewContentRect();
	
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
		cellPtr->CellWidthChanged(FRectWidth(contentRect));	
}

void TListView::UpdateListView()
{
	if (IsListViewUpdating())
	{
		DrawListFrame();
		DrawAllCells();
		vScroller->UpdateScrollBar(true);
	}
	else
	{
		// Updating is off, but we should remember the area which needs redrawing then we can redraw it when the drawing
		// is reenabled.
		// Get the intersection of the clipRgn and the contentRect and add that to the invalled rgn
		RgnHandle			handle=NewRgn();
		
		if (handle)
		{
			UPortSaver		safe(GetDrawPort());
			GetClip(handle);
#if TARGET_API_MAC_CARBON
			Rect			r;
			::GetRegionBounds(handle,&r);
			InvalRect(r);
#else
			InvalRect((**handle).rgnBBox);
#endif
			DisposeRgn(handle);
		}
	}
}

TListView::TClickResult TListView::HandleMouseClick(EventRecord &event)
{
	UPortSaver			safe(GetDrawPort());
	TListViewCellPtr	lastCell=0L;
	Boolean				wasDoubleClick=IsDoubleClick();
	Rect				contentRect=GetListViewContentRect();
	Point				localPoint=event.where;
	
	GlobalToLocal(&localPoint);

	lastClickChangedSelection=false;

	if (!FPointInRect(localPoint,listViewRect))
		return kNotHandled;
	
	if (!vScroller->HandleMouseClick(localPoint))
	{	
		SnapShotSelection();
	
		do
		{
			TListViewCellPtr		cellPtr=PointToCell(localPoint);

			if (cellPtr && !cellPtr->IsPointInCellContent(localPoint))
				cellPtr=0L;

			if (!cellPtr)
			{
				// The user is not pointing in a cell, autoscroll the list if necessary
				lastCell=0L;
				
				if (InRange(localPoint.h,contentRect.left,contentRect.right))
				{
					// If less than one scroll scale above the rect then move up at one scoll unit at a time, else
					// do so at two units at a time.
					if (!InRange(localPoint.v,contentRect.top,contentRect.bottom))
					{
						signed short		delta=0;
						unsigned long		finalTicks;
						Point				tempPoint;
				
						tempPoint.h=localPoint.h;
	
						if (localPoint.v<contentRect.top)
						{
							delta=(localPoint.v-contentRect.top)/vScroller->GetScrollScale();
							if (!delta)
								delta=-1;
							tempPoint.v=contentRect.top;
						}
						else
						{
							delta=(localPoint.v-contentRect.bottom)/vScroller->GetScrollScale();
							if (!delta)
								delta=1;
							tempPoint.v=contentRect.bottom-1;
						}
						
						ScrollList(delta);
						Delay(3L,&finalTicks);
						
						// Now make out they pointed to the bottom cell in the list
						cellPtr=PointToCell(tempPoint);
						if (cellPtr && !cellPtr->IsPointInCellContent(localPoint))
							cellPtr=0L;					
					}
				}
			}

			if (cellPtr!=lastCell)
			{
				lastCell=cellPtr;
				
				// This cell was clicked on, handle it appropiately
				if (event.modifiers&cmdKey)
				{
					if (GetClickFlags()&kCanSelectOnlyOne)
						Select1Cell(cellPtr,!cellPtr->IsCellSelected());
					else
						cellPtr->SetCellSelection(!cellPtr->IsCellSelected());
				}
				else if (event.modifiers&shiftKey && !(GetClickFlags()&kCanSelectOnlyOne))
				{
					TListViewCellPtr			wasMostRecently=GetMostRecentlySelectedCell();
					UListViewUpdateBlocker		updateBlock(this);
				
					// Extend selection
					SelectAllBetween(wasMostRecently,cellPtr,true);
					
					if (wasMostRecently)
						recentlySelectedCell=wasMostRecently;
				}
				else
				{
					if (!cellPtr->IsCellSelected())
						Select1Cell(cellPtr,true);
					else
					{
						// Check for double click
						if (wasDoubleClick)
							return kDoubleClick;
					}
				}
				
				if (!(GetClickFlags()&kUserCanDragSelect) && cellPtr->IsCellSelected())
					cellPtr->HandleMouseClick(localPoint,event.modifiers);
			}
			
			if (!cellPtr && GetClickFlags()&kDeselectIfClickInEmpty)
				SelectAllCells(false);

			GetMouse(&localPoint);
			
			wasDoubleClick=false;
		}
		while ((GetClickFlags()&kUserCanDragSelect) && StillDown() && !(event.modifiers&cmdKey));
		
		lastClickChangedSelection=HasSelectionChanged();
	}
	else
		return kWasInScrollBar;

	return kWasInCells;
}

Boolean TListView::HasSelectionChanged()
{
	TListIndexer		indexer(cellsList);

	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
	{
		if (cellPtr->GetSelectionSnapShot()!=cellPtr->IsCellSelected())
			return true;
	}
	
	return false;
}

void TListView::SnapShotSelection()
{
	TListIndexer		indexer(cellsList);

	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
		cellPtr->SnapShotSelection();
}

/*e*/
// Builds a rgn of all the cells whose selection state is == selected
RgnHandle TListView::GetSelectedCellsRgn(Boolean selected)
{
	RgnHandle		rgnHandle=NewRgn(),tempRgn=NewRgn();
	
	Try_
	{
		ThrowIfMemFull_(rgnHandle);
		ThrowIfMemFull_(tempRgn);
	
		TListIndexer		indexer(cellsList);
		Rect				emptyRect={0,0,0,0};
		Rect				contentRect=GetListViewContentRect();
		TListViewCellPtr	startCell,endCell;
		Point				tempPoint;
		
		tempPoint.h=contentRect.left;
		tempPoint.v=contentRect.top;
		startCell=PointToCell(tempPoint);
		
		tempPoint.h=contentRect.left;
		tempPoint.v=contentRect.bottom-1;
		endCell=PointToCell(tempPoint);
		
		indexer.StartIndexingAt(GetCellLink(startCell));
		
		while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
		{
			if (cellPtr->IsCellSelected())
			{
				RectRgn(tempRgn,&emptyRect);
				cellPtr->GetSelectRgn(tempRgn);
				UnionRgn(tempRgn,rgnHandle,rgnHandle);
			}
			
			if (cellPtr==endCell)
				break;
		}
		
		RectRgn(tempRgn,&contentRect);
		SectRgn(rgnHandle,tempRgn,rgnHandle);
		
		DisposeRgn(tempRgn);
		return rgnHandle;
	}
	Catch_(err)
	{
		if (rgnHandle)
			DisposeRgn(rgnHandle);
		if (tempRgn)
			DisposeRgn(tempRgn);
		
		throw;
	}
	
	return 0L;
}

// Use this to delete all the cells in the list. Make sure all your destructors are virtual.
void TListView::DeleteAllCells()
{
	UListViewUpdateBlocker		updateBlock(this);
	
	TListIndexer		indexer(cellsList,false);

	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetPrevData())
	{
		delete cellPtr;
	}
}

Boolean TListView::IsDoubleClick()
{
	Boolean			well;
	unsigned long	currentTime=TickCount();

	if ((GetDblTime()+lastClickTime)>=currentTime)
		well=true;
	else
		well=false;
		
	lastClickTime=currentTime;
	
	return well;
}

void TListView::Select1Cell(TListViewCellPtr onlyMe,Boolean state)
{
	UListViewUpdateBlocker	block(this);
	TListIndexer			indexer(cellsList);

	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
	{
		if (cellPtr!=onlyMe)
			cellPtr->SetCellSelection(false);
		else
			cellPtr->SetCellSelection(state);
	}
}

// Selects all between start and end inclusive.
// If one is 0L then just the one which isn't 0L is selected. If they're both 0L then nothing happens.
void TListView::SelectAllBetween(TListViewCellPtr start,TListViewCellPtr end,Boolean state)
{
	if ((!start || !end) || start==end)
	{
		// One of them is 0L, or they are equal, either way it's a one cell op
		TListViewCellPtr		cell=Greater(start,end);
		
		if (cell)
			Select1Cell(cell,state);
		else
			SelectAllCells(false);
		
		return;
	}
	
	// Index through the list selecting all the ones in the range
	TListIndexer		indexer(cellsList);
	Boolean				selecting=false;
	Boolean				startEndCell;
	
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
	{
		startEndCell=(cellPtr==start || cellPtr==end);
	
		if (!selecting && startEndCell)
		{
			startEndCell=false;
			selecting=true;
		}
	
		if (selecting)
			cellPtr->SetCellSelection(state);
		else
			cellPtr->SetCellSelection(false);
		
		if (selecting && startEndCell)
			selecting=false;
	}
}

void TListView::CycleSelection(Boolean cycleDown)
{
	TCellIndex			selectedCells=CountSelectedCells();
	TCellIndex			selectCell;

	if (selectedCells)
	{
		if (cycleDown)
		{
			TListViewCellPtr	lastSelectedCell=GetNthSelectedCell(selectedCells-1);
			
			selectCell=lastSelectedCell->GetCurrentCellIndex()+1;
			if (selectCell>=CountCells())
				selectCell=0;
		}
		else
		{
			TListViewCellPtr	firstSelectedCell=GetNthSelectedCell(0);
			
			selectCell=firstSelectedCell->GetCurrentCellIndex();
			if (selectCell)
				selectCell--;
			else
			{
				selectCell=CountCells();
				if (selectCell)
					selectCell--;
			}
		}
	}
	else
	{
		if (cycleDown)
		{
			selectCell=CountCells();
			if (selectCell)
				selectCell--;
		}
		else
			selectCell=0;
	}
	
	Select1Cell(GetNthCell(selectCell),true);
	
	if (TListViewCellPtr temp=GetNthCell(selectCell))
		ScrollIntoView(temp);
}

void TListView::AttachCell(TListViewCellPtr cell,TListViewCellPtr positionCell,Boolean afterPositionIndex)
{
	cellsList->InsertElement(GetCellLink(cell),GetCellLink(positionCell),afterPositionIndex);
	CellAttached(cell);
}

void TListView::DetachCell(TListViewCellPtr cell)
{
	UListViewUpdateBlocker		blocked(this);
	TCellHeight					wasCellHeight=cell->GetCellHeight();
	
	if (cell->IsCellSelected())
		CellSelectionChanged(cell,false);
	
	cell->SetCellHeight(0);
	CellDetached(cell);			// Note : This is done after SetCellHeight. If it's not then SetCellHeight can't call the bosslist's CellHeightChanged as the listptr has been zeroed
	cellsList->UnlinkElement(&cell->link);
	cell->SetCellHeight(wasCellHeight);

	if (!recentlySelectedCell)
		recentlySelectedCell=GetNthSelectedCell(0);
}

void TListView::CellDeleted(TListViewCellPtr cell)
{
	ReactToCellDeleted(cell);
	DetachCell(cell);
}

void TListView::CellDetached(TListViewCellPtr cell)
{
	ReactToCellDetached(cell);
	cell->SetListView(0L);
}

void TListView::CellAttached(TListViewCellPtr cell)
{
	cell->SetListView(this);
	
	TListViewCellPtr		prevCell=(TListViewCellPtr) (cell->link.GetPrevElement() ? cell->link.GetPrevElement()->GetData() : 0);
	if (prevCell)
		cell->SetDistanceToCellTop(prevCell->GetDistanceToCellTop()+prevCell->GetCellHeight());
	else
		cell->SetDistanceToCellTop(0);
	
	CellSizeChanged(cell,cell->GetCellHeight());
	cell->InvalCell();
	
	ReactToCellAttached(cell);
	
	if (cell->IsCellSelected())
		CellSelectionChanged(cell,true);
}

void TListView::CellSelectionChanged(TListViewCellPtr cell,Boolean newState)
{
	if (newState)
		recentlySelectedCell=cell;
	else
	{
		if (cell==recentlySelectedCell)
			recentlySelectedCell=GetNthSelectedCell(0);
	}
	
	ReactToSelectionChanged(cell,newState);
}

void TListView::CellSizeChanged(TListViewCellPtr cell,signed short deltaDistance)
{
	if (!deltaDistance)
		return;
	
	UListViewUpdateBlocker		blocked(this);
	
	// All cells after this need their distanceToCellTop altering
	TListIndexer		indexer(cellsList);
	
	indexer.StartIndexingAt(cell->link.GetNextElement());
	
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
		cellPtr->SetDistanceToCellTop(cellPtr->GetDistanceToCellTop()+deltaDistance);
		
	// Look at total scrollable distance now and plug it into the scroller
	TListViewCellPtr		tempCell=(TListViewCellPtr)cellsList->GetLastElement();
	TCellHeight				maxDist=0;
	
	if (tempCell)
		maxDist=tempCell->GetDistanceToCellTop()+tempCell->GetCellHeight();
	
	// The list will need redrawing unless the cell is below the list view content rect
	Rect					contentRect=GetListViewContentRect();
	if (cell->GetDistanceToCellTop()<(vScroller->GetDistanceToTop()+FRectHeight(contentRect)))
		InvalRect(contentRect);		// NB : Won't draw yet

	vScroller->SetTotalScrollableDistance(maxDist);
}

TListViewCellPtr TListView::PointToCell(Point localPoint)
{
	Rect				contentRect=GetListViewContentRect();

	if (!FPointInRect(localPoint,contentRect))
		return 0L;

	TListIndexer		indexer(cellsList);
	TCellHeight			distanceToTop=localPoint.v-contentRect.top+vScroller->GetDistanceToTop();
	Rect				cellRect;
		
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
	{
		cellRect=cellPtr->GetCellRect();
		if (FPointInRect(localPoint,cellRect))
			return cellPtr;
	}
	
	return 0L;
}

TListViewCellPtr TListView::GetNthCell(TCellIndex index)
{
	return (TListViewCellPtr)cellsList->GetNthElementData(index);
}

TCellIndex TListView::CountCells()
{
	return cellsList->CountElements();
}

TCellIndex TListView::CountSelectedCells()
{
	TListIndexer		indexer(cellsList);
	TCellIndex			selected=0;
		
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
	{
		if (cellPtr->IsCellSelected())
			selected++;
	}
	
	return selected;
}

TListViewCellPtr TListView::GetNthSelectedCell(TCellIndex index)
{
	TListIndexer		indexer(cellsList);
	TCellIndex			selected=0;
		
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
	{
		if (cellPtr->IsCellSelected())
		{
			if (selected==index)
				return cellPtr;
			selected++;
		}
	}
	
	return 0L;
}

void TListView::SelectAllCells(Boolean selectThem)
{
	UListViewUpdateBlocker	block(this);
	TListIndexer			indexer(cellsList);
		
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
		cellPtr->SetCellSelection(selectThem);	
}

void TListView::DrawInvalledRgn()
{
	// Clip to the update rgn and draw all the cells
	UPortSaver		safePortSaver(windowPtr);
	UClipSaver		safe(invalledRgn);
	
	DrawAllCells();
	
	ValidateAll();
}

// > 0 is scroll down, data moves up
// < 0 is scroll up, data moves down
// distance specified in pixels
// If drawing is not permitted this proc invals the rgn instead
// Is assumes the scroll bar has been altered and it's distanceToTop value represents the new position.
// So basically is should only be called by the scroll bar object.
void TListView::ScrollContent(signed short distanceToScroll)
{
	if (!distanceToScroll)
		return;

	Rect			contentRect=GetListViewContentRect();

	if (IsListViewUpdating())
	{
		UPortSaver			safePort(windowPtr);
		UForeColourSaver	safe1(listForeColour);
		UBackColourSaver	safe2(listBackColour);
		
		if (FAbs(distanceToScroll)<FRectHeight(contentRect))
		{		
			// We are scrolling less than the height of the contentRect therefore some data will still be visible so we
			// can simply use ScrollRect
			::ScrollRect(&contentRect,0,-distanceToScroll,invalledRgn);
			DrawInvalledRgn();
		}
		else
			DrawAllCells();
	}
	else
		InvalRect(contentRect);
}

void TListView::ValidateAll()
{
	Rect		rect={0,0,0,0};
	
	RectRgn(invalledRgn,&rect);
}

void TListView::EnableDisableList(Boolean newState)
{
	if (enabled==newState)
		return;

	enabled=newState;

	if (enabled)
		vScroller->EnableScroller();
	else
		vScroller->DisableScroller();
}

// This proc is to be called if you change the order of the cells in a linked list directly
void TListView::RecalcCellDistances()
{
	TListIndexer		indexer(cellsList);
	TCellHeight			distance=0;
	
	while (TListViewCellPtr cellPtr=(TListViewCellPtr)indexer.GetNextData())
	{
		cellPtr->SetDistanceToCellTop(distance);
		distance+=cellPtr->GetCellHeight();
	}
}

void TListView::ScrollSelectionIntoView()
{
	if (!recentlySelectedCell)	// No selection
		return;
	ScrollIntoView(recentlySelectedCell);
}

void TListView::CentreSelectionInView()
{
	if (!recentlySelectedCell)	// No selection
		return;
	CentreInView(recentlySelectedCell);
}



