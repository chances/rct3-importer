Blender Scripts
===============
Belgabor
v2.0, Nov 2008

NOTE: If you're looking at the plain text version of this file, you may notice a few strange characters
or punctiation in the text. These allow automatic generation of a formatted HTML version. The engine used
for this (http://www.methods.co.nz/asciidoc/index.html[asciidoc]) allows to do that with the least impact
on readability in plain text. In short, just ignore the strange stuff :)




Overview
--------

This script pack contains the following Blender scripts:

- `bake_active_action.py`, Bake action with constraints
- `bake_active_action_kf.py`, Same as above, but bakes only keyframes. Unfortuantely this almost never works
  correctly, so don't use it (included for completeness' sake).
- `bake_nla_to_action.py`, <<nlabake,Bake NLA strips to actions>>. Includes constraint baking.
- `export_model_xml.py`, <<export,The modxml export script>>.
- `simplify_action.py`, Two algorithms to remove ``unnecessary'' keyframes.
- <<other,Supplementary scripts>>
  * `model_xml_set_export_name.py`, Set the exported name for various Blender objects. Necessary if you need
    to name something with more letters than Blender allows.
  * `model_xml_setup_groups.py`, Assign actions to groups (groups in Blender work as placeholders for models/
    LODs).
  * `rct3_setup_file.py`, Setup various RCT3-relevant options for the modxml file. Basically those are things
    you normally enter in the main Scenery OVL creation window in the importer.
  * `rct3_setup_groups.py`, Setup LOD distances for your groups.

The scripts called `action_script_tools.py` and `rct3_bags.py` are Python modules used by the other scripts.

IMPORTANT: If you update these modules to a new verison, make sure to remove the compiled python files Blender
creates (`action_script_tools.pyc` and `rct3_bags.pyc`). Unfortunately Blender seems to be unable to refresh
them when necessary and will happily use the old compiled version which will almost surely lead to obscure
errors.


[[self]]
Summary of creating a self-sufficient modxml file
-------------------------------------------------

This section will _briefly_ explain what you need to do to create a self-sufficient modxml file, which means
you don't need to change anything in the Importer when you open it as a scenery file (not everything is supported,
so if you need some rarely used options, you'll still need to set them in the Importer). This also means that
you can directly convert it to a scenery ovl with ovlmake.

Please at least also read the <<export,section about the export script>>.

[IMPORTANT]
=================================================
- *Always check at least once in the Importer that everything is as it should be*, especially that no crap crept in!
- To ensure that no crap creeps in, *always immediately prefix things you do not want to export with a ``_''*. This
  applies for example to the default camera and lamp Blender puts in if you don't plan to use them for effect
  points/bones. If you do not need them at all (eg. for preview or icon renders), delete them.
- *Never leave anything unrenamed!* The Importer and ovlmake now reject everything with a default Blender name.
  I've added this because I had several people asking me to help where such things (often Actons) caused trouble.
  Usually those where remenants from testing and did no longer serve a purpose, so in most cases something unrenamed
  is something that wasn't supposed to be put into the ovl files anyways.
- If you need to rename something to a name that's longer than Blender permits, use the ``Set export name'' script.
  Unfortunately this only works with things you can select in the 3D window, for other things (for example Textures)
  you have to go through the ID Property Editor (see <<export_advanced,below>>).
- All of the settings explained here are stored inside the blend file, so if you save it after setting everything up,
  you do not have to do it on the next export.
=================================================


[[self_objects]]
Objects
~~~~~~~

Not much to keep in mind here, basically you need to UV-map your objects and

IMPORTANT: *Properly set double sidedness!* This is determined on the _Mesh_ panel of the _Editing_ panels.
Unfortunately by default it's activated, so *make sure to deactivate it if you don't need it!*

If you want to import objects from SketchUp, there is a couple of things you need to keep in mind additionally.
I recommend to use my ASE export script and use Goofo's ASE import script for Blender, the following hints assume
you did that.

