// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Library
// CStdStream.h
// © Mark Tully 2000
// 26/9/00
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

#include <iostream>

class CStdStream : public LStream
{
protected:
    std::iostream *mStream;

public:
    CStdStream(
	std::iostream *inStream) :
	mStream(inStream)
	{
	    inStream->seekp(0,std::ios::end);
	    mLength=inStream->tellp();
	    inStream->seekp(0,std::ios::beg);
	    inStream->seekg(0,std::ios::beg);
	}

    virtual ExceptionCode GetBytes(
	void *outBuffer,
	SInt32 &ioCount)
	{
	    mStream->seekg(mMarker,std::ios::beg);

		if (ioCount>0)
		{
		    mStream->read((char*)outBuffer,ioCount);
		    ioCount=mStream->gcount();
		}

	    if (ioCount<0 || mStream->fail())
			return readErr;

	    mMarker+=ioCount;

	    return 0 ;
	}

    virtual ExceptionCode PutBytes(
	const void *inBuffer,
	SInt32 &ioCount)
	{
	    mStream->seekp(mMarker,std::ios::beg);

	    SInt32 wasPos=mStream->tellp();

	    mStream->write((char*)inBuffer,ioCount);
	    ioCount=mStream->tellp()-wasPos;

	    if (mStream->fail())
		return writErr;

	     
	    if ((mMarker+ioCount)>mLength)
		mLength=mMarker+ioCount;

	    mMarker+=ioCount;

	    return 0 ;
	}
};