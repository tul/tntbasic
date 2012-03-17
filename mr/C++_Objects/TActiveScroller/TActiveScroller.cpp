// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TActiveScroller.cpp
// This object allows you to do a real time update scroll
// Mark Tully
// 13/12/97
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1997, Mark Tully and John Treece-Birch
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

#include	"TNT_Debugging.h"
#include	"TActiveScroller.h"
#include	"Utility Objects.h"

TActiveScroller		*TActiveScroller::activeObject=0L;

TActiveScroller::TActiveScroller(ControlHandle scrollBar)
{
	disabled=false;
	this->scrollBar=scrollBar;
	wasScrollHilight=kControlInactivePart;
	
	// make the tracking proc
	scrollBarProcUPP=NewControlActionUPP(TrackingProc);
	if (!scrollBarProcUPP)
		Throw_(memFullErr);

	AdjustToControlSize();
}

TActiveScroller::~TActiveScroller()
{
	DisposeControlActionUPP(scrollBarProcUPP);
}

void TActiveScroller::DisableScroller()
{
	if (disabled)
		return;
	disabled=true;

	#if TARGET_API_MAC_CARBON==1
		wasScrollHilight=GetControlHilite(scrollBar);
	#else
		wasScrollHilight=(**scrollBar).contrlHilite;
	#endif
	if (wasScrollHilight!=kControlInactivePart)
		HiliteControl(scrollBar,kControlInactivePart);
}

void TActiveScroller::EnableScroller()
{
	UInt8		temp;

	if (!disabled)
		return;
	disabled=false;

	#if TARGET_API_MAC_CARBON==1
		temp=GetControlHilite(scrollBar);
	#else
		temp=(**scrollBar).contrlHilite;
	#endif
		
	if (wasScrollHilight!=temp)
		HiliteControl(scrollBar,wasScrollHilight);
}

void TActiveScroller::SetScrollBarValue(short value)
{
	#if TARGET_API_MAC_CARBON==1
		Rect		rect;	
		GetControlBounds(scrollBar,&rect);
		UClipSaver		safe(rect);
	#else
		UClipSaver		safe((**scrollBar).contrlRect);	
	#endif
	
	SetControlValue(scrollBar,value);
}

short TActiveScroller::GetScrollBarValue()
{
	return GetControlValue(scrollBar);
}

Boolean TActiveScroller::HandleMouseClick(Point inPoint)
{
	#if TARGET_API_MAC_CARBON==1
		UPortSaver		safe(GetControlOwner(scrollBar));
	#else
		UPortSaver		safe((**scrollBar).contrlOwner);
	#endif

	short			partCode;

	if (partCode=TestControl(scrollBar,inPoint))
	{
		switch (partCode)
		{
			case kControlUpButtonPart:
			case kControlDownButtonPart:
			case kControlPageDownPart:
			case kControlPageUpPart:
			case kControlIndicatorPart:
				SetupScroll();
				if (partCode==kControlIndicatorPart)
				{
					// Dim the thumb
					HiliteControl(scrollBar,kControlIndicatorPart);
					IndicatorProc(inPoint);
					// Undim the thumb
					HiliteControl(scrollBar,kControlNoPart);
				}
				else
				{
					// Must call this to pass the current object to the tracking proc
					SetTrackingObject(this);
					TrackControl(scrollBar,inPoint,scrollBarProcUPP);
				}
				ShutdownScroll();
				return true;
				break;
		}
	}
	
	
	return false;	// missed the scroll bar
}

void TActiveScroller::AdjustToControlSize()
{
	// Recalculate the track rect
	#if TARGET_API_MAC_CARBON==1	
		GetControlBounds(scrollBar,&trackRect);
	#else
		trackRect=(**scrollBar).contrlRect;	
	#endif
	
	// How we set up the values for the tracking bar size and pin location
	// is determined by whether we are tracking a horizontal or vertical
	// scroll bar.  In either case, we calcualte the scroll bar rect
	// without the scroll arrows.  We then enlarge that rect for the drag
	// slop area.
	if (IsScrollBarVertical())
	{
		barLength=FRectHeight(trackRect)-(kWidthOfScrollArrow*2);
		barZeroPosition=trackRect.top+kWidthOfScrollArrow;
		
		FInsetRect(trackRect,-kThumbTrackWidthSlop,-kThumbTrackLengthSlop);
	}
	else
	{
		barLength=FRectWidth(trackRect)-(kWidthOfScrollArrow * 2);
		barZeroPosition=trackRect.left+kWidthOfScrollArrow;
		
		FInsetRect(trackRect,-kThumbTrackLengthSlop,-kThumbTrackWidthSlop);
	}
}

pascal void TActiveScroller::TrackingProc(ControlHandle theControl,ControlPartCode ctlPart)
{
	GetTrackingObject()->HandleScrollBarAction(ctlPart);
}

void TActiveScroller::IndicatorProc(Point origPoint)
{
	#if TARGET_API_MAC_CARBON==1
		UPortSaver			safe(GetControlOwner(scrollBar));		
	#else
		UPortSaver			safe((**scrollBar).contrlOwner);
	#endif
	Point				lastPoint,currentPoint;
	short				value;
	short				originalValue=GetScrollBarValue();
	
	// While we're tracking the thumb, we're calculating its value based
	// on the current mouse location.  That works great, assuming you
	// clicked _exactly_ in the center of the thumb.  We calculate how
	// much we need to compensate the value caluculations based on the
	// click distance from the center of the thumb.	
	valueSlop=0;		// Important as the valueSlop is used in the CalcScrollBarValueFromPoint proc
	valueSlop=originalValue-CalcScrollBarValueFromPoint(origPoint);
	currentPoint=lastPoint=origPoint;

	while (StillDown())
	{
		// Only attempt to update the value if the mouse moved.
		GetMouse(&currentPoint);
		
		if (EqualPt(currentPoint,lastPoint))
			continue;
		
		// Remeber where the last mouse location was 
		lastPoint=currentPoint;				
		
		// Check to see if the user tracked outside of the slop rect.
		// If they did, restore the original value of the control.
		// This mimics the same behaviour as the standard scroll bar.
		if (FPointInRect(currentPoint,trackRect))
			value=CalcScrollBarValueFromPoint(currentPoint);
		else
			value=originalValue;
		
		value=Limit(value,::GetControlMinimum(scrollBar),::GetControlMaximum(scrollBar));
		
		SetScrollBarValue(value);
	}
}

Boolean TActiveScroller::IsScrollBarVertical()
{
	Rect		rect;

	#if TARGET_API_MAC_CARBON==1	
		GetControlBounds(scrollBar,&rect);
	#else
		rect=(**scrollBar).contrlRect;
	#endif
	
	return (FRectHeight(rect)>FRectWidth(rect));
}

short TActiveScroller::CalcScrollBarValueFromPoint(Point inPoint)
{
	short		value;

	if (IsScrollBarVertical())
		value=((inPoint.v-barZeroPosition) * (GetControlMaximum(scrollBar)-GetControlMinimum(scrollBar))) / barLength;
	else
		value=((inPoint.h-barZeroPosition) * (GetControlMaximum(scrollBar)-GetControlMinimum(scrollBar))) / barLength;
	
	value+=valueSlop;
	
	return value;
}
