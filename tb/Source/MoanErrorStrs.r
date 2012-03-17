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

type 'MErr' {
    pstring;
};

resource 'MErr' (16098, "Moan: Missing next")
{
	"Expected a 'N'ext to match a previous 'F'or."
};

resource 'MErr' (16099, "Moan: Missing to")
{
	"Expected a 'T'o."
};

resource 'MErr' (16100, "Moan: Unexp (")
{
	"Unexpected '('."
};

resource 'MErr' (16101, "Moan: Too many nested fors")
{
	"Too many nested 'F'or loops, maximum is 15."
};

resource 'MErr' (16102, "Moan: Bad input")
{
	"Error in input string, probably a bad or out of place character."
};

resource 'MErr' (16103, "Moan: Expected reg")
{
	"Expected a 'R'egister, e.g. 'R3' or 'X'"
};

resource 'MErr' (16104, "Moan: Expected an =")
{
	"Expected a '=' operator."
};

resource 'MErr' (16105, "Moan: Unexpected next")
{
	"'N'ext without matching 'F'or."
};

resource 'MErr' (16106, "Moan: Expected a jump")
{
	"Expected a 'J'ump."
};

resource 'MErr' (16107, "Moan: Expected a label")
{
	"Expected a label, e.g. 'J A' or 'Jump Altcode'."
};

resource 'MErr' (16108, "Moan: Unknown func")
{
	"Unknown moan function, probably an error caused by using a capital letter where you shouldn't!"
};

resource 'MErr' (16109, "Moan: Unexpected op")
{
	"Unexpected operator."
};

resource 'MErr' (16110, "Moan: Unexpected op")
{
	"Malformed expression pair, check your brackets."
};

resource 'MErr' (16111, "Moan: Unexpected op")
{
	"Expected a ')' to match a previous '('"
};

resource 'MErr' (16112, "Moan: Bad reg id")
{
	"Bad register, valid registers are 'RA' thru 'RZ','R0' thru 'R9' and X,Y,I."
};

resource 'MErr' (16113, "Moan: Bad local reg")
{
	"“∆VaNm” is a bad local moan register, valid registers are 'R0' thru 'R9', 'X', 'Y', and 'I'."
};

resource 'MErr' (16114, "Moan: Bad global reg")
{
	"“∆VaNm” is a bad global moan register, valid registers are 'RA' thru 'RZ'."
};

resource 'MErr' (16115, "Moan: Bad address")
{
	"Internal moan error, bad branch address."
};

resource 'MErr' (16116, "Moan: Undefined label")
{
	"Label “∆VaNm” is undefined, can't jump."
};

resource 'MErr' (16119, "Moan: Expected value")
{
	"Expected a value."
};


