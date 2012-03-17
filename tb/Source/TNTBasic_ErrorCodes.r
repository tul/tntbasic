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

resource 'MErr' (16091, "File Type Wrong Length") {
	"The file type is an incorrect length.  I"
	"t must be exactly 4 characters long."
};

resource 'MErr' (16090, "File Type Could Not Be Found") {
	"The type for this file could not be foun"
	"d."
};

resource 'MErr' (16089, "Wrong File Type") {
	"The file could not be opened because it "
	"is the wrong type."
};

resource 'MErr' (16088, "Unexpected End Of File") {
	"Unexpected end of file."
};

resource 'MErr' (16087, "Could Not Delete File") {
	"Could not delete file."
};

resource 'MErr' (16086, "Could Not Create Folder") {
	"Could not create folder."
};

resource 'MErr' (16085, "Could Not Open File") {
	"Could not open file."
};

resource 'MErr' (16084, "Could Not Create File") {
	"Could not create file."
};

resource 'MErr' (16083, "File Is Not Open") {
	"The file is not currently open."
};

resource 'MErr' (16082, "unexpected char") {
	"Unexpected character."
};

resource 'MErr' (16081, "expected [") {
	"Expected a “[” to follow the array “∆VaN"
	"m”."
};

resource 'MErr' (16080, "prog info too new") {
	"Cannot read the program info resource be"
	"cause it is too new."
};

resource 'MErr' (16079, "for index must be int") {
	"The index variable in a for loop must be"
	" an integer variable or an item from an "
	"integer array."
};

resource 'MErr' (16078, "Viewport Not Open") {
	"The specified viewport is not open"
};

resource 'MErr' (16077, "Viewport Off Screen") {
	"The viewport can only be displayed if it"
	" is completely on the screen."
};

resource 'MErr' (16076, "Viewport Out Of Canvas") {
	"The viewport can only be specified so th"
	"at all of it lies inside its canvas."
};

resource 'MErr' (16075, "Viewport Canvas Zero") {
	"Viewports cannot be specified to copy fr"
	"om canvas 0."
};

resource 'MErr' (16074, "Expected Open Bracket") {
	"Expected an Open Bracket"
};

resource 'MErr' (16073, "not a variable") {
	"Expected a variable."
};

resource 'MErr' (16072, "expected type name") {
	"Expected a type name, eg. “int”, “str” o"
	"r “float”."
};

resource 'MErr' (16071, "unmatched syntax error") {
	"*** Uncategorised syntax error ***"
};

resource 'MErr' (16070, "attempt to share decl var") {
	"You cannot share parameters, or variable"
	"s which have been declared already as un"
	"-shared."
};

resource 'MErr' (16069, "shared must be in a proc") {
	"Shared variables can only be declared in"
	"side procedures. You may want to use “Gl"
	"obal” here instead."
};

resource 'MErr' (16068, "illegal proc name") {
	"Invalid procedure name."
};

resource 'MErr' (16067, "proc in proc") {
	"A procedure cannot be declared inside an"
	"other procedure."
};

resource 'MErr' (16066, "Resource Index Out Of Range") {
	"There are not enough resources of that t"
	"ype.  The index for that resource cannot"
	" be found."
};

resource 'MErr' (16061, "Syntax Error") {
	"Syntax Error."
};

resource 'MErr' (16060, "Manged proc call") {
	"Illegal procedure call."
};

resource 'MErr' (16059, "not a procedure") {
	"Cannot call “∆PrNm” as it's not a proced"
	"ure."
};

resource 'MErr' (16058, "illegal exp") {
	"Illegal expression."
};

resource 'MErr' (16057, "unexpected to") {
	"Unexpected “to”."
};

resource 'MErr' (16056, "unexpected ;") {
	"Unexpected “;”."
};

resource 'MErr' (16055, "unexpected comma") {
	"Unexpected “,”."
};

resource 'MErr' (16054, "unexpected param") {
	"Unexpected parameter."
};

resource 'MErr' (16053, "expected str exp") {
	"Expected a string value."
};

resource 'MErr' (16052, "expected num exp") {
	"Expected a numeric value."
};

resource 'MErr' (16051, "expected to") {
	"Expected a “to”."
};

resource 'MErr' (16050, "expected ;") {
	"Expected a “;”."
};

