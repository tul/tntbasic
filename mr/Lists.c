// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Lists.c
// Mark Tully
// 24/5/96
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

#include		"Marks Routines.h"

// more basic version of EckerCreatelistview
// pass as dialogptr dialog item and rows / coloumns
ListHandle CreateListView(DialogPtr theWindow,short listNum,short width,short height)
{
	ListHandle		theList=0L;
	Rect			bounds;
	Rect			dimensions={0,0,0,0};
	Point			cellSize={0,0}; // auto calc
	
	bounds=ItemRect(theWindow,listNum);
	bounds.right-=17; // scroll bar width
	dimensions.right=width;
	dimensions.bottom=height;
	theList=LNew(&bounds,&dimensions,cellSize,0L,GetDialogWindow(theWindow),true,false,false,true);

	return theList;
}

// DialogPtr the DIALOG that the list view is to be created in
// listNum is the item number whose rect should be used to create the list
// procID is the procID of the LDEF
// NOTE: PROCID IS THE RESID FOR LDEFS
// it isn't like the CDEFs/WDEFs where procId is resId*16+varCode
// if your list simply refuses to update or infact contain any data and it works with the
// default ldef then you probalbly have the wrong procId. (I know about this....)
// cellHeight is the height in pixels of a single cell in the list. Try 13 for geneva 9 or 0
// for autocalc
// width number of cells across (generally 1)
// number down, set this to zero to have initally no cells
ListHandle EckerCreateListView(DialogPtr theDialog,short listNum,short procID,short cellHeight,short width,short height)
{
	ListHandle		theList=0L;
	Rect			bounds;
	Rect			dimensions={0,0,0,0};
	Point			cellSize={0,0}; // auto calc
	
	cellSize.v=cellHeight;
	bounds=ItemRect(theDialog,listNum);
	bounds.right-=kScrollBarWidth; // scroll bar width
	dimensions.right=width;
	dimensions.bottom=height;
	theList=LNew(&bounds,&dimensions,cellSize,procID,GetDialogWindow(theDialog),true,false,false,true);

	return theList;
}

void UpdateList(DialogPtr theWindow,ListHandle theList)
{
	GrafPtr		savePort;
	PenState	savePen;
	Rect		frame;
	
	GetPort(&savePort);
	SetPortDialogPort(theWindow);
	
	GetPenState(&savePen);
	PenSize(1,1);
	frame=(**theList).rView;
	InsetRect(&frame,-1,-1);
	FrameRect(&frame); // frame ListView
#if TARGET_API_MAC_CARBON
	{
		RgnHandle		visRgn=::NewRgn();
		
		::GetPortVisibleRegion(GetDialogPort(theWindow), visRgn);
		::LUpdate(visRgn,theList);
		::DisposeRgn(visRgn);
	}
#else
	LUpdate(theWindow->visRgn,theList); // update contents (Note: can't use update rgn for some reason)
#endif
	SetPenState(&savePen);
	
	SetPort(savePort);
}

// Gets the entire rect (ie the content rect + the scrollbar(s))
Rect GetFullListRect(ListHandle theList)
{
	Rect		boundsRect;
	
	boundsRect=(**theList).rView;
	if ((**theList).vScroll)
		boundsRect.right+=kScrollBarWidth;
	if ((**theList).hScroll)
		boundsRect.bottom+=kScrollBarWidth;
	
	return boundsRect;
}

void SelectAllInList(ListHandle theList,Boolean select)
{
	Cell	theCell={0,0};
	
	for (theCell.v=(**theList).dataBounds.top; theCell.v<(**theList).dataBounds.bottom; theCell.v++)
	{
		for (theCell.h=(**theList).dataBounds.left; theCell.h<(**theList).dataBounds.right; theCell.h++)
			LSetSelect(select,theCell,theList);
	}
}

// This proc goes throught the list and erases any empty cells. This is useful if you've
// modified the list with LDoDraw off.
void EraseEmptyCells(ListHandle theList)
{
	RgnHandle	dontEraseRgn,eraseRgn;
	GrafPtr		savePort;

	// ready to erase
	GetPort(&savePort);
	SetPort((**theList).port);

	// prepare to erase all of the list
	eraseRgn=NewRgn();
	RectRgn(eraseRgn,&(**theList).rView);
	
	// except for the cell space
	dontEraseRgn=GetVisibleCellsRgn(theList,lDontCare);
	DiffRgn(eraseRgn,dontEraseRgn,eraseRgn);
	
	// do it
	{
		RGBColor		save;
		RGBColor		white={0xFFFF,0xFFFF,0xFFFF};
		GetForeColor(&save);
		RGBForeColor(&white);
		PaintRgn(eraseRgn);
		RGBForeColor(&save);		// erase in white
	}
	
	// finish
	DisposeRgn(eraseRgn);
	DisposeRgn(dontEraseRgn);

	SetPort(savePort);
}

// returns a rgn which encloses the visible cells
// selectState = lDontCare for any cells that are visible to be added
// or lMustBeSelected or lMustntBeSelected
RgnHandle GetVisibleCellsRgn(ListHandle theList,signed char selectState)
{
	Cell		theCell;
	RgnHandle	theRgn=0L,cellRgn=0L;
	Rect		cellRect;

	// opens up some rgns
	theRgn=NewRgn();
	if (!theRgn)
		return 0L;
		
	cellRgn=NewRgn();
	if (!cellRgn)
	{
		DisposeRgn(theRgn);
		return 0L;
	}
		
	for (theCell.v=(**theList).visible.top; theCell.v<(**theList).visible.bottom; theCell.v++)
	{
		for (theCell.h=(**theList).visible.left; theCell.h<(**theList).visible.right; theCell.h++)
		{
			// Does this cell exist? (is it inside the databounds?)
			if (CellExists(theCell,theList))
			{
				if (selectState==lMustBeSelected)
				{
					if (!LGetSelect(false,&theCell,theList))
						continue; // if it's supposed to be selected and isn't then skip it
				}
				else if (selectState==lMustntBeSelected)
				{
					if (LGetSelect(false,&theCell,theList))
						continue; // if it's supposed to be !selected and is then skip it
				}
				
				LRect(&cellRect,theCell,theList);
				RectRgn(cellRgn,&cellRect);
				UnionRgn(cellRgn,theRgn,theRgn);
			}
		}
	}
	
	DisposeRgn(cellRgn);
	
	return theRgn;
}

