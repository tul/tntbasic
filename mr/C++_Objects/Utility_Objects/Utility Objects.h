// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Utility Objects.h
// A set of stack based C++ objects which are useful for common MacOS operation. See .cpp file
// for descriptions.
// Mark Tully
// 23/4/97
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1997, Mark Tully and John Treece-Birch
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
	I learnt a very important lesson today. Never, ever, call an object's default constructor directly. I had the default
	constructor doing something, then special case constructor calling the default constructor and doing a bit more
	besides. The very act of calling the default constructor in this way crashed the mac regularly in mad places.
	So I defined a common Init() routine which I called from the default constructor and from all the other constructors.
	This worked fine.
	Obviously the constructor does more than what you code. It must allocate memory or some low level object tracking
	stuff or sommat. Just don't mess OK?
*/

#pragma once
#include			<QDOffscreen.h>
#include			<Appearance.h>
#include			"Useful Defines.h"

typedef class UPortSaver
{
	private:
		GrafPtr		savedPort;
		void		Init();
	
	public:
					UPortSaver();
					UPortSaver(WindowPtr newPort);
					#if TARGET_API_MAC_CARBON==0
					UPortSaver(CWindowPtr newPort);
					#endif
					#if TARGET_API_MAC_CARBON
					UPortSaver(DialogPtr inDialog);
					#endif
					~UPortSaver();
} UPortSaver;

typedef class UWorldSaver
{
	private:
		GWorldPtr	saveWorld;
		GDHandle	saveDev;
		inline void	Init();
	
	public:
					UWorldSaver();
					UWorldSaver(GWorldPtr newWorld);
					~UWorldSaver();
} UWorldSaver;

typedef class UResLoadSaver
{
	private:
		Boolean		origState;
		void		Init();
		
	public:
					UResLoadSaver();
					UResLoadSaver(Boolean resLoadFlag);
					~UResLoadSaver();
} UResLoadSaver;

typedef class UHandleLocker
{
	private:
		char		wasState;
		Handle		lockedH;
	
	public:
					UHandleLocker(Handle handle,bool lockHi=false);
					~UHandleLocker();
} UHandleLocker;

typedef class UPixelsLocker
{
	private:
		PixMapHandle		pixelsH;
		void				Init(PixMapHandle lockMe);
	
	public:
							UPixelsLocker(PixMapHandle handle);
							UPixelsLocker(GWorldPtr gworldPtr);
							~UPixelsLocker();
} UPixelsLocker;

typedef class UResFileSaver
{
	private:
		short		savedFile;
		void		Init();
		
	public:
					UResFileSaver();
					UResFileSaver(short file);
					~UResFileSaver();
} UResFileSaver;

typedef class TColourPresets
{
	public:
		static const RGBColour		kWhite,kBlack,kBackGrey,kDarkGrey;
};

typedef class UForeColourSaver
{
	private:
		RGBColor	saved;
		
	public:
		void		Remember();
		void		Remember(const RGBColour &newCol);
		void		Restore();
		
					UForeColourSaver();
					UForeColourSaver(const RGBColor &newCol);
					~UForeColourSaver();
} UForeColourSaver;

typedef class UBackColourSaver
{
	private:
		RGBColor	saved;
		
	public:
		void		Remember();
		void		Remember(const RGBColour &newCol);
		void		Restore();
		
					UBackColourSaver();
					UBackColourSaver(const RGBColor &newCol);
					~UBackColourSaver();
} UBackColourSaver;

typedef class UClipSaver
{
	private:
		RgnHandle	saved;
		void		Init();

	public:
					UClipSaver(RgnHandle newRgn);
					UClipSaver(const Rect &rect);
					UClipSaver();
					~UClipSaver();
					
		RgnHandle	GetSavedRgn()			{ return saved; }
} UClipSaver;

typedef class UTextSaver
{
	private:
		short		savedFontId,savedFontSize,savedMode;
		Style		savedFace;

	public:
					UTextSaver();
					UTextSaver(short fontId,short fontSize,Style face,short textMode);

		void		Remember();
		void		Remember(short fontId,short fontSize,Style face,short textMode);
		void		Restore();

					~UTextSaver();
} UTextSaver;

typedef class UResFetcher
{
	private:
		Boolean			releaseFlag;
		Handle			handle;
		void /*e*/		Init(short resFile,ResType type,short id);
	
	public:
		/*e*/			UResFetcher(ResType type,short id);
		/*e*/			UResFetcher(short file,ResType type,short id);
						~UResFetcher();
		
		Boolean			GetReleaseFlag()					{ return releaseFlag; }
		void			SetReleaseFlag(Boolean inNewState)	{ releaseFlag=inNewState; }
		Handle			GetResourceHandle();
		
						operator Handle()	{ return GetResourceHandle(); }

} UResFetcher;

typedef class UMenuUnhilighter
{
	public:
						~UMenuUnhilighter();
} UMenuUnhilighter;

typedef class UWindowUpdater
{
	private:
		WindowPtr			theWindow;
		UPortSaver			safePort;

	public:
							UWindowUpdater(WindowPtr theWindow);
							#if TARGET_API_MAC_CARBON
							UWindowUpdater(DialogPtr theDialog);
							#endif
							~UWindowUpdater();
} UWindowUpdater;

typedef class UHandleReleaser
{
	private:
		bool				mReleaseFlag;
		Handle				mHandle;

	public:
							UHandleReleaser()						{ mHandle=0L; mReleaseFlag=true; }
							UHandleReleaser(Handle inHandle,bool inReleaseFlag=true);
							~UHandleReleaser();

							operator Handle()						{ return GetHandle(); }
		UHandleReleaser		&operator = (Handle inHandle);
		Handle				GetHandle()								{ return mHandle; }

		void				SetReleaseFlag(bool inNewState)			{ mReleaseFlag=inNewState; }
		bool				GetReleaseFlag()						{ return mReleaseFlag; }
		
		Handle				Release()								{ mReleaseFlag=false; return mHandle; }
		
} UHandleReleaser, StHandleReleaser;

typedef class UPenStateSaver
{
	private:
		PenState			wasState;
		
		void				Init();
	
	public:
							UPenStateSaver();
							UPenStateSaver(short penWide,short penHigh);
							UPenStateSaver(PenState &newState);
							~UPenStateSaver();
} UPenStateSaver;

typedef class UForeBrush
{
	private:
		UForeColourSaver	mForeColour;
		Pattern				mForePattern;
		
		void				Init();
				
	public:
							UForeBrush();
							UForeBrush(ThemeBrush inBrush);
							
		virtual void		SetForeBrush(ThemeBrush inBrush);
							~UForeBrush();
} UForeBrush;

class UBackBrush
{
	private:
		UBackColourSaver	mBackColour;
		Pattern				mBackPattern;
		
		void				Init();
				
	public:
							UBackBrush();
							UBackBrush(
								ThemeBrush inBrush);
							
		virtual void		SetBackBrush(
								ThemeBrush inBrush);
							~UBackBrush();
};

class UCFReleaser
{
	protected:
		CFTypeRef			mRef;
		
	public:
							UCFReleaser(
								CFTypeRef		inRef) :
								mRef(inRef)
							{
							}
							~UCFReleaser()
							{
								::CFRelease(mRef);
							}
};
