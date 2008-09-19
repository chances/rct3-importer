#!BPY
"""
Name: '[RCT3] Setup RCT3 object options'
Blender: 246
Group: 'Object'
Tooltip: 'Setup general RCT3 object options.'
"""

__author__ = "Belgabor"
__version__ = "1.0"

from Blender import Scene
import sys, time
from rct3_bags import RCT3GeneralBag

def main():
    r = RCT3GeneralBag(Scene.GetCurrent())
    r.getFromUser()

if __name__=="__main__":
    #import rpdb2; rpdb2.start_embedded_debugger("password",True)
    main()