// returns true if a point is in one of the cells
Boolean PtInCells(Point thePoint,ListHandle theList,signed char selectState)
{
	RgnHandle	theCells=GetVisibleCellsRgn(theList,selectState);
	Boolean		res;
	
	res=PtInRgn(thePoint,theCells);
	DisposeRgn(theCells);
	
	return res;
}

// For a cell to exist it must be inside the lists databounds.
Boolean CellExists(Cell theCell,ListHandle theList)
{
	if (theCell.h>=(**theList).dataBounds.left &&
		theCell.h<(**theList).dataBounds.right &&
		theCell.v>=(**theList).dataBounds.top &&
		theCell.v<(**theList).dataBounds.bottom)
		return true;
	else
		return false;
}

Boolean AnyCellsSelected(ListHandle theList)
{
	Cell theCell={0,0};
	
	return LGetSelect(true,&theCell,theList);
}

short CountSelectedCells(ListHandle theList)
{
	short	count=0;
	
	ForEachSelectedCellDo(theList,false,CountProc,&count);
	return count;
}

// used for selecting next previous in a list
// theList	:	the list handle to do the biz with
// up		:	up if true, down if false
short UpDownList(ListHandle theList,Boolean up,Boolean loopAround)
{
	Cell		theCell={0,0};
	Cell		firstCell={0,0},lastCell={0,0};
	
	// get the first and last selected cells
	if (LGetSelect(true,&firstCell,theList)) // got any cells
	{
		lastCell=firstCell;
		lastCell.v++;
		if (!LGetSelect(true,&lastCell,theList))	// got > 1 cell
			lastCell=firstCell;
	}
	else
	{
		// no cells selected
		firstCell.v=(**theList).dataBounds.bottom;
		lastCell.v=(**theList).dataBounds.top-1;
		if ((**theList).dataBounds.bottom-(**theList).dataBounds.top==0)
			return -1;
	}
	
	// change the cell
	if (up)
	{
		theCell=firstCell;
		theCell.v--;
	}
	else
	{
		theCell=lastCell;
		theCell.v++;
	}
	
	// check for loop
	if (theCell.v<(**theList).dataBounds.top)
	{
		if (loopAround)
			theCell.v=(**theList).dataBounds.bottom-1;
		else
			theCell.v=-1;
	}
	else if (theCell.v>=(**theList).dataBounds.bottom)
	{
		if (loopAround)
			theCell.v=(**theList).dataBounds.top;
		else
			theCell.v=-1;
	}
	
	if (theCell.v!=-1)
		BetterSelectCell(theList,theCell);
	
	return theCell.v;
}

// selects a cell deselecting others first
void BetterSelectCell(ListHandle theList,Cell theCell)
{
	SelectAllInList(theList,false);
	LSetSelect(true,theCell,theList);
	LAutoScroll(theList);
}

Boolean IsCellVisible(ListHandle list,Cell cell)
{
	return FPointInRect(cell,(**list).visible);
}

// This proc scrolls to a cell. Centering it in the list.
void ScrollToCell(ListHandle list,Cell cell)
{
	Rect		visible=(**list).visible;
	short		amountToScrollBy;
	short		centreCell;
	
	centreCell=FRectHeight(visible)+visible.top;
	
	if (cell.v<centreCell)										// Cell if off top of list. Try to scroll it into center of list
	{
		amountToScrollBy=visible.top-cell.v;					// Amount to scroll to get it visible just
		amountToScrollBy+=FRectHeight(visible)/2;				// + Dist to centre
	}
	else
	{
		amountToScrollBy=visible.bottom-cell.v-1;
		amountToScrollBy-=FRectHeight(visible)/2;
	}
			
	LScroll(0,-amountToScrollBy,list);
}

Boolean CountProc(ListHandle theList,Cell theCell,short cellCount,void *refCon)
{
	short	*counter=(short *)refCon;
	
	*counter=cellCount;
	return true;
}

Boolean ForEachSelectedCellDo(ListHandle theList,Boolean mustBeVisible,CellProc routine,void *refCon)
{
	Cell	theCell;
	short	vEndCount,hEndCount,hStartCount,cellCounter=0;
	
	if (mustBeVisible)
	{
		theCell.v=(**theList).visible.top;
		vEndCount=(**theList).visible.bottom;
		theCell.h=(**theList).visible.left;
		hEndCount=(**theList).visible.right;
	}
	else
	{
		theCell.v=(**theList).dataBounds.top;
		vEndCount=(**theList).dataBounds.bottom;
		theCell.h=(**theList).dataBounds.left;
		hEndCount=(**theList).dataBounds.right;
	}
	hStartCount=theCell.h;

	for (; theCell.v<vEndCount; theCell.v++)
	{
		for (theCell.h=hStartCount; theCell.h<hEndCount; theCell.h++)
		{
			// is the cell selected?
			if (LGetSelect(false,&theCell,theList))
			{
				cellCounter++;
				if (!routine(theList,theCell,cellCounter,refCon))
					return false;
			}
		}
	}
	
	return true;
}