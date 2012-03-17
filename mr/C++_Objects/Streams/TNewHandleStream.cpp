// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// CNewHandleStream.cpp
// Mark Tully
// 16/7/98
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

// An implementation of PowerPlants LHandleStream which has options of what should happen to the handle when the object
// is destroyed.
// If you specify false for disposeHandle in the constructor then the handle is not disposed. If true it is disposed,
// unless it is a resource handle in which case it is released.

#include			"TNewHandleStream.h"
#include			"Marks Routines.h"
#include			"Utility Objects.h"

CNewHandleStream::CNewHandleStream()
{
	mDisposeHandle=true;
}

CNewHandleStream::CNewHandleStream(Handle inHandle,bool inDisposeHandle) : LHandleStream(inHandle)
{
	mDisposeHandle=inDisposeHandle;
}

bool CNewHandleStream::GetDisposeHandleFlag()
{
	return mDisposeHandle;
}

void CNewHandleStream::SetDisposeHandleFlag(bool inDisposeHandle)
{
	mDisposeHandle=inDisposeHandle;
}

// When Powerplant makes a write and there is no handle it allocates it in the SetLength proc. If the handle is allocated
// then change the mode to dispose it.
void CNewHandleStream::SetLength(SInt32 inLength)
{
	if (!mDataH)
		mDisposeHandle=true;
	
	LHandleStream::SetLength(inLength);
}

// When you change the handle, powerplant disposes of the old handle, stop it doing that if the options state so
// Note : PowerPlants implementation isn't virtual, could cause problems
void CNewHandleStream::SetDataHandle(Handle inHandle)
{
	if (mDataH)
	{
		Handle			theHandle=DetachDataHandle();
		
		if (mDisposeHandle)
			BetterDisposeHandle(theHandle);
	}
	LHandleStream::SetDataHandle(inHandle);
}

CNewHandleStream::~CNewHandleStream()
{
	// Detach the handle to stop the base classes destructor releasing the handle
	Handle		theHandle=DetachDataHandle();

	if (mDisposeHandle)
		BetterDisposeHandle(theHandle);
}

// writes ae desc data out to the stream
void CNewHandleStream::WriteAEDescData(
	const AEDesc	&inDesc)
{
	Size			byteCount=::AEGetDescDataSize(&inDesc);
	
	ExceptionCode	err = noErr;
	SInt32			endOfWrite = GetMarker() + byteCount;

	if (endOfWrite > GetLength()) {		// Need to grow Handle

		try {
			SetLength(endOfWrite);
		}

		catch (ExceptionCode inErr) {	// Grow failed. Write only what fits.
			byteCount = GetLength() - GetMarker();
			err = inErr;
		}

		catch (const LException& inException) {
			byteCount = GetLength() - GetMarker();
			err = inException.GetErrorCode();
		}
	}
										// Copy bytes into Handle
	if (byteCount > 0) {				//   Byte count will be zero if
										//   mDataH is nil
//		::BlockMoveData(inBuffer, *mDataH + GetMarker(), ioByteCount);
		UHandleLocker		locked(mDataH);
		err=::AEGetDescData(&inDesc,*mDataH + GetMarker(), byteCount);
		SetMarker(byteCount, streamFrom_Marker);
	}

	ThrowIfOSErr_(err);
}