.Importing ASE from SketchUp
- Fix the object names. The names may not contain any ``"''s, unfortunately the import script adds them.
- Go to edit mode, select all vertices and remove doubles (Press ``W'' -> Remove Doubles). Set the faces to smooth
  display (_Editing_ panels, _Link and Materials_ panel). Use the _EdgeSplit_ modifier to choose which parts of
  your object need to be smooth/welded or flat/unwelded (refer to my Blender tips page if you don't know how that
  works. http://belgabor.vodhin.org/blender/).
- Pretty pretty please with sugar on top, fix SketchUp's errors. I've seen only a very few objects where SketchUp
  didn't at least add a couple of unnecessary faces.


[[self_textures]]
Textures
~~~~~~~~

Blender objects do primarily use _Materials_ to gain their texturing. Each _Material_ can have multiple _Texture_-
channels (to make the difference to textures in RCT3-context clear, a Blender _Texture_ will always be written like
_this_). The combination of _Material_ and assigned _Textures_ define two things for RCT3: the texture with its
settings and some of the mesh settings (e.g. texture style).

- Always reuse _Materials_ and/or _Textures_ if possible (the dropdown next to the name). This prevents unnecessary
  or even error-producing duplicates.
- The only thing that matters on the _Material_ is the assigned _Textures_ and their settings, other _Material_-
  settings do not matter (not even the name).
- The actual image texture used is determined by an _Image_-_Texture_. You need to set the proper image file for
  this _Texture_. Note that to properly setup the _Texture_, you need to switch to the _Texture buttons_ of the
  _Material_ panels.
  * The name of the _Texture_ (not the file name!) determines the name of the texture for RCT3.
  * The _UseAlpha_ button determines if an alpha channel of the texture is used. By default it's on, so please
    *make sure to turn it off if you don't need it!*. This make use of an internal alpha channel, external alpha
    channels are currently not supported.
  * The color sliders on the _Colors_ panel determine recolorability. Setting a color slider to a value above 1
    (which is the default), activates the respective color choice (like the RGB model, red is first, green second
    and blue third).
- Further _None_-type _Textures_ determine options (strictly speaking most need not be _None_, but some do, so better
  use it for all). For all of them, the _Texture_'s name determines what they do.
  * If the name starts with ``SI'', it is assumed to be a texture style and exported as written. The validiwy is
    checked in the Importer, not in the export script. If the name is longer than Blender allows, you'll need to use
    the ID Property Browser to set the correct name for export (see <<export_advanced,below>>).
  * If the name starts with ``fudge:'', the mesh's normal fudging will be set. Valid choices are ``fudge:'' followed
    by either a ``+'' or ``-'' followed by ``X'', ``Y'' or ``Z'', or followed by ``rim'' (should be obvious if you
    take a look at the options the Importer provides).
  * If you need certain flags set, add a _None_-_Texture_ called ``terrain'', ``cliff'', ``water'', ``watermask'',
    ``billboard'' or ``animatedbillboard'' (of course they are mutually exclusive).
  * Remember that double-sidedness is defined elsewhere (see <<self_objects,above>>). Transparency is determined
    automatically from the texture style.


[[self_animation]]
Animation
~~~~~~~~~

- Only use armature-based animation with Loc and/or Quat Pose-IPOs, other types of animation will not work.
- Remember to prefix bones that you do not need to export with ``_'' (e.g. bones only used as targets for
  constraints).
- Actions can be renamed easily in an _Action Editor_ window. Make sure to prefix actions that should not be
  exported with a ``_''. This applies for exampre to all _Actions_ that need to be baked (_Actions_ with
  constraints or that are part of an NLA-composed animation).
- To learn more about constraints, refer to the flatride tutorial.

Getting rid of _Actions_
^^^^^^^^^^^^^^^^^^^^^^^^
This is unfortunately a bit complicated in Blender. There is no direct delete command, you need to make use of
the automatic purging process Blender does when saving your file. To make an _Action_ get purged, you need to
make sure it's not selected into anything and remove the fake user Blender automatically adds for _Actions_
(otherwise they would be purged whenever not selected into an object, which is often correct for _Actions_).
To remove the fake user, select the _Action_ in an _Action Editor_ window and deactivate the ``F''-button next
to it's name. Then press the ``X''-button to deselct it. Now save your blend file and reload it. Check that it
really is gone.

Baking and quaternion fixing
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Whenever you need to bake an animation, you'll notice some settings in regard to ``quaternion fixing'' in the
respective script. This section will explain what this is about. The problem is that to bake constraints,
a script needs to look at each individual frame and doing that, looses the animation history (explaining why
would be beyond briefness).

What does that mean??
Let's assume you animate a full 360 degree rotation. Up to 180 degrees, there is no problem. But once you go
beyond 180 degrees (let's say 185 degrees), the loss of animation history takes effect as the script does not
know we came the long way. It always calculates the short way, so it assumes the rotation went 175 degrees
the other way. When you play the animation, this may lead to what ever rotates visibly snapping the wrong way
in one frame before continuing proper rotation.

How is that fixed??
All baking scripts (with the exception of the keyframe only action bake script) contain code that try to detect
such rotational errors and fix them. Unfortunately it's not always successful, generally speaking the smaller
the temporal difference between baked frames is, the higher the success probability (e.g. you should set _frame
tick_ to 1 in the NLA baking script).

How do I see when it fails??
You can have a look at the IPO curves. Normally for a 360 degree rotation, the QuatW-channel starts at 1 and in
a smooth curve goes down to -1 via 0. At the same time the Quat-channel of the rotation axis starts at 0 and
smoothly goes up to 1 (or down to -1, that depends on direction) and back to 0. If you bake that and fixing
fails (e.g. if you deactivate it to test), the QuatW-channel ``bounces'', i.e. it goes to 0, has a visible
kink there and goes back to 1. On the other hand the rotation axis channel has a sigularity, it goes up to 1,
then jumps to -1 within one frame from where it goes back to 0 (if you rotated the other way, it goes to -1 and
jumps to 1). Unfortunately the situation often is not that clean, that's why the script fails.

Is there a way to make this not an issue at all??
Yes. If you export at 30 fps for RCT3 and bake every frame, it's not an issue. As RCT3 works at 30 fps, it will
not show the ``back-snap''. Currently I'm somewhat drawn between suggesting to do that and recommending to use
simplification, so I'd recommend testing both and see what works.

Simplification
^^^^^^^^^^^^^^
Some scripts try to remove frames deemed ``unnecessary'' to remove strain from the RCT3 engine. If possible,
do that but not at the cost of fluent animation. It is not really known (only suspected) that lots of frames
have an adverse effect on RCT3's performance.


[[self_groups]]
Groups
~~~~~~

Blender groups are used to define RCT3/Importer models and LODs.

- The group name will be used as model name.
- Group only the objects you want activated for a model.
- If the group is animated (ie should turn up as an animated model), always include the armature in the group.
- Use the modxml group setup script (Object -> Scripts -> [modxml] Setup actions/animations for groups) to assign
  animations to a group (LOD in this context). Do so in correct order (i.e. idle, start, loop, stop for flatrides).
- Use the rct3 group setup script (Object -> Scripts -> [RCT3] Setup groups as LODs) to assign a LOD distance to
  each group.

NOTE: A group will be used as an animated model if it either contains an armature or has animations assigned. The
armature does not need to be exported (i.e. it's name may start with a ``_'' for that! This allows you to force
animated models if necessary with an (otherwise unused) tag armature.


[[self_end]]
Finishing touches
~~~~~~~~~~~~~~~~~

The RCT3 file setup script (Objects -> Scripts -> [RCT3] Setup RCT3 object options) can be used to add some final
settings. These are on one hand the (scenery) object's name, prefix and output directory and on the other hand some
of the visual options. The absolute minimum for a self-sufficient modxml file for scenery object creation is setting
the name. For flatrides, you do not need to set anything at all here.




[[export]]
Export Script
-------------

The export script can export Meshes, Empties, Cameras, Lamps, Groups, Actions and Bezier Curves/Circles.
Armatures are also exported in a supporting role for Bones.

- Lamps. A lamp lamp will be exported as a light effect bone. There are basically three modes.
  * *Full automatic mode* I will not discuss that here as it doesn't work reliably in connection with armature
    based animation.
  * *Half-automatic mode* The recommended way. You need to name your Lamp ``lightstartXX_'' (note the underscore!).
    XX is the running number of your light effect bones. The exact light effect bone type is determined from
    the Lamp's settings.
    The _Ray Shadow_ button determines whether the light shines (activated) or not (not activated).
    The _OnlyShadow_ button determines whether the bulb is shown (not activated) or not (activated).
    So combinations of these two select ``lightstart'', ``nblightstart'' and ``simplelightstart''. _Ray Shadow_ off
    and _OnlyShadow_ on would mean a non-shining light without a bulb and is not supported. The lamps distance
    value is taken as the radius (0.5 will give 50cm). Normally, the lights colour is taken form the Lamps colour.
    If you want to have a recolourable light, activate the _Sphere_ button. The Energy value will then determine
    which colour option will be taken $$(rounded to full value: <=1: first, 2: second, >=3 third)$$.
    To keep those from interfering with lighting set up for rendering in Blender, put your Lamps for light effect
    bones on a separate layer and activate the _Layer_ button for them.
  * *Manual mode* just name your Lamps as you would in the importer. Refer to the script readme if Blenders name
    lengh limit kicks in, but make sure the Object name doesn't end with an underscore in that case!
- Empties are catch-all effect points/bones. If you set the display size to less 0.6, the export script will
  transform them appropriately for peep related effects and the X-axis points in the direction they are looking.
- Cameras should be also named appropriately. For flat rides, cameraXX define peep cameras and staticXX define
  (doh) static cameras. Setting the lens value of a camera to 15.5 gives you approximately the same rendered
  view as in RCT3.
- Groups will turn up as model list entries if you open the modxml file as a scenery file in the importer.
- The double-sided setting of mesh objects is transferred to the importer if you open the modxml file as a
  scenery file.

All named things whose name starts with a ``_'' (underscore character) are not exported. This is especially useful
for actions and armature bones as they cannot be (de)selected for export (eg. helper bones you don't need in the
final skeleton and actions needed for NLA compositing).

You may only use Actions with Loc and Quat Pose IPOs.

You can compose your animation in Blender's NLA. The <<nlabake,NLA bake script>> allows you to bake your
NLA-composition into action(s). This also bakes bone contraints.


A word about constraints
~~~~~~~~~~~~~~~~~~~~~~~~

Currently Blender does not set keyframes properly on bones influenced by constraints.

If you have such bones, you need to bake the action. Either indirectly via the NLA and my script or via two simpler
Action-to-Action bake scripts enclosed in this pack. One bakes every frame, the other one only keyframes. Both are
slightly modified versions of a script created by vladius. The original is available here:
http://blenderartists.org/forum/showthread.php?t=109244


Settings
~~~~~~~~

.Options
Apply Modifiers:: 
  Same as in Goofos ASE export script. Applies all modifiers befor exporting. Do not deactivate without good reason.
Bone Weights:: 
  Export bone weights. Doesn't work reliably with Apply Modifiers, see <<export_bones,below>>.
Materials:: 
  Same as in Goofos ASE export script. Does currently nothing.
RCT3 Fixes:: 
  Export everything appropriately for RCT3. Deactivation not yet implemented.
Simplify Rotations:: 
  Simplifies rotations for the animation template. *Deprecated*

.Animation
Animation template length:: 
  Length for the animation template. The animation template can be used as a starting point for manual animation. 
  Set to 0 to deactivate. *Deprecated*
Evaluation FPS:: 
  Frames per second for export. You can use this to speed your animations up or down without needing to resize them 
  in Blender. Set to Zero to use Blenders current setting.
Export FPS:: 
  Frames per second for export. Use 30 or something you can divide 30 by without rest. Set to Zero to use Blenders 
  current setting.
  It's a bit hard to explain these two. Basically the export script takes a frame number and calculates its
  time code using Evaluation FPS.
  The Export FPS rating serves both as a maximum frames per second and also to "round" keyframe positions to
  RCT3 compatible values.
Keyframes only:: 
  Export only keyframes (or every frame). Set this for baked animations and if you don't need Blenders IPO 
  interpolation modes. Either way the export script only writes necessary frames, ie when something changed. 
  If set, the next option governs if really only set keyframes are exported.
KO-Mode:: 
  Select mode for keyframe only operation  (Keyframes are only added if they add new information).
  ``0'';; exports only set keyframes.
  ``3'';; can add three new keyframes between two set ones, at 1/6, 1/2 and 5/6 of the frame distance (ie for set 
          keyframes at 1 and 31 at 6, 16 and 26).
  ``5'';; can add up to five new keyframes between two set ones, at 1/15, 1/5, 1/2, 4/5 and 14/15 of the frame 
          distance (ie for set keyframes at 1 and 31 at 3, 11, 16, 21 and 29).
Rotation format:: 
  Format of rotationl keyframes. Either ``E'' for euler angles, ``A'' for axis/angle or ``Q'' for quaternions. 
  I currently recommend using ``Q''.
Write non-anima:: 
  If activated, bones you didn't animate in an Action get added to the exported animation in their rest state.

.Context
Mesh Origins:: 
  Exports mesh origins as bones. Not very useful anymore.
Selection Only:: 
  Export only selected things.

.Export Script
Store Options:: 
  Store these options in your blend file so you don't need to reset them whenever you export. This is a property of 
  the current scene, so if you use more than one, you need to set it for every one.
Show Options:: 
  Show these options on export. Can only be deactivated if you store the settings. If you do so, the option window 
  will be skipped and the stored options used.


[[export_bones]]
Bone Weights
~~~~~~~~~~~~

Unfortunately Blender has a problem with bone weights when applying modifiers the way this script does it. All
bone weights > 0 are set to 1.0. I have implemented a workaround, but that doesn't work if the modifiers change
the number of vertices (as e.g. the EdgeSplit modifier does). Therefore the script warns you if both Apply Modifiers
and Bone Weights are activated.

If Bone Weights are deactivated, vertices assigned to more than one bone will be assigend to all with the same weight.


[[export_advanced]]
Advanced Options
~~~~~~~~~~~~~~~~

There are a few useful things you can do by assinging Blender properties. You can edit them with the ID
Property Browser (Main Help menu).
  
- If you need to get the export options back after deactivating them, select your secene in the browser,
  select modxml -> export and remove the SHOWOPT property (or set it to 1).
- If you need to name something longer than Blender supports, select it in the browser. Then add a new
  subgroup called 'modxml' and select it. Add a new subgroup called 'general' and select it. Finally add
  a string called 'name' and set it to whatever you need it's exported name to be.




[[nlabake]]
NLA Bake Script
---------------

Using this script you can bake your NLA-composed Animation to action(s).

NOTE: _A warning about the NLA to save you some grieve I had:_ Currently resizing Actions to factors < 1 can give you a lot of
trouble, so don't do it. This includes reversing actions by using negative factors. I reported this as a bug on the Blender 
bug tracker and it *should be fixed in recent versions*. You now should not be able to resize actions in problematic ways anymore.

Before running this script, select the armature you want to bake for. All bones will be baked.


Options
~~~~~~~

Auto-Split:: 
  If activated, the script splits the NLA animation on named markers. You need to have one at the first
  frame to be exported if you want to have an action for things before the next named marker. Markers with names
  starting with a ``_'' (underscore) mark sections to be cut out and the section they start is not baked.
Start Frame:: 
  Frame to start baking from
End Frame:: 
  Frame to end baking at
Frame tick:: 
  Bake one keyframe for every X number of frames. The last frame will be baked no matter what.
Include last:: 
  Include the end frame or stop one before. Useful in auto-split mode as it works on written action level. Leave 
  activated for RCT3 animations
Action prefix:: 
  Prefix for generated action(s). The rest of the name is 'NLA' in non-auto-split mode and the respective marker 
  name otherwise.
Location:: 
  If activated, location will be baked
Rotation:: 
  If activated, rotation will be baked
Scale:: 
  If activated, size/scale will be baked. Useless for RCT3

.Simplification:
Apply:: 
  Removes ``unnecessary'' keyframes from the resulting actions
Threshold:: 
  Determines what keyframes are seen as ``unnecessary''. The nearer to 1, the more frames are removed. The default 
  of 0.01 seems to be a reasonable setting, but you can try other values as well (0.1 or 0.001 for example)




[[other]]
Other Scripts
-------------

I've now added a couple of supplementary scripts that allow you to set up a modxml file as a self-sufficient
Scenery file (can be converted to an ovl without any modifications in the importer). All of them can be found 
among the object scripts (3D Window in object made, Object Menu -> Scripts).

Most of their uses have ben explained <<self,above>> and the usage is pretty much self-explanatory. The only one
that may need some explaining is the modxml group setup script for assigning animations. First, click on the group
you want to (re)assign animations to, then click on the animations in order. Once you are ready, click outside the
list to end (if there are still unassigned animations left).

