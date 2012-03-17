// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHierarchicalElement.h
// © Mark Tully and TNT Software 1998
// 16/12/98
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

#include			"CHierListElementT.h"
#include			"TBufferedBoolean.h"
#include			"CSharedHierListElementT.h"

class				CHierarchicalGroup;
class				CHierarchicalTable;
class				LPane;
class				STableCell;

// This cannot be a shared hierarchical element as the element must be found in one list only so that things like get index
// work amongst others.
class CHierarchicalElement : public CHierListElementT< CHierarchicalElement >
{
	friend class CHierarchicalTable;

	protected:
		LPane							*mPane;
		TBufferedBoolean				mIsVisible;
		CHierarchicalTable				*mTable;
		CHierarchicalGroup				*mSuperGroup;

		virtual void					SetTable(
											CHierarchicalTable	*inTable);

	public:
		CSharedHierListElementT<CHierarchicalElement>	mPublicLink;			// Use this for whatever you want.

										CHierarchicalElement();
		virtual							~CHierarchicalElement();
										
		virtual LPane					*GetPane();
		virtual void					SetPane(
											LPane			*inPane);

		virtual CHierarchicalGroup		*GetSuperGroup() const;
		virtual void					SetSuperGroup(
											CHierarchicalGroup	*inGroup);

		virtual void					GetElementCell(
											STableCell		&outCell);
		virtual bool					IsInTable();
		virtual CHierarchicalTable		*GetTable()							{ return mTable; }

		virtual bool					IsVisible() const						{ return mIsVisible; }
		virtual void					SetVisibility(
											bool			inNewStatus);
											
		virtual SInt32					GetCellHeight();
		virtual void					SetCellHeight(
											SInt32			inNewRowHeight,
											bool			inRefresh);

		virtual void					HiliteCell(
											bool			inHilite);

};