RCT3 File Importer
==================

NOTE: If you're looking at the plain text version of this file, you may notice a few strange characters
or punctiation in the text. These allow automatic generation of a formatted HTML version. The engine used
for this (http://www.methods.co.nz/asciidoc/index.html[asciidoc]) allows to do that with the least impact
on readability in plain text. In short, just ignore the strange stuff :)

IMPORTANT: This ``documentation'' refers to the old scenery editor when you had to replace scenery and
couldn't make new scenery. It is now mainly included as a historic document, most information is superceded.
All in all if you're new, *please read the online tutorials on http://rct3.sourceforge.net instead of this!*




License
-------

  Copyright 2005 Jonathan Wilson
  The RCT3 File Importer is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2, or (at your option) any later
  version. See the file COPYING for more details.




Basics
------

The program includes source code so you can see how it works.

To use it,

- find a scenery file you wish to replace (any scenery is replacable as are walls).
- note down the name (e.g. barrel.common.ovl.
- after that, create a new scenery object in a 3D program.
- For reference, a normal thickness base block goes (in 3DS max units anyway) from -2 to 2 in the
  X axis, 0 to 1 in the Y axis and -2 to 2 in the Z axis.
- After you have the model, export it as a `*.ase` file.
- There is an issue with texturing because the ASE file format uses per-face texture coordinates whereas
  RCT3 uses per-vertex texture coordinates. So when exporting, you need to make sure that each vertex has
  one and only one texture coordinate for it (I know nothing about 3D packages like 3DS so I know nothing
  about how that is done).
- Make sure that it contains only one mesh (since thats all I support at this point)
- If your 3d program doesnt export to ASE directly, look online for a converter, there are a bunch of
  converters that can convert from various formats into `*.ASE`.
- Note that the scenery piece you make will inherit the attributes of the scenery piece you are replacing
  so it should match up (e.g. if its a 1/4 tile piece, the new one should be a 1/4 tile piece etc)
- Once you have the ASE file, the texture gets made as a bitmap with 8 bit indexed color (transparent
  textures are not supported yet).
- Next, choose a TextureStyle. To see what styles are available, open the rct3.dgf file with a text editor
  and search for TextureStyle. Styles beginning with SIOpaque are good ones to try.
- To create the ovl file, load the program.
- Press _Open ASE File_ and load the ASE file.
- Then press _Open Texture_ and load the texture.
- Fill in the TextureStyle name with the texture style you picked.
- Then press _Save OVL File_ to save the ovl file, it will ask you where to save it and what to call it.

Assuming all goes well, it will spit out a .unique.ovl and a .common.ovl.
Use these to replace the origonal files (after backing the origonals up of course) and try in-game.

WARNING: This program has not been tested in all possible configurations with all possible input files.
*BACK UP* all files you modify or mess with.

I am unaware of any instances where this program will mess up RCT3 but if it does, I need to
know so I can fix it.
Note that if you modify a file and frontier later patches that file, the patch will not apply properly.
So, before you apply a patch, restore modified files to the origonal backed up copies.
It is not my intention to release anything that screws up RCT3 but (due to bugs and/or me not understanding
the data formats properly), ovl files created with this program may cause crashes, errors, loss of data (parks
etc) or other side effects. Should any bugs or issues be found, I will fix them as soon as I an able.
When using custom ovl files, make sure to save your parks and building files and etc often to prevent problems.
And, remember, lots of stuff is still unknown so you might not get the exact results you expect (e.g. lighting might not be 100% on your object)

If you have any questions about the program or the source code (or about the work I
I have done in reverse engineering the RCT3 code), please do ask.

  You can post in the relavent threads on the RCT2.com forums or the atari forums.
  Or you can email jonwil@tpgi.com.au.
  Or you can sometimes find me in #rct3 on irc.enterthegame.com (the official atari RCT3 chat)
  And in #rct3 on irc.centralchat.net
  Of go to http://www.ataricommunity.com/chat/chat.html
  Or you can use my MSN JonathanWilson623@hotmail.com
  Or my ICQ 77572965
  Or my AIM jonwil2002




Small Additions by Klinn - May 2005
-----------------------------------

1. Added a 'completed' message when theme has been successfully installed.
2. Corrected ``Top'' & ``Left'' and ``Bottom'' & ``Right'' coordinate assignments
   for icon references. If you had previously switched these to make them
   come out right, load your theme and switch them back to normal.
3. Added support for re-colorable objects.

The _Create Scenery OVL_ has a new box at the bottom. Select one of four
options to determine how many color sets the player will be able to change:

- no recoloring.
- player will be able to change one set of colors.
- player will be able to change two sets of colors.
- player will be able to change three sets of colors.

You must also create a texture for the object which is in a very specific
format. This is the process to create a texture which supports recoloring,
assuming you are starting with a 24-bit or 32-bit RGB image:

- anything that should be changed with the first color must drawn in shades of blue.
- anything that should be changed with the second color must be drawn in shades of magenta.
- anything that should be changed with the third color must be drawn in shades of yellow.
- a PDF file should be included in this zip archive which contains a sample texture
  that came from the game's generic wall set - this will make the concept clearer.
- when converting an image to 256-color indexed mode, you MUST use the special
  color table supplied in this zip file to map the RGB colors to the proper
  indexed color slot; use `RCT3-RecolorableOVLColorTable.act` for Adobe PhotoShop
  or `RCT3-RecolorableOVLColorTable.cpl` with Corel Photo-Paint.
- use the converted 256-color bitmap texture when creating the OVL file.

A few more technical details for the curious. The indexed color table
for a recolorable object is split into three groups:

- colors 1 to 85 will become shades of the player's choice for the first color.
- colors 86 to 170 will become shades of the player's choice for the second color.
- colors 171 to 255 will become shades of the player's choice for the third color.
- the shades in each group run from lightest to darkest.
- use of color 0 is unknown, suggest leaving as pure white for now.




Brief Instructions On Using The Importer - by Klinn, updated by Belgabor
------------------------------------------------------------------------

Here are the steps I follow to put an object into the game using JonWil's Importer, Belgabor's
mod 1.5. These are by no means definitive, and not necessarily even good practice, but it should
at least get you started and then you can refine the information to suit your needs. I'll number
the steps in a detailed fashion just so we can refer to them from other parts of the process.
Yes, the formatting is horrible, but I'll clean it up later - brain dump first!

I'm not going to address how to create the 3D mesh itself, convert it to ASE (if you don't use
Milkshape), etc. Look in the Atari RCT3 General Forum for JonWil's threads for lots of advice
on creating 3D meshes in the correct format and helpful hints contributed by folks who are using
a variety of modelling software. The steps below are just about using JonWil's utility.

