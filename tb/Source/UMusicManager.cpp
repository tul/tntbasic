// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// UMusicManager.cpp
// © John Treece-Birch 2001
// 8/2/01
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2001, Mark Tully and John Treece-Birch
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

#include	"UMusicManager.h"
#include	"Marks Routines.h"
#include	<Files.h>
#include	"Utility Objects.h"
#include	"UTBResourceManager.h"
#include	"CGraphicsContext16.h"

Movie			UMusicManager::sMovie;
bool			UMusicManager::sMusicPlaying;
WindowPtr		UMusicManager::sWindow;
short			UMusicManager::sVolume;
unsigned long	UMusicManager::sNextTask;

void UMusicManager::Initialise()
{
	::EnterMovies();
	sMovie=0L;
	sMusicPlaying=false;
	
	Rect	smallRect={0,0,10,10};
	
	ThrowIfMemFail_(sWindow=::NewCWindow(0L,&smallRect,"\p",false,kWindowDocumentProc,0L,false,0L));
	
	sVolume=100;
	sNextTask=0;
}

void UMusicManager::ShutDown()
{
	::ExitMovies();
	
	CleanUp();
	
	if (sWindow)
		::DisposeWindow(sWindow);
}

void UMusicManager::CleanUp()
{
	UnloadMusic();
}

void UMusicManager::LoadMusic(
	Handle			inHandle)
{
	short		movieResID=0,movieResFile,fileRefNum;
	Str255		movieName;
	Boolean		wasChanged;
	FSSpec		spec;
	
	UnloadMusic();
	
	::FindFolder(kOnSystemDisk,kTemporaryFolderType,kCreateFolder,&spec.vRefNum,&spec.parID);
	::CopyPStr("\pTNT Basic Music File",spec.name);
	
	::FSpCreate(&spec,'TNTB','MPEG',0);
	::FSpOpenDF(&spec,fsWrPerm,&fileRefNum);
	
	long	handleSize=::GetHandleSize(inHandle);
	
	::HLock(inHandle);
	::FSWrite(fileRefNum,&handleSize,*inHandle);
	::HUnlock(inHandle);
	
	::FSClose(fileRefNum);
	
	UPortSaver		safe(sWindow);
	
	::OpenMovieFile(&spec,&movieResFile,fsRdPerm);
	::NewMovieFromFile(&sMovie,movieResFile,&movieResID,movieName,newMovieActive,&wasChanged);
	::CloseMovieFile(movieResFile);
	
	::GoToBeginningOfMovie(sMovie);
}

void UMusicManager::UnloadMusic()
{
	if (sMovie)
	{
		::StopMovie(sMovie);
		::DisposeMovie(sMovie);
		sMovie=0L;
		sMusicPlaying=false;
	}
	
	// Delete the temp file
	FSSpec		spec;
	
	::FindFolder(kOnSystemDisk,kTemporaryFolderType,kCreateFolder,&spec.vRefNum,&spec.parID);
	::CopyPStr("\pTNT Basic Music File",spec.name);
	::FSpDelete(&spec);
}

void UMusicManager::PlayMusic()
{
	if ((sMovie) && (!sMusicPlaying))
	{
		::StartMovie(sMovie);
		sMusicPlaying=true;
	}
}

void UMusicManager::StopMusic()
{
	if ((sMovie) && (sMusicPlaying))
	{
		::StopMovie(sMovie);
		sMusicPlaying=false;
	}
}

long UMusicManager::GetMusicLength()
{
	if (sMovie)
	{
		TimeScale	scale=::GetMovieTimeScale(sMovie);
		TimeValue	value=::GetMovieDuration(sMovie);
		
		return (value*60)/scale;
	}
	
	return 0;
}

long UMusicManager::GetMusicPosition()
{
	if (sMovie)
	{
		TimeRecord	time;
		
		::GetMovieTime(sMovie,&time);
		
		TimeValue	value=time.value.hi+time.value.lo;
		
		return (value*60)/time.scale;
	}
	
	return 0;
}

void UMusicManager::SetMusicPosition(
	long		inValue)
{
	if (sMovie)
	{
		TimeValue	value=(inValue*GetMovieTimeScale(sMovie))/60;
		
		::SetMovieTimeValue(sMovie,value);
	}
}

void UMusicManager::SetMusicVolume(
	short		inValue)
{
	sVolume=inValue;

	if (sMovie)
	{
		short	volume=0;
		short	temp=sVolume;
		
		if (temp>=100)
		{
			volume=1<<8;
			temp-=100;
		}
	
		if (temp>=50)
		{
			volume|=1<<7;
			temp-=50;
		}
	
		if (temp>=25)
		{
			volume|=1<<6;
			temp-=25;
		}
		
		if (temp>=12)
		{
			volume|=1<<5;
			temp-=12;
		}
		
		if (temp>=6)
		{
			volume|=1<<4;
			temp-=6;
		}
		
		if (temp>=3)
		{
			volume|=1<<3;
			temp-=3;
		}
		
		if (temp>=2)
		{
			volume|=1<<2;
			temp-=2;
		}
		
		if (temp>=1)
		{
			volume|=1<<1;
			temp-=1;
		}
		
		::SetMovieVolume(sMovie,volume);
	}
}

void UMusicManager::Pause()
{
	if ((sMovie) && (sMusicPlaying))
		::StopMovie(sMovie);
}

void UMusicManager::Resume()
{
	if ((sMovie) && (sMusicPlaying))
		::StartMovie(sMovie);
}

void UMusicManager::Task()
{
	if ((sMovie) && (sMusicPlaying) && (sNextTask<CGraphicsContext16::sMsTimer.GetTime()))
	{
		::MoviesTask(sMovie,0L);
		sNextTask=CGraphicsContext16::sMsTimer.GetTime()+60;
	}
}