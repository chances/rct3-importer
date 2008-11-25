#!BPY

"""
Name: '[RCT3] Set texture name/reference'
Blender: 246
Group: 'Object'
Tooltip: 'Rename a texture and/or set it to referenced.'
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
from rct3_bags import RCT3TextureBag, ModxmlNameBag

def main():
    texlist = []
    texdict = {}
    namdict = {}

    for tex in B.Texture.Get():
        if tex.name[0] != '_':
            texlist.append(tex.name)
            texdict[tex.name] = RCT3TextureBag(tex)
            namdict[tex.name] = ModxmlNameBag(tex)

    texlist = sorted(texlist)

    if not len(texlist):
        Errors.popUpMsg('ERROR', 'No textures', "Your blend file doesn't contain any exported textures")

    pupmenu = "Select texture%t"
    for tex in texlist:
       pupmenu += "|" + tex

    i = B.Draw.PupMenu(pupmenu)
    if i <= 0:
        return
    
    texname = texlist[i-1]
    if namdict[texname]['name'] != "":
        n = namdict[texname]['name']
    else:
        n = texname

    r = B.Draw.PupStrInput("Exp. Name:", n, 100)
    if r != None:
        if (r == "") or (r == texname):
            namdict[texname]['name'] = ""
        else:
            namdict[texname]['name'] = r
        namdict[texname].store()

    r = B.Draw.PupStrInput("Ref. Path:", texdict[texname].reference, 100)
    if r != None:
        texdict[texname].reference = r
        texdict[texname].store()

if __name__ == '__main__':
    main()