I'm going to be creating a theme called ``KLN-ElectroBits'', with a scenery item called
``KLN-EBitOne''. Here we go:



Step 1 - Preparation
~~~~~~~~~~~~~~~~~~~~

Before even using JonWil's importer, you need to set up the files required.

1. Create a working directory to hold the files you're working with, e.g. `My Documents\RCT3-CustomObjects`.
   Below are the basic ones you'll need.
2. Create a texture bitmap for your object, must have square power-of-two dimensions, e.g. 128x128,
   256x256, etc.
3. Create the object mesh in your 3D Editor of choice, convert to ASE or MS3D format, e.g. I have
   one called `KLN-EBitOne.ase`.
4. Create an icon texture bitmap, 24-bit colour, size 512x512 pixels. I suggest drawing a series of
   64x64 pixel boxes across it in a variety of colours. Each box will eventually become an icon in
   the game, but just use distinctive colours for now. You should have a checkboard of 8 coloured boxes
   across by 8 down if each one is 64x64 pixels.
5. Decide on a prefix for all of your themes, e.g. ``KLN-''.
6. Decide on a name for your theme, the whole group of objects, e.g. ``ElectroBits''.
7. Decide on a name for the specific object(s), e.g. ``KLN-EBitOne''. I strongly recommend that you use
   your initials as a prefix. If you choose something simple like ``Wall'', it will likely be over-written
   by somebody else's custom wall later.



