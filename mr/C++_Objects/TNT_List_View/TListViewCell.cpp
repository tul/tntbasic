// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TListViewCell.cpp
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

#include			"TListViewCell.h"
#include			"TListView.h"

TListViewCell::TListViewCell(TCellHeight cellHeight,void *refCon) :
	link(this)
{
	this->refCon=refCon;
	this->cellHeight=cellHeight;
	distanceToCellTop=0;
	selected=false;
	selectionSnapShot=false;
	bossListView=0L;
}

TListViewCell::~TListViewCell()
{
	if (bossListView)
		bossListView->CellDeleted(this);
}

void TListViewCell::GetSelectRgn(RgnHandle rgn)
{
	Rect		temp=GetCellRect();

	::RectRgn(rgn,&temp);
}

void TListViewCell::SetCellSelection(Boolean newState)
{
	if (selected==newState)
		return;
	
	selected=newState;
	
	if (bossListView)
		bossListView->CellSelectionChanged(this,newState);
	
	// Draw the cell in it's new state
	InvalCell();
}

TCellIndex TListViewCell::GetCurrentCellIndex()
{
	return link.GetCurrentIndex();
}

TListViewPtr TListViewCell::GetListView()
{
	return bossListView;
}

void TListViewCell::SetCellHeight(TCellHeight newHeight)
{
	signed short		delta=newHeight-cellHeight;

	cellHeight=newHeight;

	if (bossListView)
		bossListView->CellSizeChanged(this,delta);
}

TCellHeight TListViewCell::GetCellHeight()
{
	return cellHeight;
}

WindowPtr TListViewCell::GetDrawPort()
{
	if (bossListView)
		return bossListView->GetDrawPort();
	else
		return 0L;
}

Rect TListViewCell::GetClipRect()
{
	if (bossListView)
		return bossListView->GetListViewContentRect();
	else
	{
		Rect	rect={0,0,0,0};
	
		return rect;
	}
}

Rect TListViewCell::GetCellRect()
{
	if (bossListView)
	{
		return bossListView->GetCellsRect(this);
	}
	else
	{
		Rect	rect={0,0,0,0};
	
		return rect;
	}
}

void TListViewCell::InvalCell()
{
	if (bossListView)
		bossListView->InvalRect(GetCellRect());
}


