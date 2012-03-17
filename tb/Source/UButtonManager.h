// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CButtonManager.h
// © John Treece-Birch 2000
// 12/7/00
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

/*
	Classes for managing buttons etc.
*/

#pragma once

#include		"CProgram.h"
#include		"UTBException.h"
#include		"CTBSprite.h"
#include		"CSpriteManager.h"
#include		"Blast Layer Manager.h"
#include		<TArray.h>

class CButton
{
	protected:
		TTBInteger					mImage,mOverImage,mDownImage;
		CTBSprite					*mSprite;
	
	public:
									CButton(
										TTBInteger		inX,
										TTBInteger		inY,
										TTBInteger		inImage,
										TTBInteger		inOverImage,
										TTBInteger		inDownImage,
										CImageBank		*inBank,
										CProgram		&ioState);

		virtual						~CButton();

		bool						IsPointIn(
										TTBInteger	inX,
										TTBInteger	inY)
									{
										Rect	theRect;
									
										theRect.top=inY;
										theRect.left=inX;
										theRect.bottom=inY+1;
										theRect.right=inX+1;
									
										return mSprite->RectCollision(theRect);
									}
									
		void						ImageUp(
										CProgram		&ioState)
									{
										mSprite->SetSpriteImage(ioState.GetSpriteManager().GetImage(mImage,mSprite->GetBank()));
									}
									
		void						ImageOver(
										CProgram		&ioState)
									{
										mSprite->SetSpriteImage(ioState.GetSpriteManager().GetImage(mOverImage,mSprite->GetBank()));
									}
									
		void						ImageDown(
										CProgram		&ioState)
									{
										mSprite->SetSpriteImage(ioState.GetSpriteManager().GetImage(mDownImage,mSprite->GetBank()));
									}
		
		CTBSprite					*GetSprite()		{ return mSprite; }
};

class UButtonManager
{
	protected:
		static TArray<CButton*>		sButtons;
						
	public:							
		static void					ResetButtons()
									{
										ArrayIndexT		max=sButtons.GetCount();
								
										for (ArrayIndexT n=1; n<=max; n++)
										{
											if (sButtons[n])
											{
												delete sButtons[n];
												sButtons[n]=0L;
											}
										}
									}
		
		static void					NewButton(
										TTBInteger		inIndex,
										TTBInteger		inX,
										TTBInteger		inY,
										TTBInteger		inImage,
										TTBInteger		inOverImage,
										TTBInteger		inDownImage,
										CImageBank		*inBank,
										CProgram		&ioState)
									{
										inIndex++;
									
										if (inIndex>sButtons.GetCount())
											sButtons.InsertItemsAt(inIndex-sButtons.GetCount(),LArray::index_Last,(CButton*)0);
									
										if (sButtons[inIndex])
										{
											delete sButtons[inIndex];
											sButtons[inIndex]=0L;
										}
										
										sButtons[inIndex]=new CButton(inX,inY,inImage,inOverImage,inDownImage,inBank,ioState);
									}
									
		static TTBInteger			IsPointIn(
										TTBInteger		inX,
										TTBInteger		inY)
									{
										ArrayIndexT		max=sButtons.GetCount();
									
										for (short n=1; n<=max; n++)
										{
											if (sButtons[n])
											{
												if (sButtons[n]->IsPointIn(inX,inY))
													return n-1;
											}
										}
										
										return -1;
									}
									
		static void					UpdateButtons(
										TTBInteger		inHilite,
										TTBInteger		inTrack,
										CProgram		&ioState);
										
		static bool					ValidButtons()
									{
										if (sButtons.GetCount())
											return true;
										
										return false;
									}

		static void					DeleteAllButtonsUsingBank(
										CImageBank	*inImageBank);
};