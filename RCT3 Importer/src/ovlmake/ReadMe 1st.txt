OvlMake
=======
Note: OvlMake is released under the GPL. Among a lot of things mostly concerning developers, for users this means you USE IT AT YOUR OWN RISK!

Introduction
------------
OvlMake is the successor to OvlCompliler, a command line tool to compile simple xml files into ovl files. OvlCompiler was written by the_cook to provide a tool enabling creation of animated scenery for RCT3.
OvlMake is fully compatible with OvlCompiler, it basically works the same way. Therefore please read the_cook's original readme, which I've included ("OvlCompiler ReadMe.txt") before continuing here (of course you need to type 'ovlmake' intead of 'ovlcompiler' :-) ).

Improvements over OvlCompiler
-----------------------------
* You don't need to give an output file. !!!Please read below to understand what happens if you omit it.!!!
* OvlMake supports a few command line options. Run it without giving an input file to see them.
* You don't need to care about matching the name attributes of the bsh and ban tags anymore. They also don't need to match the filename of the xml or ovl file.
* You can give more than one bsh or ban tag, but only the first will be used. Mone will generate a warning as there is no sensible way they can be used within the limits of ovlcompiler's xml syntax. Best forget I told you it's possible =)
* Within the bsh tag you can reference bones via number (as described for ovlcompiler) or by name. This applies to the bone attribute of geomobj tags and the parent attribute of bone tags.
* The root ovl tag supports two new attributes, 'name' and 'file'. These influence what happens if you don't give an output filename (read below for a full explanation).
* Able to handle all image formats the importer can.
* OvlMake is basically a command line tool wrapping the library that will do the work in the next importer version, basically this means a full rewrite of ovlcompiler. All the original bugs therefore have been replaced by a bunch of entirely new ones =)
* OvlMake supports three different input formats. The first is ovlcompiler's xml format with the enhancements I've explained so far. The second one is the scn format of the RCT3 Importer. The third is a new xml format that will be used in future importer versions. This format was not designed to be written by humans, so some of it's attributes have obscure values. You can convert ovlcompiler xml and importer scn files into this format with a command line switch. !!!Caveat: ovlmake tells scn and xml files apart by the extension, not the content. Everything that doesn't have xml will be treated as a scn file. Do not feed it arbitrary files, that may lead to a crash!!!

!!!Giving no output filename!!!
-------------------------------
If you give no output filename, ovlmake tries to do "the right thing". Usually that will be what you want, but there might be some cases where it's not. Therefore it's essential for you to know what ovlmake does if you don't give an output filename.
!!!ovlmake overwrites files without warning!!!
There are basically two cases: The input file contains information about where and under what name to save or not.
Before I explain them, keep in mind that this information is stored in two parts. The first is the base name of the ovl file (eg the 'xyz' in 'xys.common.ovl'), the second one is the path the ovl is created in.

1. The input file contains the information
If the input file contains the info, it is used. This is the case for newer scn files that have been saved after creating the ovl. The same is true for the new importer xml format. It can also be given as the name and file attributes of the root ovl tag in ovlcompiler xml files. !!!The file tag stores only a directory, make sure it ends with a path separator ('\')!!!

2. The input file does not contain the information
Easy. The name defaults to the input file name, the path to the input file's path.

If you give an output file name, it supercedes any information given in the file. If only one part of information is given in the file, the other takes it's default.

A few final words
-----------------
I've put considearble efford into finally understanding how ovl files work. While I have not yet understood all, I was able to rewrite the creation code in a much cleaner fashion. This also leads to the creation of cleaner ovl files by ovlmake. Therefore it may be a good idea to use ovlmake to compile your scn files for static scenery as well.
I will not explain the new importer xml format here in detail. As I've said above, it was not made to be human read/writable. Currently there is only one thing where you might be tempted to use it: using multiple lods with animation. I guess anyone who may be inclined to do so is adventerous enough to figure it out for himself from converting scn and ovlcompiler xml files =)


Finally I'd like to thank the_cook for the original ovlcompiler and hope he won't kick my ass for replacing it =)

Cheers
Belgabor