resource 'MErr' (16049, "expected ,") {
	"Expected a “,”."
};

resource 'MErr' (16048, "expected str in ass") {
	"Expected a string value but found a nume"
	"ric value."
};

resource 'MErr' (16047, "expected num value in ass") {
	"Expected a numeric value but found a str"
	"ing value."
};

resource 'MErr' (16046, "Negative Sprite Index") {
	"A negative sprite index was used.  Only "
	"positive numbers and zero can be used to"
	" index a sprite."
};

resource 'MErr' (16045, "expected ; ,") {
	"Expected “,” or “;” but found “to”."
};

resource 'MErr' (16044, "Could Not Load Input") {
	"The Input Sprocket Setup resource could "
	"not be loaded."
};

resource 'MErr' (16043, "Cant declare var with proc name") {
	"The variable “∆VaNm” cannot be declared "
	"because a procedure already exists with "
	"that name."
};

resource 'MErr' (16042, "Cant index non array") {
	"The ∆Typ1 “∆VaNm” cannot be indexed as i"
	"t is not an array."
};

resource 'MErr' (16041, "cant dim non array") {
	"Cannot dimension the ∆Typ1 “∆VaNm” as it"
	" is not an array."
};

resource 'MErr' (16040, "var redecl") {
	"Variable “∆VaNm” redeclared. Was declare"
	"d of type ∆Typ1, now declared of type ∆T"
	"yp2."
};

resource 'MErr' (16039, "undecl var") {
	"Identifier “∆VaNm” has not been declared"
	"."
};

resource 'MErr' (16038, "Too Many Coords") {
	"There were too many co-ords passed to th"
	"is command."
};

resource 'MErr' (16037, "Res Type Wrong Length") {
	"The resource type must be exactly 4 char"
	"acters long."
};

resource 'MErr' (16036, "Map Not Found") {
	"This map does not exist."
};

resource 'MErr' (16034, "Map Not Loaded") {
	"This map is not currently loaded into me"
	"mory.  To load a map into memory use the"
	" command \"Load Map\"."
};

resource 'MErr' (16033, "bad font size") {
	"Font size must be greater than zero."
};

resource 'MErr' (16032, "Matrix must be 2D") {
	"Special FX matricies must be 2 dimension"
	"al."
};

resource 'MErr' (16031, "bad canvas dim") {
	"Invalid canvas dimension."
};

resource 'MErr' (16030, "bad canvas index") {
	"Canvas indexes can only be between 0 and"
	" 20."
};

resource 'MErr' (16029, "canvas not open") {
	"Canvas ∆CvIx not open."
};

resource 'MErr' (16028, "can't resize main canvas") {
	"The main canvas cannot be resized."
};

resource 'MErr' (16027, "can't close main canvas") {
	"The main canvas cannot be closed."
};

resource 'MErr' (16026, "Globals Must Be Before Procedures") {
	"All globals must be declared before any "
	"procedures are defined."
};

resource 'MErr' (16025, "Bad Global Variable") {
	"You cannot declare individual array memb"
	"ers to be global. You may however declar"
	"e the entire array as global."
};

resource 'MErr' (16024, "Bad Parameter Variable") {
	"A procedures parameters cannot be receiv"
	"ed directly into an array member."
};

resource 'MErr' (16023, "Can't Use Global As Parameter") {
	"Variables declared as globals cannot be "
	"used to receive parameter values."
};

resource 'MErr' (16022, "Can't Share Parameter") {
	"A procedures parameter cannot be declare"
	"d as shared. Either change the name of t"
	"he parameter or change the name of the v"
	"ariable you are attempting to share."
};

resource 'MErr' (16021, "Bad Shared Variable") {
	"Array members cannot be individually dec"
	"lared as shared. You may however share t"
	"he entire array."
};

resource 'MErr' (16020, "Too many breaks") {
	"Attempt to break out of more loops than "
	"are currently executing."
};

resource 'MErr' (16019, "Creator Code Not Defined") {
	"This command requires the creator code t"
	"o be defined.  You can set a creator cod"
	"e in the “Info” section of your project."
};

resource 'MErr' (16018, "prog running") {
	"A TNT Basic progam is already executing."
};

