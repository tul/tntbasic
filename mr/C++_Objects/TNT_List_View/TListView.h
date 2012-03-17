// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// TListView.h
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

#pragma once

#include							"TListViewCell.h"
#include							"TListViewScroller.h"
#include							"TBroadcaster.h"
#include							"Marks Routines.h"

typedef class TListView : public TBroadcaster
{
	friend void TListViewCell::InvalCell();;											// Needs to call InvalRect
	friend TListViewCell::~TListViewCell();;											// Needs to call CellDeleted
	friend void TListViewCell::SetCellHeight(TCellHeight newHeight);;					// Needs to call CellSizeChanged
	friend Rect TListViewCell::GetCellRect();;											// Needs to call GetCellsRect
	friend void TListViewScroller::SetScrollBarValue(short value);;						// Needs to call ScrollContent
	friend void TListViewScroller::SetScrollScale(TCellHeight newScale);;				// Needs to call InvalRect
	friend void TListViewCell::SetCellSelection(Boolean newState);;						// Needs to call CellSelectionChanged

	private:
		WindowPtr					windowPtr;
		void						*refCon;
		Rect						listViewRect;
		RgnHandle					invalledRgn;										// This region is built up of invalled rects if drawing is off.
		unsigned short				drawCounter;
		TListViewCellPtr			recentlySelectedCell;
		Boolean						enabled;
		Boolean						lastClickChangedSelection;
		unsigned long				lastClickTime;
		RGBColour					listForeColour,listBackColour;

		void						DrawCellsInRect(const Rect &rect);
		void						DrawAllCells()										{ DrawCellsInRect(GetListViewContentRect()); }										
		Rect						GetCellsRect(TListViewCellPtr cell);				// This is called by TListViewCell and that should be all.
		void						DrawInvalledRgn();
		void						ScrollContent(signed short distanceToScroll);		// Only called by list scroller object
		void						PutRowInView(TCellHeight distanceIntoCells,unsigned short distanceIntoContentRect);
		void						Init(WindowPtr windowPtr,const Rect &listRect,TLinkedList *listToUse,void *refCon);

	public:		
		typedef unsigned long		TListViewClickFlags;
		
		// ClickFlags
		enum
		{
			kUserCanDragSelect=1,				// While the mouse is down the HandleMouseDown won't exit
			kDeselectIfClickInEmpty=2,			// Cells are deselected if the user clicks into empty space
			kCanSelectOnlyOne=4,				// Only one cell is selected as a time in the mouse down loop
		};

		// These are the message classes sent to any listener.
		enum
		{
			kSelectionChanged,		// cellPtr, (Boolean*) cell new state
			kCellDetached,			// cellPtr, 0L
			kCellAttached,			// cellPtr, 0L
			kCellDeleted,			// cellPtr, 0L
			kUpdateStateChanged,	// listViewPtr, (Boolean*) new update state
		};


	private:
		TListViewClickFlags			clickFlags;

	protected:
		TListViewScroller			*vScroller;
		TLinkedList					*cellsList;											// Call RecalcCellDistances if you directly modifiy the linked list
			
		virtual void				SnapShotSelection();
		virtual Boolean				HasSelectionChanged();
		
		virtual void				DrawListFrame();									// Override this to draw a frame around the list view. It may violate the list view rect.
		virtual void				ValidateAll();										// This clears the invalled rgn
		virtual void				InvalRect(const Rect &rect);						// This updates an area of the list, if drawing is off the rect is added to a region which is updated later.

		// A derived class should call these too keep the base class informed of what's happening
		// These all call the informative function procs - ReactTo...
		virtual void				CellSizeChanged(TListViewCellPtr cell,signed short deltaDistance);	// This proc has to alter all the distanceToTop variables for all cells after this one in the list view
		virtual void				CellDeleted(TListViewCellPtr cell);
		virtual void				CellAttached(TListViewCellPtr cell);
		virtual void				CellDetached(TListViewCellPtr cell);
		virtual void				CellSelectionChanged(TListViewCellPtr cell,Boolean state);	// Override if you need to react to the selection of a cell

		// Override these too get messages when stuff happens in the list (make sure you still call the original)
		// Alternatively add listeners
		virtual void				ReactToSelectionChanged(TListViewCellPtr cell,Boolean newState)		{ BroadcastMessage(kSelectionChanged,cell,&newState); }
		virtual void				ReactToCellDetached(TListViewCellPtr cell)							{ BroadcastMessage(kCellDetached,cell,0L); }
		virtual void				ReactToCellAttached(TListViewCellPtr cell)							{ BroadcastMessage(kCellAttached,cell,0L); }
		virtual void				ReactToCellDeleted(TListViewCellPtr cell)							{ BroadcastMessage(kCellDeleted,cell,0L); }
		virtual void				ReactToUpdateStateChanged(Boolean newState)							{ BroadcastMessage(kUpdateStateChanged,this,&newState); }
		
		virtual Boolean				IsDoubleClick();
		virtual TListElement		*GetCellLink(TListViewCellPtr cell)			{ return &cell->link; }

		// This constructor allows a dervived class to specifiy a list to use by passing 0L as the listToUse then using
		// specify list. Or just passing the list to use to this constructor. The destructor of this class will dispose
		// the list.
		/*e*/						TListView(WindowPtr windowPtr,const Rect &listViewRect,TLinkedList *listToUse,void *refCon=0L);
		virtual void				SpecifyList(TLinkedList *list)				{ cellsList=list; }
		virtual void				RecalcCellDistances();		// Call this if you directly modifiy the linked list
		
		virtual void				DrawCellsPreProcess()		{}			// Override these if you need to change the font
		virtual void				DrawCellsPostProcess()		{}
	
	public:
		/*e*/						TListView(WindowPtr windowPtr,const Rect &listViewRect,void *refCon=0L);
									~TListView();
		
		// Flag accessors
		virtual TListViewClickFlags	GetClickFlags()											{ return clickFlags; }
		virtual void				SetClickFlags(TListViewClickFlags newFlags)				{ clickFlags=newFlags; }
									
		// RefCon accessors
		virtual void				*GetRefCon()										{ return refCon; }
		virtual void				SetRefCon(void *newRef)								{ refCon=newRef; }

		// Enable/Disable
		virtual void				EnableDisableList(Boolean enabled);
		virtual Boolean				IsListEnabled()										{ return enabled; }

		// List drawing
		virtual void				SetListViewUpdating(Boolean updateInstantly);		// Calls to this are accumulative. If changes are made while drawing is off, they are drawn the instant that it's enabled again.
		virtual Boolean				IsListViewUpdating()								{ return drawCounter==0; }

		// Cell attachment/detachment
		virtual void /*e?*/			AttachCell(TListViewCellPtr cell,TListViewCellPtr positionCell,Boolean afterPositionIndex);
		virtual void				DetachCell(TListViewCellPtr cell);
		virtual void				DeleteAllCells();									// Deallocates all the cells which have been added. Careful.

		// Cell access
		virtual TListViewCellPtr	GetNthCell(TCellIndex index);
		virtual TCellIndex			CountCells();

		// Selection access
		virtual TCellIndex			CountSelectedCells();
		virtual TListViewCellPtr	GetNthSelectedCell(TCellIndex index);
		virtual void				SelectAllCells(Boolean selectThem);
		virtual void				CycleSelection(Boolean cycleDown);					// If false it cycles up true cycles down
		virtual void				SelectAllBetween(TListViewCellPtr start,TListViewCellPtr end,Boolean state);
		virtual TListViewCellPtr	GetMostRecentlySelectedCell()						{ return recentlySelectedCell; }
		virtual void				Select1Cell(TListViewCellPtr onlyMe,Boolean state);
		virtual Boolean				ClickChangedSelection()								{ return lastClickChangedSelection; }
		virtual RgnHandle /*e*/		GetSelectedCellsRgn(Boolean selected);
		
		// Scrolling the listview
		virtual void				ScrollList(signed short distanceToScroll);			// The number of scroll units to scroll. + scrolls you down the list (data moves up) - is opposite.
		virtual void				ScrollIntoView(TListViewCellPtr cellPtr);
		virtual void				ScrollIntoView(TListViewCellPtr cellPtr,TCellHeight distanceIntoCell);	// Scrolls the cell you pass into view. It makes sure the row at distanceIntoCell is visible.
		virtual void				CentreInView(TListViewCellPtr cellPtr,TCellHeight distanceInfoCell);	// Similar to ScrollIntoView except it centres.
		virtual void				CentreInView(TListViewCellPtr cellPtr);
		virtual void				ScrollSelectionIntoView();
		virtual void				CentreSelectionInView();
		virtual void				SetScrollOffset(unsigned short inNewValue);
		virtual unsigned short		GetScrollOffset();

		// List size
		virtual Rect				GetListViewRect()		{ return listViewRect; }	// this gets you the entire list view rect as set by ChangeListViewRect
		virtual Rect				GetListViewContentRect();							// this gets you the cell content rect
		virtual Rect				GetScrollBarRect();									// Override this if you want to put you scroll bar somewhere other than normal
		virtual void				ChangeListViewRect(const Rect &newRect);			// use this to move and resize the listview

		// List colour
		virtual RGBColour			GetForeColour()								{ return listForeColour; }
		virtual RGBColour			GetBackColour()								{ return listBackColour; }
		virtual void				SetForeColour(const RGBColour &newCol)		{ listForeColour=newCol; }
		virtual void				SetBackColour(const RGBColour &newCol)		{ listBackColour=newCol; }

		// Scrolling scale
		virtual TCellHeight			GetScrollScale()									{ return vScroller->GetScrollScale(); }
		virtual void				SetScrollScale(TCellHeight newScale)				{ vScroller->SetScrollScale(newScale); }

		// General
		enum TClickResult
		{
			kNotHandled,			// Click outside of list view rect
			
			kWasInCells,			// Click was in a cell
			kWasInScrollBar,		// Click in scroll bars
			kDoubleClick			// This is a double click
		};
		
		virtual WindowPtr			GetDrawPort()					{ return windowPtr; }
		virtual TListViewCellPtr	PointToCell(Point localPoint);						// localPoint is the point in the window
		virtual TClickResult		HandleMouseClick(EventRecord &event);
		virtual void				UpdateListView();

} TListView, *TListViewPtr;





