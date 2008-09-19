#!BPY
"""
Name: 'Simplify Action'
Blender: 242a
Group: 'Animation'
Tooltip: 'Remove "unnecessary" keyframes from action.'
"""

__author__ = "Belgabor"
__version__ = "1.0"

from Blender import Scene, Armature, Window
import sys, time
from action_script_tools import *

settings = ConfigBag("bakeaction")

def main():
    '''
    Main script driver
    '''
    global settings
    # Get current scene
    scene= Scene.GetCurrent()

    bag = Simplifier2.getBag()
    settings.attachBag(bag)

    bag = settings.addBag("Method...")
    bag.addSetting("simpNew", 1, "t", "New method", "Use new simplification method")

    bag = ActionSelector.getBag()
    settings.attachBag(bag)

    settings.addStoreBag(StoreObject=scene)

    if not settings.getFromUser("Simplify action..."):
        return

    action = ActionSelector.selectBagged(settings)
    if not action:
        return

#    if settings["actionselectUseCurrent"]:
#        # Get current object(must be armature)
#        armatureObject= scene.objects.active
#        if armatureObject.getType()!='Armature':
#            Draw.PupMenu("Error!%t|Select armature first.")
#            return
#
#        action=armatureObject.getAction()
#    else:
#        actions = Armature.NLA.GetActions()
#        men = "Select action to simplify%t"
#        aclist = []
#        for an, ac in actions.items():
#            men += "|"+an
#            aclist.append(ac)
#        i = Draw.PupMenu(men)
#        if i <= 0:
#            return
#        action = aclist[i-1]

    Window.WaitCursor(1)
    timing = -time.clock()
    a = Armature.NLA.CopyAction(action)
    a.name = action.name + "-simple"

    for chname, ipo in a.getAllChannelIpos().items():
        print "Simplifying %s..." % chname
        if settings["simpNew"]:
            Simplifier2.simplifyIpoBagged(chname, ipo, settings)
        else:
            Simplifier.simplifyIpoBagged(chname, ipo, settings)
    timing += time.clock()
    Window.WaitCursor(0)
    print "Took %f seconds" % timing


if __name__=="__main__":
    #import rpdb2; rpdb2.start_embedded_debugger("password",True)
    main()
