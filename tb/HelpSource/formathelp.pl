#!/usr/bin/perl -w

# formathelp.pl
# © Mark Tully and TNT Software 2002
# 12/4/02

# ***** BEGIN LICENSE BLOCK *****
# 
#  Copyright (c) 2002, Mark Tully and John Treece-Birch
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
# 
#  * Redistributions of source code must retain the above copyright notice, this list
#    of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright notice, this
#    list of conditions and the following disclaimer in the documentation and/or other
#    materials provided with the distribution.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
#  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
#  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
#  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
#  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGE.
# 
# ***** END LICENSE BLOCK *****

# Script to convert TNT Basic .hlp files into html help files for the help system
# .hlp files are (possibly, or nearly) XML files describing the help. This formats
# them into html using a skeleton file.
# You can embed html into the xml file if you want.

# Saves us formatting them and means we can change format at any time
# Also automatically hyperlinks commands to other TNT Basic commands and
# builds the TOC for each command section.

# This script will convert all .hlp files it finds in the help source dir. It will use the
# 'tbformat' command to automatically format TNT Basic code embeded in the .hlp files.
# It could be expanded to eventually automatically hyperlink all the cmds in the source
# into the help system

# Usage
# cd into the Help Source dir and run formathelp.pl. It generates html files to the
# help folder in the distribution folder.
# By default it only converts newer files.
# pass -c to do a clean convert, convert all files regardless of whether they're new
# or changed.
# pass -t to build a commands index into the file commandsindex.txt. This is then used
# to cross hyperlink the text in the desc tags of the files. To use it, specify a command
# to be linked by surrounding it with @@. Eg @@Draw Frame@@ or @@Animate Sprite@@.
# Files to link to are looked up from the commandsindex.txt file (generated with -t),
# looked up from the manualcommandsindex.txt (manual setups) or guessed from the command name.
# You will need the commandsindex.txt file to be updated if you add a help file that uses
# the <covers> tag to cover more than one command, as otherwise the hyperlinker won't know
# which file that command is covered in.

# The .hlp files are basically XML files with the following tags:
# <name>		Name of the command / page
# <synopsis>	Brief description of what the command does or what the guide is about
# <syntax>		List of the various ways of calling the command separated by newlines (only needed for commands)
# <group>		The name of the directory that the table of contents for this group of commands can be found
# <type>		Specifies the type of the help file, currently either 'command' or 'guide'. If omitted defaults to 'command'.
# <covers>		Declares a comma separated list of other commands this help file covers, or just alternate names for this file (references to these synonyms will link to this page)
# <desc>		Description of the command, within the description the following tags can also be used
#	<tnt>		Declares a block of TNT Basic code, will be syntax highlighted using tbformat
#	@@command@@	Hyperlinks the command/pagename enclosed in the @@ to the relevant page
#	<url>http://www.tntbasic.com/</url>	Hyperlinks the url to the web
#	<sub>		Declare a sub title
#	<section>	Declare a titled section
#	<anchor>	Declare an html anchor that can be linked to. This is like using the <covers> tag, except it also lets links to this references link to a particular anchor within the file, rather than use the file only
#	<leftimage>	Creates a left aligned image on the page. the info between the tags should be the file name in the help/gfx directory to display. eg <leftimage>title.gif</leftimage>

################

use Getopt::Std;

# process options

use vars qw($opt_c $opt_t);		# stops perl moaning the vars just popped into existence without being declared
getopts('ct');			# c = clean

# global hash for all the skeleton files used to generate the html
%gSkeletons = ();
$gSkeletons{"command"}="skeleton.htm";
$gSkeletons{"guide"}="guide_skel.htm";

# global hash for all tag/value pairs
%tags = ();

# global hash for all command/file pairs
# maps from cmd name => htm file path rel to help root dir
%commands = ();

# run from inside the help source dir
$gSourceDirPath = ".";

