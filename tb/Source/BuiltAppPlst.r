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

type 'TEXT' {
  	string;
};

resource 'TEXT' (129, "build plist")
{
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<!DOCTYPE plist SYSTEM \"file://localhost/System/Library/DTDs/PropertyList.dtd\">\n"
"<plist version=\"0.9\">\n"
"<dict>\n"
"	<key>CFBundleName</key>\n"
"	<string>$GAMENAME</string>\n"
"	<key>CFBundlePackageType</key>\n"
"	<string>APPL</string>\n"
"	<key>CFBundleGetInfoString</key>\n"
"	<string>Built with TNT Basic</string>\n"
"	<key>CFBundleIconFile</key>\n"
"	<string>128</string>\n"
"	<key>CFBundleIdentifier</key>\n"
"	<string>com.tntsoftware.$CREATORCODE</string>\n"
"	<key>CFBundleName</key>\n"
"	<string>$GAMENAME</string>\n"
"	<key>CFBundleSignature</key>\n"
"	<string>$CREATORCODE</string>\n"
"	<key>LSPrefersCarbon</key>\n"
"	<true/>\n"
"	<key>CFBundleDevelopmentRegion</key>\n"
"	<string>English</string>\n"
"	<key>CFBundleDocumentTypes</key>\n"
"	<array>\n"
"		<dict>\n"
"			<key>CFBundleTypeExtensions</key>\n"
"			<array>\n"
"				<string>$CREATORCODE</string>\n"
"			</array>\n"
"			<key>CFBundleTypeIconFile</key>\n"
"			<string>130</string>\n"
"			<key>CFBundleTypeName</key>\n"
"			<string>$GAMENAME Data File</string>\n"
"			<key>LSTypeIsPackage</key>\n"
"			<$ISPACKAGEBOOL/>\n"
"		</dict>\n"
"	</array>\n"
"	</dict>\n"
"	</plist>\n"
};