Step 2 - Create Parts of Theme in JonWil's Editor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before assembling a theme, you need to create the parts it needs.

1. Start RCT3Imp.
   a. Enter the Prefix from step 1.5
   b. On the main _RCT3 Scenery Editor_ window, enter your Theme Name from step 1.6.
   c. Select the theme type it is most similar to, e.g. ``Sci-fi''. This affects if it will appear when
      the user is filtering the scenery list by RCT3 theme.
2. Select the _Create New Scenery OVL_ button.
   a. Click the _Open_ button in the 3D Object box, select your object's model file (ASE or MS3D, from
      step 1.3 above).
      Depending on your modeler you might need to fix the orientation of your model file.
      * If you use a modeler with a right-handed, z-axis up coorinate system (eg Blender), click _Fix
        Orient._ in the _Transformation Matrix_ Box.
      * If you use a modeler with a right-handed, y-axis up coorinate system (eg MilkShape), click
        _Edit_ in the _Transformation Matrix_ Box. In the new Window click _MirrorZ_ and _Ok_.
      * If you will always import from that same modeler, select "File->Save Default Matrix" to save
        the fixup as a default. You won't need to repeat the 2.a steps for further objects then.
        NOTE: This setting is stored in a small file in the same directory as the importer executable.
        If you delete it, the default is gone.
   b. Click the _Add_ button in the Texture box
      * Press _Open Texture_
      * Select the texture bitmap for object (step 1.2 above).
      * Press _Ok_
   c. If your object needs more than one texture, repeat from 2.b
   d. Select the first model mesh in the mesh list and press _Mesh Flags_.
      IMPORTANT: Biturn creates a dummy ``root'' mesh. Ignore that one.
      * Select the Texture Name of the texture that should be applied to this mesh in the respective
        dropdown list.
      * Select the Texture Style Name ``SIOpaque''.
      * Select _No Colors_ for the number of colors the player will be able to re-color in-game.
   e. If your object has more than one mesh, repeat from 2.b selecting the next mesh.
   f. Click _Save OVL File_ button, make sure you're in your working directory (step 1.1), give OVL
      file a name. The importer proposes ``prefix3DObjectname'' (from step 1.3/2.2.a). It's usually a
      good idea to keep it. Whatever you choose, at least keep the prefix. You should now see the
      `$$*.common.ovl$$` and `$$*.unique.ovl$$` files appear in your working directory.
   g. Select ``File -> Save Scenery File'' to save the object's settings. You can load them later if
      you need to change something or to recreate the ovl with an updated model file.
   h. Close the Create Scenery OVL window.
3. Select the _Icon Textures_ button.
   a. Click the _Add_ button.
   b. Click _Load Texture_, select the icon texture bitmap (step 1.4).
   c. Give it a _Texture Name_, e.g. ``KLN-IconTexture''. You can also keep the default which is the
      name of your bitmap file.
   d. Click the _Close_ button to get rid of the _Icon Textures Manager_ window.
