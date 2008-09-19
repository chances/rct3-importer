Export Script
-------------

The export script can export Meshes, Empties, Cameras, Lamps, Groups, Actions and Bezier Curves/Circles.
Armatures are also exported in a supporting role for Bones.
- Lamps. A lamp lamp will be exported as a light effect bone. There are basically three modes.
  * Full automatic mode. I will not discuss that here as it doesn't work reliably in connection with armature
    based animation.
  * Half-automatic mode. The recommended way. You need to name your Lamp lightstartXX_ (note the underscore!).
    XX is the running number of your light effect bones. The exact light effect bone type is determined from
    the Lamp's settings.
        The _Ray Shadow_ button determines whether the light shines (activated) or not (not activated).
        The _OnlyShadow_ button determines whether the bulb is shown (not activated) or not (activated).
    So combinations of these two select lightstart, nblightstart and simplelightstart. _Ray Shadow_ off and
    _OnlyShadow_ on would mean a non-shining light without a bulb and is not supported. The lamps distance
    value is taken as the radius (0.5 will give 50cm). Normally, the lights colour is taken form the Lamps colour.
    If you want to have a recolourable light, activate the _Sphere_ button. The Energy value will then determine
    which colour option will be taken (rounded to full value: <=1: first, 2: second, >=3 third).
    To keep those from interfering with lighting set up for rendering in Blender, put your Lamps for light effect
    bones on a separate layer and activate the _Layer_ button for them.
  * Manual mode: just name your Lamps as you would in the importer. Refer to the script readme if Blenders name
    lengh limit kicks in, but make sure the Object name doesn't end with an underscore in that case!
- Empties are catch-all effect points/bones. If you set the display size to less 0.6, the export script will
  transform them appropriately for peep related effects and the X-axis points in the direction they are looking.
- Cameras should be also named appropriately. For flat rides, cameraXX define peep cameras and staticXX define
  (doh) static cameras. Setting the lens value of a camera to 15.5 gives you approximately the same rendered
  view as in RCT3.
- Groups will turn up as model list entries if you open the modxml file as a scenery file in the importer.
- The double-sided setting of mesh objects is transferred to the importer if you open the modxml file as a
  scenery file.

All named things whose name starts with a '_' (underscore character) are not exported. This is especially useful
for actions and armature bones as they cannot be (de)selected for export.(eg helper bones you don't need in the
final skeleton and actions needed for NLA compositing)

You may only use Actions with Loc and Quat Pose IPOs.

You can compose your animation in Blender's NLA. The other script (described below) allows you to bake your
NLA-composition into action(s). This also bakes bone contraints.

A word about constraints: Currently Blender does not set keyframes properly on bones influenced by constraints.
If you have such bones, you need to bake the action. Either indirectly via the NLA and my script or via two simpler
Action-to-Action bake scripts enclosed in this pack. One bakes every frame, the other one only keyframes. Both are
slightly modified versions of a script created by vladius. The original is available here:
http://blenderartists.org/forum/showthread.php?t=109244

Options:
Apply Modifiers - Same as in Goofos ASE export script. Applies all modifiers befor exporting. Do not deactivate
  without good reason.
Bone Weights - Export bone weights. Doesn't work reliably with Apply Modifiers, see below.
Materials - Same as in Goofos ASE export script. Does currently nothing.
RCT3 Fixes - Export everything appropriately for RCT3. Dactivation not yet implemented.
Simplify Rotations - Simplifies rotations for the animation template. Not very useful anymore.
Animation template length - Length for the animation template. The animation template can be used as a starting
  point for manual animation. Set to 0 to deactivate.
Evaluation FPS - Frames per second for export. You can use this to speed your animations up or down without
  needing to resize them in Blender. Set to Zero to use Blenders current setting.
Export FPS - Frames per second for export. Use 30 or something you can divide 30 by without rest. Set to Zero
  to use Blenders current setting.
  It's a bit hard to explain these two. Basically the export script takes a frame number and calculates its
  time code using Evaluation FPS.
  The Export FPS rating serves both as a maximum frames per second and also to "round" keyframe positions to
  RCT3 compatible values.
