// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CSpriteManager.h
// © Mark Tully 2000
// 1/1/00
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

#include						<TArray.h>
#include						"TNTBasic_Types.h"
#include						"CImageBank.h"
#include						"ResourceTypes.h"

class							BlastLayerManager;
class							CTBSprite;
class							CTBSprite16;
class							CGraphicsContext;
class							CResourceContainer;

class CSpriteManager
{
	protected:
		CGraphicsContext		*mGraphicsContext;
	
		TArray<CTBSprite*>		mSprites;
	
		TArray<ResIDT>			mBankId;
		TArray<CImageBank*>		mImageBank;
		
		TTBInteger				mDefaultBank;
	
	public:
								CSpriteManager(
									CGraphicsContext	*inGraphicsContext);
									
		virtual					~CSpriteManager();
		
		void					LoadImages(
									CResourceContainer	*inContainer,
									ResIDT				inId);
		void					UnloadImages(
									ResIDT				inId);
	
		void					DeleteImage(
									TTBInteger		inImageIndex,
									CImageBank		*inBank);
	
		CTBSprite /*e*/			*GetSprite(
									TTBSpriteIndex		inIndex);
		CTBSprite				*GetSpriteNoCreate(
									TTBSpriteIndex		inIndex);
		
		CSpriteImage			*GetImage(
									TTBInteger			inIndex,
									CImageBank		*inBank);
									
		void					KillSprite(
									TTBInteger			inIndex);
		void					KillAllSprites();

		CImageBank /*e*/		*GetBankWithOpen(
									ResIDT				inResId);
		
		CImageBank				*GetBank(
									TTBInteger			inBankId)
								{
									ArrayIndexT		value=GetBankIndex(inBankId);
								
									if (value)
										return mImageBank[value];
									
									return 0L;
								}
								
		ArrayIndexT				GetBankIndex(
									TTBInteger			inBankId)
								{
									ArrayIndexT		max=mBankId.GetCount();
								
									for (ArrayIndexT n=1; n<=max; n++)
									{
										if (mBankId[n]==inBankId)
											return n;
									}
									
									return 0;
								}
								
		ArrayIndexT				GetFreeSpace()
								{
									ArrayIndexT		max=mImageBank.GetCount();
								
									for (ArrayIndexT n=1; n<=max; n++)
									{
										if (!mImageBank[n])
											return n;
									}
								
									return mImageBank.GetCount()+1;
								}
		ArrayIndexT				GetMaxSprites() { return mSprites.GetCount(); }	// returns the maximum boundary on created sprites (not necessary == number of sprites)
		
		void					SetDefaultSpriteBank(
									TTBInteger		inBank);
		CImageBank				*GetDefaultSpriteBank()							{ return GetBank(mDefaultBank); }
		
		void					SetSpritePriority(
									TTBSpriteIndex			inIndex,
									TTBSpriteIndex			inPriority);
									
		void					UpdateSpriteRenderPosition(
									CTBSprite16			*inSprite);
									
		ResIDT					GetBankResId(
									CImageBank			*inBank);
};
