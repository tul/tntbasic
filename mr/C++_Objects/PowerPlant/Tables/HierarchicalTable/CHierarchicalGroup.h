// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CHierarchicalGroup.h
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

#include			"CHierarchicalElement.h"
#include			<UTables.h>

class CHierarchicalGroup : public virtual CHierarchicalElement
{
	protected:
		TBufferedBoolean					mSubGroupVisible;
		CLinkedListT<CHierarchicalElement>	mSubGroup;				// list of CHierarchicalElements
	
	public:	
		enum TVisibilityOperation
		{
			kHideHeaderAndSubGroup,
			kHideSubGroupOnly,
			kHideHeaderElementOnly,
			kShowHeaderAndSubGroup,
			kShowSubGroupOnly,
			kShowHeaderElementOnly
		};
										CHierarchicalGroup();

		virtual bool					IsSubGroupVisible() const;
		virtual void /*e*/				SetSubGroupVisibility(
											bool					inNewStatus);
		virtual void /*e*/				SetVisibilityOfBoth(
											bool					inNewStatus);
		
		virtual TableIndexT				CountVisibleSubElements();
											
		virtual void /*e*/				SubElementVisibilityChanged(
											CHierarchicalElement	*inElement,
											TVisibilityOperation	inOp);

		virtual void /*e*/				InsertElement(
											CHierarchicalElement	*inElement,
											CHierarchicalElement	*inPositionElement,
											bool					inAfterPositionElement);
		virtual void					RemoveElement(
											CHierarchicalElement	*inElement);
};