4. Select the _Icon References_ button. We'll be creating two icons, one for the theme group as a whole,
   one for the specific scenery item. The ``icons'' will just be those boring solid-coloured boxes described
   in step 1.4, but that will show you how to get started.
   NOTE: If you need a lot of icons, you can simply press the _Auto_ button in the Icon References Manager
   window to automatically create 64x64 references for your complete icon texture.
   a. Click the _Add_ button.
   b. Create an icon for the group as a whole. I'll use the name ``KLN-ElectroBits'' and set Top=0, Left=0,
      Bottom=63, Right=63. This is the first coloured square of the icon texture bitmap, the one in the
      upper-left corner (step 1.4). Click the _OK_ button.
   c. Create an icon for the scenery item. I'll use the name ``KLN-EBitOne'' and set Top=0, Left=64, Bottom=63,
      Right=127. This corresponds to the second coloured square of the first row of the icon texture bitmap.
      Click the _OK_ button.
   d. You should see the two icon references in the list. Click the _Close_ button to get rid of the _Icon
      References Manager_ window.
5. Select the _Text Strings_ button. This is where we'll be creating some names used later on in the program.
   a. Click the _Add_ button, enter a text string for the theme or group of objects as a whole, e.g.
      ``KLN-ElectroBits'', this will be copied to the name field if you leave it empty, click the _OK_ button.
   b. Click the _Add_ button again, enter a text string for the scenery object, e.g. ``KLN-EBitOne'', click
      the _OK_ button.
   c. Click the _Close_ button to get rid of the _Text Strings Manager_ window.



Step 3 - Combine Info Into Scenery Item and Theme File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Now we can finally pull the pieces together. I'm going to using specific values and settings that have worked
for me, but no doubt you'll want to change them to suit the objects you're making.

1. Select the _Scenery_ button.
   a. Click the _Add_ button.
   b. Click the _Select OVL_ button, choose the object's OVL file (from step 2.2).
   c. From the _Group Icon_ list, select the one we made in step 2.4.b.
   d. From the _Scenery Icon_ list, select the one we made in step 2.4.c.
   e. For _Location Within Theme_, specify a subfolder within the theme's overall folder, e.g.
      ``KLN-ElectroBitsSet''.
   f. Set _Cost_ = 1000, _Refund_ = 800. (becomes $10 and $8 respectively in US/Canada)
   g. The _Group Name String_ is the one from step 2.5.a.
   h. The _Scenery Name String_ is the one from step 2.5.b.
   i. Set _Position_ X=0, Y=0, Z=0.
   j. Set _Size_ X=4, Y=3, Z=4.
   k. Set _Size In Squares_ X=1, Y=1.
   l. Set _Size Flag_ to ``Full Tile''.
   m. Set _Scenery Type_ to ``Small Scenery''.
   n. Set _Supports Type_ to ``None''.
   o. Set _Extra Parameter_ to ``None''.
   p. Set _Unknown1_ to ``Unknown1'', _Unknown2_ to ``Unknown2''.
   q. Set _Unknown3_, _4_, _5_, _6_, _7_, _8_, _9_ all equal to zero.
   r. Don't check any of the _Flags_.
   s. Click the _OK_ button.
   t. Click the _Close_ button to get rid of the _Scenery Manager_ window.
2. Time to save our theme file with all this info we've been entering. Select _Save Theme_ from the _File_ menu.



Step 4 - Install Theme in Game
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

