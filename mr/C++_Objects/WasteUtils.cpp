// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// WasteUtils.cpp
// © Mark Tully & TNT Software 2000
// 7/6/00
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

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

#include	"WasteUtils.h"
#include	"WasteIntf.h"

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ WEIsAnchorAtEnd
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
bool WEIsAnchorAtEnd(
	WEHandle		hWE)
{
	return BTST((*hWE)->flags, weFAnchorIsEnd)!=0;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ WEFastSetStyle
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Sets the style without making an undo entry and can effect any text range, not just the selection
OSErr WEFastSetStyle(
	WEStyleMode		mode,
	const TextStyle *ts,
	SInt32			inStart,
	SInt32			inEnd,
	WEHandle		hWE)
{
	WEPtr pWE;
	WEActionHandle hAction;
	ScriptCode fontScript;
	Boolean saveWELock;
	OSErr err;

	// lock the WE record
	saveWELock = _WESetHandleLock((Handle) hWE, true);
	pWE = *hWE;

	// return an error code if this instance is read-only
	err = weReadOnlyErr;
	if (BTST(pWE->features, weFReadOnly))
	{
		goto cleanup;
	}

	// stop any ongoing inline input session
	WEStopInlineSession(hWE);

	if (BTST(pWE->features, weFMonoStyled))
	{
		// MONOSTYLED TEXT
		// apply the change to the whole text, not just to the selection range
		if ((err = _WESetStyleRange(0, pWE->textLength, mode, (WETextStyle *) ts, hWE)) != noErr)
		{
			goto cleanup;
		}

		// invalidate the null style record
		BCLR(pWE->flags, weFUseNullStyle);

		// redraw the text
		if ((err = _WERedraw(0, pWE->textLength, hWE)) != noErr)
		{
			goto cleanup;
		}
	}
	else if (inStart == inEnd)
	{
		// MULTISTYLED TEXT; NULL SELECTION
		// first make sure the nullStyle field contains valid information
		_WESynchNullStyle(hWE);

		// apply style changes to the nullStyle record
		_WECopyStyle((WETextStyle *) ts, &pWE->nullStyle.runStyle, pWE->nullStyle.runStyle.tsFace, mode);

		// special case: if this instance is empty, propagate the
		// change to the style table (this avoids some subtle problems)
		if (pWE->textLength == 0)
		{
			if ((err = _WESetStyleRange(0, 0, weDoAll + weDoReplaceFace, &pWE->nullStyle.runStyle, hWE)) != noErr)
			{
				goto cleanup;
			}
		}

#if !WASTE_NO_SYNCH
		// if the font was altered, synchronize the keyboard script
		if (BTST(pWE->flags, weFNonRoman) && (mode & weDoFont))
		{
			fontScript = FontToScript(pWE->nullStyle.runStyle.tsFont);
			if (fontScript != GetScriptManagerVariable(smKeyScript))
			{
				KeyScript(fontScript);
			}
		}
#endif
	}
	else
	{
		// MULTISTYLED TEXT; NON-EMPTY SELECTION

		// increment modification count
		pWE->modCount++;

		// check for "smart" font modes
		if (BTST(pWE->flags, weFNonRoman) && ((mode & weDoSmartFont) == weDoSmartFont))
		{
			if ((err = _WESmartSetFont(mode, ts, hWE)) != noErr)
			{
				goto cleanup;
			}
			mode &= ~weDoFont;
		}

		// set the style of the selection range
		if ((err = _WESetStyleRange(inStart,inEnd, mode, (WETextStyle *) ts, hWE)) != noErr)
		{
			goto cleanup;
		}

		// and redraw the text
		if ((err = _WERedraw(inStart,inEnd, hWE)) != noErr)
		{
			goto cleanup;
		}
	}

	// clear the result code
	err = noErr;

cleanup:
	// unlock the WE record
	_WESetHandleLock((Handle) hWE, saveWELock);

	// return result code
	return err;
}