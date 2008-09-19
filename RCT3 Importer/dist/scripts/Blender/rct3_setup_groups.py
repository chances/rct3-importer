#!BPY
"""
Name: '[RCT3] Setup groups as LODs'
Blender: 242a
Group: 'Object'
Tooltip: 'Setup a lod distance for each group.'
"""

__author__ = "Belgabor"
__version__ = "1.0"

from Blender import Group, Draw
import sys, time
from action_script_tools import Errors
from rct3_bags import RCT3GroupBag

def main():
    draws = {}
    grlist = []
    grdict = {}
    for gr in Group.Get():
        if gr.name[0] != '_':
            bag = RCT3GroupBag(gr)
            draws[gr.name] = Draw.Create(bag.lodDistance)
            grlist.append(gr.name)
            grdict[gr.name] = bag

    grlist = sorted(grlist)

    if not len(grlist):
        Errors.popUpMsg('ERROR', 'No groups', "Your blend file doesn't contain any exported groups")

    pup_block = []
    for gr in grlist:
       pup_block.append((gr, draws[gr], 0.0, 400000.0, "LOD distance for group '%s'. Use 0.0 do deactivate as LOD." % gr))

    if Draw.PupBlock('Groups...', pup_block):
        for gr in grlist:
            group = grdict[gr]
            group.lodDistance = draws[gr].val
            group.store()

#def main():
#    draws = {}
#    grlist = []
#    grdict = {}
#    for gr in Group.Get():
#        if gr.name[0] != '_':
#            dist = 0.0
#            try:
#                dist = gr.properties['modxml']['rct3']['loddistance']
#            except:
#                pass
#            draws[gr.name] = Draw.Create(dist)
#            grlist.append(gr.name)
#            grdict[gr.name] = gr
#
#    grlist = sorted(grlist)
#
#    if not len(grlist):
#        Errors.popUpMsg('ERROR', 'No groups', "Your blend file doesn't contain any exported groups")
#
#    pup_block = []
#    for gr in grlist:
#       pup_block.append((gr, draws[gr], 0.0, 400000.0, "LOD distance for group '%s'. Use 0.0 do deactivate as LOD." % gr))
#
#    if Draw.PupBlock('Groups...', pup_block):
#        for gr in grlist:
#            group = grdict[gr]
#            if draws[gr].val > 0.0:
#                if not ('modxml' in group.properties):
#                    group.properties['modxml'] = {}
#                if not ('rct3' in group.properties['modxml']):
#                    group.properties['modxml']['rct3'] = {}
#                group.properties['modxml']['rct3']['loddistance'] = draws[gr].val
#            else:
#                try:
#                    del group.properties['modxml']['rct3']['loddistance']
#                except:
#                    pass
#

if __name__=="__main__":
    #import rpdb2; rpdb2.start_embedded_debugger("password",True)
    main()
