#!BPY

"""
Name: '[modxml] Set object export name'
Blender: 246
Group: 'Object'
Tooltip: 'Rename an object on export. Usful to circumvent Blender's name length limitation.'
"""
__author__ = "Belgabor"
__version__ = "1.0"

# #not anymore goofos,# belgabor, yesmydear
#
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

import Blender as B # convention to easily identify Blender modules

def main():
    scene = B.Scene.GetCurrent()
    obj = scene.objects.active
    if obj == None:
        B.Draw.PupMenu("Error!%t|Select an object.")
        return

    if not ("properties" in dir(obj)):
        B.Draw.PupMenu("Error!%t|Selected object cannot be renamed.")
        return

    n = obj.name
    try:
        n = obj.properties['modxml']['general']['name']
    except:
        pass

    r = B.Draw.PupStrInput("Name:", n, 100)
    if r != None:
        if not ('modxml' in obj.properties):
            obj.properties['modxml'] = {}
        if not ('general' in obj.properties['modxml']):
            obj.properties['modxml']['general'] = {}
        if (r == "") or (r == obj.name):
            if "name" in obj.properties['modxml']['general']:
                del obj.properties['modxml']['general']['name']
        else:
            obj.properties['modxml']['general']['name'] = r

if __name__ == '__main__':
    main()

