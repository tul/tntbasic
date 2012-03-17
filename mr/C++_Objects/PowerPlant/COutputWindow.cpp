// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// COutputWindow.cpp
// © Mark Tully 2000
// 16/3/00
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
	Implements a text output window similar to SIOUX but powerplant savvy
	The window ppob can be of your own design but it must have a LTextEditView with a PaneID of 'Text' and have a ppob id
	of kPPob_OutputWindow.
	
	Before use call COutputWindow::SetSuperCommander with the super commander for the window. If you don't it uses the top
	commander.
*/

#include		"TNT_Debugging.h"
#include		"COutputWindow.h"
#include		"Utility Objects.h"
#include		<LWindow.h>
#include		"Marks Routines.h"

LCommander		*COutputWindow::sSuperCommander=0;
SInt16			COutputWindow::sAppResFile=::CurResFile();
		
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Destructor
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
COutputWindow::~COutputWindow()
{
	delete GetWindowNoOpen();
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetTextEditView									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the te view in the window - opening it if necessary
LTextEditView *COutputWindow::GetTextEditView()
{
	LTextEditView	*te=dynamic_cast<LTextEditView*>(GetWindow()->FindPaneByID(kPaneId_Text));
	AssertThrow_(te);
	
	return te;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ WriteCharsToConsole								/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Writes the chars to the console, auto scrolling the teview to reveal the new text
long COutputWindow::WriteCharsToConsole(
	char						*inBuffer,
	long						inSize)
{
	GrafPtr				wasPort;
	::GetPort(&wasPort);

	LTextEditView		*te=GetTextEditView();
	
	te->SetSelectionRange(32767,32767);
	
	Size	teLen=(*te->GetMacTEH())->teLength;
	
	// if we're going to cause some bovver by inserting the text then delete some of it
	// we delete the oldest text first and try to delete at least half the buffer
	if ((inSize + teLen) > LTextEditView::kMaxChars)
	{
		// drop the beginning if too long
		if (inSize>LTextEditView::kMaxChars)
		{
			inBuffer+=(inSize-LTextEditView::kMaxChars);
			inSize=LTextEditView::kMaxChars;
		}
		
		Handle		temp=te->GetTextHandle();
		
		if (::HandToHand(&temp))
		{
			temp=0;
			te->SetTextPtr(0,0,0);
		}
		else
		{
			Size		preserveBytes=LTextEditView::kMaxChars/2;
			
			if (preserveBytes>teLen)
				preserveBytes=teLen;
				
			if ((preserveBytes+inSize)>LTextEditView::kMaxChars)
				preserveBytes=0;
			
			::BlockMoveData(*temp+teLen-preserveBytes,*temp,preserveBytes);
			if (::BetterSetHandleSize(temp,preserveBytes+inSize,' ')==noErr)
			{
				if (mRemapNewLines)
					CopyDataAndRemap(*temp+preserveBytes,inBuffer,inSize);
				else
					::BlockMoveData(inBuffer,*temp+preserveBytes,inSize);
			}
			te->SetTextHandle(temp,0);
			::DisposeHandle(temp);
		}
	}
	else
	{
		bool	done=false;
		
		if (mRemapNewLines)
		{
			Size	i;
			Handle	temp=NULL;
			
			// need to replace all \n with \r. the odd thing is, when coming into the output stream, all \n and \r are switched,
			// don't know why a \r would be switched for a \n, seems pretty dumb
			for (i=0; i<inSize; i++)
			{
				if (inBuffer[i]=='\n')
				{
					temp=::NewHandle(inSize);
					if (!temp)
						te->SetTextPtr(0,0,0);		// clear mem, we're full
					else
					{
						UHandleReleaser	rel(temp);
						UHandleLocker	lock(temp);
						
						CopyDataAndRemap(*temp,inBuffer,inSize);
						te->Insert(*temp,inSize,0,true);
						done=true;
						break;
					}
				}
			}
		}
		
		if (!done)
			te->Insert(inBuffer,inSize,0,true);
	}

	
	GrafPtr	newPort;
	::GetPort(&newPort);
	if (newPort!=wasPort)	// port was changed, change it back and inval the pp view cache
	{
		LView::OutOfFocus(nil);
		::SetPort(wasPort);
	}
	
	return inSize;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetWindowNoOpen
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the first window with the specified id, return nil for none.
LWindow *COutputWindow::GetWindowNoOpen()
{
	return LWindow::FindWindowByID(mWinId);
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetWindow											/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Attempts to open the window, throws if fail
LWindow *COutputWindow::GetWindow()
{
	LWindow		*window;
	
	if (!(window=GetWindowNoOpen()))
	{
		UResFileSaver		safe(sAppResFile);
	
		window=LWindow::CreateWindow(kPPob_OutputWindow,sSuperCommander ? sSuperCommander : LCommander::GetTopCommander());
		ThrowIfNil_(window);
		
		window->SetPaneID(mWinId);	// track the window via a ref con so that if it's deleted we won't have a dangling ptr
		
		// After opening select the text view as the current target
		window->SwitchTarget(GetTextEditView());
	}
	
	return window;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CopyDataAndRemap									/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// copies a buffer of data and remaps all \n into \r
void COutputWindow::CopyDataAndRemap(
	char		*outBuffer,
	const char	*inBuffer,
	Size		inSize)
{
	char		*out = outBuffer-1;
	const char	*in = inBuffer-1;
	Size		i;

	// use mad preincrement notation for better ppc performance
	for (i=0; i<inSize; i++) 
	{
		char	c=*++in;
		if (c=='\n')
			*++out='\r';
		else
			*++out=c;
	}
}