# dest dir root relative to cd
$gDestDirRoot = "../Distribution/TNT Basic Help/";

# destination directories relative to dest dir root for the different types
%gDestDirs = ();
$gDestDirs{"command"}="commands/";
$gDestDirs{"guide"}="guides/";

# path from html file  to help root
%gPathToRoot = ();
$gPathToRoot{"command"}="../";
$gPathToRoot{"guide"}="../";

$descFont = "<FONT SIZE=\"-1\" FACE=\"Geneva\">";
$preSectionTitle = "<\/blockquote><p><b><FONT COLOR=\"#330099\" SIZE=\"-1\" FACE=\"Helvetica,Arial\">";
$postSectionTitle = "<\/FONT><\/b><\/p><\/td><\/tr><tr><td><blockquote>$descFont";

if (!$opt_t)
{
	&loadCommands();		# don't load commands if building an index
}

opendir(SOURCEDIR, $gSourceDirPath) or die ("Can't open source dir $gSourceDirPath! $!\n");

@contents = readdir(SOURCEDIR);

closedir(SOURCEDIR);

foreach $filename (@contents)
{
	if ($filename =~ m/\.hlp$/)
	{
		if ($opt_t)
		{
			# read command and add command/filename pair to hash table
			&extractCommand($filename);
		}
		else
		{
			$sourceModDate = (stat($filename))[9];
						
			# if running in clean mode convert all files
			if ($opt_c)
			{
				&convertFile($filename);
			}
			else
			{
				# we don't know what sub directory this file will end up in until we parse it, so the only
				# way we can test if it exists is to either parse it, or to try all possible file names and
				# convert it if none of them exist.
				# the safest way would be to extract the tags and use the type to check the file name, but that
				# will be a bit slow, so check all possible locations and check also that it only exists in one
				# location (in case the type was changed then the file was reconverted)

				$foundCount=0;
				$outOfDate=0;
				$foundDestFiles="";
				foreach $type (keys %gDestDirs)
				{
					$tryFileName=$gDestDirRoot.&getDestFileNameForType($filename,$type);

					# does the dest file exist?
					if ( -e $tryFileName)
					{
						$foundCount++;
						$foundDestFiles.=$tryFileName."\n";

						if ( $sourceModDate > (stat($tryFileName))[9])
						{
							$outOfDate=1;
						}
					}
				}

				if ($foundCount==0)
				{
					print "$filename: Dest file does not exist, creating...\n";
					&convertFile($filename);
				}
				elsif ($foundCount==1 && $outOfDate)
				{
					print "$filename: Source newer than dest, refreshing...\n";
					&convertFile($filename);
				}
				elsif ($foundCount>1)
				{
					die "$filename: Multiple destination files found for input, please remove some of them to stop automatic links getting confused\n***$foundDestFiles***\nAfter deleting and checking there are no other duplicates, it would be a good idea to run a table rebuild then a clean convert to verify all links\n";
				}
			}
		}
	}
}

if ($opt_t)
{
	print "Saving commands...\n";
	&saveCommands();
}

################

