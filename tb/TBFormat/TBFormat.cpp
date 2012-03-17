/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 2002, Mark Tully and John Treece-Birch
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

//#include "CFlexBuffer.h"
#include <stdlib.h>
#include <stdio.h>

#define FLEX_SUPPORT_NAMESPACE TBParser
#include	"CFlexBufferSupport.h"
#undef FLEX_SUPPORT_NAMESPACE

#include "basic.tab.h"

#define	kMaxBufferSize		1024*1024*1
#define	kMaxTags			10000

extern char *basictext;
extern int	basicleng;
int basiclex();

struct STags
{
	enum ETagType
	{
		kRem,
		kCommand,
		kKeyword
	};
	ETagType	type;
	SInt32		start,end;
};

#define kRemStartTag		"<rem>"
#define kRemEndTag			"</rem>"
#define	kKeywordStartTag	"<key>"
#define kKeywordEndTag		"</key>"
#define kCommandStartTag	"<comm>"
#define kCommandEndTag		"</comm>"

int main()
{
	// read in stream and write to out stream
	char			*buffer=(char*)malloc(kMaxBufferSize);		// 1 mb
//	STag			*tags=(STag*)mallac(kMaxTags*sizeof(STag));
//	SInt32			tagCount=0;
	
	if (!buffer)
	{
		printf("Out of ram!\n");
		return 1;
	}
	
	size_t			read;
	
	read=fread(buffer,1,kMaxBufferSize,stdin);
	
	buffer[read]=0;
	buffer[read+1]=0;
	
	// Convert ¾ chrs into speech marks
	// Convert mac line endings to unix line endings for full on parsing using flex buffer class
	for (size_t i=0; i<read; i++)
	{
		if (buffer[i]=='¾')
			buffer[i]='"';
		else if (buffer[i]=='\r')
			buffer[i]='\n';
	}
		
	{
//		CFlexBuffer					buff(buffer,read+2,&TBParser::AllocAndSwitchToBuffer,&TBParser::DeallocBuffer,&TBParser::GetCurrentLexOffset);
		int							token;
		SInt32						end,start,len,lastPrintOffset=0;
		
		// sets up flex to scan an in memory buffer
		TBParser::AllocAndSwitchToBuffer(buffer,read+2);

		while (token=::basiclex())
		{
			if ((token>=IF) || (token==REM))	// more than or equal to the IF token should be shown in blue
			{
				start=TBParser::GetCurrentLexOffset();
				len=basicleng;
				
				end=start+len;
				
				if (start>lastPrintOffset)
				{
					// print text upto symbol out to the stdout
					fwrite(buffer+lastPrintOffset,1,start-lastPrintOffset,stdout);
				}

				// print tag
				if (token==REM)
					printf(kRemStartTag);
				else if (token<PRINT)
					printf(kKeywordStartTag);
				else
					printf(kCommandStartTag);
				
				// print out tag content
				fwrite(buffer+start,1,len,stdout);
				
				// print out end tag
				if (token==REM)
					printf(kRemEndTag);
				else if (token<PRINT)
					printf(kKeywordEndTag);
				else
					printf(kCommandEndTag);
				
				lastPrintOffset=start+len;
			}
		}
		
		if (lastPrintOffset<read)
		{
			// dump remaining text
			fwrite(buffer+lastPrintOffset,1,read-lastPrintOffset,stdout);
		}

	}

//	fwrite(buffer,1,read,stdout);
	
	free(buffer);
	
	return 0;
}