resource 'MErr' (16017, "param miscount") {
	"The call to the procedure “∆PrNm” from l"
	"ine ∆SrLn has the wrong number of argume"
	"nts."
};

resource 'MErr' (16016, "param mis match") {
	"The call to the procedure “∆PrNm” from l"
	"ine ∆SrLn has at least one parameter of "
	"the wrong type."
};

resource 'MErr' (16015, "Stack overflow") {
	"Stack overflow."
};

resource 'MErr' (16014, "proc not def") {
	"Procedure “∆PrNm” not defined."
};

resource 'MErr' (16013, "null dim") {
	"Invalid array dimension."
};

resource 'MErr' (16012, "array already dimed") {
	"Array already dimensioned."
};

resource 'MErr' (16011, "array index wrong arity") {
	"Array index is the wrong arity."
};

resource 'MErr' (16010, "Array index out of bounds.") {
	"Array index out of bounds."
};

resource 'MErr' (16009, "array not dim") {
	"Array not dimensioned."
};

resource 'MErr' (16008, "bad frame rate") {
	"Bad frame rate (either far too high or z"
	"ero)."
};

resource 'MErr' (16007, "not in graphics mode") {
	"The program is not in graphics mode."
};

resource 'MErr' (16006, "read out of data") {
	"Out of readable data."
};

resource 'MErr' (16005, "read type mismatch") {
	"Read type mismatch."
};

resource 'MErr' (16004, "undef label") {
	"Label “∆LbNm” is undefined."
};

resource 'MErr' (16003, "dupe label") {
	"Duplicated label."
};

resource 'MErr' (16002, "For loop id mismatch") {
	"The identifier at a for loop's \"Next\" st"
	"atement does not match the one stated at"
	" the beginning of the for loop."
};

resource 'MErr' (16001, "Arith overflow") {
	"Arithmetic overflow."
};

resource 'MErr' (16000, "unrecongnised binary op") {
	"Binary operator not recognised."
};

resource 'MErr' (-30360, "NetSprocket Init Failed") {
	"NetSprocket initialisation failed.  Chec"
	"k NetSprocket is installed correctly on "
	"this machine."
};

resource 'MErr' (-30365, "Receiving Data Problem") {
	"A problem occured when trying to receive"
	" data."
};

resource 'MErr' (-30367, "Invalid Game Reference") {
	"The game reference passed was invalid."
};

resource 'MErr' (-30371, "Attempt To Host Failed") {
	"The attempt to host the game failed."
};

resource 'MErr' (16092, "must be registered") {
	"Sorry but that function is only availabl"
	"e in the registered version. Please see "
	"the TNT Basic “Read Me” file for how to "
	"register."
};

resource 'MErr' (16093, "can't find display") {
	"Can't find a suitable display mode for r"
	"esolution ∆X    by ∆Y   ."
};

resource 'MErr' (16094, "Could Not Initialise Input") {
	"The input could not be initialised."
};

resource 'MErr' (16095, "Could Not Poll Input") {
	"The input element could not be polled."
};

resource 'MErr' (16096, "Could Not Configure Input") {
	"The input could not be configured."
};

resource 'MErr' (16097, "Viewport bigger than canvas") {
	"You cannot open a viewport that is bigge"
	"r than its source canvas."
};

resource 'MErr' (16117, "missing end proc") {
	"Expected an 'End Proc' statement."
};

resource 'MErr' (16118, "bad file type") {
	"The file format “∆VaNm” is not recognise"
	"d by this version of TNT Basic."
};

resource 'MErr' (16120, "no input elements defiend") {
	"Your input resource has no input element"
	"s defined!"
};

resource 'MErr' (16121, "Invalid Data Type") {
	"The specified data type was not recognis"
	"ed."
};

resource 'MErr' (16122, "Networking Not Initialised") {
	"Networking has not yet been initialised."
	"  To initialise networking use the comma"
	"nd \"Initialise Networking\", it only need"
	"s to be called once and it can be done a"
	"t the start of the program."
};

resource 'MErr' (16123, "Network Game Already Running") {
	"A network game is already running.  You "
	"cannot be connected to more than one net"
	"work game at a time.  Make sure you call"
	" \"Leave Net Game\" before trying to host "
	"or join another game."
};

resource 'MErr' (16124, "Unrecognised resource file format") {
	"TNT Basic does not understand this file format."
};