# loads the global commands hash from the commandsindex file
sub loadCommands()
{
	my $INFILE = "commandsindex.txt";
	my $comm="";
	my $file="";
	my $anchor="";
	
	open (INPUT, $INFILE) or die "Can't open output file \"$INFILE\" $!";
	
	while (<INPUT>)
	{
		chomp;		# get rid of cr
		($comm,$file) =  split(/ = /);
		$commands{$comm}=$file;
	} 
	
	close INPUT;

	# load manually entered commands index
	$INFILE = "manualcommandsindex.txt";
	open (INPUT, $INFILE) or die "Can't open output file \"$INFILE\" $!";
	
	while (<INPUT>)
	{
		chomp;		# get rid of cr
		($comm,$file) =  split(/ = /);
		($file,$anchor) = split(/#/,$file);
		
		# check specified file exists
		if (!-e $gDestDirRoot . "/" . $file)
		{
			die ("File $file for manually specified command $comm cannot be found!");
		}
		
		if (defined $anchor)
		{
			$file=$file."#$anchor";
		}
		
		$commands{$comm}=$file;
	} 
	
	close INPUT;
}

# saves the global commands hash to the commandsindex file
sub saveCommands()
{
	my $OUTFILE = "commandsindex.txt";
	my @keys = keys %commands;

	open (RESULT,">$OUTFILE") or die "Can't open output file \"$OUTFILE\" $!";
	
	foreach $num (@keys)
	{
		print RESULT "$num = $commands{$num}\n";
	}
	
	close RESULT;
}

# returns the dest file name, relative to the help root, for the file passed. uses the parsed tags to determine
# the path
# outfilepath = getDestFileName ( infilepath )
sub getDestFileName()
{
	my $INFILE = shift;
	return &getDestFileNameForType($INFILE,$tags{"type"});
}

# same as above, except pass the type as well
# outfilepath = getDestFileNameForType (infilepath,type)
sub getDestFileNameForType()
{
	my $INFILE = shift;
	my $outFile;
	my $type=shift;
	my @temp;
	my $filename;

	if (!defined $gDestDirs{$type})
	{
		die "Couldn't find destination dir for file of type $type\n";
	}

	@temp = split (/\//,$INFILE);		# strip path from file name
	$filename=$temp[@temp-1];
	$filename=~s/\.hlp$/\.htm/;
	$outFile=$gDestDirs{$type}.$filename;

	return $outFile;
}
	

# extracts the name/html file pair from a .hlp file and stores it in the global hash commands
# void extracCommand ($infilepath)
sub extractCommand()
{
	my $INFILE = shift;
	my @temp;
	my $extra="";
	my $filename="";
	my $anchor;

	&extractTags($INFILE);
	&checkTags($INFILE);		# will default any unspecified tags if needed

	# the dest file name we're about to put into the commands hash needs to be relative to the help root
	# the sub dir the html file will be in will depend on the type of the file, as defined by the <type> tag
	# now that the file has been parsed, we can get the dest file name

	$filename = &getDestFileName($INFILE);

	$commands{$tags{"name"}}=$filename;
	
	# sometimes a help page covers more than one command. A comma separated list of commands also covered is
	# listed in the optional 'covers' tag.
	if (exists($tags{"covers"}))
	{
		@temp = split (/, /,$tags{"covers"});
	
		foreach $extra (@temp)
		{
			$commands{$extra}=$filename;
		}
	}

	if (exists($tags{"desc"}))
	{
		while ($tags{"desc"} =~ m%[^<]*<anchor>((\n|\r|.)*)</anchor>%g)
		{
			$anchor=$1;
			$commands{$anchor}=&ref2anchorname($anchor);
			$commands{$anchor} = $filename."#".$commands{$anchor};
			print "$anchor = ".$commands{$anchor}."\n";
		}
	}
}

########################################################################
## ref2anchorname
########################################################################
# takes a string and strips the spaces and makes it lower case
sub ref2anchorname()
{
	my	$anchor = shift;

	$anchor = lc($anchor);		# lower case
	$anchor =~ s/[ \r\n]//g;		# remove spaces
	return $anchor;
}

########################################################################
## cleanhtml
########################################################################
# takes some text that contains a mixture of valid tags and text and
# replaces all the < and > that are not forming part of a valid tag
# declaration with &lt; and &gt; so they will be displayed correctly in
# a browser
sub cleanhtml()
{
	my	$input = shift;
	my  $output="";
	my  $pretag;
	my  $value;
	my  $tag;
	my	$remainder=$input;

	#printf ("cleanhtml ENTER <<<<<<<<<<<< $input >>>>>>>>>>>>>\n");

	while ($input =~ m%((\n|\r|.)*?)	# non greedy pre tag - match as much as possible without grabbing stuff that could be matched by later components
					<					# open tag
					([^>]*)				# tag content
					>					# close tag
					((\n|\r|.)*?)		# tag value non greedy - can grab </---> as long as it doesn't match the end tag we're currently looking for
					<					# open tag
					/\3>				# /tagcontent and tag close
					%gx)
	{
		$remainder=$';	# store str after matched pattern and set found to true. the contents of remainder will become the pretag if we have another successful match, else it will have to be used as the posttag of this match
		#printf("cleanhtml - match\n$1********$3**********$4*********END MATCH\n");
		#$found=1;
		$pretag=$1;
		$tag=$3;
		$value=$4;

		$pretag=~s/</&lt;/g;
		$pretag=~s/>/&gt;/g;
		$value=&cleanhtml($value);		# as value can contain more tagged up data we need to recurse into it and parse its tags
		$output.="$pretag<$tag>$value</$tag>";
	}

	#printf("Dont forget $remainder\n");
	$remainder=~s/</&lt;/g;
	$remainder=~s/>/&gt;/g;
	$output.=$remainder;
	
	return $output;
}

#############################################################################

# pass a .hlp file to extract all its tags into the global hash %tags
sub extractTags()
{
	my $INFILE = shift;

	# reset the hash
	%tags = ();
			
	if (length($INFILE)>32)
	{
		print "*** WARNING: Filename: $INFILE is too long for Mac OS 9\n";
	}
	
	# open input file
	open (SOURCE,$INFILE) or die "Can't open input file $INFILE $!";	
	
	# read all input
	$in="";
	while (<SOURCE>)
	{
		$in.=$_;
	}
	
	close SOURCE;
	
	# preprocess and literal < and > from the text. These will be prefixed with a /
# TODO should get rid of this by automatically identifying < and > that aren't tag delimiters
#	$in =~ s%/>%&gt;%g;
#	$in =~ s%/<%&lt;%g;
	
	# extract all tags
	
	# print "$in\n";
	
	#while ($in =~ /<([^>]*)>([^<]*)<\/\1>/g)	#	Non greedy, doesn't allow sub tags
	#while ($in =~ /[^<]*<([^>]*)>(.*)<\/\1>[^<]*/g)	
	#while ($in =~ /[^<]*<([^>]*)>((\n|\r|.)*)<\/\1>[^<]*/g)		works
	
	while ($in =~ m%[^<]*				# not a tag open
					<					# open tag
					([^>]*)				# tag content
					>					# close tag
					((\n|\r|.)*)		# tag value
					<					# open tag
					/\1>				# /tagcontent and tag close
					([^<]*)				# stuff after tag
					%gx)
	{
	#	print "Tag -- $1 Value -- $2\n";
		$tags{$1}=&cleanhtml($2);
	}	
}

################

# pass the in file path relative to cd
sub convertFile()
{
	my $INFILE = shift;

	&extractTags($INFILE);
	#&dumpTags();
	&checkTags($INFILE);
	&dumpSkel($INFILE);
}

sub dumpTags()
{
	my @keys;

	print "Dumping tags...\n";
	
	@keys = keys %tags;
	
	foreach $num (@keys)
	{
		print "$num = $tags{$num}\n";
	}
}

# pass the name of the source file the tags were read from
# expects the tags has to be fully filled out with the contents of said file
# void dumpSkel( $inputfilepath )
sub dumpSkel()
{
	my $INFILE = shift;
	my $OUTFILE;
	my $skelFile = $gSkeletons{$tags{"type"}};
	my $isCommand = $tags{"type"} eq "command";
	my $isGuide = $tags{"type"} eq "guide";
	
	$OUTFILE = $gDestDirRoot.&getDestFileName($INFILE);
	print "Creating file $OUTFILE using $skelFile...\n";

	open (SKEL,$skelFile) or die "Can't open template file \"$skelFile\" $!";
	open (RESULT,">$OUTFILE") or die "Can't open output file \"$OUTFILE\" $!";
	
	&formatDesc();

	if ($isCommand)
	{
		&formatSyntax();
	}

	$TOC = "../".$tags{"group"}."/".$tags{"group"}."_frameset.htm";

	while (<SKEL>)
	{
		s/\$NAME/$tags{"name"}/gi;
		s/\$DESC/$tags{"desc"}/gi;
		s/\$TOC/$TOC/gi;
		s/\$SYNOPSIS/$tags{"synopsis"}/gi;

		if ($isCommand)
		{
			s/\$SYNTAX/$tags{"syntax"}/gi;
		}

		# change to mac line endings - this is necessary to get the help indexed correctly using apple help indexing tools
		#s/\n/\r/g;		# update: actually i don't think it is, i think just just need to ensure the Mac OS 9 file type is set to TEXT
		
		print RESULT $_;
	}

	close SKEL;	
	close RESULT;
}


# applies the TBFormat command to the text passed to format it
sub tbformat()
{
	my $input = shift;		# get input
	my $output;
	
	# replace " with ¾ (opt speech mark) or echo doesn't echo the text properly
	$input =~ s/\"/¾/g;

	# note any < and > symbols in the source will have been changed to &gt; and &lt; by
	# the cleanhtml func. Ideally we could do with switching them back before passing to
	# tb format and then switching them back afterwards. However, switching them back
	# afterwards is difficult as by that point there's loadsa tags in the source containing
	# < and > of their own. The easier and more efficient solution is to get the tbformat
	# to accept &gt; and &lt; as > < and preserve them through into its output
	# at the moment, tbformat doesn't have any handling for &lt; &gt; and so treats them as
	# unknown tokens - however unknown tokens are output in the same colour as < > would be
	# (black) so it's ok
	#$input =~ s/&gt;/>/g;
	#$input =~ s/&lt;/</g;
	
	$output = `echo "$input" | ../TBFormat/tbformat`;

	$output = "</FONT><FONT COLOR=\"#000000\" SIZE=\"-2\" FACE=\"Monaco,Arial\">".$output."<\/FONT>".$descFont;

	# replace tags with proper formattting
	$output =~ s/<rem>((.|\r|\n)*?)<\/rem>/<FONT COLOR=\"#FF0000\">$1<\/FONT>/g;
	$output =~ s/<key>((.|\r|\n)*?)<\/key>/<FONT COLOR=\"#0000FF\">$1<\/FONT>/g;
	$output =~ s/<comm>((.|\r|\n)*?)<\/comm>/<FONT COLOR=\"#4985c6\">$1<\/FONT>/g;	

	# expand tabs to spaces for formatting
	$output =~ s/[\t]/&nbsp;&nbsp;&nbsp;&nbsp;/g;

	return $output;
}

# create a hyper link to a tnt basic command. Pass in the name of a command and get back the hyperlink string
# to insert into a html document.
# pass in the root from the document to contain the aref to the help root as a relative dir. eg ../
# htmlaref = linkCommand ( $incommandname , $inPathToHelpRoot )
sub linkCommand()
{
	my $command = shift;
	my $pathToRoot = shift;
	my $destfile = "";
	my @keys;
	my $type;
	my $tryPath;
	my $found=0;

	if (not exists ($commands{$command}))
	{
		# try and guess the link to the dest file...
		$destfile = $command;
		$destfile =~ s/ //g;
		$destfile = $destfile.".htm";
		$destfile = lc($destfile);
	
		# for each sub dir that can contain output files, see if the file is in there
		@keys = keys %gDestDirs;

		foreach $type (@keys)
		{
			$tryPath=$gDestDirRoot.$gDestDirs{$type}.$destfile;
			print "Looking for $tryPath\n";
	
			if (-e $tryPath)
			{
				# found it - mustn't have been in our index, probably a htm file not generated from a .hlp file
				$destfile=$gDestDirs{$type}.$destfile;
				# cache it for future use
				$commands{$command}=$destfile;
				$found=1;
				last;
			}
		}

		$found or die "*** ERROR: Cannot hyperlink command $command!\n";
	}
	else
	{
		$destfile=$commands{$command};
	}
	
	return "<A HREF=\"" . $pathToRoot . $destfile . "\">$command</A>";
}

# this sub formats the desc tag into html
# hyperlinks commands
# replaces single crs with <br>
# replaces double-crs with <p> and </p> pairs
# formats text in <sub></sub> pairs with sub-title formatting
# makes <url></url> tags into hyper links
sub formatDesc()
{
	my	$pathToRoot;

	if (exists ($tags{"desc"}))
	{
		$pathToRoot=$gPathToRoot{$tags{"type"}};
		defined $pathToRoot or die "Path to root for type ".$tags{"type"}." hasn't been defined\n";

		$tags{"desc"} =~ s/^[ \t\r\n]*//;										# drop initial white space if any
		$tags{"desc"} =~ s/<sub>((.|\r|\n)*?)<\/sub>[\n\r]/<b>$1<\/b><br>/gi;	# change sub title texts to be in bold
		$tags{"desc"} =~ s/<section>((.|\r|\n)*?)<\/section>[\n\r]/$preSectionTitle$1$postSectionTitle/gi;	# change section titles
		$tags{"desc"} =~ s/<anchor>((.|\r|\n)*?)<\/anchor>/"<a name=\"".&ref2anchorname($1)."\">"/gie;			# create html anchors
		$tags{"desc"} =~ s%<leftimage>((.|\r|\n)*?)</leftimage>%<img src="../gfx/$1">%gi;

		$tags{"desc"} =~ s/<url>((.|\r|\n)*?)<\/url>/<a HREF="$1">$1<\/a>/gi;		# link urls
		
		$tags{"desc"} =~ s/<tnt>((.|\r|\n)*?)<\/tnt>
						/&tbformat($1)/gixe;									# put through tnt basic formatter

		$tags{"desc"} =~ s/(^|[^\n\r])[\n\r]($|[^\n\r])/$1<br>$2/g;				# sub single crs as <br> tags
		$tags{"desc"} =~ s/([^\n\r]+)[\n\r]+/<p>$1<\/p>/g;						# sub 2 or more crs as <p></p> paras
		
		$tags{"desc"} =~ s/@@(.*?)@@/&linkCommand($1,$pathToRoot)/ge;
		$tags{"desc"} = $descFont.$tags{"desc"}."</FONT>";
	}
}

# formats the syntax line
# adds a br between multiple lines
sub formatSyntax()
{
	if (exists ($tags{"syntax"}))
	{
		$tags{"syntax"} =~ s/([^\n\r]+)[\n\r]/$1<br>/g;
	}
}

# checks all required tags have been found - reports errors if not
sub checkTags()
{
	my $INFILE = shift;
	my $fileType;

	if (exists ($tags{"type"}))
	{
		$fileType=$tags{"type"};
		if ($fileType ne "command" && $fileType ne "guide")
		{
			die "*** ERROR: File $INFILE has an unknown type ($fileType) specified in its <type> tag!\n";
		}
	}
	else
	{
		$fileType=$tags{"type"}="command";
	}

	if (not exists ($tags{"name"}))
		{ die "*** ERROR: File $INFILE has no <name> tag!\n"; }
	if (not exists ($tags{"group"}))
		{ die "*** ERROR: File $INFILE has no <group> tag!\n"; }
	if (not exists ($tags{"desc"}))
		{ die "*** ERROR: File $INFILE has no <desc> tag!\n"; }
	if (not exists ($tags{"synopsis"}))
		{ die "*** ERROR: File $INFILE has no <synopsis> tag!\n"; }
		
	if ($fileType eq "command")
	{
		if (not exists ($tags{"syntax"}))
			{ die "*** ERROR: File $INFILE has no <syntax> tag!\n"; }
	}
}
