// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TListViewScroller.cpp
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

#include			"TListViewScroller.h"
#include			"TListView.h"

// What is the hangover?
// ---------------------
// Well, you know when you drink too much, well the next day...
// No.
// If the scrollable distance is not a multiple of the scroll scale then there will be half a cell or so at the bottom of
// the list. The hangover specifies the number of pixels which are missing. The hangover is activated when the list gets to
// the bottom to scroll the list the extra few pixels. It's deactivated at the top of the list.

TListViewScroller::TListViewScroller(TListView *masterListView,ControlHandle scrollBar) : TActiveScroller(scrollBar)
{
	scrollScale=1;
	this->masterListView=masterListView;
	scrollableDistance=0;
	hangOver=0;
	hangOverActive=false;
	value=::GetControlValue(scrollBar);
	max=::GetControlMaximum(scrollBar);
}

TListViewScroller::~TListViewScroller()
{
	DisposeControl(scrollBar);
}

void TListViewScroller::SetScrollBarPosition(const Rect &newPosition)
{
#if TARGET_API_MAC_CARBON
	::SetControlBounds(scrollBar,&newPosition);
#else
	(**scrollBar).contrlRect=newPosition;
#endif
	AdjustToControlSize();
	SetTotalScrollableDistance(GetTotalScrollableDistance());
}
	
void TListViewScroller::SetControlMax(short newMax)
{
	max=newMax;
	if (masterListView->IsListViewUpdating())
		::SetControlMaximum(scrollBar,newMax);

	if (value>max)
		SetControlValue(max);
}

void TListViewScroller::SetControlValue(short newVal)
{
	value=Limit(newVal,0,max);
	if (masterListView->IsListViewUpdating())
		SetScrollBarValue(value);
}

void TListViewScroller::UpdateScrollBar(Boolean forceUpdate)
{
	// Only redraw the scroll bar if it's changed or if we are being forced too
	// When the window is updating we are forced too
	// When list viewing is reenabled we only draw if it changed

	// Load the current values into the scroll bar and update it
#if TARGET_API_MAC_CARBON
	if (max!=::GetControlMaximum(scrollBar) || value!=::GetControlValue(scrollBar) || forceUpdate)
#else
	if (max!=(**scrollBar).contrlMax || value!=(**scrollBar).contrlValue || forceUpdate)
#endif
	{
		::SetControlMaximum(scrollBar,max);
		::SetControlValue(scrollBar,value);
		
		Draw1Control(scrollBar);
	}
}

void TListViewScroller::HandleScrollBarAction(ControlPartCode ctlPart)
{
	Rect				contentRect=masterListView->GetListViewContentRect();
	TCellHeight			unitsToPage=FRectHeight(contentRect)/GetScrollScale();

	if (unitsToPage>1)
		unitsToPage--;
	else
		unitsToPage=1;

	switch (ctlPart)
	{
		case kControlPageUpPart:
			SetControlValue(GetControlValue()-unitsToPage);
			break;
			
		case kControlPageDownPart:
			SetControlValue(GetControlValue()+unitsToPage);
			break;
			
		case kControlUpButtonPart:
			SetControlValue(GetControlValue()-1);
			break;
		
		case kControlDownButtonPart:
			SetControlValue(GetControlValue()+1);
			break;
	}
}

void TListViewScroller::SetScrollBarValue(short value)
{
	value=Limit(value,0,max);
	this->value=value;

	signed short		delta=value-GetScrollBarValue();
	TActiveScroller::SetScrollBarValue(value);					// Must install the new value before calling scrollContent

	signed short		distanceToScroll=delta*GetScrollScale();

	// Just hit the max, enable hangOver
	if (value==max && !hangOverActive)
	{
		hangOverActive=true;
		distanceToScroll-=hangOver;
	}
	if (value==0 && hangOverActive)
	{
		hangOverActive=false;
		distanceToScroll+=hangOver;
	}

	masterListView->ScrollContent(distanceToScroll);
}

TCellHeight TListViewScroller::GetDistanceToTop()
{
	TCellHeight		dist=GetControlValue()*GetScrollScale();
	
	if (hangOverActive)
		dist-=hangOver;
	
	return dist;
}

void TListViewScroller::SetTotalScrollableDistance(TCellHeight newScrollableDistance)
{	
	scrollableDistance=newScrollableDistance;
	
	// Work out the new control maximum
	Rect			contentRect=masterListView->GetListViewContentRect();
	TCellHeight		dist;
	
	if (newScrollableDistance>FRectHeight(contentRect))
		dist=newScrollableDistance-FRectHeight(contentRect);
	else
		dist=0;
		
	short			newMax=dist/GetScrollScale();
	if (newMax<0)
		newMax=0;
		
	// Work out the hangover
	hangOverActive=false;
	hangOver=GetScrollScale()-dist%GetScrollScale();
	if (hangOver==GetScrollScale())
		hangOver=0;
	if (hangOver)
		newMax++;
	
	SetControlMax(newMax);
}

void TListViewScroller::SetScrollScale(TCellHeight newScale)
{
	TCellHeight			wasDistToTop=GetDistanceToTop();

	if (!newScale)
		newScale=1;
	scrollScale=newScale;

	masterListView->SetListViewUpdating(false);
	SetTotalScrollableDistance(GetTotalScrollableDistance());	
	SetControlValue(wasDistToTop/newScale);	
	masterListView->InvalRect(masterListView->GetListViewContentRect());
	masterListView->SetListViewUpdating(true);
}







