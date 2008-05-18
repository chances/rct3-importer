#!BPY

"""
Name: 'Bake NLA strips to Action'
Blender: 245
Group: 'Animation'
Tooltip: 'Version 0.1'
"""
__author__ = "Belgabor"
__version__ = "0.1"
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

import Blender, math, sys as osSys #os
from Blender import sys, Draw, Scene, Armature, Window, Object, Mathutils

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
    for ipo in act.getAllChannelIpos().values():
        for icu in ipo:
            icu.recalc()

def main():
    global rest_bones

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

    settings = {}
    settings['autoMode'] = 0
    settings['startFrame'] = rcontext.sFrame
    settings['endFrame'] = rcontext.eFrame
    settings['everyFrames'] = 1
    settings['includeLast'] = 1
    settings['actionPrefix'] = "baked_"
    settings['doLoc'] = 1
    settings['doRot'] = 1
    settings['doSca'] = 0

    OPTION_AUTOMODE = Draw.Create(settings['autoMode'])
    OPTION_STARTFRAME = Draw.Create(settings['startFrame'])
    OPTION_ENDFRAME = Draw.Create(settings['endFrame'])
    OPTION_EVERYFRAMES = Draw.Create(settings['everyFrames'])
    OPTION_INCLUDELAST = Draw.Create(settings['includeLast'])
    OPTION_ACTIONPREFIX = Draw.Create(settings['actionPrefix'])
    OPTION_DOLOC = Draw.Create(settings['doLoc'])
    OPTION_DOROT = Draw.Create(settings['doRot'])
    OPTION_DOSCA = Draw.Create(settings['doSca'])

    pupoptions = []
    pupoptions.append(('Options...'))
    pupoptions.append(('Auto-Split', OPTION_AUTOMODE, 'Split action on named keyframes.'))
    pupoptions.append(('Start Frame', OPTION_STARTFRAME, 1, 30000, 'Frame to start baking from'))
    pupoptions.append(('End Frame', OPTION_ENDFRAME, 1, 30000, 'Frame to end baking at'))
    pupoptions.append(('Frame tick', OPTION_EVERYFRAMES, 1, 30000, 'Bake one keyframe for every X number of frames'))
    pupoptions.append(('Include last', OPTION_INCLUDELAST, 'Include the end frame or stop one before.'))
    pupoptions.append(('Action prefix', OPTION_ACTIONPREFIX, 0, 30, 'Prefix for generated action(s)'))
    pupoptions.append(('Generated keyframe ipos...'))
    pupoptions.append(('Location', OPTION_DOLOC, 'Store location.'))
    pupoptions.append(('Rotation', OPTION_DOROT, 'Store rotation.'))
    pupoptions.append(('Scale', OPTION_DOSCA, 'Store scale.'))

    if not Draw.PupBlock('Bake NLA to action...', pupoptions):
        return

    Window.WaitCursor(1)

    settings['autoMode'] = OPTION_AUTOMODE.val
    settings['startFrame'] = OPTION_STARTFRAME.val
    settings['endFrame'] = OPTION_ENDFRAME.val
    settings['everyFrames'] = OPTION_EVERYFRAMES.val
    settings['includeLast'] = OPTION_INCLUDELAST.val
    settings['actionPrefix'] = OPTION_ACTIONPREFIX.val
    settings['doLoc'] = OPTION_DOLOC.val
    settings['doRot'] = OPTION_DOROT.val
    settings['doSca'] = OPTION_DOSCA.val

    # Store current
    storeFrame = rcontext.currentFrame()
    storeNLAOverride = armature_obj.enableNLAOverride
    storeAction = armature_obj.getAction()

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

    # process
    for frame in range(settings['startFrame'], settings['endFrame']):
        if settings['autoMode']:
            frname = None
            try:
                frname = timeline.getName(frame)
            except:
                pass
            if frname:
                print "%d: '%s'" % (frame, frname)
                if curr_action:
                    frameToWrite = frame
                    if not settings['includeLast']:
                        frameToWrite = frame - 1
                    if frameToWrite != lastFrameWritten:
                        rcontext.currentFrame(frameToWrite)
                        bakeFrame(frameToWrite - actionFrameOffset, armature_obj, pose, xforms)
                    calculateIpos(curr_action)
                if frname[0] == '_':
                    curr_action = None
                else:
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
        calculateIpos(curr_action)

    # Clean up
    rcontext.currentFrame(storeFrame)
    armature_obj.enableNLAOverride = storeNLAOverride
    if storeAction:
        storeAction.setActive(armature_obj)

    Window.WaitCursor(0)


if __name__=="__main__":
    main()

