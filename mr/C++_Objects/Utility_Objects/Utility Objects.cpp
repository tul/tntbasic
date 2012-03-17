// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Utility Objects.cpp
// A set of stack based C++ objects which are useful for common MacOS operation.
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

// All these stack based objects do something in the constructor and then undo it in the
// destructor. They are used for the times when you have to do something at the beginning of
// the proc and undo it at the end, eg port setting in an update routine, handle locking etc.

#include		"TNT_Debugging.h"
#include		"Utility Objects.h"
#include		"Marks Routines.h"

// ********* TColourPresets *********
const RGBColour		TColourPresets::kWhite=cWhite;
const RGBColour		TColourPresets::kBlack=cBlack;
const RGBColour		TColourPresets::kBackGrey=cBackGrey;
const RGBColour		TColourPresets::kDarkGrey=cGrey;

// ********* UPortSaver *********
// Preserved the current graf port and switches to the one passed

void UPortSaver::Init()
{
	GetPort(&savedPort);
	if (savedPort && IsValidPort)
	{
		if (!IsValidPort(savedPort))
			savedPort=0;
	}
}

UPortSaver::UPortSaver(WindowPtr thePort)
{
	Init();
	#if TARGET_API_MAC_CARBON
	SetPortWindowPort(thePort);
	#else
	SetPort(thePort);
	#endif
}

#if TARGET_API_MAC_CARBON
// under carbon dialogptrs are different from window ptrs
UPortSaver::UPortSaver(DialogPtr inDialog)
{
	Init();
	SetPortDialogPort(inDialog);
}
#endif

#if TARGET_API_MAC_CARBON==0
UPortSaver::UPortSaver(CWindowPtr thePort)
{
	Init();
	SetPort((WindowPtr)thePort);
}
#endif

UPortSaver::UPortSaver()
{
	Init();
}

UPortSaver::~UPortSaver()
{
	if (savedPort)
	{
		if (IsValidPort && IsValidPort(savedPort))
			SetPort(savedPort);
	}
}

// ********* UResLoadSaver *********

void UResLoadSaver::Init()
{
	if (LMGetResLoad())
		origState=true;
	else
		origState=false;
}

UResLoadSaver::UResLoadSaver()
{
	Init();
}

UResLoadSaver::UResLoadSaver(Boolean resLoadFlag)
{
	Init();
	SetResLoad(resLoadFlag);
}

UResLoadSaver::~UResLoadSaver()
{
	SetResLoad(origState);
}

// ********* UWorldSaver *********
// Preserveds the current gworld and switches to the one passed

void UWorldSaver::Init()
{
	GetGWorld(&saveWorld,&saveDev);
}

UWorldSaver::UWorldSaver()
{
	Init();
}

UWorldSaver::UWorldSaver(GWorldPtr theWorld)
{
	Init();
	SetGWorld(theWorld,0L);
}

UWorldSaver::~UWorldSaver()
{
	SetGWorld(saveWorld,saveDev);
}

// ********* UHandleLocker *********
// Locks the passed handle and then restores it's state on exit

UHandleLocker::UHandleLocker(Handle lockMe,bool lockHi)
{
	wasState=HGetState((Handle)lockMe);
	if (lockHi)
		HLockHi((Handle)lockMe);
	else
		HLock((Handle)lockMe);
	lockedH=lockMe;
}

UHandleLocker::~UHandleLocker()
{
	HSetState((Handle)lockedH,wasState);
}

// ********* UPixelsLocker *********
// Locks the passed pixmap handle/gworld ptr for drawing operations and then unlocks it on exit
UPixelsLocker::UPixelsLocker(PixMapHandle lockMe)
{
	Init(lockMe);
}

UPixelsLocker::UPixelsLocker(GWorldPtr lockMe)
{
	Init(GetGWorldPixMap(lockMe));
}

void UPixelsLocker::Init(PixMapHandle lockMe)
{
	pixelsH=lockMe;
	LockPixels(lockMe);
}

UPixelsLocker::~UPixelsLocker()
{
	UnlockPixels(pixelsH);
}

// ******** UResFileSaver *********
// Changes to the passed res file preserving the current one. Pass -1 for no change.

void UResFileSaver::Init()
{
	savedFile=CurResFile();
}

UResFileSaver::UResFileSaver()
{
	Init();
}

UResFileSaver::UResFileSaver(short newFile)
{
	Init();
	if (newFile!=-1)
		UseResFile(newFile);
}

UResFileSaver::~UResFileSaver()
{
	UseResFile(savedFile);
}

// ******** UForeColourSaver ********
// blah blah blah...

