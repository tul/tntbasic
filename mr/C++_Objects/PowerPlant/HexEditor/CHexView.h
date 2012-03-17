// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHexView.h
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

#include		<LView.h>
#include		"CAEDescDescriptor.h"
#include		"CHandleDataStore.h"

class CHexView : public LView, public CAEDescDescriptor
{
	private:
		typedef LView				inheritedPane;

	protected:
		class CHexEditorPane		*mHexPane,*mAsciiPane;
		CHandleDataStore			mHandleStore;
		
		bool						mEditing;
		CDataStore::TDataOffset		mSelectionStart,mSelectionEnd;

		virtual void /*e*/			FinishCreateSelf();
	
		virtual void				ReflowData();
		virtual void				CalcSubPaneRects(
										Rect			&outHexRect,
										Rect			&outASCIIRect);
		
		static const UInt16			kGapBetweenSubPanes=0;
	
	public:
		enum { class_ID = FOUR_CHAR_CODE('HxVw') };
	
		/*e*/						CHexView(
										LStream			*inStream);

		virtual void /*e*/			GetDescriptor(
										AEDesc			&outDesc);
		virtual void /*e*/			SetDescriptor(
										const AEDesc	&inDesc);

		virtual void				GetSelection(
										CDataStore::TDataOffset		&outSelStart,
										CDataStore::TDataOffset		&outSelEnd);
		virtual void				SetSelection(
										CDataStore::TDataOffset		inSelStart,
										CDataStore::TDataOffset		inSelEnd);
		virtual void				AdjustSelection(
										CDataStore::TDataOffset		inSelStart,
										CDataStore::TDataOffset		inSelEnd);		

		virtual void /*e*/			ResizeFrameBy(
										SInt16			inWidthDelta,
										SInt16			inHeightDelta,
										Boolean			inRefresh);

		virtual void /*e*/			InsertData(
										CDataStore::TDataOffset		inSelStart,
										CDataStore::TDataOffset		inSelEnd,
										unsigned char				*inData,
										CDataStore::TDataOffset		inLength);

		virtual void /*e*/			DeleteDataRange(
										CDataStore::TDataOffset		inSelStart,
										CDataStore::TDataOffset		inSelEnd);
		
		virtual bool				IsEditing()						{ return mEditing; }
		virtual bool				IsEditable()					{ return true; }
		virtual void				StartEdit();
		virtual void				StopEdit(
										bool		inFixChanges);
};