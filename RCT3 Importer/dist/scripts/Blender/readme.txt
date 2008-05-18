Export Script
-------------

The export script can export Meshes, Empties, Cameras, Lamps, Groups, Actions and Bezier Curves/Circles. Armatures are also exported in a supporting role for Bones.
- Lamps are so far buggy. Only use them properly named (ie to the correct RCT3 effect point name)
- Empties are catch-all effect points/bones. If you set the display size to less 0.6, the export script will transform them appropriately for peep related effects and the X-axis points in the direction they are looking.
- Cameras should be also named appropriately. For flat rides, cameraXX define peep cameras and staticXX define (doh) static cameras. Setting the lens value of a camera to 15.5 gives you approximately the same rendered view as in RCT3.
- Groups will turn up as model list entries if you open the modxml file as a scenery file in the importer.
- The double-sided setting of mesh objects is transferred to the importer if you open the modxml file as a scenery file.

All named things whose name starts with a '_' (underscore character) are not exported. This is especially useful for actions and armature bones as they cannot be (de)selected for export.(eg helper bones you don't need in the final skeleton and actions needed for NLA compositing)

You may only use Actions with Loc and Quat Pose IPOs.

You can compose your animation in Blender's NLA. The other script (described below) allows you to bake your NLA-composition into action(s). This also bakes bone contraints.

A word about contraits: Currently Blender does not set keyframes properly on bones influenced by constraints. If you have such bones, you need to bake the action. Either indirectly via the NLA and my script or via a simpler Action-to-Action bake script available here:
http://blenderartists.org/forum/showthread.php?t=109244

Options:
Apply Modifiers - Same as in Goofos ASE export script. Applies all modifiers befor exporting. Do not deactivate without good reason.
Materials - Same as in Goofos ASE export script. Does currently nothing.
RCT3 Fixes - Export everything appropriately for RCT3. Dactivation not yet implemented.
Simplify Rotations - Simplifies rotations for the animation template. Not very useful anymore.
Animation template length - Length for the animation template. The animation template can be used as a starting point for manual animation. Set to 0 to deactivate.
Evaluation FPS - Frames per second for export. You can use this to speed your animations up or down without needing to resize them in Blender. Set to Zero to use Blenders current setting.
Export FPS - Frames per second for export. Use 30 or something you can divide 30 by without rest. Set to Zero to use Blenders current setting.
  It's a bit hard to explain these two. Basically the export script takes a frame number and calculates its time code using Evaluation FPS.
  The Export FPS rating serves both as a maximum frames per second and also to "round" keyframe positions to RCT3 compatible values.
Keyframes only - Export only kepframes (or every frame). Set this for baked animations and if you don't need Blenders IPO interpolation modes. Either way the export script only writes necessary frames, ie when something changed.
Rotation format - Format of rotationl keyframes. Either "E" for euler angles, "A" for axis/angle or "Q" for quaternions. I currently recommend using "Q".
Mesh Origins - Exports mesh origins as bones. Not very useful anymore.
Selection Only - Export only selected things.
Store Options - Store these options in your blend file so you don't need to reset them whenever you export. This is a property of the current scene, so if you use more than one, you need to set it for every one.
Show Options - Show these options on export. Can only be deactivated if you store the settings. If you do so, the option window will be skipped and the stored options used.

Advanced Options:
There are a few useful things you can do by assinging Blender properties. You can edit them with the ID Property Browser (Main Help menu).
- If you need to get the export options back after deactivating them, select your secene in the browser, select modxml -> export and remove the SHOWOPT property (or set it to 1).
- If you need to name something longer than Blender supports, select it in the browser. Then add a new subgroup called 'modxml' and select it. Add a new subgroup called 'general' and select it. Finally add a string called 'name' and set it to whatever you need it's exported name to be.


NLA Bake Script
---------------

Using this script you can bake your NLA-composed Animation to action(s).

A warning about the NLA to save you some grieve I had: Currently resizing Actions to factors < 1 can give you a lot of trouble, so don't do it. This includes reversing actions by using negative factors.

Before running this script, select the armature you want to bake for. All bones will be baked.

Options:
Auto-Split - If activated, the script splits the NLA animation on named markers. You need to have one at the first frame to be exported if you want to have an action for things before the next named marker. Markers with names starting with a '_' (underscore) mark sections to be cut out and the section they start is not baked.
Start Frame - Frame to start baking from
End Frame - Frame to end baking at
Frame tick - Bake one keyframe for every X number of frames. The last frame will be baked no matter what.
Include last - Include the end frame or stop one before. Useful in auto-split mode as it works on written action level. Leave activated for RCT3 animations
Action prefix - Prefix for generated action(s). The rest of the name is 'NLA' in non-auto-split mode and the respective marker name otherwise.
Location - If activated, location will be baked
Rotation - If activated, rotation will be baked
Scale - If activated, size/scale will be baked. Useless for RCT3

