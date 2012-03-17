// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// UStdIO.cpp
// © Mark Tully 2000
// 8/2/00
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
	Implements a virtual console for receiving output from ansi c calls such as printf() and cout <<. It is managed by an
	abstract class which simply has a WriteCharsToConsole function which receives the textural data. The subclasses can then
	handle the data as they wish. At any time the data can be redirected to another std out capture class by use of the
	SetOutputTarget() and GetOutputTarget() calls.
	
	The routine is only called upon to output chars when the new line char is received.
	
	The same idea is implemented for input now aswell.
*/

#include			"TNT_Debugging.h"
#include			"UStdIO.h"

#if ALLOW_MSL_CONSOLE_HACK
#ifndef __CONSOLE__
#include <console.h>
#endif
#endif

CLinkedListT<CStdOutTarget>		CStdOutTarget::sTargets;
CLinkedListT<CStdInSource>		CStdInSource::sSources;

static int      s_instances=0;
std::streambuf  *s_wasBuf=NULL;
long WriteCharsToConsole(char *buffer, long n);

class StdOutStreamBuf : public std::streambuf
{
    public:
        int overflow(int ch)
        {
            char    write=ch;
            WriteCharsToConsole(&write,1);
            if (s_wasBuf)
            {
                s_wasBuf->sputc(ch);
            }
        }
};

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ 
CStdOutTarget::CStdOutTarget() :
	mStdOutLink(this)
{
	// by default become active upon creation
	sTargets.LinkElement(&mStdOutLink);
	mStdOutActive=true;
    if (s_instances==0)
    {
        printf("Installing str buf override\n");
        s_wasBuf=std::cout.rdbuf(new StdOutStreamBuf);
    }
    s_instances++;
}

CStdOutTarget::~CStdOutTarget()
{
    s_instances--;
    if (s_instances==0)
    {
        printf("Removing str buf override\n");
        std::streambuf  *buf=std::cout.rdbuf(s_wasBuf);
        delete buf;
    }
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Constructor										/*e*/
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ 
CStdInSource::CStdInSource() :
	mStdInLink(this)
{
	// by default become active upon creation
	sSources.LinkElement(&mStdInLink);
	mStdInActive=true;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetOutputTarget									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the currently active output target. Can return nil if there's none.
CStdOutTarget *CStdOutTarget::GetOutputTarget()
{
	CListElementT<CStdOutTarget>		*target=sTargets.GetFirstElement();
	
	while (target)
	{
		if (target->GetData()->IsStdOutActive())
			return target->GetData();
		else
			target=target->GetNextElement();
	}
	
	return 0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetInputSource									Static
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the currently active input source. Can return nil if there's none.
CStdInSource *CStdInSource::GetInputSource()
{
	CListElementT<CStdInSource>		*target=sSources.GetFirstElement();
	
	while (target)
	{
		if (target->GetData()->IsStdInActive())
			return target->GetData();
		else
			target=target->GetNextElement();
	}
	
	return 0;
}

#if ALLOW_MSL_CONSOLE_HACK

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ Console package definitions
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

// The problem with the standard read console function is that it can't handle end of files which is problematic for things
// reading a virtual console as they wait for an eof from the user which is never transmitted. This hack can be installed
// by the InstallReadConsoleHack function to return an EOF error when the eof is reached and make sure the caller knows the
// end of the console has been reached. You may need to clear the end of file bit for the console again afterwards.
int __Hacked_Read_Console(std::__file_handle handle, unsigned char * buffer, size_t * count, std::__idle_proc idle_proc);

int __Hacked_Read_Console(std::__file_handle handle, unsigned char * buffer, size_t * count, std::__idle_proc idle_proc)
{
#pragma unused(handle,idle_proc)

//	if (!check_console())
//		return(__io_error);
	fflush(stdout);                   /* mm-960717*/
	*count = ReadCharsFromConsole((char *) buffer, *count);
	
	if (*count == -1)
		return(std::__io_error);
		
	if (*count == 0)
		return(std::__io_EOF);
	
	return(std::__no_io_error);
}

// See above
void InstallReadConsoleHack()
{
	std::__files[0].read_proc=__Hacked_Read_Console;
}

/*
 *	The following four functions provide the UI for the console package.
 *	Users wishing to replace SIOUX with their own console package need
 *	only provide the four functions below in a library.
 */

/*
 *	extern short InstallConsole(short fd);
 *
 *	Installs the Console package, this function will be called right
 *	before any read or write to one of the standard streams.
 *
 *	short fd:		The stream which we are reading/writing to/from.
 *	returns short:	0 no error occurred, anything else error.
 */

short InstallConsole(short fd)
{
#pragma unused (fd)

	return 0;
}

/*
 *	extern void RemoveConsole(void);
 *
 *	Removes the console package.  It is called after all other streams
 *	are closed and exit functions (installed by either atexit or _atexit)
 *	have been called.  Since there is no way to recover from an error,
 *	this function doesn't need to return any.
 */

void RemoveConsole(void)
{
}

/*
 *	extern long WriteCharsToConsole(char *buffer, long n);
 *
 *	Writes a stream of output to the Console window.  This function is
 *	called by write.
 *
 *	char *buffer:	Pointer to the buffer to be written.
 *	long n:			The length of the buffer to be written.
 *	returns short:	Actual number of characters written to the stream,
 *					-1 if an error occurred.
 */

#endif

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ WriteCharsToConsole
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Maps onto add outputted chars proc of the current target (if any)
long WriteCharsToConsole(char *buffer, long n)
{
	CStdOutTarget		*current=CStdOutTarget::GetOutputTarget();
	
	if (current)
	{
		Try_
		{
			n=current->WriteCharsToConsole(buffer,n);
		}
		Catch_(err)
		{
			return -1;
		}
	}
	else
		n=0;

	return n;
}

#if ALLOW_MSL_CONSOLE_HACK

/*
 *	extern long ReadCharsFromConsole(char *buffer, long n);
 *
 *	Reads from the Console into a buffer.  This function is called by
 *	read.
 *
 *	char *buffer:	Pointer to the buffer which will recieve the input.
 *	long n:			The maximum amount of characters to be read (size of
 *					buffer).
 *	returns short:	Actual number of characters read from the stream,
 *					-1 if an error occurred.
 */

long ReadCharsFromConsole(char *buffer, long n)
{
	CStdInSource		*current=CStdInSource::GetInputSource();
	
	if (current)
	{
		Try_
		{
			long	newn;
			newn=current->ReadCharsFromConsole(buffer,n);
			if (newn==0)	// didn't read all the data - must be at end of the data as an error hasn't occurred
			{
				buffer[0]=EOF;
				n=newn;
			}
			else
				n=newn;
		}
		Catch_(err)
		{
			return -1;
		}
	}
	else
		n=0;

	return n;
}

/*
 *	return the name of the current terminal ...

	stdin == 0
	stdout == 1
	stderr == 2

 */
char *__ttyname(long fildes)
{
	/*	all streams have the same name ...*/
	static char *__CStdOutTargetDeviceName = "CStdOutTarget";
	static char *__CStdInSourceDeviceName = "CStdInSource";
	
	if (fildes >= 1 && fildes <= 2)	// cout and cerr
		return (__CStdOutTargetDeviceName);
	else if (fildes==0)
		return (__CStdInSourceDeviceName);

	return (NULL);
}

#endif
