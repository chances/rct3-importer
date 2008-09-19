#!BPY
"""
Name: '[modxml] Setup actions/animations for groups'
Blender: 242a
Group: 'Object'
Tooltip: 'Setup a lod distance for each group.'
"""

__author__ = "Belgabor"
__version__ = "1.0"

from Blender import Group, Draw
import sys, time
from action_script_tools import Errors, ActionSelector
from rct3_bags import ModxmlGroupBag

def selectactions():
    exclusion = []
    action = ActionSelector.selectOne(exportOnly = True, exclusionList = exclusion)
    while not (action is None):
        yield action
        exclusion.append(action.name)
        action = ActionSelector.selectOne(exportOnly = True, exclusionList = exclusion)

def main():
    grlist = []
    grdict = {}

    for gr in Group.Get():
        if gr.name[0] != '_':
            bag = ModxmlGroupBag(gr)
            grlist.append(gr.name)
            grdict[gr.name] = bag

    grlist = sorted(grlist)

    if not len(grlist):
        Errors.popUpMsg('ERROR', 'No groups', "Your blend file doesn't contain any exported groups")

    pupmenu = "Select group to assign actions to%t"
    for gr in grlist:
       pupmenu += "|" + gr
       if grdict[gr].animations != "":
           pupmenu += " (" + grdict[gr].animations + ")"
    pupmenu += "|All"

    i = Draw.PupMenu(pupmenu)
    if i <= 0:
        return

    actions = []
    for a in selectactions():
        actions.append(a.name)

    actionstr = ",".join(actions)
    if i > len(grlist):
        # All
        for gr in grlist:
            grdict[gr].animations = actionstr
            grdict[gr].store()
    else:
        grdict[grlist[i-1]].animations = actionstr
        grdict[grlist[i-1]].store()

if __name__=="__main__":
    #import rpdb2; rpdb2.start_embedded_debugger("password",True)
    main()