void UForeColourSaver::Remember()
{
	GetForeColor(&saved);
}

void UForeColourSaver::Remember(const RGBColour &newCol)
{
	Remember();
	RGBForeColor(&newCol);	
}

void UForeColourSaver::Restore()
{
	RGBForeColor(&saved);
}

UForeColourSaver::UForeColourSaver()
{
	Remember();
}

UForeColourSaver::UForeColourSaver(const RGBColor &newCol)
{
	Remember(newCol);
}

UForeColourSaver::~UForeColourSaver()
{
	Restore();
}

// ******** UBackColourSaver ********
// blah blah blah...

void UBackColourSaver::Remember()
{
	GetBackColor(&saved);
}

void UBackColourSaver::Remember(const RGBColour &newCol)
{
	Remember();
	RGBBackColor(&newCol);	
}

void UBackColourSaver::Restore()
{
	RGBBackColor(&saved);
}

UBackColourSaver::UBackColourSaver()
{
	Remember();
}

UBackColourSaver::UBackColourSaver(const RGBColor &newCol)
{
	Remember(newCol);
}

UBackColourSaver::~UBackColourSaver()
{
	Restore();
}

// ********* UClipSaver *********
// This object saves the current clipping rgn for the current port and sets it to the one
// specfied by either a rgn handle or a rect. If you don't pass a param to the constructor
// it saves the current clip rgn but doesn't change it

void UClipSaver::Init()
{
	saved=NewRgn();
	if (saved)
		GetClip(saved);
}

UClipSaver::UClipSaver(const Rect &clipRect)
{
	Init();
	if (saved)
		ClipRect(&clipRect);
}

UClipSaver::UClipSaver(RgnHandle handle)
{
	Init();
	if (saved)
		SetClip(handle);
}

UClipSaver::UClipSaver()
{
	Init();
}

UClipSaver::~UClipSaver()
{
	if (saved)
	{
		SetClip(saved);
		DisposeRgn(saved);
	}
}

// ********* UTextSaver *********
// This object saves the text data for the current port. Just make sure the destructor is called
// on the same port eh?

void UTextSaver::Remember()
{
	GrafPtr		currentPort;
	
	GetPort(&currentPort);

	#if TARGET_API_MAC_CARBON
	savedFontId=::GetPortTextFont(currentPort);
	savedFontSize=::GetPortTextSize(currentPort);
	savedFace=::GetPortTextFace(currentPort);
	savedMode=::GetPortTextMode(currentPort);
	#else
	savedFontId=currentPort->txFont;
	savedFontSize=currentPort->txSize;
	savedFace=currentPort->txFace;
	savedMode=currentPort->txMode;
	#endif
}

void UTextSaver::Remember(short fontId,short fontSize,Style face,short textMode)
{
	Remember();
	
	TextFont(fontId);
	TextFace(face);
	TextMode(textMode);
	TextSize(fontSize);
}

void UTextSaver::Restore()
{
	TextFont(savedFontId);
	TextFace(savedFace);
	TextMode(savedMode);
	TextSize(savedFontSize);
}

UTextSaver::UTextSaver()
{
	Remember();	
}

UTextSaver::UTextSaver(short fontId,short fontSize,Style face,short textMode)
{
	Remember(fontId,fontSize,face,textMode);
}

UTextSaver::~UTextSaver()
{
	Restore();
}

// ********* UResFetcher *********
// This object fetches a resource for you to access and releases it when it's destructed, if it's can't fetch it throws an
// error.

/*e*/
UResFetcher::UResFetcher(ResType type,short id)
{
	Init(-1,type,id);
}

/*e*/
UResFetcher::UResFetcher(short resFile,ResType type,short id)
{
	Init(resFile,type,id);
}

/*e*/
void UResFetcher::Init(short resFile,ResType type,short id)
{
	releaseFlag=true;
	handle=BetterGetResource(resFile,type,id);
	if (!handle)
		Throw_(BetterGetResErr());
}

Handle UResFetcher::GetResourceHandle()
{
	return handle;
}

UResFetcher::~UResFetcher()
{
	if (releaseFlag)
		BetterReleaseResource(handle);
}

// ********* UMenuUnhilighter *********
// When this object is destroyed it unhilights the menu. That's it.

UMenuUnhilighter::~UMenuUnhilighter()
{
	HiliteMenu(0);
}

// ********* UWindowUpdater *********
// This object is the same as UPortSaver except that it also calls begin update and endupate which is useful for update procs

UWindowUpdater::UWindowUpdater(WindowPtr theWindow) : safePort(theWindow)
{
	this->theWindow=theWindow;
	BeginUpdate(theWindow);
}

