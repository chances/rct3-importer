#!BPY

"""
Name: '[Bake] NLA strips to Action'
Blender: 245
Group: 'Animation'
Tooltip: 'Version 0.2'
"""
__author__ = "Belgabor"
__version__ = "0.2"
__bpydoc__ = """

"""

# ***** BEGIN GPL LICENSE BLOCK *****
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software Foundation,
# Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# ***** END GPL LICENCE BLOCK *****

# Properly baking bones with constraints was learned from vladius'
# script available here: http://blenderartists.org/forum/showthread.php?t=109244

# The simplification code is based on a script by aleksey grishchenko (http://eggnot.com/) 2006
# to be found here: http://blenderartists.org/forum/showthread.php?t=84599

import Blender, math, time, sys as osSys #os
from Blender import sys, Draw, Scene, Armature, Window, Object, Mathutils, IpoCurve
from action_script_tools import *

timing_fix = 0.0
timing_simple = 0.0

def invertMatrix(mtx):
    mtxi= Mathutils.Matrix(mtx)
    mtxi.invert()
    return mtxi

def bakeFrame(frame, arm_obj, pose, xforms):
    global rest_bones

    for bone in pose.bones.values():
        rest_bone = rest_bones[bone.name]
        rest_matrix = rest_bone.matrix['ARMATURESPACE']
        matrix = bone.poseMatrix

        parent_bone = rest_bone.parent
        if parent_bone:
            parent_pose_bone = pose.bones[parent_bone.name]
            matrix = matrix * invertMatrix(parent_pose_bone.poseMatrix)
            rest_matrix = rest_matrix * invertMatrix(parent_bone.matrix['ARMATURESPACE'])

        matrix = matrix*invertMatrix(rest_matrix)

        bone.loc = Mathutils.Vector(matrix.translationPart())
        bone.quat = Mathutils.Quaternion(matrix.toQuat())
        bone.size = Mathutils.Vector(matrix.scalePart())
        bone.insertKey(arm_obj, frame, xforms, True)

def calculateIpos(act):
    global settings, timing_fix, timing_simple
#    if settings['doFix']:
#        print "  Fixing..."
#        QuaternionFixer.fixActionBagged(act, settings)
#    if settings['doSimplify']:
#        print "  Simplifying..."
#        Simplifier.simplifyActionBagged(act, settings, inPlace=True)
#    else:
    frames = sorted(act.getFrameNumbers())
    length = len(frames)
    minfr = frames[0]
    maxfr = frames[length-1]
    for chname, ipo in act.getAllChannelIpos().items():
        print "  Bone %s" % chname
        if settings['doFix']:
            t = -time.clock()
            print "    Fixing..."
            QuaternionFixer.fixIpoBagged(chname, ipo, frames, length, settings)
            t += time.clock()
            print "    ... took %f s" % t
            timing_fix += t
        if settings['doSimplify']:
            t = -time.clock()
            print "    Simplifying..."
            Simplifier2.simplifyIpoBagged(chname, ipo, settings)
            t += time.clock()
            print "    ... took %f s" % t
            timing_simple += t
        else:
            for icu in ipo:
                icu.recalc()


settings = ConfigBag("bakenla")

