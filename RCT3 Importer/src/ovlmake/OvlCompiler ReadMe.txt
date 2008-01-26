Ovlcompiler
===========

Introduction
------------

OvlCompiler is a command line tool that can be used to compile simple XML files into .ovl files that contain animations. An XML file specifies the meshes (in .ase format), textures (in .bmp format), details of the bones and the animation steps for the bones. The tool is limited simplistic animations where a mesh is assigned to a single bone.



Command Line Usage
------------------

The tool is invoked using the following format:

    ovlcompiler inputfile outputfile

where "inputfile" specifies a .xml file and "outputfile" specifies a .common.ovl file, eg:

    ovlcompiler c:\test.xml c:\test.common.ovl

the name of the outputfile should correspond to the name given to the bsh in the xml description otherwise "No SVD for SID" error can occur.



Importing .ovl Files Into RCT3
------------------------------

Once the ovlcompiler tool has generated a .common.ovl it can be added to a theme within the importer in the usual manner. A typical .sid file is provided for creating ride events.



XML Format
----------

The following is an example of the XML that ovlcompiler understands

<?xml version="1.0"?>
<!DOCTYPE ovlcompiler SYSTEM "../ovlcompiler.dtd">
<ovl>
  <bsh name="test" model="C:\test.ASE">
    <geomobj name="TEST" bone="1" ftx="texture" txs="SIOpaque" />
    <bone name="testbone">
        <pos1>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos1>
        <pos2>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos2>
    </bone>
  </bsh>
  <ban name="test">
    <bone name="testbone">
      <translate time="0.0">0.0 0.0 0.0</translate>
      <translate time="2.0">0.0 1.0 0.0</translate>
      <translate time="5.0">0.0 1.0 0.0</translate>
      <translate time="7.0">0.0 0.0 0.0</translate>
      <rotate time="0.0">0.0      0.0 0.0 </rotate>
      <rotate time="1.0">1.570796 0.0 0.0 </rotate>
      <rotate time="6.0">1.570796 0.0 0.0 </rotate>
      <rotate time="7.0">0.0      0.0 0.0 </rotate>
    </bone>
  </ban>
  <ftx name="texture" image="C:\test.bmp" alpha="C:\test_alpha.bmp"/>
</ovl>

ovl:        ovl is the root node and must always be present.
fix:        Describes coordinate system fixing for the models, matrices and keyframes.
            handedness:     Whether your source coordinate system is "left"- or "right"handed. In almost all cases, "right" is the correct choice.
            up:             Determines which axis ("x", "y" or "z") points up in your source coordinate system. "z" for most modellers (Blender, SketchUp), "y" for Milkshape.
            Important: The default is to use RCT3's system ("left", "y"), which in most user cases is wrong.
            There currently is no automatic fixing for ms3d files! You need to add this tag.
            The setting is global for the file, the fix tag is a direct child of ovl.
            Quick:
              <fix handedness="right" up="z" /> for Blender & Sketchup,
              <fix handedness="right" up="y" /> for Milkshape
bsh:        bsh describes the bone meshes, a child of ovl
            name:           The name of the object. Should match both the name in the ban node and the name used for the outputfile.
            model:          The path of the .ase or .ms3d file that contains the mesh data.
geomobj:    geomobj describes the mappings between meshes in the model file, bone numbers and textures. Geomobj elements are children of bsh elements, there should be at least one geomobj.
            name:           The name of the mesh as given in the model file. This is case sensitive.
            bone:           The number of the bone to which this mesh is to be assigned. If the attribute is not specified then the root bone 0 is used. The root bone is generated automatically by the compiler.
            ftx:            The name of the texture to use. Textures are specified using "ftx" elements at the "ovl" level.
            txs:            The style with which the texture should be applied to the model.
            placing:        The texture placing setting. Optional. Can be "both" (default), "texture" or "glass" (the latter is called 'Unknown' in the Importer).
                            New in ovlmake 0.3: you can alternatively use "solid", "transparent" or "glassy" respectively. Those reflect better what this setting actually means.
            flags:          The texture flags setting. Optional. Can be "none" (default), "sign", "terrain", "cliff", "water", "watermask", "billboard" or "animatedbillboard"
                            Note: If you set this to "sign", the txs attribute will be ignored (and you can leave it out).
                                  If you set it to something else (not "sign" or "none"), both ftx and txs attributes will be ignored (and you can leave them out).
            doublesided:    Determines wether faces are textured on one side (0, default) or both sides (1). Optional.
