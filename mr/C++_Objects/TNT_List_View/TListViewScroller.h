// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TListViewScroller.h
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

#include							"TActiveScroller.h"
#include							"TListViewCell.h"

class TListView;

typedef class TListViewScroller : public TActiveScroller
{
	private:
		short						max,value;
		TListView					*masterListView;
		TCellHeight					scrollScale;
		TCellHeight					scrollableDistance;
		TCellHeight					hangOver;
		Boolean						hangOverActive;

		virtual void				SetControlMax(short newMax);
		
		virtual short				GetControlMax()					{ return max; }

		virtual void				HandleScrollBarAction(ControlPartCode ctlPart);
		virtual void				SetScrollBarValue(short newValue);


	public:
									TListViewScroller(TListView *masterListView,ControlHandle scrollBar);
									~TListViewScroller();
	
		virtual TCellHeight			GetScrollScale()							{ return scrollScale; }
		virtual void				SetScrollScale(TCellHeight newScale);
		virtual void				SetControlValue(short newVal);
		virtual short				GetControlValue()							{ return value; }

		virtual TCellHeight			GetDistanceToTop();
		virtual void				SetTotalScrollableDistance(TCellHeight newScrollableDist);
		virtual TCellHeight			GetTotalScrollableDistance()	{ return scrollableDistance; }

		virtual void				SetScrollBarPosition(const Rect &newPosition);
		virtual void				UpdateScrollBar(Boolean forceUpdate);

} TListViewScroller;
