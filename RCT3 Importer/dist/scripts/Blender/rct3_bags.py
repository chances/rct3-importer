#!BPY

__author__ = "belgabor"
__url__ = ["rct3.sourceforge.net"]
__version__ = "1.0"

__bpydoc__ = """\
"Helper classes for RCT3 information
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

from action_script_tools import ConfigBag, Errors
import cStringIO

class ModxmlNameBag(ConfigBag):
    def __init__(self, obj):
        super(ModxmlNameBag, self).__init__(Application="modxml", Group="general", StoreObject=obj)
        bag = self.addBag()
        bag.addSetting("name", obj.name, 's', "Name", "Object export name", AutoDelete="")
        self.retrieve()

    def __call__(self):
        return self.__getitem__("name")

class ModxmlGroupBag(ConfigBag):
    def __init__(self, obj):
        super(ModxmlGroupBag, self).__init__(Application="modxml", Group="rct3", StoreObject=obj)
        bag = self.addBag()
        bag.addSetting("animations", "", 's', "Animations", "Assigend animations", AutoDelete="")
        self.retrieve()

    def setAnimations(self, anim):
        self["animations"] = anim
    animations = property(lambda self:self["animations"], setAnimations)

class RCT3GroupBag(ConfigBag):
    def __init__(self, obj):
        super(RCT3GroupBag, self).__init__(Application="modxml", Group="rct3", StoreObject=obj)
        bag = self.addBag()
        bag.addSetting("loddistance", 0.0, 'n', "LOD distance", Min=0.0, Max=400000.0, AutoDelete=True)
        self.retrieve()

    def setLodDistance(self, dist):
        self["loddistance"] = dist
    lodDistance = property(lambda self:self["loddistance"], setLodDistance)

class RCT3GeneralBag(ConfigBag):
    def __init__(self, obj):
        super(RCT3GeneralBag, self).__init__(Application="modxml", Group="rct3", StoreObject=obj)
        bag = self.addBag()
        bag.addSetting("name", "", 's', "Name  ", "Name for scenery ovl.", Max=100, AutoDelete=True)
        bag.addSetting("prefix", "", 's', "Prefix  ", "Prefix for scenery ovl.", Max=100, AutoDelete=True)
        bag.addSetting("path", "", 's', "Path  ", "Output path for scenery ovl.", Max=100, AutoDelete=True)
        bag.addSetting("version", 0, 'n', "Version", "Structure verison. Vanilla(0), Soaked(1) or Wild(2)", Min=0, Max=2, AutoDelete=True)
        bag.addSetting("noshadow", 0, 't', "No Shadow", "Set the no shadow flag.", AutoDelete=True)
        bag.addSetting("rotation", 0, 't', "Vary Rotation", "Rotational variaton like trees.", AutoDelete=True)
        bag.addSetting("transformed", 0, 't', "Transformed Preview", "Preview transformed/animated state. Only important for coaster cars.", AutoDelete=True)
        bag.addSetting("sway", 0.0, 'n', "Sway", "Sway factor (trees)", Min=0.0, Max=10.0, AutoDelete=True)
        bag.addSetting("brightness", 1.0, 'n', "Brightness", "Brightness variation (trees)", Min=0.0, Max=10.0, AutoDelete=True)
        bag.addSetting("scale", 0.0, 'n', "Scale", "Scale variation factor (trees)", Min=0.0, Max=10.0, AutoDelete=True)
        self.retrieve()

    def getFromUser(self):
        if not super(RCT3GeneralBag, self).getFromUser("General RCT3 scenery settings", doAutoStore=False, doAutoRetrieve=False):
            return False
        if not (self["version"] in [0, 1, 2]):
            Errors.popUpMsg('ERROR', 'Illegal version', "Version needs to be 0, 1 or 2")
            return False
        self.store()
        return True

    def getMetadata(self, indent = 0):
        if not self.isAnySet():
            return None
        outp = cStringIO.StringIO()
        Tab = "\t"
        outp.write("%s<metadata role=\"rct3\">\n" % (Tab*indent))
        if "name" in self:
            outp.write("%s<name>%s</name>\n" % (Tab*(indent+1), self["name"]))
        if "prefix" in self:
            outp.write("%s<prefix>%s</prefix>\n" % (Tab*(indent+1), self["prefix"]))
        if "path" in self:
            outp.write("%s<path>%s</path>\n" % (Tab*(indent+1), self["path"]))
        if "version" in self:
            outp.write("%s<version>%d</version>\n" % (Tab*(indent+1), self["version"]))
        if "noshadow" in self:
            outp.write("%s<flag>noshadow</flag>\n" % (Tab*(indent+1)))
        if "rotation" in self:
            outp.write("%s<flag>rotation</flag>\n" % (Tab*(indent+1)))
        if "transformed" in self:
            outp.write("%s<flag>transformedPreview</flag>\n" % (Tab*(indent+1)))
        if "sway" in self:
            outp.write("%s<sway>%f</sway>\n" % (Tab*(indent+1), self["sway"]))
        if "brightness" in self:
            outp.write("%s<brightness>%f</brightness>\n" % (Tab*(indent+1), self["brightness"]))
        if "scale" in self:
            outp.write("%s<scale>%f</scale>\n" % (Tab*(indent+1), self["scale"]))
        outp.write("%s</metadata>\n" % (Tab*indent))
        r = outp.getvalue()
        outp.close()
        return r

