// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// TNT Basic
// CStringEditBuffer.h
// © Mark Tully 2003
// 5/10/03
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2003, Mark Tully and John Treece-Birch
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

#include		"CCString.h"

class CStringEditBuffer
{
	public:
		typedef CCString::TCharIndex	TCursorIndex;

		enum EClipOp
		{
			kClip_Copy,
			kClip_Cut,
			kClip_Paste
		};

		enum EKeyClass
		{
			kKeyClass_Null,				// undefined
			kKeyClass_Navigation,		// charcode
			kKeyClass_Printing,			// charcode
			kKeyClass_Delete,			// charcode
			kKeyClass_Clipboard,		// EClip_ enum value
		};
		
	protected:
		CCString			mEditBuffer;				// the text being edited
		TCursorIndex		mCursorIndex;			// the cursor is before this zero based character offset. so newly typed chars will have this index. index will be >=0 and <=strlen(mEditBuffer)


		void				ClampCursor();

		void /*e*/			InputPrintableKey(
								UInt32			inCharCode);
		void /*e*/			InputClipboardKey(
								UInt32			inClipOp);
		void /*e*/			InputDeleteKey(
								UInt32			inCharCode);
		void /*e*/			InputNavKey(
								UInt32			inCharCode);
	
	public:
							CStringEditBuffer(
								TCursorIndex	inDefaultBufferSize=512);
				
		void /*e*/			SetBuffer(
								const CCString	&inString);
		void				SetCursorIndex(
								TCursorIndex	inIndex);

		static EKeyClass /*e*/	ClassifyKey(
									UInt32			inTBRawKey,
									UInt32			&outData);
										
		const CCString		&GetBuffer();
		TCursorIndex /*e*/	GetCursorIndex();
		
		void /*e*/			InputKeyStroke(
								UInt32			inTBRawKey);		
};