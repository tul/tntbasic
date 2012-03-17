// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ
// AllNewMountVol.h
// Mark Tully
// 29/3/96
// Based on MoreFilesExtras.h
// ÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑÑ

/* ***** BEGIN LICENSE BLOCK *****
*
* Copyright (c) 1996, Mark Tully and John Treece-Birch
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

typedef unsigned char Str8[9];

typedef struct
{
	short length;				/* length of this record */
	VolumeType media;			/* type of media, always AppleShareMediaType */
	short flags;				/* 0 = normal mount; set bit 0 to inhibit greeting messages */
	char nbpInterval;			/* NBP interval parameter; 7 is a good choice */
	char nbpCount;				/* NBP count parameter; 5 is a good choice */
	short uamType;				/* User Authentication Method */
	short zoneNameOffset;		/* offset from start of record to zoneName */
	short serverNameOffset;		/* offset from start of record to serverName */
	short volNameOffset;		/* offset from start of record to volName */
	short userNameOffset;		/* offset from start of record to userName */
	short userPasswordOffset;	/* offset from start of record to userPassword */
	short volPasswordOffset;	/* offset from start of record to volPassword */
	Str31 zoneName;				/* server's AppleTalk zone name */					
	Str31 serverName;			/* server name */					
	Str27 volName;				/* volume name */					
	Str31 userName;				/* user name (zero length Pascal string for guest) */
	Str8 userPassword;			/* user password (zero length Pascal string if no user password) */					
	char filler1;				/* to word align volPassword */
	Str8 volPassword;			/* volume password (zero length Pascal string if no volume password) */					
	char filler2;				/* to end record on word boundry */
} AllNewVolMountInfo, *AllNewVolMountInfoPtr;
