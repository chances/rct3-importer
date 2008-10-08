#!BPY

__author__ = "belgabor"
__url__ = ["rct3.sourceforge.net"]
__version__ = "1.0"

__bpydoc__ = """\
"Helper classes and functions for Blender Python scripts
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

# The simplification code (Simplifier) is based on a script by aleksey grishchenko (http://eggnot.com/) 2006
# to be found here: http://blenderartists.org/forum/showthread.php?t=84599

from Blender import Types, Draw, Ipo, IpoCurve, Armature, Scene
import sys, time

class ConfigBag(object):
    class Setting(object):
        def __init__(self, Name, Default, Type, ButtonName, ButtonTooltip = "", Min = 0, Max = 0, Temporary = False, AutoDelete = False):
            self._name = Name
            self._default = Default
            self._val = Default
            if not(Type in ("n", "t", "s")):
                raise Exception, "Setting: Unknown setting type '%s'" % Type
            self._type = Type
            self._draw = None
            self._buttonname = ButtonName
            self._tooltip= ButtonTooltip
            self._min = Min
            self._max = Max
            self._temporary = Temporary
            self._autodelete = AutoDelete

        name = property(lambda self:self._name)

        def setVal(self, Val):
            self._val = Val
        val = property(lambda self:self._val, setVal)

        def prepareUI(self, options):
            self._draw = Draw.Create(self._val)
            if self._type == "n":
                options.append((self._buttonname, self._draw, self._min, self._max, self._tooltip))
            elif self._type == "s":
                options.append((self._buttonname, self._draw, 0, self._max, self._tooltip))
            else:
                options.append((self._buttonname, self._draw, self._tooltip))

        def resolveUI(self):
            if not self._draw:
                raise Exception, "Setting: resolveUI called before prepareUI"
            self._val = self._draw.val
            self._draw = None

        def isSet(self):
            if not (self._autodelete is False):
                if self._default == self._val:
                    return False
                else:
                    if not (self._autodelete is True):
                        if self._val == self._autodelete:
                            return False
            return True

        def store(self, target):
            if not self._temporary:
                if self.isSet():
                    target[self._name] = self._val
                else:
                    try:
                        del target[self._name]
                    except:
                        pass

        def retrieve(self, target):
            if (not self._temporary) and (self._name in target):
                self._val = target[self._name]


    class SettingBag(object):
        def __init__(self, displayName="", storeName="", seps = 0, parent = None):
            self._parent = parent
            self._displayname = displayName
            self._storename = storeName
            self._settings = []
            self._settingsdict = {}
            self._separators = seps

        def setDisplayName(self, name):
            self._displayname = name
        def getDisplayName(self):
            return self._displayname
        displayname = property(fget = getDisplayName, fset = setDisplayName)

        def setStoreName(self, name):
            self._storename = name
        def getStoreName(self):
            return self._storename
        storename = property(fget = getStoreName, fset = setStoreName)

        def setSeparators(self, seps):
            self._separators = seps
            print "setSeparators %d" % self._separators
        def getSeparators(self):
            print "getSeparators %d" % self._separators
            return self._separators
        separators = property(getSeparators, setSeparators)

        def __len__(self):
            return len(self._settingsdict)
        def __getitem__(self, key):
            return self._settingsdict[key].val
        def __setitem__(self, key, value):
            self._settingsdict[key].setVal(value)
        def __contains__(self, key):
            if not (key in self._settingsdict):
                return False
            return self._settingsdict[key].isSet()

        def addSetting(self, Name, Default, Type, ButtonName, ButtonTooltip = "", Min = 0, Max = 0, Temporary = False, AutoDelete = False):
            setting = ConfigBag.Setting(Name, Default, Type, ButtonName, ButtonTooltip, Min, Max, Temporary, AutoDelete)
            self._settings.append(setting)
            self._settingsdict[Name] = setting
            if self._parent != None:
                self._parent._settings[Name] = setting
            return setting

        def attach(self, parent):
            self._parent = parent
            for s in self._settings:
                self._parent._settings[s.name] = s

        def prepareUI(self, options):
            if self._displayname != "":
                options.append((self._displayname))
            for s in self._settings:
                s.prepareUI(options)
            for i in xrange(self._separators):
                options.append((''))

        def resolveUI(self):
            for s in self._settings:
                s.resolveUI()

        def isAnySet(self):
            for s in self._settings:
                if s.isSet():
                    return True
            return False

        def store(self, target):
            t = target
            if self._storename != "":
                if not (self._storename in target):
                    target[self._storename] = {}
                t = target[self._storename]
            for s in self._settings:
                s.store(t)

        def retrieve(self, target):
            t = target
            if self._storename != "":
                if not (self._storename in target):
                    return
                t = target[self._storename]
            for s in self._settings:
                s.retrieve(t)

    def __init__(self, Application="", Group="", StoreObject = None):
        self._application = Application
        self._group = Group
        self._bags = []
        self._settings = {}
        self._storeobject = StoreObject
        self._autostore = False
        self._storebag = None

    def setApplication(self, name):
        self._application = name
    def getApplication(self):
        return self._application
    application = property(fget = getApplication, fset = setApplication)

    def setGroup(self, name):
        self._group = name
    def getGroup(self):
        return self._group
    group = property(fget = getGroup, fset = setGroup)

    def addBag(self, displayName="", storeName="", seps = 0):
        bag = ConfigBag.SettingBag(displayName, storeName, seps=seps, parent=self)
        self._bags.append(bag)
        return bag

    def addStoreBag(self, DisplayName="", ButtonName="", Tooltip="", StoreObject=None, AutoStore=True, seps = 0):
        if DisplayName=="":
            DisplayName="Script Options..."
        if ButtonName=="":
            ButtonName="Store"
        if Tooltip=="":
            Tooltip="Store these options in your blend file"
        self._storebag = self.addBag(DisplayName, seps=seps)
        self._storebag.addSetting("store", 0, "t", ButtonName, Tooltip)
        self._autostore = AutoStore
        if StoreObject:
            self._storeobject = StoreObject
        if self._storeobject == None:
            raise Exception, "addAutoStoreBag called, but no store object set"

    def attachBag(self, bag, seps=-1):
        bag.attach(self)
        if seps >= 0:
            bag.setSeparators(seps)
        self._bags.append(bag)

    def join(self, otherconfig, full = False):
        self._application = otherconfig._application
        if full:
            self._group = otherconfig._group

    def __len__(self):
        return len(self._settings)
    def __getitem__(self, key):
        return self._settings[key].val
    def __setitem__(self, key, value):
        self._settings[key].setVal(value)
    def __contains__(self, key):
        if not (key in self._settings):
            return False
        return self._settings[key].isSet()

    def prepareUI(self, options):
        for s in self._bags:
            s.prepareUI(options)

    def resolveUI(self):
        for s in self._bags:
            s.resolveUI()

    def isAnySet(self):
        for s in self._bags:
            if s.isAnySet():
                return True
        return False

    def getFromUser(self, title="", doAutoStore=True, doAutoRetrieve=True):
        if doAutoRetrieve and self._autostore:
            self.retrieve()
        didstore = False
        try:
            didstore = self["store"]
        except:
            pass
        options=[]
        self.prepareUI(options)
        if title == "":
            title = self._application+" settings:"
        if not Draw.PupBlock(title, options):
            return False
        self.resolveUI()
        if doAutoStore and self._autostore:
            if self["store"]:
                self.store()
            else:
                self.store(storeBagOnly = True) # If the user doesn't want to store, remember that
        return True

    def storeInObject(self, obj, storeBagOnly = False):
        if self._application == "":
            raise Exception, "ConfigBag.storeInObject: application not set"
        if not ("properties" in dir(obj)):
            raise Exception, "ConfigBag.storeInObject: object cannot store properties"
        if not (self._application in obj.properties):
            obj.properties[self._application] = {}
        p = obj.properties[self._application]
        if self._group != "":
            if not (self._group in p):
                p[self._group] = {}
            p = p[self._group]
        if storeBagOnly:
            if not self._storebag:
                raise Exception, "ConfigBag.storeInObject: store bag only store requested but store bag not set"
            self._storebag.store(p)
        else:
            for b in self._bags:
                b.store(p)

    def retrieveFromObject(self, obj):
        if self._application == "":
            raise Exception, "ConfigBag.retrieveFromObject: application not set"
        if not ("properties" in dir(obj)):
            raise Exception, "ConfigBag.retrieveFromObject: object cannot store properties"
        if not (self._application in obj.properties):
            return
        p = obj.properties[self._application]
        if self._group != "":
            if not (self._group in p):
                return
            p = p[self._group]
        for b in self._bags:
            b.retrieve(p)

    def store(self, StoreObject=None, storeBagOnly = False):
        if StoreObject:
            self._storeobject = StoreObject
        if self._storeobject == None:
            raise Exception, "store called, but no store object set"
        self.storeInObject(self._storeobject, storeBagOnly)

    def retrieve(self, StoreObject=None):
        if StoreObject:
            self._storeobject = StoreObject
        if self._storeobject == None:
            raise Exception, "retrieve called, but no store object set"
        self.retrieveFromObject(self._storeobject)

class QuaternionFixer(object):

    @staticmethod
    def detect(curve, index, channel, frames, threshold, bounceThreshold, verbose):
        if curve == None:
            return 0
        #w0 = curve.bezierPoints[point-1].vec[1][1]
        #w1 = curve.bezierPoints[point].vec[1][1]
        #w2 = curve.bezierPoints[point+1].vec[1][1]
        frame = frames[index]
        w0 = curve[frames[index-1]]
        w1 = curve[frame]
        w2 = curve[frames[index+1]]
        w3 = curve[frames[index+2]]
        aw1 = abs(w1)
        aw2 = abs(w2)
        if (abs(w1-w0) < threshold) and (abs(w2-w0) < threshold) and (abs(w3-w0) < threshold):
            # flatline
            #print "Flatline on frame %d, channel %s" % (frames[point], channel)
            return 1
        if ((w0 + w3) > (w1+w2)) and (min(w1, w2) < bounceThreshold) and (w1 < w0) and (w2 < w3):
            # prevent misdetection of flips
            s = 1.0
            if w0<0:
                s = -1.0
            if ((w3 * s) > 0) and (((w2 * s) >= 0.0) or (aw2 < bounceThreshold)) and (((w1 * s) >= 0.0) or (aw1 < bounceThreshold)):
                # Bounce
                if verbose:
                    print "  Frame %d: Bounce on channel %s" % (frame, channel)
                return 2
        a = w1 > 0.0
        b = w2 > 0.0
        if (aw1 == 0.0) and (aw2 == 0.0):
            return 0
        #if (abs(w1+w2) > (threshold/max(aw1, aw2))):
        #    return 0
        if  ((a and not b) or (b and not a)) and (((abs(w0)<=aw1) and (abs(w3)<=aw2)) or ((abs(w0-w1)<abs(w1-w2)) and (abs(w3-w2)<abs(w1-w2)))):
            # Flip
            if verbose:
                print "  Frame %d: Flip on channel %s" % (frame, channel)
            return 3
        return 0

    @staticmethod
    def fixIpo(chname, ipo, frames, length, threshold = 0.001, bounceThreshold = 0.01, verbose = False, benchmark = False):
        if (not ipo[Ipo.PO_QUATW]):
            return

        if verbose:
            print "      Fixing IPO "+chname
        flips = []
        t = 0.0
        if benchmark:
            t = - time.clock()
        for index in xrange(1, length-2):
            # These are not actual frames, but zero-based indices into the list.
            #w0 = ipo[Ipo.PO_QUATW].bezierPoints[current_frame-1].vec[1][1]
            #w1 = ipo[Ipo.PO_QUATW].bezierPoints[current_frame].vec[1][1]
            #w2 = ipo[Ipo.PO_QUATW].bezierPoints[current_frame+1].vec[1][1]
            #print "%d: %f %f %f / %f" % (current_frame+1, w0, w1, w2, abs(w0-w2))
            #if (abs(w1) < zerothreshold) and (w1 < abs(w0)) and (abs(w0-w2) < threshold):
            #    print "W-Bounce at %d" % (current_frame+1)
            ch = []
            ch.append(QuaternionFixer.detect(ipo[Ipo.PO_QUATW], index, "w", frames, threshold, bounceThreshold, verbose))
            ch.append(QuaternionFixer.detect(ipo[Ipo.PO_QUATX], index, "x", frames, threshold, bounceThreshold, verbose))
            ch.append(QuaternionFixer.detect(ipo[Ipo.PO_QUATY], index, "y", frames, threshold, bounceThreshold, verbose))
            ch.append(QuaternionFixer.detect(ipo[Ipo.PO_QUATZ], index, "z", frames, threshold, bounceThreshold, verbose))
            if ch[0] and ch[1] and ch[2] and ch[3]:
                # only act if all have something interesting
                #print "Interesting frame %d" % (current_frame + 1)
                # require at least one flip and one bounce
                a = 0;
                b = 0;
                for c in ch:
                    if c==2:
                        b = b + 1
                    if c==3:
                        a = a + 1
                if a and b:
                    print "      -> [%s] Fixing quat at frame %d " % (chname, frames[index])
                    # first frame to be fixed is the one after the current
                    flips.append(index+1)
        if benchmark:
            t += time.clock()
            print "      Detection: %f s" % t
            t = -time.clock()
        if len(flips):
            mult = 1.0
            for index in xrange(0, length):
                if index in flips:
                    mult = -mult
                if mult < 0.0:
                    frame = frames[index]
                    ipo[Ipo.PO_QUATW][frame] = ipo[Ipo.PO_QUATW][frame] * mult
                    ipo[Ipo.PO_QUATX][frame] = ipo[Ipo.PO_QUATX][frame] * mult
                    ipo[Ipo.PO_QUATY][frame] = ipo[Ipo.PO_QUATY][frame] * mult
                    ipo[Ipo.PO_QUATZ][frame] = ipo[Ipo.PO_QUATZ][frame] * mult
            ipo[Ipo.PO_QUATW].recalc()
            ipo[Ipo.PO_QUATX].recalc()
            ipo[Ipo.PO_QUATY].recalc()
            ipo[Ipo.PO_QUATZ].recalc()
        if benchmark:
            t += time.clock()
            print "      Removal: %f s" % t

    @staticmethod
    def fixIpoBagged(chname, ipo, frames, length, settings):
        if settings["doFix"]:
            QuaternionFixer.fixIpo(chname, ipo, frames, length, settings["fixThreshold"], settings["fixBounceThreshold"], settings["fixVerbose"], benchmark=settings["fixBenchmark"])

    @staticmethod
    def fixAction(action, threshold = 0.001, bounceThreshold = 0.01, verbose = False, benchmark=False):
        frames = sorted(action.getFrameNumbers())
        length = len(frames)
        for chname, ipo in action.getAllChannelIpos().items():
            QuaternionFixer.fixIpo(chname, ipo, frames, length, threshold, bounceThreshold, verbose, benchmark)

    @staticmethod
    def fixActionBagged(action, settings):
        if settings["doFix"]:
            QuaternionFixer.fixAction(action, settings["fixThreshold"], settings["fixBounceThreshold"], settings["fixVerbose"], settings["fixBenchmark"])

    @staticmethod
    def getBag():
        bag = ConfigBag.SettingBag("Quaternion fixing...")
        bag.addSetting("doFix", 1, "t", "Apply", "Fix singularities in quaternion IPO curves")
        bag.addSetting("fixThreshold", 0.001, "n", "Threshold", "Threshold for fixing", 0, 1)
        bag.addSetting("fixBounceThreshold", 0.01, "n", "Bounce Threshold", "Threshold for fixing (bounce detection)", 0, 1)
        bag.addSetting("fixVerbose", 0, "t", "Verbose", "Show information about fixing in console")
        bag.addSetting("fixBenchmark", 0, "t", "Benchmark", "Output fix step timings to the console")
        return bag

class Simplifier(object):
    curvemap = {'LocX': Ipo.PO_LOCX, 'LocY': Ipo.PO_LOCY, 'LocZ': Ipo.PO_LOCZ, 'QuatW': Ipo.PO_QUATW,  'QuatX': Ipo.PO_QUATX, 'QuatY': Ipo.PO_QUATY, 'QuatZ': Ipo.PO_QUATZ, 'SizeX': Ipo.PO_SCALEX, 'SizeY': Ipo.PO_SCALEY, 'SizeZ': Ipo.PO_SCALEZ}


    ###############################################################
    # dist_lseg and douglas was taken from http://emergent.unpy.net
    # without any changes
    ###############################################################
    @staticmethod
    def dist_lseg(l1, l2, p):
        x0, y0, z0 = l1
        xa, ya, za = l2
        xi, yi, zi = p

        dx = xa-x0
        dy = ya-y0
        dz = za-z0
        d2 = dx*dx + dy*dy + dz*dz

        if d2 == 0: return 0

        t = (dx * (xi-x0) + dy * (yi-y0) + dz * (zi-z0)) / d2
        if t < 0: t = 0
        if t > 1: t = 1
        dist2 = (xi - x0 - t * dx) ** 2 + (yi - y0 - t * dy) ** 2 + (zi - z0 - t * dz) ** 2

        return dist2 ** .5

    @staticmethod
    def douglas(st, tolerance=.01, first=True):

        if len(st) == 1:
            yield st[0]
            return

        l1 = st[0]
        l2 = st[-1]

        worst_dist = 0
        worst = 0

        for i, p in enumerate(st):
            if p is l1 or p is l2: continue
            dist = Simplifier.dist_lseg(l1, l2, p)
            if dist > worst_dist:
                worst = i
                worst_dist = dist

        if first: yield st[0]
        if worst_dist > tolerance:
            for i in Simplifier.douglas(st[:worst+1], tolerance, False):
                yield i
            yield st[worst]
            for i in Simplifier.douglas(st[worst:], tolerance, False):
                yield i
        if first: yield st[-1]
    ########################################################33

    @staticmethod
    def simplifyCurve(ipo, curve, threshold = 0.01, setLinear = True, newIpo = None, fetchXVals = None, benchmark=False):
        t = 0.0
        if benchmark:
            t = -time.clock()
        icu = None
        if not newIpo:
            icu = curve
        else:
            icu = newIpo.addCurve(curve.name)

        poly = []
        for beZier in curve.bezierPoints:
            polyx = beZier.vec[1][0]
            #print beZier.getPoints() # deprecated???
            poly.append([polyx,curve[polyx],0])
            #print polyx
            #curve.delBezier(polyx) #deprecated???

        if (newIpo == None) and (fetchXVals == None):
            #for i in range(0, len(icu.bezierPoints)):
            #    icu.delBezier(0)
            ipo[Simplifier.curvemap[curve.name]] = None
            curve = ipo.addCurve(curve.name)

        for x,y,z in Simplifier.douglas(poly, threshold):
            if fetchXVals != None:
                fetchXVals.add(round(x))
            else:
                icu.append((x,y))
            #print "new point x,y:", x, y

        if fetchXVals == None:
            if setLinear:
                icu.interpolation = IpoCurve.InterpTypes['LINEAR']
            icu.recalc()
        if benchmark:
            t += time.clock()
            print "      [%s] Simplify: %f" % (curve.name, t)
        return icu

    @staticmethod
    def redoCurve(ipo, curve, fetchXVals, setLinear = True, benchmark=False):
        t = 0.0
        if benchmark:
            t = -time.clock()
        poly = []
        for x in fetchXVals:
            poly.append((x, curve[x]))

        #for i in range(0, len(curve.bezierPoints)):
        #    curve.delBezier(0)

        ipo[Simplifier.curvemap[curve.name]] = None
        curve = ipo.addCurve(curve.name)

        for x, y in poly:
            curve.append((x, y))
        if setLinear:
            curve.interpolation = IpoCurve.InterpTypes['LINEAR']
        curve.recalc()
        if benchmark:
            t += time.clock()
            print "      [%s] Redo: %f" % (curve.name, t)

    @staticmethod
    def simplifyCurveBagged(curve, settings, newIpo = None):
        if settings["doSimplify"]:
            return Simplifier.simplifyCurve(curve, settings["simplifyThreshold"], settings["simplifyLinear"], newIpo, benchmark=settings["simplifyBenchmark"])
        else:
            return None

    @staticmethod
    def simplifyIpo(chname, ipo, threshold = 0.01, setLinear = True, quatSafeMode = True, benchmark=False):
        curves = []
        for c in ipo.curveConsts.values():
            if ipo[c]:
                curves.append(c)
        if quatSafeMode:
            xvals = set()
            for icu in ipo:
                if (icu.name == "QuatW") or (icu.name == "QuatX") or (icu.name == "QuatY") or (icu.name == "QuatZ"):
                    Simplifier.simplifyCurve(ipo, icu, threshold, fetchXVals = xvals, benchmark=benchmark)
            for c in curves:
                icu = ipo[c]
                if (icu.name == "QuatW") or (icu.name == "QuatX") or (icu.name == "QuatY") or (icu.name == "QuatZ"):
                    Simplifier.redoCurve(ipo, icu, xvals, setLinear, benchmark=benchmark)
                else:
                    Simplifier.simplifyCurve(ipo, icu, threshold, setLinear, benchmark=benchmark)
        else:
            for c in curves:
                Simplifier.simplifyCurve(ipo, ipo[c], threshold, setLinear, benchmark=benchmark)

    @staticmethod
    def simplifyIpoBagged(chname, ipo, settings):
        if settings["doSimplify"]:
            return Simplifier.simplifyIpo(chname, ipo, settings["simplifyThreshold"], settings["simplifyLinear"], settings["simplifyQuatMode"], benchmark=settings["simplifyBenchmark"])
        else:
            return None

    @staticmethod
    def simplifyAction(act, threshold = 0.01, setLinear = True, quatSafeMode = True, benchmark = False, inPlace = False):
        a = None
        if inPlace:
            a = act
        else:
            a = Armature.NLA.CopyAction(act)
            a.name = act.name + "-simple"

        for chname, ipo in a.getAllChannelIpos().items():
            Simplifier.simplifyIpo(chname, ipo, threshold, setLinear, quatSafeMode, benchmark)

        return a

    @staticmethod
    def simplifyActionBagged(act, settings, inPlace = False):
        if settings["doSimplify"]:
            return Simplifier.simplifyAction(act, settings["simplifyThreshold"], settings["simplifyLinear"], settings["simplifyQuatMode"], settings["simplifyBenchmark"], inPlace)
        else:
            return None

    @staticmethod
    def getBag():
        bag = ConfigBag.SettingBag("Simplify Action(s)...")
        bag.addSetting("doSimplify", 1, "t", "Apply", "Do simplification (keyframe reduction) on action(s).")
        bag.addSetting("simplifyThreshold", 0.01, "n", "Threshold", "The lower, the less frames are removed", 0, 1)
        bag.addSetting("simplifyLinear", 1, "t", "Set Linear", "Set the interpolation mode to linear")
        bag.addSetting("simplifyQuatMode", 1, "t", "Quaternion Safe", "Prevent small jerks by simplifying quaternion curves as a whole")
        bag.addSetting("simplifyBenchmark", 0, "t", "Benchmark", "Output simplification step timings to the console")
        return bag

class Simplifier2(object):

    @staticmethod
    def simplifyCurve(ipo, curve, threshold = 0.01, setLinear = True, newIpo = None, fetchXVals = None, benchmark=False):
        t = 0.0
        if benchmark:
            t = -time.clock()
        icu = None
        if newIpo == None:
            icu = curve
        else:
            icu = newIpo.addCurve(curve.name)

        minV = None
        maxV = None

        for beZier in curve.bezierPoints:
            if (minV == None) or (beZier.vec[1][0] < minV):
                minV = beZier.vec[1][0]
            if (maxV == None) or (beZier.vec[1][0] > maxV):
                maxV = beZier.vec[1][0]
        minV = int(round(minV))
        maxV = int(round(maxV))


        pts = set()
        pts.add(minV)
        pts.add(maxV)

        # Preevaluate and determine linear regions
        curveeval = {}
        oldval = curve[minV]
        is_lin = False
        maxamp = 0.0
        #print "%d/%d" % (minV, maxV)
        for i in xrange(minV, maxV+1):
            #print "[%d]" % i
            y = curve[i]
            curveeval[i] = y
            if abs(y) > maxamp:
                maxamp = y
            if i > minV:
                #print "%f %f" % (y, oldval)
                if abs(y-oldval) > 0.0001:
                    if is_lin:
                        pts.add(i-1)
                    is_lin = False
                else:
                    is_lin = True
            oldval = y

        oldlen = 0
        while oldlen != len(pts):
            oldlen = len(pts)
            oldpts = sorted(pts)
            fx = oldpts[0]
            fy = curveeval[fx]
            for nx in oldpts[1:]:
                ny = curve[nx]
                minD = None
                maxD = None
                minX = None
                maxX = None
                for evx in xrange(fx+1, nx):
                    evy = curveeval[evx]
                    liny = fy + ((ny - fy) * (evx-fx) / (nx-fx))
                    dist = evy - liny
                    if dist > threshold:
                        if (maxD == None) or (dist>maxD):
                            maxX = evx
                    if dist < -threshold:
                        if (minD == None) or (dist<minD):
                            minX = evx
                if (maxX != None) or (minX != None):
                    pts.add(int(round((fx+nx)/2.0)))
#                if maxX != None:
#                    pts.add(maxX)
#                if minX != None:
#                    pts.add(minX)
                fx = nx
                fy = ny

        if (newIpo == None) and (fetchXVals == None):
            #for i in range(0, len(icu.bezierPoints)):
            #    icu.delBezier(0)
            ipo[Simplifier.curvemap[curve.name]] = None
            curve = ipo.addCurve(curve.name)

        for x in pts:
            if fetchXVals != None:
                fetchXVals.add(x)
            else:
                icu.append((x,curveeval[x]))
            #print "new point x,y:", x, y

        if fetchXVals == None:
            if setLinear:
                icu.interpolation = IpoCurve.InterpTypes['LINEAR']
            icu.recalc()
        if benchmark:
            t += time.clock()
            print "      [%s] Simplify: %f" % (curve.name, t)
        return icu

    @staticmethod
    def redoCurve(ipo, curve, fetchXVals, setLinear = True, benchmark=False):
        t = 0.0
        if benchmark:
            t = -time.clock()
        poly = []
        for x in fetchXVals:
            poly.append((x, curve[x]))

        #for i in range(0, len(curve.bezierPoints)):
        #    curve.delBezier(0)

        ipo[Simplifier.curvemap[curve.name]] = None
        curve = ipo.addCurve(curve.name)

        for x, y in poly:
            curve.append((x, y))
        if setLinear:
            curve.interpolation = IpoCurve.InterpTypes['LINEAR']
        curve.recalc()
        if benchmark:
            t += time.clock()
            print "      [%s] Redo: %f" % (curve.name, t)

    @staticmethod
    def simplifyCurveBagged(curve, settings, newIpo = None):
        if settings["doSimplify"]:
            return Simplifier2.simplifyCurve(curve, settings["simplifyThreshold"], settings["simplifyLinear"], newIpo, benchmark=settings["simplifyBenchmark"])
        else:
            return None

    @staticmethod
    def simplifyIpo(chname, ipo, threshold = 0.01, setLinear = True, quatSafeMode = True, benchmark=False):
        curves = []
        for c in ipo.curveConsts.values():
            if ipo[c]:
                curves.append(c)
        if quatSafeMode:
            xvals = set()
            for icu in ipo:
                if (icu.name == "QuatW") or (icu.name == "QuatX") or (icu.name == "QuatY") or (icu.name == "QuatZ"):
                    Simplifier2.simplifyCurve(ipo, icu, threshold, fetchXVals = xvals, benchmark=benchmark)
            for c in curves:
                icu = ipo[c]
                if (icu.name == "QuatW") or (icu.name == "QuatX") or (icu.name == "QuatY") or (icu.name == "QuatZ"):
                    Simplifier2.redoCurve(ipo, icu, xvals, setLinear, benchmark=benchmark)
                else:
                    Simplifier2.simplifyCurve(ipo, icu, threshold, setLinear, benchmark=benchmark)
        else:
            for c in curves:
                Simplifier2.simplifyCurve(ipo, ipo[c], threshold, setLinear, benchmark=benchmark)

    @staticmethod
    def simplifyIpoBagged(chname, ipo, settings):
        if settings["doSimplify"]:
            return Simplifier2.simplifyIpo(chname, ipo, settings["simplifyThreshold"], settings["simplifyLinear"], settings["simplifyQuatMode"], benchmark=settings["simplifyBenchmark"])
        else:
            return None

    @staticmethod
    def simplifyAction(act, threshold = 0.01, setLinear = True, quatSafeMode = True, benchmark = False, inPlace = False):
        a = None
        if inPlace:
            a = act
        else:
            a = Armature.NLA.CopyAction(act)
            a.name = act.name + "-simple"

        for chname, ipo in a.getAllChannelIpos().items():
            Simplifier2.simplifyIpo(chname, ipo, threshold, setLinear, quatSafeMode, benchmark)

        return a

    @staticmethod
    def simplifyActionBagged(act, settings, inPlace = False):
        if settings["doSimplify"]:
            return Simplifier2.simplifyAction(act, settings["simplifyThreshold"], settings["simplifyLinear"], settings["simplifyQuatMode"], settings["simplifyBenchmark"], inPlace)
        else:
            return None

    @staticmethod
    def getBag():
        bag = ConfigBag.SettingBag("Simplify Action(s)...")
        bag.addSetting("doSimplify", 1, "t", "Apply", "Do simplification (keyframe reduction) on action(s).")
        bag.addSetting("simplifyThreshold", 0.003, "n", "Threshold", "The lower, the less frames are removed", 0, 1)
        bag.addSetting("simplifyLinear", 1, "t", "Set Linear", "Set the interpolation mode to linear")
        bag.addSetting("simplifyQuatMode", 1, "t", "Quaternion Safe", "Prevent small jerks by simplifying quaternion curves as a whole")
        bag.addSetting("simplifyBenchmark", 0, "t", "Benchmark", "Output simplification step timings to the console")
        return bag

class ActionSelector(object):
    @staticmethod
    def selectOne(exportOnly = False, exclusionList = []):
        actions = Armature.NLA.GetActions()
        men = "Select action (click outside to end/cancel)%t"
        aclist = []
        for an in actions.keys():
            if exportOnly and (an[0] == '_'):
                continue
            if an in exclusionList:
                continue
            aclist.append(an)
        if not len(aclist):
            return None
        aclist = sorted(aclist)
        for an in aclist:
            men += "|"+an
        i = Draw.PupMenu(men)
        if i <= 0:
            return None
        return actions[aclist[i-1]]

    # Cannot be static, or the generator will not work corrcetly
    def selectMulti(self, exportOnly = False, exclusionList = []):
        action = ActionSelector.selectOne(exportOnly, exclusionList)
        while not (action is None):
            yield action
            exclusionList.append(action.name)
            action = ActionSelector.selectOne(exportOnly, exclusionList)

    @staticmethod
    def select(useCurrent = False, exportOnly = False, exclusionList = []):
        if useCurrent:
            # Get current object(must be armature)
            armatureObject= Scene.GetCurrent().objects.active
            if armatureObject.getType()!='Armature':
                Draw.PupMenu("Error!%t|Select armature first.")
                return None

            return armatureObject.getAction()
        else:
            return ActionSelector.selectOne(exportOnly, exclusionList)

    @staticmethod
    def selectBagged(settings, exportOnly = False, exclusionList = []):
        return ActionSelector.select(settings["actionselectUseCurrent"])

    @staticmethod
    def getBag(default = 1):
        bag = ConfigBag.SettingBag("Action selection...")
        bag.addSetting("actionselectUseCurrent", default, "t", "Use current", "Use current action of selected armature instead of selecting action from list.")
        return bag

class Errors(object):
	total = { 'errors'   : 0,
			  'warnings' : 0,
			  'info'     : 0 }
	silent = True
	userSilent = 0

    @staticmethod
    def processErr(etype,cat='',msg1='',msg2='', Silent=False):
        ''' Process error according to type [E]rror, [W]arning or [I]nformation '''
        if etype == 'E':
            type_txt = 'ERROR'
            Errors.total['errors'] += 1
        if etype == 'W':
            type_txt = 'WARNING'
            Errors.total['warnings'] += 1
        if etype == 'I':
            type_txt = 'INFORMATION'
            Errors.total['info'] += 1

        Errors.printMsg(type_txt,cat,msg1,msg2)
        if Silent:
            Errors.userSilent += 1

        if (not Errors.silent) and (not Silent):
            Errors.popUpMsg(type_txt,cat,msg1,msg2)

    @staticmethod
    def finalReport():
        if self._userSilent:
            if self._silent:
                Errors.printMsg('CONCLUSION', 'Report', 'There were silent messages.', 'Please see the console for more information.')
            else:
                Errors.popUpMsg('CONCLUSION', 'Report', 'There were silent messages.', 'Please see the console for more information.')

    @staticmethod
    def printMsg(type_txt,cat,msg1="",msg2=""):
        ''' Print message on console '''
        txt = type_txt
        if cat:
            txt += ' ['+cat+']'
        txt += ': '+msg1+' '+msg2
        print txt

    @staticmethod
    def popUpMsg(type_txt,cat="",msg1="",msg2=""):
        ''' Display popup version of message '''
        txt = type_txt
        if cat:
            txt += ' ['+cat+']'
        txt += '%t|'+msg1 # %t = format code for title, | = separator
        if msg2:
            txt += '|'+msg2
        result = Draw.PupMenu(txt)

