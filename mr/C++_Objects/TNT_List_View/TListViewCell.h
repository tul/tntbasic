// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TListViewCell.h
// Mark Tully
// 28/3/98
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

#include							"TLinkedList.h"

typedef unsigned short TCellHeight;
typedef TListIndex TCellIndex;

class TListView;

typedef class TListViewCell
{
	friend class TListView;

	private:
		TListElement				link;
		TListView					*bossListView;
		TCellHeight					cellHeight;
		TCellHeight					distanceToCellTop;
		void						*refCon;
		Boolean						selected;
		Boolean						selectionSnapShot;
		
		Boolean						GetSelectionSnapShot()								{ return selectionSnapShot; }
		void						SnapShotSelection()									{ selectionSnapShot=selected; }
		
		TCellHeight					GetDistanceToCellTop()								{ return distanceToCellTop; }
		void						SetDistanceToCellTop(TCellHeight newDist)			{ distanceToCellTop=newDist; }
		
		void						SetListView(TListView *newLV)						{ bossListView=newLV; }
		
	protected:
		virtual WindowPtr			GetDrawPort();										// Gets the port that a cell should draw into
		virtual Rect				GetClipRect();										// Gets the rect in the port which the cell should not violate
		virtual void				SetCellHeight(unsigned short newHeight);			// Allows the cell to change it's height
		
		virtual void				DrawCell(Rect drawRect)=0L;							// Never call this directly. This must be override to draw your cell contents.
		virtual void				HandleMouseClick(Point localPoint,short modifiers) {}// Called when the user clicks in your cell
		virtual void				CellWidthChanged(TCellHeight width)					{} // Called when the width of your cell has changed. Calc how high it should be and change the height or do nothing.
	
	public:
									TListViewCell(TCellHeight cellHeight,void *refCon=0L);
		virtual						~TListViewCell();
		
		// RefCon accessors
		virtual void				*GetRefCon()										{ return refCon; }
		virtual void				SetRefCon(void *newRef)								{ refCon=newRef; }

		// Selection accessors
		virtual Boolean				IsCellSelected()									{ return selected; }
		virtual void				SetCellSelection(Boolean newState);

		// Public functions
		virtual Rect				GetCellRect();										// Gets the cells rectangle.
		virtual void				InvalCell();										// This causes the cell to be redrawn. Use this when you've changed the cell data. Don't call the cell's DrawCell proc directly.
		virtual TCellIndex			GetCurrentCellIndex();								// Returns the cell's index in the list. Note this is obviously not constant.
		virtual TListView			*GetListView();										// Gets the list view which the cell is in.
		virtual TCellHeight			GetCellHeight();									// Returns the height of the cell at the moment
		virtual Boolean				IsPointInCellContent(Point localPoint) { return true; }	// This is called when localPoint is inside the cells rect and it needs to know wheter it's actually hitting the cell data. Override if your data doesn't completely fill the cell
		virtual void				GetSelectRgn(RgnHandle rgn);						// Put the cells region in this

} TListViewCell, *TListViewCellPtr;