#if TARGET_API_MAC_CARBON
UWindowUpdater::UWindowUpdater(DialogPtr theDialog) : safePort(theDialog)
{
	theWindow=GetDialogWindow(theDialog);
	BeginUpdate(theWindow);
}
#endif

UWindowUpdater::~UWindowUpdater()
{
	EndUpdate(theWindow);
}

// ********* UHandleReleaser *********
// This object can be used to release a resource / dispose a mac memory handle on destruct. On construct it does nothing.
// It's useful if you need to make sure you release a resource/dispose a handle before you return from a proc.
// If you change you mind about the release you can call the SetReleaseFlag proc to change whether the handle gets released.
// So you could leave it on it's default setting of true until you know everything has worked and then set the flag to false
// and return without releasing the handle.
// It can handle nil handles without error.

UHandleReleaser::UHandleReleaser(Handle inHandle,bool inReleaseFlag)
{
	mReleaseFlag=inReleaseFlag;
	mHandle=inHandle;
}

UHandleReleaser::~UHandleReleaser()
{
	if (mReleaseFlag)
		BetterDisposeHandle((Handle)mHandle);
}

// When assigning a new handle, the old one is disposed of if the release flag is set
UHandleReleaser &UHandleReleaser::operator=(Handle inHandle)
{
	if (mReleaseFlag)
		BetterDisposeHandle((Handle)mHandle);
	
	mHandle=inHandle;
	
	return *this;
}

// ********* UPenStateSaver ***********

void UPenStateSaver::Init()
{
	GetPenState(&wasState);
}

UPenStateSaver::UPenStateSaver()
{
	Init();
}

UPenStateSaver::UPenStateSaver(PenState &newState)
{
	Init();
	SetPenState(&newState);
}

UPenStateSaver::UPenStateSaver(short penWide,short penHigh)
{
	Init();
	PenSize(penWide,penHigh);
}

UPenStateSaver::~UPenStateSaver()
{
	SetPenState(&wasState);
}

//#include "Root.h" // bkg console stuff - rem out

void UForeBrush::Init()
{
//	ECHO("Enter UForeBrush::Init()");
	
	GrafPtr		currentPort;
	::GetPort(&currentPort);

	#if TARGET_API_MAC_CARBON
	PixPat	**pattern=(PixPat**)::NewHandleClear(sizeof(PixMap));
	
	::GetPortPenPixPat(currentPort,pattern);
	
	// Not completely sure about the above call, seems to work but there's no docs on it so I'm not sure if I'm
	// using it right...
	
//	ECHO("UForeBrush::Init() GetPortPenPixPat() call completed");

	mForePattern=(*pattern)->pat1Data;
	::DisposeHandle((Handle)pattern);
	#else
	mForePattern=(**((CGrafPtr)currentPort)->pnPixPat).pat1Data;
	#endif
}

UForeBrush::UForeBrush()
{
	Init();
}

UForeBrush::UForeBrush(ThemeBrush inBrush)
{
	Init();
	SetForeBrush(inBrush);
}

void UForeBrush::SetForeBrush(ThemeBrush inBrush)
{
	SetThemePen(inBrush,16,true);
}

UForeBrush::~UForeBrush()
{
	::PenPat(&mForePattern);
}

// **** UBackBrush ***

void UBackBrush::Init()
{
//	ECHO("Enter UBackBrush::Init()");
	
	GrafPtr		currentPort;
	::GetPort(&currentPort);

	#if TARGET_API_MAC_CARBON
	PixPat	**pattern=(PixPat**)::NewHandleClear(sizeof(PixMap));
	
	::GetPortBackPixPat(currentPort,pattern);
	
	// Not completely sure about the above call, seems to work but there's no docs on it so I'm not sure if I'm
	// using it right...
	
//	ECHO("UBackBrush::Init() GetPortBackPixPat() call completed");

	mBackPattern=(*pattern)->pat1Data;
	::DisposeHandle((Handle)pattern);
	#else
	// this might not compile under non carbon - not tested...
	mBackPattern=(**((CGrafPtr)currentPort)->bkPixPat).pat1Data;
	#endif
}

UBackBrush::UBackBrush()
{
	Init();
}

UBackBrush::UBackBrush(
	ThemeBrush		inBrush)
{
	Init();
	SetBackBrush(inBrush);
}

void UBackBrush::SetBackBrush(
	ThemeBrush		inBrush)
{
	SetThemeBackground(inBrush,16,true);
}

UBackBrush::~UBackBrush()
{
	::BackPat(&mBackPattern);
}