1. Select the _Install Theme_ button. If all goes well, check in the game's installation folder, e.g.
   `C:\Program Files\Atari\RollerCoaster Tycoon\Style\Themed\` and you should find a folder with your theme's
   name, inside it is a subfolder (from step 3.1.e), and inside that are your object's OVL files.



Step 5 - Test In-Game, Revise
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
1. Start up the game with a new scenario or sandbox park.
2. Go to the _Scenery_ -> _Scenery Items_ list, you should see a group named as per step 3.1.g, using an icon
   as per step 3.1.c.
3. Open the group, should see an icon for the object (step 3.1.d) using name from step 3.1.h. When you move
   your cursor over it, the rotating preview should appear in the bottom left. If you get any crashes, probably
   a bad ASE file, check previous posts for help.
4. Place your custom object, get annoyed at how it isn't what you expected, re-do 3D mesh and re-import it, make
   some real icons in the Icon Texture bitmap, etc, etc. Any time you make some changes, don't forget to
   ``re-compile'' the scenery item info (step 3) and re-install the theme (step 4). Whew.

Be sure to check the Atari RCT3 General Forum for JonWil's threads about his Importer. People are slowly figuring
out what some of the flags and other settings do, so helpful info can usually be found there.




Brief Instructions On My Effect Point Improvements - Belgabor
-------------------------------------------------------------

First we need to get some terms straight:

* scenery item - what you eventually want to be able to place in RCT3
* object - a part of your item, what will be listed as 'mesh' in the importer, so essentially a separately
  texturable piece of your scenery item. In Blender these are objects, so that's what they will be called =)
* mesh - connected bunch of vertices that make up faces.
* vertex (pl vertices) - a point in 3D space.

Probably much of this is common knowledge, but you need to know what I exactly mean (especially with ``object'').
So let's say you want to create a lamp, a pyramid with the light floating over the tip:

_[picture missing, see atari forums]_

You'll notice the point over the tip of the pyramid in the center of Blender's 3D cursor. This is a (from the
pyramid) separate object which consists of only one vertex. This is essential, separate object, only one vertex.

Now you export, convert etc. and bring your intem into the create scenery dialog of the importer. You should see
at least two meshes (in the example: the pyramid and the single vertex object). If you go to the mesh options
you'll find that the single vertex object has been automatically disabled (like the root object created by biturn).

If you now add an effect point you'll see a combo box that says 'Reset Default'. If you open it you'll see
your single vertex object listed. Select it and press 'Pos'. The position of the vertex will be entered into
the matrix, effect point set

If you press the Create button you'll get a list of templates for effect point names. The ones for lightstart
and particlestart lead to dialogs with more options, the other ones just paste in the effect point name and
select the number part if there is one.

An even slicker option is that if you name one-vertex objects like the effect point you want to create (and
the conversion of the item in biturn doesn't mangle them) you can press the 'Auto' button below the effect
point list and they will be created automatically. Even if you don't name them, just press 'auto' to add all
your one-vertex objects as effect points and edit their names.




Brief Instructions On My Recolorable Texture Improvements - Belgabor
--------------------------------------------------------------------

Introduction flom the changelog: Make the first color shades of red, the second shades of green and the third
shades of blue. Colors should be clean (eg no green in red). The darkest color should be still slightly colored,
not completely black (eg RGB 1,0,0 for red is enough). The brightest color should be the full color (eg RGB 255,0,0),
not the lightened color like in the original Blue/Magenta/Yellow scheme. Important: These textures need to be
truecolor, indexed images will be used as-is and need to use the old Blue/Magenta/Yellow palette. Also the RGB
textures are saved with the old palette, so don't be surprised if they look Blue/Magenta/Yellow in the RCT3 dumper =)

Now here is the tutorial to explain why this is easier (at least for me) than the other method:

- Create your texture in grayscale. Convert to RGB truerolor begore continuing (if it was already,
  convert to grayscale and back to ensure clean grayscale).
- Now select the part of the texture for the frist color choice.
- Adjust the values. This works like this:
  * Go to Layer -> Colors -> Values (Gimp) or Image -> Adjustments -> Levels (Photoshop)
  * At the top of dialog that opens there is a combobox called _Channel_, select _Green_
  * _Output levels_: Draw the right slider completely to the left (the corresponding edit
    box should read '0')
  * Do the same for the Blue channel
  * Go to the red channel. Leave the right output values slider alone =)
  * Slide the left output slider one tic right (the corresponding edit box should read '1')
  * Click _ok_
  * Your selection should now be colored red. (bdw the manipulation of the red channel
    ensures that the darkest part is slightly red).
- Deselect, select the part for the second color choice
- Again adjust the values like above, only switch green and red.
- Repeat for the third choice, you should know what to do by now =)
- The resulting picture sould be saved as truecolor.
- Import and have fun =)
