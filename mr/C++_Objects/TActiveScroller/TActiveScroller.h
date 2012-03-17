// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TActiveScroller.h
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

#pragma once

typedef class TActiveScroller
{
	private:
		ControlActionUPP				scrollBarProcUPP;
		
		Boolean							disabled;
		
		// The rect which the mouse must stay in for the track to be valid, if the mouse strays out of this the scroll
		// bar snaps to the original position
		Rect							trackRect;	
		// This is the length of the scroll bar in pixels, it is used to calculate the value of the bar from a point
		short							barLength;
		// This is the position that the mouse should be for a reading of control min
		short							barZeroPosition;
		// This is the number which should be added to all values obtained from the CalcValueFromPoint to have keep them
		// accurate. It's value depends on where in the thumb the user clicked
		short							valueSlop;
		
		// This is used to pass the current tracking object to the tracking proc
		static TActiveScroller			*activeObject;
		static TActiveScroller			*GetTrackingObject() {return activeObject;}
		static void						SetTrackingObject(TActiveScroller *newOb) {activeObject=newOb;}
		
		void							IndicatorProc(Point origPoint);
		static pascal void				TrackingProc(ControlHandle theControl,ControlPartCode ctlPart);

	protected:
		ControlHandle					scrollBar;
		short							wasScrollHilight;				// Used when deactivate and activate are called
	
		// Constants
		static const short				kWidthOfScrollArrow=24;
		static const short				kThumbTrackWidthSlop=25;
		static const short				kThumbTrackLengthSlop=113;

		virtual short					CalcScrollBarValueFromPoint(Point inPoint);
		virtual Boolean					IsScrollBarVertical();

	public:
										TActiveScroller(ControlHandle scrollBar);
		virtual							~TActiveScroller();
		
		// Use this to temporarily disable the control and then enable it again. It starts of enabled.
		virtual void					DisableScroller();
		virtual void					EnableScroller();
		virtual Boolean					IsScrollerActive()			{ return !disabled; }
		
		// Override these, to draw the contents of the scrollable pane, take the current value of the scrollBar, set the new
		// value using SetControlValue() and then redraw your pane.
		virtual void					SetScrollBarValue(short value);
		virtual short					GetScrollBarValue();
		
		// Override these if you need to be informed when a scroll starts
		virtual void					SetupScroll() {};
		virtual void					ShutdownScroll() {};
		
		// Override this to handle the kControlPageUpPart/kControlPageDownPart/kControlUpButton/kControlDownButtonPart actions
		virtual void					HandleScrollBarAction(ControlPartCode ctlPart)=0L;
		
		// Call this whenever you resize the scroll bar
		virtual void					AdjustToControlSize();
		
		// Call this to handle a mouse click. If it returns true then the mouse click was handled, else it wasn't
		virtual Boolean					HandleMouseClick(Point inPoint);

} TActiveScroller, *TActiveScrollerPtr;

// Using this class
// The main procs you override are SetScrollBarValue and HandleScrollBarAction. SetScrollBarAction should call the one it's
// overriding at some point and HandleScrollBarAction usually just calls SetScrollBarValue(GetScrollBarValue()+k) with
// k varing depending where the click was.

// Using Enable/Disable Scroller
// These commands are designed to temporarily enable/disable the scroll bars
// When disable is called, it stores the hilite state of the bar. When enable is called it restores it. It assumes that the
// bars state has not changed since you called disable. If it has then the restored state could be wrong.
// The control starts off in the enabled state. You should always call disable before a call to enable.