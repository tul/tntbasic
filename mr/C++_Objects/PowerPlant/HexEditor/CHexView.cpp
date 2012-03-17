// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHexView.cpp
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

// The hex view provides a hex dump of data with a ascii version alongside it. It autosizes the columns when resized.

#include		<LCommander.h>
#include		"TNT_Debugging.h"
#include		"CHexView.h"
#include		"CHexEditorPane.h"
#include		"Marks Routines.h"
#include		"Utility Objects.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
CHexView::CHexView(
	LStream			*inStream) :
	inheritedPane(inStream)
{
	mHexPane=mAsciiPane=0L;
	mEditing=false;
	mSelectionStart=mSelectionEnd=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetSelection
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the current selection
void CHexView::GetSelection(
	CDataStore::TDataOffset		&outSelStart,
	CDataStore::TDataOffset		&outSelEnd)
{
	outSelStart=mSelectionStart;
	outSelEnd=mSelectionEnd;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ AdjustSelection
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Changes the selection without updating or refreshing the view
void CHexView::AdjustSelection(
	CDataStore::TDataOffset		inSelStart,
	CDataStore::TDataOffset		inSelEnd)
{
	CDataStore::TDataOffset		max=mHandleStore.GetLength();

	inSelStart=Limit(inSelStart,0,max);
	inSelEnd=Limit(inSelEnd,inSelStart,max);
	
	// If it's changed
	if ((inSelStart!=mSelectionStart) || (inSelEnd!=mSelectionEnd))
	{
		mSelectionStart=inSelStart;
		mSelectionEnd=inSelEnd;
	
		mHexPane->AdjustSelection(mSelectionStart,mSelectionEnd);
		mAsciiPane->AdjustSelection(mSelectionStart,mSelectionEnd);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ InsertData														/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Inserts some data between the inDestStart and inDestEnd markers (which would in general be set to the selection but we
// must allow flexibility for the hex edit pane to modify an existing unseleced char when the second nibble is typed)
// The caller should adjust selection markers accordingly
void CHexView::InsertData(
	CDataStore::TDataOffset		inDestStart,
	CDataStore::TDataOffset		inDestEnd,
	unsigned char				*inData,
	CDataStore::TDataOffset		inLength)
{
	// Do we need to change the length of the store?
	CDataStore::TDataOffset		destLength=inDestEnd-inDestStart;
	
	if (destLength!=inLength)
	{
		// Resize the store, then insert the new data
		mHandleStore.ResizeStore(inDestStart,destLength,inLength);

		ThrowIfOSErr_(mHandleStore.PutBytes(inDestStart,inData,inLength));
		ReflowData();
		mHexPane->RedrawDataRange(inDestStart,mHandleStore.GetLength());
		mAsciiPane->RedrawDataRange(inDestStart,mHandleStore.GetLength());		
	}
	else
	{
		ThrowIfOSErr_(mHandleStore.PutBytes(inDestStart,inData,inLength));
		mHexPane->RedrawDataRange(inDestEnd-destLength,inDestEnd);
		mAsciiPane->RedrawDataRange(inDestEnd-destLength,inDestEnd);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ DeleteDataRange													/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Delete the data between the ranges specified. Caller should call adjust selection to ensure the selection markers are
// properly adjusted.
void CHexView::DeleteDataRange(
	CDataStore::TDataOffset		inSelStart,
	CDataStore::TDataOffset		inSelEnd)
{
	mHandleStore.ResizeStore(inSelStart,inSelEnd-inSelStart,0);

	ReflowData();
	mHexPane->RedrawDataRange(inSelStart,mHandleStore.GetLength());
	mAsciiPane->RedrawDataRange(inSelStart,mHandleStore.GetLength());
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetSelection
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the selection, if the hex view isn't editing then no visual changes take place, otherwise the selection is redrawn.
void CHexView::SetSelection(
	CDataStore::TDataOffset		inSelStart,
	CDataStore::TDataOffset		inSelEnd)
{
	CDataStore::TDataOffset		max=mHandleStore.GetLength();

	inSelStart=Limit(inSelStart,0,max);
	inSelEnd=Limit(inSelEnd,inSelStart,max);
	
	// If it's changed
	if ((inSelStart!=mSelectionStart) || (inSelEnd!=mSelectionEnd))
	{
		mSelectionStart=inSelStart;
		mSelectionEnd=inSelEnd;
	
		mHexPane->SetSelection(mSelectionStart,mSelectionEnd);
		mAsciiPane->SetSelection(mSelectionStart,mSelectionEnd);
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetDescriptor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the current contents of the hex view
void CHexView::GetDescriptor(
	AEDesc			&outDesc)
{
	Size		length=mHandleStore.GetLength();
	
	#if TARGET_API_MAC_CARBON
		Handle			h=mHandleStore.GetDataHandle(false);
		UHandleLocker	lock(h,true);
		ThrowIfOSErr_(::AECreateDesc(typeData,*h,length,&outDesc));
	#else
		Handle		handle=::NewHandle(length);
		ThrowIfMemFull_(handle);
		
		::BlockMoveData(*mHandleStore.GetDataHandle(false),*handle,length);
		
		outDesc.descriptorType=typeData;
		outDesc.dataHandle=handle;
	#endif
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ SetDescriptor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the contents of the hex view
void CHexView::SetDescriptor(
	const AEDesc	&inDesc)
{
	// Create a handle based data store for the data
	#if TARGET_API_MAC_CARBON
	Handle			dataHandle=GetAEDataHandle(inDesc);
	#else
	Handle			dataHandle=inDesc.dataHandle;	
	ThrowIfOSErr_(::HandToHand(&dataHandle));
	#endif
	
	Size			handleSize=::GetHandleSize(dataHandle);
	
	mHandleStore.SetDataHandle(dataHandle,true);
	
	// Do a select all
//	mHexPane->SetSelection(0,handleSize);
//	mAsciiPane->SetSelection(0,handleSize);
	SetSelection(0,handleSize);
	
	ReflowData();
	
	Refresh();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ReflowData
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Resizes the image to be the ideal height.
void CHexView::ReflowData()
{
	// Resize the image
	SDimension32	imageSize;
	
	GetImageSize(imageSize);
	
	ResizeImageTo(imageSize.width,mHexPane->GetIdealHeight(),true);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ ResizeFrameBy
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Overridden to reflow the text if the width changes
void CHexView::ResizeFrameBy(
	SInt16		inWidthDelta,
	SInt16		inHeightDelta,
	Boolean		inRefresh)
{
	inheritedPane::ResizeFrameBy(inWidthDelta,inHeightDelta,inRefresh);

	if (inWidthDelta)
	{	
		// Reposition the sub panes, we only bother about moving them on the x axis as their height will be correct
		Rect			hexPane,asciiPane;
		SDimension16	frame;
		
		CalcSubPaneRects(hexPane,asciiPane);
		
		mHexPane->PlaceInSuperImageAt(hexPane.left,hexPane.top,true);
		mHexPane->GetFrameSize(frame);
		mHexPane->ResizeFrameTo(hexPane.right-hexPane.left,frame.height,true);
		
		mAsciiPane->PlaceInSuperFrameAt(asciiPane.left,asciiPane.top,true);
		mAsciiPane->GetFrameSize(frame);
		mAsciiPane->ResizeFrameTo(asciiPane.right-asciiPane.left,frame.height,true);
		
		ReflowData();
	}
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalcSubPaneRects
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the rectangles inside of this view which the two sub panes should occupy.
void CHexView::CalcSubPaneRects(
	Rect			&outHexRect,
	Rect			&outASCIIRect)
{
	SDimension16	frameSize;
	
	GetFrameSize(frameSize);
	
	if (frameSize.width>0)
	{
		UInt16		availableSpace=frameSize.width-kGapBetweenSubPanes-CHexEditorPane::kLeftIndent*2-CHexEditorPane::kRightIndent*2;		
		UInt16		fullBlocksPerRow=availableSpace/CHexEditorPane::k4ByteAsciiAndHexBlockPixelWidth;
		UInt16		halfBlocksPerRow=availableSpace%CHexEditorPane::k4ByteAsciiAndHexBlockPixelWidth;
		
		if (halfBlocksPerRow>=CHexEditorPane::k2ByteAsciiAndHexBlockPixelWidth)
			halfBlocksPerRow=1;
		else
			halfBlocksPerRow=0;
				
		FSetRect(outHexRect,0,0,frameSize.height,CHexEditorPane::kLeftIndent+CHexEditorPane::kRightIndent+fullBlocksPerRow*CHexEditorPane::k4ByteHexBlockPixelWidth+halfBlocksPerRow*CHexEditorPane::k2ByteHexBlockPixelWidth);
		FSetRect(outASCIIRect,0,0,frameSize.height,CHexEditorPane::kLeftIndent+CHexEditorPane::kRightIndent+fullBlocksPerRow*CHexEditorPane::k4ByteAsciiBlockPixelWidth+halfBlocksPerRow*CHexEditorPane::k2ByteAsciiBlockPixelWidth);
		
		// Align the ascii rect to the right hand side of the view
		outASCIIRect.left=frameSize.width-outASCIIRect.right;
		outASCIIRect.right=frameSize.width;
	}
	else
	{
		FSetRect(outHexRect,0,0,0,0);
		FSetRect(outASCIIRect,0,0,0,0);
	}	
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ FinishCreateSelf									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Creates the hex and ascii sub panes inside this view
void CHexView::FinishCreateSelf()
{
	Rect			hexPaneRect,asciiPaneRect;
	
	CalcSubPaneRects(hexPaneRect,asciiPaneRect);

	SPaneInfo		paneInfo=
					{
						0L,
						hexPaneRect.right-hexPaneRect.left,
						hexPaneRect.bottom-hexPaneRect.top,
						true,
						true,
						{ true, true, false, true },
						hexPaneRect.left,
						hexPaneRect.top,
						0,
						this
					};
		
	mHexPane=new CHexEditorPane(LCommander::GetDefaultCommander(),paneInfo,true);
	ThrowIfMemFull_(mHexPane);	

	paneInfo.width=asciiPaneRect.right-asciiPaneRect.left;
	paneInfo.height=asciiPaneRect.bottom-asciiPaneRect.top;
	paneInfo.top=asciiPaneRect.top;
	paneInfo.left=asciiPaneRect.left;

	mAsciiPane=new CHexEditorPane(LCommander::GetDefaultCommander(),paneInfo,false);
	ThrowIfMemFull_(mAsciiPane);	

	mHexPane->SetDataStore(&mHandleStore);
	mAsciiPane->SetDataStore(&mHandleStore);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StartEdit
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Changes the status of this to be editing - redraws sub panes in their "editing" mode
void CHexView::StartEdit()
{
	mEditing=true;
	
	mHexPane->SetEditStatus(true);
	mAsciiPane->SetEditStatus(true);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ StopEdit
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Stops the edit. Redraws sub panes in non-edit mode. If true then the changes are committed, if false they are undone.
void CHexView::StopEdit(
	bool			inFixChanges)
{
	mEditing=false;

	mHexPane->SetEditStatus(false);
	mAsciiPane->SetEditStatus(false);
}
