#!BPY
"""
Name: '[Bake] Active Action'
Blender: 242a
Group: 'Animation'
Tooltip: 'Bake constraints to action keys.'
"""

__author__ = "vladius"
__url__ = ["vladius.fanatic.ru"]
__version__ = "1.1"

__bpydoc__ = """\
"Action Bake let's you bake the current object action's constraints to normal action keys.

Usage:

Select an object with an active action, and select Bake Active Action from the Object->Scripts
menu of the 3d View.
"""

# Small usability upgrades by Belgabor (v1.1)
# - Moved to Animation menu
# - Changed menu name
# - Error reported via popup instead of console only

from Blender import *
from Blender import Types
import sys
from action_script_tools import *

def invertMatrix(mtx):
    mtxi= Mathutils.Matrix(mtx)
    mtxi.invert()
    return mtxi

POSE_XFORM= [Object.Pose.LOC,Object.Pose.ROT]
settings = ConfigBag("bakeaction")

def bakeAction(action,armatureObject):
    """
        Bakes supplied action for supplied armature.
        Returns baked action.
    """
    global settings
    pose=armatureObject.getPose()
    armature_data=armatureObject.getData();
    pose_bones= pose.bones.values()
    rest_bones= armature_data.bones


    startFrame= min(action.getFrameNumbers());
    endFrame= max(action.getFrameNumbers());

    print "-------------------------------------"
    print "Baking Action "+action.getName()
    baked_name = "baked_"+action.getName()
    if action.getName()[0] == "_":
        baked_name = action.getName()[1:]
    baked_action = Armature.NLA.NewAction(baked_name)


    old_quats={}
    old_locs={}

    baked_locs={}
    baked_quats={}

    action.setActive(armatureObject)
    for current_frame in xrange(startFrame, endFrame+1):
        Redraw()

        Set('curframe', current_frame)

        for i in range(len(pose_bones)):

            bone_name=pose_bones[i].name;



            rest_bone=rest_bones[bone_name]
            matrix=Mathutils.Matrix(pose_bones[i].poseMatrix)
                #print "pose matrix\n", matrix
            rest_matrix=Mathutils.Matrix(rest_bone.matrix['ARMATURESPACE'])
               #print "rest matrix\n", rest_matrix

            parent_bone=rest_bone.parent

            #print "Frame:", current_frame


            if parent_bone:
                parent_pose_bone=pose.bones[parent_bone.name]
                matrix=matrix*invertMatrix(parent_pose_bone.poseMatrix)
                rest_matrix=rest_matrix*invertMatrix(parent_bone.matrix['ARMATURESPACE'])

                #print "before\n", matrix
            #print "before quat\n", pose_bones[i].quat;

                #print "localised pose matrix\n", matrix
                #print "localised rest matrix\n", rest_matrix
            matrix=matrix*invertMatrix(rest_matrix)


            old_quats[bone_name]=Mathutils.Quaternion(pose_bones[i].quat);
            old_locs[bone_name]=Mathutils.Vector(pose_bones[i].loc);



            baked_locs[bone_name]=Mathutils.Vector(matrix.translationPart())
            baked_quats[bone_name]=Mathutils.Quaternion(matrix.toQuat())

        baked_action.setActive(armatureObject)
        Set('curframe', current_frame)
        for i in range(len(pose_bones)):
            pose_bones[i].quat=baked_quats[pose_bones[i].name]
            pose_bones[i].loc=baked_locs[pose_bones[i].name]
            pose_bones[i].insertKey(armatureObject, current_frame, POSE_XFORM)

        action.setActive(armatureObject)
        Set('curframe', current_frame)

        for name, quat in old_quats.iteritems():
            pose.bones[name].quat=quat

        for name, loc in old_locs.iteritems():
            pose.bones[name].loc=loc

    QuaternionFixer.fixActionBagged(baked_action, settings)
    Simplifier2.simplifyActionBagged(baked_action, settings, inPlace=True)
    pose.update()
    return baked_action

def main():
    '''
    Main script driver
    '''
    global settings
    # Get current scene
    scene= Scene.GetCurrent()
    # Get current object(must be armature)
    armatureObject= scene.objects.active
    if armatureObject.getType()!='Armature':
        Draw.PupMenu("Error!%t|Select armature first.")
        return

#    bag = settings.addBag("Quaternion fixing")
#    bag.addSetting("doFix", 1, "t", "Apply", "Fix singularities in quaternion IPO curves")
#    bag.addSetting("fixThreshold", 0.001, "n", "Threshold", "Threshold for fixing", 0, 1)
#    bag.addSetting("fixBounceThreshold", 0.01, "n", "Bounce Threshold", "Threshold for fixing (bounce detection)", 0, 1)
#    bag.addSetting("fixVerbose", 0, "t", "Verbose", "Show information about fixing in console")
    bag = QuaternionFixer.getBag()
    settings.attachBag(bag)
    bag = Simplifier2.getBag()
    settings.attachBag(bag)

#    bag = settings.addBag("Script Options...")
#    bag.addSetting("store", 0, "t", "Store", "Store these options in your blend file")
#
#    settings.retrieveFromObject(scene)

    settings.addStoreBag(StoreObject=scene)

    if not settings.getFromUser("Bake action constraints..."):
        return

#    if settings["store"]:
#        settings.storeInObject(scene)

    # Get current pose



    """
    for i in range(len(pose_bones)):
        print rest_bones.values()[i].name
        print "head(arm) ", rest_bones.values()[i].head['ARMATURESPACE']
        print "tail(arm) ", rest_bones.values()[i].tail['ARMATURESPACE']
        print "roll(arm) ", rest_bones.values()[i].roll['ARMATURESPACE']
        print "head(bone) ", rest_bones.values()[i].head['BONESPACE']
        print "tail(bone) ", rest_bones.values()[i].tail['BONESPACE']
        print "roll(bone) ", rest_bones.values()[i].roll['BONESPACE']
        print "armature-space\n", rest_bones.values()[i].matrix['ARMATURESPACE']
        print "bone-space\n", rest_bones.values()[i].matrix['BONESPACE']
    """

    """
    for i in range(len(pose_bones)):
        print pose_bones[i].name
        print "pose matrix\n", pose_bones[i].poseMatrix
    """
    """
    print "Starting to bake actions"
    actions = Armature.NLA.GetActions()

    for action_name, action in actions.iteritems():
        bakeAction(action,pose_bones,rest_bones)
    """

    action=armatureObject.getAction()

    Window.WaitCursor(1)
    bakeAction(action,armatureObject)
    Window.WaitCursor(0)



if __name__=="__main__":
    #import rpdb2; rpdb2.start_embedded_debugger("password",True)
    main()
