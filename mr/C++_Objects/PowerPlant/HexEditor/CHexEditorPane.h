// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHexEditorPane.h
// © Mark Tully and TNT Software 1999
// 18/9/99
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1999, Mark Tully and John Treece-Birch
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

#include				<LPane.h>
#include				<LCommander.h>
#include				<LPeriodical.h>
#include				"CDataStore.h"

class					CHexView;

class CHexEditorPane : public LPane, public LCommander, public LPeriodical
{
	private:
		typedef LPane				inheritedPane;

	protected:
		CDataStore					*mDataStore;
		SInt16						mFontId;
		bool						mDisplayAsHex;
		bool						mEditing;
		CDataStore::TDataOffset		mSelStart,mSelEnd;
		bool						mCaretPhase;					// If true caret is black, else it's white
		bool						mOfficialTargetStatus;
		bool						mWaitingSecondNibble;			// only used by the hex editor pane for typing multi char entries
		UInt32						mLastCaretToggleTime;

		virtual void				ReplaceInvisibles(
										unsigned char			*ioCharsBuffer,
										CDataStore::TDataOffset	inCount);

		virtual RgnHandle /*e*/		GetRangeRegion(
										CDataStore::TDataOffset		inSelStart,
										CDataStore::TDataOffset		inSelEnd);

		virtual CDataStore::TDataOffset PointToOffset(
										Point						inLocalPoint);

		virtual UInt16				OffsetToXCoord(
										CDataStore::TDataOffset		inOffset);
			
		virtual void				ClickLoop(
										const SMouseDownEvent		&inMouseDown);

		enum TCaretState
		{
			kCaretOn,
			kCaretOff,
			kCaretDimmed
		};

		virtual void				HandleKey(
										unsigned char				inKey);
		virtual void				DrawHilight();
		virtual void				DrawCaret(
										TCaretState					inState);

		virtual void				UserChangedText()				{}

		virtual	void				SpendTime(
										const EventRecord&	inMacEvent);
	
	public:
		typedef UInt16				TRowIndex;
		static const UInt16			kRowHeight=15;
		static const UInt16			kCaretHeight=12;
		static const UInt16			kCaretFlashTime=30;		// ticks
		static const UInt16			kLeftIndent=4;
		static const UInt16			kRightIndent=3;
		static const UInt16			kFontDescent=4;
		static const UInt16			kHilightIndent=2;
		#define						cDisabledFrame	{0x6666,0x6666,0x6666}
		
		static const unsigned char	kMissingCharSymbol='.';

		static const UInt16			kMonaco9CharWidth=6;				
		static const UInt16			kBlockSpaceWidth=6;
		
		static const UInt16			k2ByteHexBlockPixelWidth=kMonaco9CharWidth*4+kBlockSpaceWidth;
		static const UInt16			k4ByteHexBlockPixelWidth=kMonaco9CharWidth*8+kBlockSpaceWidth;

		static const UInt16			k2ByteAsciiBlockPixelWidth=kMonaco9CharWidth*2;
		static const UInt16			k4ByteAsciiBlockPixelWidth=kMonaco9CharWidth*4;

		static const UInt16			k2ByteAsciiAndHexBlockPixelWidth=k2ByteHexBlockPixelWidth+k2ByteAsciiBlockPixelWidth;
		static const UInt16			k4ByteAsciiAndHexBlockPixelWidth=k4ByteHexBlockPixelWidth+k4ByteAsciiBlockPixelWidth;
		
		/*e*/						CHexEditorPane(
										LCommander	*inSuperCommander,
										SPaneInfo	&inPaneInfo,
										bool		inDisplayAsHex);

		virtual void				SetEditStatus(
										bool		inEditing);

		virtual void				SetSelection(
										CDataStore::TDataOffset		inSelStart,
										CDataStore::TDataOffset		inSelEnd);
		virtual void				AdjustSelection(
										CDataStore::TDataOffset		inSelStart,
										CDataStore::TDataOffset		inSelEnd);

		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//	¥ View overrides
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		virtual void				ClickSelf(
										const SMouseDownEvent		&inMouseDown);
		virtual void 				DrawSelf();
/*		virtual void				ResizeFrameBy(
										SInt16						inWidthDelta,
										SInt16						inHeightDelta,
										Boolean						inRefresh);*/

		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		//	¥ Commander overrides
		// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
		virtual void				DontBeTarget();
		virtual void				BeTarget();
		virtual Boolean				HandleKeyPress(
										const EventRecord&			inKeyEvent);


		virtual void				RedrawDataRange(
										CDataStore::TDataOffset		inStart,
										CDataStore::TDataOffset		inEnd);

		CDataStore::TDataOffset		CalcMaxBytesPerRow();
		virtual SInt32				GetIdealHeight();
		
		virtual CHexView			*GetHexView();
		
		virtual void				SetDataStore(
										CDataStore	*inDataStore);

};