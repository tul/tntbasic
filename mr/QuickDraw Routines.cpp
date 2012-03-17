// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// QuickDraw Routines.cpp
// Mark Tully
// 2/10/97
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

#include		"Marks Routines.h"
#include		<FixMath.h>
#include		<PP_Prefix.h>

// This proc draws the specified string, at the coords given, to fit inside width. The string is condensed and trucated if
// needs be. Width returns the final size of the string. Returns true if truncation was necessary.
Boolean DrawStringTrunced(StringPtr string,short x,short y,short *width,TruncCode truncCode,Boolean condenseFirst)
{
	short		normalWidth;

	if (IsNullString(string))
	{
		*width=0;
		return false;	
	}

	normalWidth=StringWidth(string);
	
	if (normalWidth>*width)
	{
		// string too wide, if we should condense the string first, do so now	
		if (condenseFirst)
		{
			GrafPtr		grafPtr;
			Style		style;
			Boolean		res;
			
			GetPort(&grafPtr);
#if TARGET_API_MAC_CARBON
			style=GetPortTextFace(grafPtr);
#else
			style=grafPtr->txFace;
#endif
			TextFace(style|condense);
			res=DrawStringTrunced(string,x,y,width,truncCode,false);
			TextFace(style);
		
			return res;
		}
		else
		{	
			Str255				truncedString;
	
			// truncate the string to fit in the width specified
			CopyPString(string,truncedString);
			TruncString(*width,truncedString,truncCode);
			
			MoveTo(x,y);
			DrawString(truncedString);
			*width=StringWidth(truncedString);
		}
	}
	else
	{
		MoveTo(x,y);
		DrawString(string);
		*width=StringWidth(string);
	}

	return (normalWidth!=*width);
}

// This routine calculates how many lines the given text will take in the current window with the current font/size
// It can fail in extreme circumstances in which case it returns 1 line.
/*short CalcLines(unsigned char *text,Size length,short wrapWidth)
{		
	Rect						tempRect={0,0,1000,wrapWidth};

	TEHandle					temp=TENew(&tempRect,&tempRect);
	if (!temp)
		return 1;	
	
	TESetText(text,length,temp);

	short						numLines=(**temp).nLines;
	TEDispose(temp);
	
	return numLines;
}*/

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ GetLineHeight
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Returns the line height in pixels that the current font settings generate.
unsigned short GetLineHeight()
{
	FontInfo	fontInfo;
	::GetFontInfo(&fontInfo);
	return fontInfo.ascent + fontInfo.descent + fontInfo.leading;
}

// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
//		¥ CalcLines
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// Based on PowerPlants UDrawingUtils::DrawTextWithJustification.
// Calcs the lines that some text would take up with the current font settings.
// * by GetLineHeight to get pixels.
unsigned short CalcLines(const unsigned char *inText,Size inLength,short inWrapWidth)
{
	unsigned short		numLines=0;
	Fixed				fixedWidth=::Long2Fix(inWrapWidth);
	Fixed				wrapWidth;
	SInt32				blackSpace, lineBytes;
	SInt32				textLeft = inLength;
	const unsigned char	*textEnd = inText + inLength;
	StyledLineBreakCode	lineBreak;
	
	while (inText < textEnd)
	{
		lineBytes = 1;
		wrapWidth = fixedWidth;
		
		SInt32	textLen = textLeft;		// Note in IM:Text 5-80 states
		if (textLen > max_Int16) {		//   that length is limited to the
			textLen = max_Int16;		//   integer range
		}
		
		lineBreak = ::StyledLineBreak((char*)inText,textLen,0,textLen,0,&wrapWidth,&lineBytes);

		inText += lineBytes;
		textLeft -= lineBytes;
		numLines++;
	}	
	
	return numLines;
}