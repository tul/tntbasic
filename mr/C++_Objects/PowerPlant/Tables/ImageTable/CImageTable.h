// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CImageTable.h
// © Mark Tully and TNT Software 2000
// 16/8/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2000, Mark Tully and John Treece-Birch
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

#include <LTableView.h>

class StRegion;

class CImageCell
{
	friend class CImageTable;
	friend class CImageTableSelector;

	protected:
		class CImageTable	*mImageTable;
		bool				mSelected;

		virtual void		DoubleClick(
								const STableCell&			inCell,
								const SMouseDownEvent&		inMouseDown) {}
		
		virtual bool		GetSelected()					{ return mSelected; }
		virtual void		SetSelected(
								bool		inSel)			{ mSelected=inSel; }
	
	public:
							CImageCell() :
								mImageTable(0),
								mSelected(false)
								{}

		CImageTable			*GetImageTable() { return mImageTable; }

		virtual void /*e*/	GetDragRegion(
								const STableCell			&inCell,
								const Rect					&inCellRect,
								StRegion					&outGRegion);

		virtual void		Click(
								const STableCell			&inCell,
								const SMouseDownEvent		&inMouseDown);									

		virtual void		DrawCell(
								const STableCell			&inCell,
								const Rect					&inRect,
								bool						inIsSelected,
								bool						inIsEnabled) = 0L;
};

class CImageTable : public LTableView
{
	friend class CImageCell;

	private:
		typedef LTableView	inheritedTable;

	protected:	
		SInt32				mCellWidth,mCellHeight;
		SInt16				mTextTraitsId;
		TArray<CImageCell*>	mImageCells;
		TableIndexT			mIBarLoc;
		bool				mIBarOnRhs;
		
		enum EColumnMode
		{
			kSingleVColumn=0,							// A single vertical column of cells
			kWrappingColumns=1							// Cells rewrap like text does
		}					mColMode;
		enum EDragAndDropMode
		{
			kOff=0,										// No drag and drop
			kOrderedDrag,								// Cells can be reordered
			kUnorderedDrag								// Cells aren't ordered
		}					mDragMode;
		static const SInt16	kIBarWidth=2;
		static const SInt16 kDnDBorderWidth=3;			// drag and drop border width

		bool				mSupportsCellDupe;

		virtual void		DrawIBar(
								TableIndexT		inIndex,
								bool			inOnRhs,
								bool			inOn);

		virtual void		ReflowCells();
		
		virtual void		DrawBackground();
		
		virtual void		DrawCell(
								const STableCell		&inCell,
								const Rect				&inLocalRect);

		virtual void		FinishCreateSelf();

		virtual void		HiliteCellActively(
								const STableCell	&inCell,
								Boolean				/* inHilite */);
		virtual void		HiliteCellInactively(
								const STableCell	&inCell,
								Boolean				/* inHilite */);

		virtual void /*e*/	ClickSelf(
								const SMouseDownEvent&	inMouseDown);
		virtual void		DrawSelf();


		virtual void /*e*/	TrackDrag(
								const STableCell		&inCell,
								const SMouseDownEvent	&inMouseDown);

	public:
		enum 				{ class_ID = FOUR_CHAR_CODE('ImTb') };

							CImageTable(
								LStream*	inStream);
		virtual				~CImageTable();

		SInt16				GetTextTraits()				{ return mTextTraitsId; }
		virtual void		ApplyForeAndBackColors() const;

		// Image access								
		virtual TableIndexT	CountImageCells() const;
		virtual TableIndexT	CountSelectedImageCells() const;
		virtual CImageCell	*GetCellImage(
								ArrayIndexT	inIndex) const;
		virtual CImageCell	*GetCellImage(
								const STableCell	&inCell) const;
		virtual void		SetCellImage(
								const STableCell	&inCell,
								CImageCell			*inIC);
		virtual CImageCell	*ReleaseCellImage(
								const STableCell	&inCell);
		virtual void		FindCellFor(
								CImageCell			*inIm,
								STableCell			&outCell);

		// Insert/Remove of cells
		virtual void /*e*/	InsertImageCell(
								CImageCell			*inImageCell,
								TableIndexT			inIndex,
								bool				inRefresh);
		virtual void		DeleteCell(
								const STableCell	&inCell,
								bool				inRefresh=true);
		virtual void		ClearTable();
	
		virtual void		ResizeFrameBy(
								SInt16		inWidthDelta,
								SInt16		inHeightDelta,
								Boolean		inRefresh);
		
		// drag and drop helpers
		virtual void		ShowInsertBar(
								Point		inPoint);
		virtual void		ClearInsertBar();
		virtual TableIndexT	GetInsertBar();

		virtual void /*e*/	MakeDragRegion(
								DragReference		inDragRef,
								RgnHandle			inRegion);
};