def main():
    global rest_bones, settings

    # Get current scene

    scene = Scene.GetCurrent()
    rcontext = scene.getRenderingContext()
    timeline = scene.timeline

    # Get current object(must be armature)

    #armature_obj = scene.getActiveObject()
    armature_obj = scene.objects.active
    if armature_obj.getType()!='Armature':
        Draw.PupMenu("Error!%t|Select armature first.")
        return
    armature_data = armature_obj.getData();
    rest_bones = armature_data.bones

    bag = settings.addBag('Options...')
    bag.addSetting("autoMode", 0, "t", "Auto-Split", "Split action on named keyframes.")
    bag.addSetting("firstOnly", 0, "t", "First only", "Bake only the first action in split mode.")
    bag.addSetting("startFrame", 0, "n", "Start Frame", "Frame to start baking from. 0 uses the current start frame (%d)." % rcontext.sFrame, 0, 30000)
    bag.addSetting("endFrame", 0, "n", "End Frame", "Frame to end baking at. 0 uses the current end frame (%d)." % rcontext.eFrame, 0, 30000)
    bag.addSetting("everyFrames", 1, "n", "Frame tick", "Bake one keyframe for every X number of frames", 1, 30000)
    bag.addSetting("includeLast", 1, "t", "Include last", "Include the end frame or stop one before.")
    bag.addSetting("actionPrefix", "baked_", "s", "Action prefix", "Prefix for generated action(s)", Max=30)

    bag = settings.addBag('Generated keyframe ipos...', seps=1)
    bag.addSetting("doLoc", 1, "t", "Location", "Store location.")
    bag.addSetting("doRot", 1, "t", "Rotation", "Store rotation.")
    bag.addSetting("doSca", 0, "t", "Scale", "Store scale.")

    settings.addStoreBag(StoreObject = scene, seps=1)

    bag = QuaternionFixer.getBag()
    settings.attachBag(bag, seps=2)

    bag = Simplifier2.getBag()
    settings.attachBag(bag)

    if not settings.getFromUser("Bake NLA to action(s)..."):
        return

    if not settings['startFrame']:
        settings['startFrame'] = rcontext.sFrame
    if not settings['endFrame']:
        settings['endFrame'] = rcontext.eFrame

    # Store current
    storeFrame = rcontext.currentFrame()
    storeNLAOverride = armature_obj.enableNLAOverride
    storeAction = armature_obj.getAction()

    start = time.clock()
    print "--------------------------------------------"
    print "Start Baking..."

    # Set up processing
    pose = armature_obj.getPose()
    armature_obj.enableNLAOverride = True
    curr_action = None
    if not settings['autoMode']:
        curr_action = Armature.NLA.NewAction(settings['actionPrefix']+'NLA')
        curr_action.setActive(armature_obj)
    xforms = []
    if settings['doLoc']:
        xforms.append(Object.Pose.LOC)
    if settings['doRot']:
        xforms.append(Object.Pose.ROT)
    if settings['doSca']:
        xforms.append(Object.Pose.SIZE)
    lastFrameWritten = 0
    actionFrameOffset = 0
    frameTick = 1
    autoMode = settings['autoMode']
    Window.DrawProgressBar(0.0, "Starting")
    length = float(settings['endFrame']-settings['startFrame']+1)

    # process
    for frame in range(settings['startFrame'], settings['endFrame']):
        progress = (frame-settings['startFrame']+1)/length
        text = ""
        if curr_action != None:
            text = curr_action.name
        else:
            text = "Skipping..."
        Window.DrawProgressBar(progress, "[%.1f] %s" % (progress*100.0, text))
        if autoMode:
            frname = None
            try:
                frname = timeline.getName(frame)
            except:
                pass
            if frname:
                if curr_action:
                    if settings['firstOnly']:
                        break
                    frameToWrite = frame
                    if not settings['includeLast']:
                        frameToWrite = frame - 1
                    if frameToWrite != lastFrameWritten:
                        rcontext.currentFrame(frameToWrite)
                        bakeFrame(frameToWrite - actionFrameOffset, armature_obj, pose, xforms)
                    Window.DrawProgressBar(progress, "[%.1f] %s" % (progress*100.0, text+" (PP)"))
                    calculateIpos(curr_action)
                if frname[0] == '_':
                    print "Skipping '%s' at %d" % (frname, frame)
                    curr_action = None
                else:
                    print "Action '%s' at %d" % (settings['actionPrefix']+frname, frame)
                    print "  Baking..."
                    curr_action = Armature.NLA.NewAction(settings['actionPrefix']+frname)
                    curr_action.setActive(armature_obj)
                    actionFrameOffset = frame - 1
                    frameTick = 1

        frameTick = frameTick - 1
        if curr_action and (frameTick == 0):
            rcontext.currentFrame(frame)
            bakeFrame(frame - actionFrameOffset, armature_obj, pose, xforms)
            lastFrameWritten = frame
            frameTick = settings['everyFrames']

    if curr_action:
        if settings['includeLast']:
            rcontext.currentFrame(settings['endFrame'])
            bakeFrame(settings['endFrame'] - actionFrameOffset, armature_obj, pose, xforms)
        else:
            if lastFrameWritten != (settings['endFrame'] - 1):
                rcontext.currentFrame(settings['endFrame'] - 1)
                bakeFrame(settings['endFrame'] - actionFrameOffset - 1, armature_obj, pose, xforms)
        Window.DrawProgressBar(progress, "[Nearly Done] %s" % (curr_action.name+" (PP)"))
        calculateIpos(curr_action)

    # Clean up
    rcontext.currentFrame(storeFrame)
    armature_obj.enableNLAOverride = storeNLAOverride
    if storeAction:
        storeAction.setActive(armature_obj)

    end = time.clock()
    print "Baked in %.2f %s" % (end-start, "seconds")
    if timing_fix > 0:
        print "  Fix Benchmark: %.2f %s" % (timing_fix, "seconds")
    if timing_simple > 0:
        print "  Simplification Benchmark: %.2f %s" % (timing_simple, "seconds")

    Window.WaitCursor(0)


if __name__=="__main__":
    main()