bone:       Describes the bones in the object. A child of bsh. There can be many bones in an object.
            name:           The name of the bone. This is matched against bone names in the ban element.
            parent:         The bone that is the parent of this bone.
pos1:       The position and orientation of the bone. The 16 floats represent a 4x4 matrix, expressed row by row. A child of bone.
pos2:       Typically identical to the pos1.
ban:        ban describe the bone animations, a child of ovl.
            name:           The name of the animation. Should match both the name in the bsh node and the name used for the outputfile.
bone:       Describes the animation for the bone. A child of ban.
            name:           The name of the bone. This is matched against bone names in the bsh element.
translate:  A translation keyframe consisting of 3 floats that specify an x,y,z position. A child of bone.
            time:           The time, in seconds, at which this keyframe occurs.
rotate:     A rotation keyframe consisting of 3 floats that specify the rotation, in radians, around the x, y and z axis of the bone. A child of bone.
            time:           The time, in seconds, at which this keyframe occurs.
ftx:        ftx describes a texture, a child of ovl. Optional if you use texture ovls (see 'reference' below).
            name:           The name of the texture, this is matched against names given in ftx attribute of the geomobj node.
            image:          The path of the .bmp file that contains the image map for this texture. Images must be 8bpp, with a height and width that is a power of 2.
            alpha:          The path of the .bmp file that contains the alpha map for this texture. Can be left blank. Images must be 8bpp, with a height and width that is a power of 2.
reference:  References a texture ovl.
            path:           Gives the relative path to the texture ovl as seen from the final place of the ovl file, WITHOUT ".common.ovl".


The above example illustrates an animation which rotates and raises the "TEST" mesh that can be found in c:\test.ase.



XML Examples
------------

Rotation About An Arbitrary Point
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
Rotation occur about the bones frame of reference, the bones frame of reference being specified by the pos1 and pos2 matricies.

  <bsh ... >
    <bone name="testbone">
        <pos1>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 8.0 0.0 1.0</pos1>
        <pos2>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 8.0 0.0 1.0</pos2>
    </bone>
  </bsh>

  <ban ... >
    <bone name="testbone">
      <translate time="0.0">0.0 8.0 0.0</translate>
      <translate time="7.0">0.0 8.0 0.0</translate>
      <rotate time="0.0">0.0      0.0 0.0 </rotate>
      <rotate time="1.0">1.570796 0.0 0.0 </rotate>
      <rotate time="6.0">1.570796 0.0 0.0 </rotate>
      <rotate time="7.0">0.0      0.0 0.0 </rotate>
    </bone>
  </ban>

The above example illustrates a rotation about a point 8 units above the origin. The pos1 and pos2 elements define the bone origin to be at y=8.0. Translation frames need to contain y=8.0 to move the mesh back into position because the inverse of the matrix is applied to the mesh, then the rotation animation steps are applied about the origin, finally the translation steps are applied.

Parent Bones
~~~~~~~~~~~~
Movement of one bone can be specified with respect to another bone, eg. a hand bone depends upon and arm bone. This is done by specifying the parent attribute in a bone element eg:

  <bsh ... >
    <bone name="parentbone">
        <pos1>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos1>
        <pos2>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos2>
    </bone>
    <bone name="childbone" parent="1">
        <pos1>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos1>
        <pos2>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos2>
    </bone>
  </bsh>

A childbone's frame of reference will now depend on the orientation of "parentbone" and will move when "parentbone" moves.

Effects Points
~~~~~~~~~~~~~~
Effects points can be added by creating a bone with the name of the effects point, eg:

    <bone name="simplelight01">
        <pos1>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos1>
        <pos2>1.0 0.0 0.0 0.0   0.0 1.0 0.0 0.0   0.0 0.0 1.0 0.0   0.0 0.0 0.0 1.0</pos2>
    </bone>

Animations can be used to move the effects point around. If a "start" point coincides with it's "end" point then the effect does not run. Effects can be made to "start" and "stop" by moving the start and endpoints apart using an animation and then bringing them back together again.

References
~~~~~~~~~~~~~~
References must have one attribute, 'path', which gives the relative path to the texture ovl as seen from the final place of the ovl file, WITHOUT ".common.ovl". Following the recommendations in the Importer, an example would be:

    <reference path="../shared/thecook-test-texture" />


Known Issues
------------

- Error reporting is very limited, typically the tool just exits without generating a file.



RCT3 Limitations
----------------

- Dynamic lighting isn't supported, ie. animations can't be used to turn lights on and off.
- Speakers can't be made into Ride Events.





The Cook
June '07
