OvlMake
=======
Note: OvlMake is released under the GPLv3. Among a lot of things mostly concerning developers, for users this means you USE IT AT YOUR OWN RISK!
See License.txt for more information.
Starting with version 0.4 input xml files are validated and ovlmake bails out on errors.

Introduction
------------
OvlMake is the successor to OvlCompliler, a command line tool to compile simple xml files into ovl files. OvlCompiler was written by the_cook to provide a tool enabling creation of animated scenery for RCT3.
Starting with v0.4, ovlmake is no longer fully compatible with ovlcompiler! Therefore please read the_cook's original readme, which I've included ("OvlCompiler ReadMe.txt") before continuing here (of course you need to type 'ovlmake' intead of 'ovlcompiler' :-) ) and modified for all changes. The order of elements now matters as well, so you need to give everything in the order they are explained.

Improvements over OvlCompiler
-----------------------------
* You don't need to give an output file. !!!Please read below to understand what happens if you omit it.!!!
* OvlMake supports a few command line options. Run it without giving an input file to see them.
* You don't need to care about matching the name attributes of the bsh and ban tags anymore. They also don't need to match the filename of the xml or ovl file.
* You can give more than one ban tag, but more than one will generate a warning as normal animated CS makes only use of one.
* Minor LOD support, affects how multiple bsh tags are handled. See below for more info.
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

Minor LOD support
-----------------
By default, ovlmake acts like ovlcompiler and writes one LOD at 4000 distance. Starting with version 0.1.3, you can add an 'options' element with a 'lods' attribute. This element has to come last within the ovl element. You can set this attribute to 1, 3 or 4. Example:
<options lods="3" />
If you define 4 lods, the last one will be static. Distances are 40/100/4000 for 3, 40/100/300/4000 for 4 lods.
How exactly ovlmake proceeds depends on the number of bsh tags you define.
1 bsh tag: The single model is copied (and made static for the 4th lod if applicable).
2 bsh tags: Not supported, treated like one and will generate a warning for the second (which isn't used).
3 bsh tags: Used in the order they are defined. For 4 lods, the last will be copied and made static.
4 bsh tags: Used in order, last will be made static (so no need to define bones for it). Generates a warning for 3 lods.
More: Treated like 4, superfluous ones are ignored and yield warnings.
If you need more fine tuning, convert to an importer xml and do it there (this much should be obvious ^.^).

Proper Transparency
-------------------
After a very long time I now found out how to do proper transparency and, as far as possible, got rid of the X-ray effect. By "as far as possible" I mean that after lots of experiments I came to the conclusion that for some objects you cannot fully prevent X-ray (even some original game objects show X-ray if you look closely). Still I think that the X-ray left is a lot less severe than before, it seems to only affect the object itself and then (probably) only parts of the same mesh. It also seems to occur mostly looking at the object from the left, provided you didn't rotate it. Left is in relation from your original view when you start a sandbox (ie looking at the entrace from inside the park.
As to when this X-ray occurs I can give no good explanation. In general I'd say the more complex the object is, the likelier it is to occur. It may also include some unknown voodoo. If your object shows it, I've provided a few customization options that may help or not. To understand them, I first need to explain how proper transparency in RCT3 works.
Meshes are stored in OVLs in two data chunks, one contains a list of all the vertices and the other the faces (triangles) as three indices into the vertex list. For transparent objects (geomobj tags in ovlcompiler xml files with transparency masked or regular / importer scn files with meshes set to placing Texture Only or Unknown), the ovl file stores the face list three times, each one sorted along one axis. Now the crux is how the faces need to be sorted. From original ovls it seems to me that only the lowest coordinate (in the sorted direction) of the triangle matters. Still I have implemented several sorting algorithms that take other things into account.
To set the sorting algorithm, you have several options. The first is to set default algorithms via the ovlmake command line. You can set an over all default with the -t or --trianglesort command line options. If necessary, you can set defaults for the single axis with --trianglesortx, --trianglesorty and --trianglesortz.
The second option is to set algorithms for each mesh. You can do that with sortx, sorty and sortz attributes in geomobj tags for ovlcompiler xml files or mesh tags in importer scenery xml files. Note that these attributes supercede whatever you give with commandline options.
Note that the axis are the in-game axis, so y is up.
Here is a list of the sort algorithms you can use:
min: Sorts by the lowest coordinate. Seems to be used by the game ovls.
max: Sorts by the highest coordinate.
mean: Sorts by the mean of the three coordinates.
minmax: Sorts by the lowest coordinate, if those match, sub-sort by the highest. This is the default used when no algorithm is given.
maxmin: Sorts by the highest coordinate, if those match, sub-sort by the lowest.
The following algorithms are experimental. You are welcome to try them, but in my experiments they either worsened things or didn't help.
angle45: Sorts as if met from a viewing angle of 45deg over ground in x and z direction, by minmax in y.
minXZbyY: Sorts x (or z) by lowest coordinate, if those match, sub-sort by lowest y coordinate. y direction is sorted by minmax.
minXZbyOther: Sorts x (or z) by lowest coordinate, if those match, sub-sort by the other one (z for x and x for z). y direction is sorted by minmax.
minYbyX: Sorts y by lowest coordinate, if those match, sub-sort by lowest x coordinate. x and z are sorted by minmax.
minYbyZ: Sorts y by lowest coordinate, if those match, sub-sort by lowest z coordinate. x and z are sorted by minmax.
mininv: Sorts by the lowest coordinate, but in reverse direction. This usually reverses the X-ray effect (ie it now occurs from the right).
minmaxinv: Sorts by minmax, but in reverse direction. Same effect as mininv.
The only thing I can do is wish you luck. Remember you can convert scn files to importer scenery xml files for fine-tunig using the -c / --convert commantline switch. There is also the new -v switch to show more output. Among other things, it will tell you which algorithms were used for which mesh.

Proper Transparency, a brief tutorial for the non-commandline savvy
-------------------------------------------------------------------
- Set up your scenery file as usual in the importer. In the group/mesh settings set "placing" to
  - "Texture Only" maked transparency (for SIAlphaMask... styles).
  - "Unknown" for full range transparency meshes (SIGlass, SIWater or SIAlpha... texture styles)
  - Keep at Texture&Mesh other styles.
- Create an OVL file in the importer, then save the scn file (the order is important!).
- Import the ovl file to make sure it works in principle and you didn't ignore any important warings.
- Read and understand the Giving no output filename section above, because that's what you'll be doing in the next step.
- Drag and drop the scn file on the ovlmake executable, do NOT move the scn file to a new directory for that, it has to stay in the directory you saved it to.
- The ovl files you created in the importer will be overwritten.
- Reimport.

A few final words
-----------------
I've put considerable efford into finally understanding how ovl files work. While I have not yet understood all, I was able to rewrite the creation code in a much cleaner fashion. This also leads to the creation of cleaner ovl files by ovlmake. Therefore it may be a good idea to use ovlmake to compile your scn files for static scenery as well.
I will not explain the new importer xml format here in detail. As I've said above, it was not made to be human read/writable. Currently there is only one thing where you might be tempted to use it: using multiple lods with animation. I guess anyone who may be inclined to do so is adventerous enough to figure it out for himself from converting scn and ovlcompiler xml files =)


Finally I'd like to thank the_cook for the original ovlcompiler and hope he won't kick my ass for replacing it =)

Cheers
Belgabor