Keyframes only - Export only keyframes (or every frame). Set this for baked animations and if you don't need
  Blenders IPO interpolation modes. Either way the export script only writes necessary frames, ie when something
  changed. If set, the next option governs if really only set keyframes are exported.
KO-Mode - Select mode for keyframe only operation.
          "0" exports only set keyframes.
          "3" can add three new keyframes between two set ones, at 1/6, 1/2 and 5/6 of the frame distance
              (ie for set keyframes at 1 and 31 at 6, 16 and 26).
          "5" can add up to five new keyframes between two set ones, at 1/15, 1/5, 1/2, 4/5 and 14/15 of
              the frame distance (ie for set keyframes at 1 and 31 at 3, 11, 16, 21 and 29).
          Keyframes are only added if they add new information.
Rotation format - Format of rotationl keyframes. Either "E" for euler angles, "A" for axis/angle or "Q" for
  quaternions. I currently recommend using "Q".
Write non-anima - If activated, bones you didn't animate in an Action get added to the exported animation
  in their rest state.
Mesh Origins - Exports mesh origins as bones. Not very useful anymore.
Selection Only - Export only selected things.
Store Options - Store these options in your blend file so you don't need to reset them whenever you export.
  This is a property of the current scene, so if you use more than one, you need to set it for every one.
Show Options - Show these options on export. Can only be deactivated if you store the settings. If you do so,
  the option window will be skipped and the stored options used.

Bone Weights:
Unfortunately Blender has a problem with bone weights when applying modifiers the way this script does it. All
bone weights > 0 are set to 1.0. I have implemented a workaround, but that doesn't work if the modifiers change
the number of vertices (as e.g. the EdgeSplit modifier does). Therefore the script warns you if both Apply Modifiers
and Bone Weights are activated.
If Bone Weights are deactivated, vertices assigned to more than one bone will be assigend to all with the same weight.

Advanced Options:
There are a few useful things you can do by assinging Blender properties. You can edit them with the ID
  Property Browser (Main Help menu).
- If you need to get the export options back after deactivating them, select your secene in the browser,
  select modxml -> export and remove the SHOWOPT property (or set it to 1).
- If you need to name something longer than Blender supports, select it in the browser. Then add a new
  subgroup called 'modxml' and select it. Add a new subgroup called 'general' and select it. Finally add
  a string called 'name' and set it to whatever you need it's exported name to be.


NLA Bake Script
---------------

Using this script you can bake your NLA-composed Animation to action(s).

A warning about the NLA to save you some grieve I had: Currently resizing Actions to factors < 1 can give you a lot of
trouble, so don't do it. This includes reversing actions by using negative factors.
Note: I reported this as a bug on the Blender bug tracker and it should be fixed in recent versions. You now should not
be able to resize actions in problematic ways anymore.

Before running this script, select the armature you want to bake for. All bones will be baked.

Options:
Auto-Split - If activated, the script splits the NLA animation on named markers. You need to have one at the first
  frame to be exported if you want to have an action for things before the next named marker. Markers with names
  starting with a '_' (underscore) mark sections to be cut out and the section they start is not baked.
Start Frame - Frame to start baking from
End Frame - Frame to end baking at
Frame tick - Bake one keyframe for every X number of frames. The last frame will be baked no matter what.
Include last - Include the end frame or stop one before. Useful in auto-split mode as it works on written action
  level. Leave activated for RCT3 animations
Action prefix - Prefix for generated action(s). The rest of the name is 'NLA' in non-auto-split mode and the
  respective marker name otherwise.
Location - If activated, location will be baked
Rotation - If activated, rotation will be baked
Scale - If activated, size/scale will be baked. Useless for RCT3
Simplification:
Apply - Removes "unnecessary" keyframes from the resulting actions
Threshold - Determines what keyframes are seen as "unnecessary". The nearer to 1, the more frames are removed.
  The default of 0.01 seems to be a reasonable setting, but you can try other values as well (0.1 or 0.001 for
  example)

