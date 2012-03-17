// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TAppleUIGuideLines.h
// Mark Tully
// 18/7/98
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

typedef class TAppleUIGuideLines
{

	public:
		static const short			kOKButtonWidth=/*58*/69;										// OK/Cancel buttons are a standard width
		static const short			kCancelButtonWidth=kOKButtonWidth;
		static const short			kButtonHeight=20;										// System font button
		static const short			kCheckBoxHeight=18;										// System font CB
		static const short			kDistOfControlFromHorizEdge=/*9*/24;					// Can be as little as 4 but recommended at 12. Taking into account the width of the dialog border this becomes 9
		static const short			kDistOfControlFromBottomEdge=/*9*/20;					// Can be as little as 4 but recommended at 12. Taking into account the width of the dialog border this becomes 9
		static const short			kDistBetweenStaticTexts=6;
		static const short			kDistToLeftMostAlertItem=71;							// Left most item is often the static text. 68 is width of icon (32) + 18 (dist of icon from edge) + 18 (dist to static text from icon) + 3 to take into account the border of the dialog (to make it look balanced)
		static const short			kDistToLeftMostAlertItem_OSX=kDistToLeftMostAlertItem+32;	// Has a bigger icon on OS X, 64x64 instead of 32x32
		static const short			kDistBetweenHorizButtons=12;
		static const short			kDistBetweenVertButtons=10;
		static const short			kDistFromButtonTextToButtonEdge=/*8*/14;				// Button width = StringWidth(text)+2*kDistFromButtonTextToButtonEdge
		static const short			kGeneva9LineHeight=13;
		static const short			kSystemFontLineHeight=16;
		
} TAppleUIGuideLines;