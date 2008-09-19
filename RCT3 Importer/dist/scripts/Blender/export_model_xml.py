#!BPY

"""
Name: 'Model XML (.modxml) v1.1'
Blender: 244
Group: 'Export'
Tooltip: 'Model XML (.modxml)'
"""
__author__ = "Belgabor"
__version__ = "1.1"
__bpydoc__ = """\
-- Model XML (.xml) v1.1 for Blender 2.44 --<br>

Based on goofos ase exporter v0.6.10

"""
# goofos, belgabor
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

import Blender, time, math, sets, sys as osSys #os
from Blender import sys, Window, Draw, Scene, Mesh, Material, Texture, Image, Mathutils, Lamp, Curve, Group, Armature, Ipo
from rct3_bags import *

# from BezierExport
def GlobaliseVector(vec, mat): return (Mathutils.Vector(vec) * mat)


#============================================
#           Write!
#============================================

gSilentMessages = False
def printmsg(info, msg, silent):
    global gSilentMessages
    if silent:
        gSilentMessages = True
    # Log to console for later reference
    print "%s: %s" % (info, msg)
    # Make known to user
    if not silent:
        Draw.PupMenu("%s:%s|%s" % (info, "%t", msg))

def errormsg(msg, silent = False):
    printmsg("Error", msg, silent)

def warningmsg(msg, silent = False):
    printmsg("Warning", msg, silent)

def infomsg(msg, silent = False):
    printmsg("Info", msg, silent)

def get_Name(ob):
    try:
        return ob.properties['modxml']['general']['name']
    except:
        return ob.name

def getMatrix_Empty(obj):
    m = obj.matrix.copy()
    if obj.drawSize <= 0.6:
        m = Mathutils.RotationMatrix(180, 4, "z") * m
    return m

def getMatrix_Camera(obj):
    m = obj.matrix.copy()
    m = Mathutils.RotationMatrix(-90, 4, "x") * m
    m = Mathutils.RotationMatrix(-90, 4, "z") * m
    return m

def get_rotFrame(bone_mat):
    global guiTable
    rf = None
    if guiTable['ROTFORM'] == 'A':
        q = bone_mat.rotationPart().toQuat()
        if math.fabs(q.w) > 0.99999:
            rf = Mathutils.Vector(0, 1, 0, 0)
        else:
            rf = Mathutils.Vector(2*math.acos(q.w), q.x/math.sqrt(1.0-(q.w*q.w)), q.y/math.sqrt(1.0-(q.w*q.w)), q.z/math.sqrt(1.0-(q.w*q.w)))
    elif guiTable['ROTFORM'] == 'Q':
        q = bone_mat.rotationPart().toQuat()
        rf = Mathutils.Vector(q.w, q.x, q.y, q.z)
    else:
        rf = bone_mat.rotationPart().toEuler()
        rf = Mathutils.Vector(rf[0]*math.pi/180.0, rf[1]*math.pi/180.0, rf[2]*math.pi/180.0)

    return rf

def is_frameSame(fra, frb):
    if (not fra) and (not frb):
        return True
    if (not fra) or (not frb):
        return False

    ret = True
    for i in range(len(fra)):
        if math.fabs(math.fabs(fra[i]) - math.fabs(frb[i])) > 0.000001:
            ret = False
            break
    return ret

def write(filename):
   start = time.clock()
   print_boxed('---------Start of modxml Export------------')
   print 'Export Path: ' + filename

   global exp_list, Tab, idnt, imgTable, worldTable, guiTable
   global restanim, tex_list
   restanim = []
   tex_list = {}

   exp_list =[]
   Tab = "\t"
   idnt = 1
   matTable = {}
   worldTable = {'ambR': 0.0, 'ambG': 0.0, 'ambB': 0.0, 'horR': 0.0, 'horG': 0.0, 'horB': 0.0} #default
   total = {'Verts': 0, 'Tris': 0, 'Faces': 0}

   scn = Blender.Scene.GetCurrent()

   file = open(filename, "w")
   write_header(file, filename, scn, worldTable)

   set_up(scn, exp_list, file, matTable, worldTable)

   #write_materials(file, exp_list, worldTable, matTable)
   write_mesh(file, scn, exp_list, matTable, total)
   write_groups(file)
   if restanim and (guiTable['MAXTIME'] > 0):
      file.write("%s<animation name=\"AnimationTemplate\">\n" % (Tab))
      firstOne = True
      for ra in restanim:
         file.write("%s<bone name=\"%s\">\n" % (2*Tab, ra[0]))
         file.write("%s<translate time=\"0.000000\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, ra[1][0], ra[1][1], ra[1][2]))
         if firstOne:
            file.write("%s<translate time=\"%.6f\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, guiTable['MAXTIME'], ra[1][0], ra[1][1], ra[1][2]))
            firstOne = False
         else:
            file.write("%s<translate time=\"max\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, ra[1][0], ra[1][1], ra[1][2]))
         file.write("%s<rotate time=\"0.000000\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, ra[2][0], ra[2][1], ra[2][2]))
         file.write("%s<rotate time=\"max\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, ra[2][0], ra[2][1], ra[2][2]))
         file.write("%s</bone>\n" % (2*Tab))
      file.write("%s</animation>\n" % (Tab))

   acts = Armature.NLA.GetActions()
   arms = Armature.Get()
   evalfps = guiTable['FPS']
   expfps = guiTable['EXPFPS']
   ex_act = {}
   for aname in acts.keys():
      if aname[0] == '_':
          continue
      print "\nExporting Action %s" % (aname)

      act = acts[aname]
      aipos = act.getAllChannelIpos()
      frames = act.getFrameNumbers()
      print " keyframes %d" % (len(frames))
      maxframe = frames[len(frames)-1]
      # Calculate in 0 to max-1 domain
      maxframe_exp = (((maxframe-1) * expfps) / evalfps) + 1

      frames_exp = []
      for fr in frames:
          frames_exp.append((((fr-1) * expfps) / evalfps) + 1)
      frames_exp = sorted(sets.Set(frames_exp))

      if (len(frames_exp) > 1) and guiTable['KEYONLY'] and (guiTable['KEYONLYMODE'] != '0'):
          newframes = sets.Set();
          for i in range(1, len(frames_exp)):
              frlength = frames_exp[i] - frames_exp[i-1]
              half = round(frlength / 2)
              if guiTable['KEYONLYMODE'] == '3':
                  sixth = round(frlength / 6)
                  if (half != sixth) and (sixth>0):
                      newframes.add(frames_exp[i-1] + sixth)
                      newframes.add(frames_exp[i-1] + half)
                      newframes.add(frames_exp[i] - sixth)
              else:
                  fifteenth = round(frlength / 15)
                  fifth = round(frlength / 5)
                  fr = False;
                  if (fifteenth>0) and (fifteenth != half):
                      fr = True;
                      newframes.add(frames_exp[i-1] + fifteenth)
                      newframes.add(frames_exp[i] - fifteenth)
                  if (fifth>0) and (fifth != fifteenth) and (fifth != half):
                      fr = True;
                      newframes.add(frames_exp[i-1] + fifth)
                      newframes.add(frames_exp[i] - fifth)
                  if fr:
                      newframes.add(frames_exp[i-1] + half)
          temp = sets.Set(frames_exp)
          temp.update(newframes)
          frames_exp = sorted(temp)

      print " max frame %d, exported %d" % (maxframe, maxframe_exp)
      for arm in arms.values():
          print "  Checking armature %s" % (arm.name)
          if arm.name[0] == '_':
             continue
          arm_obj = Blender.Object.Get(arm.name)
          arm_mat = arm_obj.matrixWorld.copy()
          ex_bones = []
          for bname in arm.bones.keys():
              if bname[0] == '_':
                  continue
              bone = arm.bones[bname]
              ex_bone = []
              ex_bone_t = []
              ex_bone_r = []
              do_bone = 0;
              if aipos.has_key(bname):
                  print "  Action %s, bone %s" % (aname, bname)
                  do_bone = 1;

                  ipos = aipos[bname]
                  old_bone_t = None
                  old_bone_tt = 0
                  old_bone_r = None
                  old_bone_rt = 0
                  i_exp_old = 0
                  qw = ipos[Ipo.PO_QUATW]
                  qx = ipos[Ipo.PO_QUATX]
                  qy = ipos[Ipo.PO_QUATY]
                  qz = ipos[Ipo.PO_QUATZ]
                  lx = ipos[Ipo.PO_LOCX]
                  ly = ipos[Ipo.PO_LOCY]
                  lz = ipos[Ipo.PO_LOCZ]

                  frlist = frames_exp
                  if not guiTable['KEYONLY']:
                      frlist = range(1, maxframe_exp+1)

                  for i_exp in frlist:
                      i = ((i_exp - 1) * evalfps / expfps) + 1

                      m = Mathutils.Matrix([1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1])
                      if qw and qx and qy and qz:
                          m = Mathutils.Quaternion(qw[i], qx[i], qy[i], qz[i]).toMatrix()
                          m.resize4x4()

                      #tt = normalizeRotation(m.rotationPart())
                      #print "Frame %d pre:  %f %f %f" % (i, tt[0], tt[1], tt[2])
                      if lx and ly and lz:
                          m = m * Mathutils.TranslationMatrix(Mathutils.Vector(lx[i], ly[i], lz[i]))

                      mdiff = m * bone.matrix['ARMATURESPACE']
                      if bone.parent:
                          # calculate locally
                          mp = bone.parent.matrix['ARMATURESPACE'].copy()
                          mp.invert()
                          mdiff = mdiff * mp

                      tf = mdiff.translationPart()
                      if (not is_frameSame(tf, old_bone_t)) or (i_exp == maxframe_exp):
                          #print "New Frame %d / %d | %f\n" % (i, i_exp, float(i_exp-1)/float(expfps))
                          if (old_bone_tt < i_exp_old) and (i_exp != maxframe_exp) or ((not is_frameSame(tf, old_bone_t)) and (i_exp == maxframe_exp) and (not (maxframe_exp == 1))):
                              # Add last frame of a constant range
                              if i_exp_old != old_bone_tt:
                                fr = [float(i_exp_old-1)/float(expfps), old_bone_t]
                                ex_bone_t.append(fr)
                                #print "  Old Frame %d | %f\n" % (i_exp_old, float(i_exp_old-1)/float(expfps))
                          old_bone_t = tf
                          old_bone_tt = i_exp
                          fr = [float(i_exp-1)/float(expfps), tf]
                          ex_bone_t.append(fr)
                      #rf = normalizeRotation(mdiff.rotationPart())
                      rf = None
                      if guiTable['ROTFORM'] == 'A':
                         q = mdiff.rotationPart().toQuat()
                         if math.fabs(q.w) > 0.99999:
                             rf = Mathutils.Vector(0, 1, 0, 0)
                         else:
                             rf = Mathutils.Vector(2*math.acos(q.w), q.x/math.sqrt(1.0-(q.w*q.w)), q.y/math.sqrt(1.0-(q.w*q.w)), q.z/math.sqrt(1.0-(q.w*q.w)))
                      elif guiTable['ROTFORM'] == 'Q':
                         q = mdiff.rotationPart().toQuat()
                         rf = Mathutils.Vector(q.w, q.x, q.y, q.z)
                      else:
                         rf = mdiff.rotationPart().toEuler()
                         rf = Mathutils.Vector(rf[0]*math.pi/180.0, rf[1]*math.pi/180.0, rf[2]*math.pi/180.0)
                      #print "Frame %d post: %f %f %f\n" % (i, rf[0], rf[1], rf[2])
                      if (not is_frameSame(rf, old_bone_r)) or (i_exp == maxframe_exp):
                          if ((old_bone_rt < (i_exp_old)) and (i_exp != maxframe_exp)) or ((not is_frameSame(rf, old_bone_r)) and (i_exp == maxframe_exp) and (not (maxframe_exp == 1))):
                              # Add last frame of a constant range
                              if i_exp_old != old_bone_rt:
                                fr = [float(i_exp_old-1)/float(expfps), old_bone_r]
                                ex_bone_r.append(fr)
                          old_bone_r = rf
                          old_bone_rt = i_exp
                          fr = [float(i_exp-1)/float(expfps), rf]
                          ex_bone_r.append(fr)
                      i_exp_old = i_exp
                      #print "Frame %d %f  trans %f/%f/%f  rot %f/%f/%f" % (i, t, tf[0], tf[1], tf[2], rf[0], rf[1], rf[2])
              else:
                  if guiTable['WRITENONANIM']:
                      # Write rest animation
                      do_bone = 1;
                      bone_mat = bone.matrix['ARMATURESPACE'].copy()
                      if bone.parent:
                         mp = bone.parent.matrix['ARMATURESPACE'].copy()
                         mp.invert()
                         bone_mat = bone_mat * mp

                      ex_bone_t.append([0.0, bone_mat.translationPart()])
                      if maxframe_exp > 1:
                         ex_bone_t.append([float(maxframe_exp-1)/float(expfps), bone_mat.translationPart()])
                      rf = None
                      if guiTable['ROTFORM'] == 'A':
                         q = bone_mat.rotationPart().toQuat()
                         if math.fabs(q.w) > 0.99999:
                             rf = Mathutils.Vector(0, 1, 0, 0)
                         else:
                             rf = Mathutils.Vector(2*math.acos(q.w), q.x/math.sqrt(1.0-(q.w*q.w)), q.y/math.sqrt(1.0-(q.w*q.w)), q.z/math.sqrt(1.0-(q.w*q.w)))
                      elif guiTable['ROTFORM'] == 'Q':
                         q = bone_mat.rotationPart().toQuat()
                         rf = Mathutils.Vector(q.w, q.x, q.y, q.z)
                      else:
                         rf = bone_mat.rotationPart().toEuler()
                         rf = Mathutils.Vector(rf[0]*math.pi/180.0, rf[1]*math.pi/180.0, rf[2]*math.pi/180.0)
                      ex_bone_r.append([0.0, rf])
                      if maxframe_exp > 1:
                         ex_bone_r.append([float(maxframe_exp-1)/float(expfps), rf])

              if do_bone:
                  ex_bone.append(ex_bone_t)
                  ex_bone.append(ex_bone_r)
                  ex_bones.append([bname, ex_bone])

          if scn.objects.selected and guiTable['SELO'] == 1:
              objects = scn.objects.selected
          elif scn.objects:
              objects = scn.objects
          lights = 0
          for obj in objects:
              if obj.parent != arm_obj:
                  continue

              if not obj.parentbonename:
                  continue

              bone_mat = None
              n = get_Name(obj)
              if obj.getType() == 'Empty':
                  bone_mat = getMatrix_Empty(obj)
              if obj.getType() == 'Camera':
                  bone_mat = getMatrix_Camera(obj)
              if obj.getType() == 'Lamp':
                  bone_mat = obj.matrix.copy()
                  l = obj.getData()
                  if l.type != Lamp.Types['Lamp']:
                     continue
                  n = make_lampName(l, lights, n)
              else:
                  continue

              parent_bone = arm.bones[obj.parentbonename]
              parent_mat = parent_bone.matrix['ARMATURESPACE'].copy() * arm_mat
              parent_mat.invert()
              bone_mat = bone_mat * parent_mat

              ex_bone = []
              ex_bone_t = []
              ex_bone_t.append([0.0, bone_mat.translationPart()])
              if maxframe_exp > 1:
                 ex_bone_t.append([float(maxframe_exp-1)/float(expfps), bone_mat.translationPart()])
              ex_bone_r = []
              rf = get_rotFrame(bone_mat)
              ex_bone_r.append([0.0, rf])
              if maxframe_exp > 1:
                 ex_bone_r.append([float(maxframe_exp-1)/float(expfps), rf])

              ex_bone.append(ex_bone_t)
              ex_bone.append(ex_bone_r)

              ex_bones.append([n, ex_bone])

      ex_act[aname] = ex_bones

   for animname in ex_act.keys():
      file.write("%s<animation name=\"%s\">\n" % (Tab, animname))
      anim = ex_act[animname]
      for bn, barr in anim:
         file.write("%s<bone name=\"%s\"" % (2*Tab, bn))
         if guiTable['ROTFORM'] == 'A':
            file.write(" rotationformat=\"axis\"");
         elif guiTable['ROTFORM'] == 'Q':
            file.write(" rotationformat=\"quaternion\"");
         file.write(">\n")
         trans = barr[0]
         for tim, vec in trans:
            file.write("%s<translate time=\"%.6f\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, tim, vec[0], vec[1], vec[2]))
         rot = barr[1]
         for tim, vec in rot:
            if (guiTable['ROTFORM'] == 'A') or (guiTable['ROTFORM'] == 'Q'):
                file.write("%s<rotate time=\"%.6f\" w=\"%.6f\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, tim, vec[0], vec[1], vec[2], vec[3]))
            else:
                file.write("%s<rotate time=\"%.6f\" x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % (3*Tab, tim, vec[0], vec[1], vec[2]))
         file.write("%s</bone>\n" % (2*Tab))
      file.write("%s</animation>\n" % (Tab))


   for tex in tex_list.values():
         fname = tex['file']
         if fname.startswith('//'):
             fname = fname[2:]
         file.write("%s<texture name=\"%s\" file=\"%s\">\n" % (Tab, tex['name'], fname))
         file.write("%s<metadata role=\"rct3\">\n" % (Tab*2))
         if tex['alpha']:
            file.write("%s<useAlpha/>\n" % (Tab*3))
         file.write("%s<recol>%d</recol>\n" % (Tab*3, tex['recol']))
         file.write("%s</metadata>\n" % (Tab*2))
         file.write("%s</texture>\n" % (Tab))
   write_post(file, filename, scn, worldTable)
   file.close()

   Blender.Window.DrawProgressBar(0, "")    # clear progressbar
   end = time.clock()
   seconds = " in %.2f %s" % (end-start, "seconds")
   totals = "Verts: %i Tris: %i Faces: %i" % (total['Verts'], total['Tris'], total['Faces'])
   print_boxed(totals)
   name = filename.split('/')[-1].split('\\')[-1]
   message = "Successfully exported " + name + seconds
   #meshtools.print_boxed(message)
   print_boxed(message)


def print_boxed(text): #Copy/Paste from meshtools, only to remove the beep :)
   lines = text.splitlines()
   maxlinelen = max(map(len, lines))
   if osSys.platform[:3] == "win":
      print chr(218)+chr(196) + chr(196)*maxlinelen + chr(196)+chr(191)
      for line in lines:
         print chr(179) + ' ' + line.ljust(maxlinelen) + ' ' + chr(179)
      print chr(192)+chr(196) + chr(196)*maxlinelen + chr(196)+chr(217)
   else:
      print '+-' + '-'*maxlinelen + '-+'
      for line in lines: print '| ' + line.ljust(maxlinelen) + ' |'
      print '+-' + '-'*maxlinelen + '-+'
   #print '\a\r', # beep when done

#============================================
#           Setup
#============================================

def set_up(scn, exp_list, file, matTable, worldTable):
   print "Setup"
   #Get selected Objects or if none selected all Objects from the current Scene
   if scn.objects.selected and guiTable['SELO'] == 1:
      objects = scn.objects.selected
   elif scn.objects:
      objects = scn.objects
   else:
      print "No Objects"
      return

   set_lists(file, exp_list, objects, matTable, worldTable)

def find_boneParent(obj):
   if not obj:
      return None

   if obj.parent:
      if obj.parent.getType() == 'Empty':
         return obj.parent
      elif obj.parent.getType() == 'Camera':
         return obj.parent
      elif obj.parent.getType() == 'Armature':
         if obj.parentType == Blender.Object.ParentTypes['BONE']:
            return obj.parentbonename
         else:
            return obj.parent
      elif obj.parent.getType() == 'Bone':
         return obj.parent
      else:
         return find_boneParent(obj.parent)
   else:
      return None


def make_boneName(obj, type = None, name = None):
   n = get_Name(obj)
   if name:
      n = name
   s = "<bone name=\"%s\"" % n
   if type:
      s += " role=\"%s\"" % type
   p = find_boneParent(obj)
   if p:
      if isinstance(p, basestring):
         s += " parent=\"%s\"" % p
      else:
         s += " parent=\"%s\"" % get_Name(p)
   s += ">"
   return s



def make_lampName(lamp, lights, name):
   s = "lightstart"
   pre = False
   if (name.startswith("light") or name.startswith("nblight") or name.startswith("simplelight")):
      if name.endswith("_"):
          s = name[:-1]
          pre = True
      else:
          return name
   if lamp.mode & Lamp.Modes['RayShadow']:
      # light should shine
      if lamp.mode & Lamp.Modes['OnlyShadow']:
         # no bulb
         s = "nb" + s
      else:
         # bulb
         pass
   else:
      # light should not shine
      if lamp.mode & Lamp.Modes['OnlyShadow']:
         # no bulb, nothing to do
         return ""
      else:
         # bulb
         s = "simple" + s

   lights += 1

   if not pre:
      s += "%02d" % lights

   if lamp.mode & Lamp.Modes['NoSpecular']:
      s += "_torch"

   if lamp.mode & Lamp.Modes['Sphere']:
      e = lamp.energy - 1
      if e < 0:
         e = 0
      elif e > 2:
         e = 2
      s += "_flexi%d" % e
   else:
      s += "_r%dg%db%d" % (lamp.col[0] *  255, lamp.col[1] * 255, lamp.col[2] * 255)

   if lamp.dist < 1.0:
        s += "_radius%dcm" % (lamp.dist * 100.0)
   else:
        s += "_radius%d" % lamp.dist
   return s


def normalizeRotation(m):
   global guiTable

   r = []
   # Deconstruct matrix
   t = -math.atan2(-m[1][2], m[2][2])
   r.append(t)
   if m[0][2] > 1:
      # Rounding error?
      t = -math.asin(1.0)
   else:
      t = -math.asin(m[0][2])
   r.append(t)
   t = -math.atan2(-m[0][1], m[0][0])
   r.append(t)

   if guiTable['SIMPROT']:
      r2 = []
      t = -math.atan2(m[1][2], -m[2][2])
      r2.append(t)
      if m[0][2] > 1:
         # Rounding error?
         t = -(math.asin(-1.0)+math.pi)
      else:
         t = -(math.asin(-m[0][2])+math.pi)
      r2.append(t)
      t = -math.atan2(m[0][1], -m[0][0])
      r2.append(t)

      s1 = math.fabs(r[0]) + math.fabs(r[1]) + math.fabs(r[2])
      s2 = math.fabs(r2[0]) + math.fabs(r2[1]) + math.fabs(r2[2])

      if s1 > s2:
         return r2
      else:
         return r
   else:
      return r

def set_lists(file, exp_list, objects, matTable, worldTable):
   global mat_cnt, restanim
   mat_cnt = 0
   mat_index = 0
   lights = 0
   #exp_list = [container1 = [ [mesh], [material_ref] ],...]

   for current_obj in objects:
      if current_obj.name[0] == '_':
         print "Skipped %s" % current_obj.name
         continue
      container = []
      print "Checking %s..." % current_obj.name
      if current_obj.getType() == 'Mesh':
         mesh = current_obj.data
         #if not mesh.faceUV:
         #   print 'Mesh Object '+obj.name+' not exported. Lacks UV data.'
         #   continue

         if len(mesh.verts) == 1:
            v = mesh.verts[0].co * current_obj.matrix
            #file.write("%s<bone name=\"%s\" role=\"vertex\">\n" % (Tab, current_obj.name))
            file.write("%s%s\n" % (Tab, make_boneName(current_obj, "vertex")))
            file.write("%s<matrix>\n" % (Tab*2))
            file.write("%s<row>1 0 0 0</row>\n" % (Tab*3))
            file.write("%s<row>0 1 0 0</row>\n" % (Tab*3))
            file.write("%s<row>0 0 1 0</row>\n" % (Tab*3))
            file.write("%s<row>%.6f %.6f %.6f 1</row>\n" % ((Tab*3), v[0], v[1], v[2]))
            file.write("%s</matrix>\n" % (Tab*2))
            file.write("%s</bone>\n" % (Tab))
            ra = []
            ra.append(get_Name(current_obj))
            ra.append(current_obj.matrixLocal.translationPart())
            ra.append([0,0,0])
            restanim.append(ra)
            continue

         if len(mesh.faces) == 0:
            infomsg('Mesh Object '+current_obj.name+' not exported. No faces.', True)
            continue
         # work around API change -.-
         hasuv = False;
         if 'faceUV' in dir(mesh):
             hasuv = mesh.faceUV
         else:
             hasuv = mesh.hasFaceUV()
         if not hasuv:
            infomsg('Mesh Object '+current_obj.name+' not exported. No UV-mapping.', True)
            continue

         container.append(current_obj)

         mat_type = 0 #1=Material, 2=UV Images
         mat_ref = []
         mats_me = mesh.materials
         mats_ob = current_obj.getMaterials(0)
         #Find used Materials by Meshes or Objects
         if guiTable['MTL'] == 1 and mats_me or mats_ob: #Materials
            if mats_me:
               me_mats = mats_me
            elif mats_ob:
               me_mats = mats_ob
            mat_ref = -1

            for i,m in matTable.iteritems():
               for mat in me_mats:
                  if mat in m:
                     for amat in me_mats:
                        if amat not in m:
                           m.append(amat)
                     mat_ref = i
                     break

            if mat_ref < 0:
               matTable[mat_index] = me_mats
               mat_ref = mat_index
               mat_cnt+=1
               mat_index+=1
         container.append(mat_ref)
         exp_list.append(container)

      elif current_obj.getType() == 'Armature':
         arm_mat= current_obj.matrixWorld
         arm_data= current_obj.getData()

         bones= arm_data.bones.values()
         for bone in bones:
            if bone.name[0] == '_':
                continue
            bone_mat = bone.matrix['ARMATURESPACE'].copy()
            m = bone_mat*arm_mat
            s = ""
            if bone.parent:
                s = " parent=\"%s\"" % get_Name(bone.parent)
            file.write("%s<bone name=\"%s\"%s role=\"bone\">\n" % (Tab, get_Name(bone), s))
            file.write("%s<matrix>\n" % (Tab*2))
            file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[0][0], m[0][1], m[0][2]))
            file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[1][0], m[1][1], m[1][2]))
            file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[2][0], m[2][1], m[2][2]))
            file.write("%s<row>%.6f %.6f %.6f 1.000000</row>\n" % ((Tab*3), m[3][0], m[3][1], m[3][2]))
            file.write("%s</matrix>\n" % (Tab*2))
            file.write("%s</bone>\n" % (Tab))
            # Get relative matrix
            if bone.parent:
                mp = bone.parent.matrix['ARMATURESPACE'].copy()
                mp.invert()
                bone_mat = bone_mat * mp

            ra = []
            ra.append(get_Name(bone))
            ra.append(bone_mat.translationPart())
            bone_mat = bone_mat.rotationPart()
            ra.append(normalizeRotation(bone_mat))
            restanim.append(ra)
      elif current_obj.getType() == 'Empty':
         #print current_obj.name
         v = current_obj.matrix.translationPart()
         m = current_obj.matrix.rotationPart()
         dorot = False
         if current_obj.drawSize <= 0.6:
            m = Mathutils.RotationMatrix(180, 3, "z") * m
            file.write("%s%s\n" % (Tab, make_boneName(current_obj, "effect")))
            dorot = True
         else:
            file.write("%s%s\n" % (Tab, make_boneName(current_obj, "bone")))
         #file.write("%s<bone name=\"%s\" role=\"effect\">\n" % (Tab, current_obj.name))
         file.write("%s<matrix>\n" % (Tab*2))
         file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[0][0], m[0][1], m[0][2]))
         file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[1][0], m[1][1], m[1][2]))
         file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[2][0], m[2][1], m[2][2]))
         file.write("%s<row>%.6f %.6f %.6f 1.000000</row>\n" % ((Tab*3), v[0], v[1], v[2]))
         file.write("%s</matrix>\n" % (Tab*2))
         file.write("%s</bone>\n" % (Tab))
         #print("   %.6f %.6f %.6f 0.000000" % (m[0][0], m[0][1], m[0][2]))
         #print("   %.6f %.6f %.6f 0.000000" % (m[1][0], m[1][1], m[1][2]))
         #print("   %.6f %.6f %.6f 0.000000" % (m[2][0], m[2][1], m[2][2]))
         #print("   %.6f %.6f %.6f 1.000000" % (v[0], v[1], v[2]))
         ra = []
         ra.append(get_Name(current_obj))
         ra.append(current_obj.matrixLocal.translationPart())
         # Deconstruct matrix
         m = current_obj.matrixLocal.rotationPart()
         if dorot:
            m = Mathutils.RotationMatrix(180, 3, "z") * m
         #print '----------'
         #print("   %.6f %.6f %.6f" % (m[0][0], m[0][1], m[0][2]))
         #print("   %.6f %.6f %.6f" % (m[1][0], m[1][1], m[1][2]))
         #print("   %.6f %.6f %.6f" % (m[2][0], m[2][1], m[2][2]))
         #print '----------'
         #print '   %f %f %f' % (r[0], r[1], r[2])
         #print '   %f %f %f' % (-math.atan2(m[1][2], -m[2][2]), -(math.asin(-m[0][2])+math.pi), -math.atan2(m[0][1], -m[0][0]))
         ra.append(normalizeRotation(m))
         restanim.append(ra)

      elif current_obj.getType() == 'Camera':
         #print current_obj.name
         v = current_obj.matrix.translationPart()
         m = current_obj.matrix.rotationPart()
         m = Mathutils.RotationMatrix(-90, 3, "x") * m
         m = Mathutils.RotationMatrix(-90, 3, "z") * m
         #file.write("%s<bone name=\"%s\" role=\"camera\">\n" % (Tab, current_obj.name))
         file.write("%s%s\n" % (Tab, make_boneName(current_obj, "camera")))
         file.write("%s<matrix>\n" % (Tab*2))
         file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[0][0], m[0][1], m[0][2]))
         file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[1][0], m[1][1], m[1][2]))
         file.write("%s<row>%.6f %.6f %.6f 0.000000</row>\n" % ((Tab*3), m[2][0], m[2][1], m[2][2]))
         file.write("%s<row>%.6f %.6f %.6f 1.000000</row>\n" % ((Tab*3), v[0], v[1], v[2]))
         file.write("%s</matrix>\n" % (Tab*2))
         file.write("%s</bone>\n" % (Tab))
         #print("   %.6f %.6f %.6f 0.000000" % (m[0][0], m[0][1], m[0][2]))
         #print("   %.6f %.6f %.6f 0.000000" % (m[1][0], m[1][1], m[1][2]))
         #print("   %.6f %.6f %.6f 0.000000" % (m[2][0], m[2][1], m[2][2]))
         #print("   %.6f %.6f %.6f 1.000000" % (v[0], v[1], v[2]))
         ra = []
         ra.append(get_Name(current_obj))
         ra.append(current_obj.matrixLocal.translationPart())
         # Deconstruct matrix
         m = current_obj.matrixLocal.rotationPart()
         m = Mathutils.RotationMatrix(-90, 3, "x") * m
         m = Mathutils.RotationMatrix(-90, 3, "z") * m
         #print '----------'
         #print("   %.6f %.6f %.6f" % (m[0][0], m[0][1], m[0][2]))
         #print("   %.6f %.6f %.6f" % (m[1][0], m[1][1], m[1][2]))
         #print("   %.6f %.6f %.6f" % (m[2][0], m[2][1], m[2][2]))
         #print '----------'
         #print '   %f %f %f' % (r[0], r[1], r[2])
         #print '   %f %f %f' % (-math.atan2(m[1][2], -m[2][2]), -(math.asin(-m[0][2])+math.pi), -math.atan2(m[0][1], -m[0][0]))
         ra.append(normalizeRotation(m))
         restanim.append(ra)

      elif current_obj.getType() == 'Curve':
         cu = Curve.Get(current_obj.getData(1))
         obMatrix = current_obj.matrixWorld
         for c, bezCurve in enumerate(cu):
            if not bezCurve.isNurb():
               file.write("%s<spline name=\"%s\" cyclic=\"%d\">\n" % (Tab, get_Name(current_obj), bezCurve.isCyclic()))
               for b, bezPoint in enumerate(bezCurve):
                  triple =  bezPoint.vec
                  handle1 = GlobaliseVector(triple[0],obMatrix)
                  node = GlobaliseVector(triple[1],obMatrix)
                  handle2 = GlobaliseVector(triple[2],obMatrix)
                  file.write("%s<node>\n" % (Tab*2))
                  file.write("%s<position x=\"%f\" y=\"%f\" z=\"%f\"/>\n" % ((Tab*3), node[0], node[1], node[2]))
                  file.write("%s<controlPrev x=\"%f\" y=\"%f\" z=\"%f\"/>\n" % ((Tab*3), handle1[0], handle1[1], handle1[2]))
                  file.write("%s<controlNext x=\"%f\" y=\"%f\" z=\"%f\"/>\n" % ((Tab*3), handle2[0], handle2[1], handle2[2]))
                  file.write("%s</node>\n" % (Tab*2))

               file.write("%s</spline>\n" % (Tab))

      elif current_obj.getType() == 'Lamp':
         l = current_obj.getData()
         if l.type != Lamp.Types['Lamp']:
            continue

         s = get_Name(current_obj);
         #if not (s.startswith("light") or s.startswith("nblight") or s.startswith("simplelight")):
         s = make_lampName(l, lights, s)
         if s == "":
            continue

         v = current_obj.matrix.translationPart()

         #file.write("%s<bone name=\"%s\" role=\"light\">\n" % (Tab, s))
         file.write("%s%s\n" % (Tab, make_boneName(current_obj, "light", s)))
         file.write("%s<matrix>\n" % (Tab*2))
         file.write("%s<row>1.000000 0.000000 0.000000 0.000000</row>\n" % ((Tab*3)))
         file.write("%s<row>0.000000 1.000000 0.000000 0.000000</row>\n" % ((Tab*3)))
         file.write("%s<row>0.000000 0.000000 1.000000 0.000000</row>\n" % ((Tab*3)))
         file.write("%s<row>%.6f %.6f %.6f 1.000000</row>\n" % ((Tab*3), v[0], v[1], v[2]))
         file.write("%s</matrix>\n" % (Tab*2))
         file.write("%s</bone>\n" % (Tab))
         ra = []
         ra.append(get_Name(current_obj))
         ra.append(current_obj.matrixLocal.translationPart())
         ra.append([0,0,0])
         restanim.append(ra)


   #If there is a world shader get some values
   world = Blender.World.GetCurrent()
   if world != None:
      worldAmb = world.getAmb()
      worldHor = world.getHor()

      worldTable['ambR'] = worldAmb[0]
      worldTable['ambG'] = worldAmb[1]
      worldTable['ambB'] = worldAmb[2]

      worldTable['horR'] = worldHor[0]
      worldTable['horG'] = worldHor[1]
      worldTable['horB'] = worldHor[2]



#============================================
#           Header/Scene
#============================================

def write_header(file, filename, scn, worldTable):
    #print "Write Header"

    file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    file.write("<model xmlns=\"http://rct3.sourceforge.net/rct3xml/model\">\n");
    glob = RCT3GeneralBag(scn)
    m = glob.getMetadata(1)
    if not (m is None):
        file.write(m)
    file.write("%s<system handedness=\"right\" up=\"z\"/>\n" % (Tab))


def write_post(file, filename, scn, worldTable):
   #print "Write Post"

   file.write("</model>\n");



#============================================
#           Materials
#============================================

def write_materials(file, exp_list, worldTable, matTable):
   print "Write Materials"

   file.write("*MATERIAL_LIST {\n")
   file.write("%s*MATERIAL_COUNT %s\n" % (Tab, mat_cnt))

   for i,m in matTable.iteritems():
      if len(m) == 1: # single mat
         mat_class = 'Standard'

         mats = m
         material = mats[0]
         mat_name = material.name

         file.write("%s*MATERIAL %d {\n" % ((Tab), i))

         idnt = 2
         mat_para(file, idnt, material, mat_name, mat_class, worldTable)
         mat_dummy(file, idnt)
         mat_map(file, idnt, mat_name)

         file.write("%s}\n" % (Tab))

      elif len(m) > 1: # multiple mat
         mat_class = 'Multi/Sub-Object'

         mats = m
         material = mats[0]
         mat_name = 'Multi # ' + material.name
         submat_no = len(mats)

         idnt = 2
         file.write("%s*MATERIAL %d {\n" % ((Tab), i))

         mat_para(file, idnt, material, mat_name, mat_class, worldTable)

         file.write("%s*NUMSUBMTLS %d\n" % ((Tab*idnt), submat_no))

         for submat_cnt,current_mat in enumerate(mats):
            material = current_mat
            mat_class = 'Standard'
            mat_name = material.name

            idnt = 2
            file.write("%s*SUBMATERIAL %d {\n" % ((Tab*idnt), submat_cnt))
            submat_cnt += 1

            idnt = 3
            mat_para(file, idnt, material, mat_name, mat_class, worldTable)
            mat_dummy(file, idnt)
            mat_map(file, idnt, mat_name)

            idnt = 2
            file.write("%s}\n" % (Tab*idnt))

         file.write("%s}\n" % (Tab))


   file.write("}\n")


def mat_para(file, idnt, material, mat_name, mat_class, worldTable):

   mat_amb = material.getAmb()
   mat_dif = material.getRGBCol()
   mat_specCol = material.getSpecCol()
   mat_spec = material.getSpec()
   mat_hard = material.getHardness()
   mat_alpha = 1.0000-material.getAlpha()

   file.write("%s*MATERIAL_NAME \"%s\"\n" % ((Tab*idnt), mat_name))
   file.write("%s*MATERIAL_CLASS \"%s\"\n" % ((Tab*idnt), mat_class))
   file.write("%s*MATERIAL_AMBIENT %.4f   %.4f   %.4f\n" % ((Tab*idnt), (worldTable['ambR']*mat_amb), (worldTable['ambG']*mat_amb), (worldTable['ambB']*mat_amb))) #-Usefull?
   file.write("%s*MATERIAL_DIFFUSE %.4f   %.4f   %.4f\n" % ((Tab*idnt), mat_dif[0], mat_dif[1], mat_dif[2]))
   file.write("%s*MATERIAL_SPECULAR %.4f   %.4f   %.4f\n" % ((Tab*idnt), mat_specCol[0], mat_specCol[1], mat_specCol[2]))
   file.write("%s*MATERIAL_SHINE %.4f\n" % ((Tab*idnt), mat_spec))
   file.write("%s*MATERIAL_SHINESTRENGTH %.4f\n" % ((Tab*idnt), (mat_hard/511.))) #-511 or 512?
   file.write("%s*MATERIAL_TRANSPARENCY %.4f\n" % ((Tab*idnt), mat_alpha))
   file.write("%s*MATERIAL_WIRESIZE 1.0000\n" % (Tab*idnt))


def mat_dummy(file, idnt):

   file.write("%s*MATERIAL_SHADING Blinn\n" % (Tab*idnt))
   file.write("%s*MATERIAL_XP_FALLOFF 0.0000\n" % (Tab*idnt))
   file.write("%s*MATERIAL_SELFILLUM 0.0000\n" % (Tab*idnt))
   file.write("%s*MATERIAL_FALLOFF In\n" % (Tab*idnt))
   file.write("%s*MATERIAL_XP_TYPE Filter\n" % (Tab*idnt))


def mat_map(file, idnt, mat_name):

   mapTable = {0:'*MAP_AMBIENT',1:'*MAP_DIFFUSE',2:'*MAP_SPECULAR',3:'*MAP_SHINE',4:'*MAP_SHINESTRENGTH',5:'*MAP_SELFILLUM',6:'*MAP_OPACITY',7:'*MAP_FILTERCOLOR',8:'*MAP_BUMP',9:'*MAP_REFLECT',10:'*MAP_REFRACT',11:'*MAP_REFRACT'}
   tex_list = [[],[],[],[],[],[],[],[],[],[],[],[]]

   mat = Material.Get(mat_name)
   MTexes = mat.getTextures()

   for current_MTex in MTexes:
      if current_MTex is not None:
         # MAP_SUBNO 0 = *MAP_AMBIENT
         if current_MTex.mapto & Texture.MapTo.AMB:
            map_getTex(current_MTex, 0, (current_MTex.dvar*current_MTex.varfac), tex_list)
         # MAP_SUBNO 1 = *MAP_DIFFUSE = COL = 1
         elif current_MTex.mapto & Texture.MapTo.COL:
            map_getTex(current_MTex, 1, current_MTex.colfac, tex_list)
         # MAP_SUBNO 2 = *MAP_SPECULAR (Color)= CSP or SPEC? = 4
         elif current_MTex.mapto & Texture.MapTo.CSP:
            map_getTex(current_MTex, 2, current_MTex.colfac, tex_list)
         # MAP_SUBNO 3 = *MAP_SHINE (Spec Level) = SPEC or CSP? = 32
         elif current_MTex.mapto & Texture.MapTo.SPEC:
            map_getTex(current_MTex, 3, (current_MTex.dvar*current_MTex.varfac), tex_list)
         # MAP_SUBNO 4 = *MAP_SHINESTRENGTH (Gloss) = HARD = 256
         elif current_MTex.mapto & Texture.MapTo.HARD:
            map_getTex(current_MTex, 4, (current_MTex.dvar*current_MTex.varfac), tex_list)
         # MAP_SUBNO 5 = *MAP_SELFILLUM
         # MAP_SUBNO 6 = *MAP_OPACITY = ALPHA = 128
         elif current_MTex.mapto & Texture.MapTo.ALPHA:
            map_getTex(current_MTex, 6, (current_MTex.dvar*current_MTex.varfac), tex_list)
         # MAP_SUBNO 7 = *MAP_FILTERCOLOR
         # MAP_SUBNO 8 = *MAP_BUMP = NOR = 2
         elif current_MTex.mapto & Texture.MapTo.NOR:
            map_getTex(current_MTex, 8, (current_MTex.norfac/25), tex_list)
         # MAP_SUBNO 9 = *MAP_REFLECT
         elif current_MTex.mapto & Texture.MapTo.REF:
            map_getTex(current_MTex, 9, (current_MTex.norfac/25), tex_list)
         # MAP_SUBNO 10 = *MAP_REFRACT (refraction)
         # MAP_SUBNO 11 = *MAP_REFRACT (displacement)
         elif current_MTex.mapto & Texture.MapTo.DISP:
            map_getTex(current_MTex, 11, (current_MTex.norfac/25), tex_list)

   # Write maps
   for current_LI in tex_list:
      subNo = tex_list.index(current_LI)
      for current_MTex in current_LI:
         tex = current_MTex[0].tex
         if tex.type == Texture.Types.IMAGE:
            map_image(file, idnt, current_MTex, subNo, tex, mapTable[subNo])


def map_getTex(MTex, map_subNo, map_amount, texes):
   # container = [[[MTex], [map_amount]], ...]
   container = []
   container.append(MTex)
   container.append(map_amount)
   texes[map_subNo].append(container)


def map_image(file, idnt, MTexCon, subNo, tex, mapType):

   img = tex.getImage()
   #path = sys.expandpath(img.getFilename()).replace('/', '\\')
   path = img.filename #or img.getFilename()
   tex_class = 'Bitmap'
   tex_mapType = 'Screen'
   tex_filter = 'Pyramidal'

   file.write("%s%s {\n" % ((Tab*idnt), mapType))

   idnt += 1
   file.write("%s*MAP_NAME \"%s\"\n" % ((Tab*idnt), tex.getName()))
   file.write("%s*MAP_CLASS \"%s\"\n" % ((Tab*idnt), tex_class))
   file.write("%s*MAP_SUBNO %s\n" % ((Tab*idnt), subNo))
   file.write("%s*MAP_AMOUNT %.4f\n" % ((Tab*idnt), MTexCon[1]))
   file.write("%s*BITMAP \"%s\"\n" % ((Tab*idnt), path))
   file.write("%s*MAP_TYPE %s\n" % ((Tab*idnt), tex_mapType))

   # hope this part is right!
   u_tiling = tex.repeat[0]*tex.crop[2]
   v_tiling = tex.repeat[1]*tex.crop[3]
   file.write("%s*UVW_U_OFFSET %.4f\n" % ((Tab*idnt), tex.crop[0]))
   file.write("%s*UVW_V_OFFSET %.4f\n" % ((Tab*idnt), tex.crop[1]))
   file.write("%s*UVW_U_TILING %.4f\n" % ((Tab*idnt), u_tiling))
   file.write("%s*UVW_V_TILING %.4f\n" % ((Tab*idnt), v_tiling))

   map_uvw(file, idnt) #hardcoded

   file.write("%s*BITMAP_FILTER %s\n" % ((Tab*idnt), tex_filter))

   idnt -= 1
   file.write("%s}\n" % (Tab*idnt))


def mat_uv(file, idnt, uv_image, uv_name, mat_class, worldTable):
   fake_val0 = '0.0000'
   fake_val1 = '0.1000'
   fake_val2 = '0.5882'
   fake_val3 = '0.9000'
   fake_val4 = '1.0000'

   file.write("%s*MATERIAL_NAME \"%s\"\n" % ((Tab*idnt), uv_name))
   file.write("%s*MATERIAL_CLASS \"%s\"\n" % ((Tab*idnt), mat_class))
   file.write("%s*MATERIAL_AMBIENT %.4f   %.4f   %.4f\n" % ((Tab*idnt), worldTable['ambR'], worldTable['ambG'], worldTable['ambB'])) #------------Usefull?
   file.write("%s*MATERIAL_DIFFUSE %s   %s   %s\n" % ((Tab*idnt), fake_val2, fake_val2, fake_val2))
   file.write("%s*MATERIAL_SPECULAR %s   %s   %s\n" % ((Tab*idnt), fake_val3, fake_val3, fake_val3))
   file.write("%s*MATERIAL_SHINE %s\n" % ((Tab*idnt), fake_val1))
   file.write("%s*MATERIAL_SHINESTRENGTH %s\n" % ((Tab*idnt), fake_val0))
   file.write("%s*MATERIAL_TRANSPARENCY %s\n" % ((Tab*idnt), fake_val0))
   file.write("%s*MATERIAL_WIRESIZE %s\n" % ((Tab*idnt), fake_val4))


def map_uv(file, idnt, uv_image, uv_name):
   map_type = '*MAP_DIFFUSE'
   map_subNo = '1'
   tex_class = 'Bitmap'
   tex_mapType = 'Screen'
   tex_filter = 'Pyramidal'

   fake_val0 = '0.0000'
   fake_val1 = '0.1000'
   fake_val2 = '0.5882'
   fake_val3 = '0.9000'
   fake_val4 = '1.0000'

   #replace "/" with "\" in image path
   uv_filename = uv_image.getFilename().replace('/', '\\')

   file.write("%s%s {\n" % ((Tab*idnt), map_type))

   idnt += 1
   file.write("%s*MAP_NAME \"%s\"\n" % ((Tab*idnt), uv_name))
   file.write("%s*MAP_CLASS \"%s\"\n" % ((Tab*idnt), tex_class))
   file.write("%s*MAP_SUBNO %s\n" % ((Tab*idnt), map_subNo))
   file.write("%s*MAP_AMOUNT %s\n" % ((Tab*idnt), fake_val4))
   file.write("%s*BITMAP \"%s\"\n" % ((Tab*idnt), uv_filename))
   file.write("%s*MAP_TYPE %s\n" % ((Tab*idnt), tex_mapType))
   file.write("%s*UVW_U_OFFSET %s\n" % ((Tab*idnt), fake_val0))
   file.write("%s*UVW_V_OFFSET %s\n" % ((Tab*idnt), fake_val0))
   file.write("%s*UVW_U_TILING %s\n" % ((Tab*idnt), fake_val4))
   file.write("%s*UVW_V_TILING %s\n" % ((Tab*idnt), fake_val4))

   map_uvw(file, idnt) #hardcoded

   file.write("%s*BITMAP_FILTER %s\n" % ((Tab*idnt), tex_filter))

   idnt -= 1
   file.write("%s}\n" % (Tab*idnt))


def map_uvw(file, idnt):

   fake_val0 = '0.0000'
   fake_val1 = '1.0000'

   file.write("%s*UVW_ANGLE %s\n" % ((Tab*idnt), fake_val0))
   file.write("%s*UVW_BLUR %s\n" % ((Tab*idnt), fake_val1))
   file.write("%s*UVW_BLUR_OFFSET %s\n" % ((Tab*idnt), fake_val0))
   file.write("%s*UVW_NOUSE_AMT %s\n" % ((Tab*idnt), fake_val1))
   file.write("%s*UVW_NOISE_SIZE %s\n" % ((Tab*idnt), fake_val1))
   file.write("%s*UVW_NOISE_LEVEL 1\n" % (Tab*idnt))
   file.write("%s*UVW_NOISE_PHASE %s\n" % ((Tab*idnt), fake_val0))


#============================================
#           Mesh
#============================================


def write_mesh(file, scn, exp_list, matTable, total):
   print "Write Geometric"
   global tex_list

   for current_container in exp_list:

      TransTable = {'SizeX': 1, 'SizeY': 1, 'SizeZ': 1}
      nameMe = {'objName': 'obj', 'meName': 'me'}
      sGroups = {}
      hasTable = {'hasMat': 0, 'hasSG': 0, 'hasUV': 0, 'hasVC': 0, 'matRef': 0}
      count = {'face': 0, 'vert': 0, 'UVs': 0, 'cVert': 0}

      obj = current_container[0]
      #mat_ref = current_container[1]
      data = obj.getData(0,1)
      nameMe['objName'] = get_Name(obj)
      nameMe['meName'] = data.name

      mats_me = [mat for mat in data.materials if mat] #fix for 2.44, get rid of NoneType Objects in me.materials
      mats_ob = obj.getMaterials(0)
      materials = False

      if mats_me:
         materials = mats_me
      elif mats_ob:
         materials = mats_ob

      if guiTable['MTL'] and materials:
         hasTable['hasMat'] = 1
         hasTable['matRef'] = current_container[1]

      if obj.getParent():
         nameMe['parent'] = get_Name(obj.getParent())

      me = Mesh.New()      # Create a new mesh

      if guiTable['MOD']:   # Use modified mesh
         me.getFromObject(obj.name, 0) # Get the object's mesh data, cage 0 = apply mod
      else:
         me.getFromObject(obj.name, 1)

      me.transform(obj.matrix)   # ASE stores transformed mesh data
      #if guiTable['RECENTER']:   # Recentre Objects to 0,0,0 feature
      #   rec_matrix = Mathutils.TranslationMatrix(obj.matrix.translationPart().negate())
      #   me.transform(rec_matrix)

      if guiTable['MESHORI']:
         if (obj.matrix != Mathutils.Matrix([1,0,0,0],[0,1,0,0],[0,0,1,0],[0,0,0,1])):
            m = obj.matrix
            file.write("%s<bone name=\"%s\" role=\"mesh\">\n" % (Tab, nameMe['objName']))
            file.write("%s<matrix>\n" % (Tab*2))
            file.write("%s<row>%.6f %.6f %.6f %.6f</row>\n" % ((Tab*3), m[0][0], m[0][1], m[0][2], m[0][3]))
            file.write("%s<row>%.6f %.6f %.6f %.6f</row>\n" % ((Tab*3), m[1][0], m[1][1], m[1][2], m[1][3]))
            file.write("%s<row>%.6f %.6f %.6f %.6f</row>\n" % ((Tab*3), m[2][0], m[2][1], m[2][2], m[2][3]))
            file.write("%s<row>%.6f %.6f %.6f %.6f</row>\n" % ((Tab*3), m[3][0], m[3][1], m[3][2], m[3][3]))
            file.write("%s</matrix>\n" % (Tab*2))
            file.write("%s</bone>\n" % (Tab))
            ra = []
            ra.append(get_Name(obj))
            ra.append(obj.matrixLocal.translationPart())
            # Deconstruct matrix
            m = obj.matrixLocal.rotationPart()
            ra.append(normalizeRotation(m))
            restanim.append(ra)

      tempObj = Blender.Object.New('Mesh', 'XML_export_temp_obj')
      tempObj.setMatrix(obj.matrix)
      tempObj.link(me)

#      if guiTable['VG2SG']:
      VGNames = data.getVertGroupNames()
      for vg in VGNames:
         me.addVertGroup(vg)
         gverts = data.getVertsFromGroup(vg, 1)
         gverts_copy = []
         for gv in gverts:
            gverts_copy.append(gv[0])
         me.assignVertsToGroup(vg, gverts_copy, 1, 1)

      obj = tempObj
      faces = me.faces
      verts = me.verts

      count['vert'] = len(verts)
      total['Verts'] += count['vert']

      if count['vert'] == 0:
         errormsg('Error: ' + nameMe['meName'] + 'has 0 Verts, skipped', True)
         continue

      vGroups = me.getVertGroupNames()
      if guiTable['VG2SG'] and len(vGroups) > 0:
         for current_VG in vGroups:
            if current_VG.lower().count("smooth."):
               hasTable['hasSG'] = 1
               smooth_num = int(current_VG.lower().replace("smooth.", ""))
               gverts = me.getVertsFromGroup(current_VG)
               for vi in gverts:
                  if not sGroups.has_key(vi):
                     sGroups[vi] = [smooth_num]
                  else:
                     sGroups[vi].append(smooth_num)

      if guiTable['UV']:
         if me.faceUV == True or me.faceUV == 1:
            hasTable['hasUV'] = 1

      if guiTable['VC']:
         if me.vertexColors:
            hasTable['hasVC'] = 1
         elif hasTable['hasMat']: # Blender material
            for current_mat in materials:
               if current_mat.getMode() & Material.Modes['VCOL_PAINT']:
                  hasTable['hasVC'] = 1
                  break

      for current_face in faces:
         if len(current_face.verts) is 3:
            count['face'] += 1
            total['Tris'] += 1
            total['Faces'] += 1
         elif len(current_face.verts) is 4:
            count['face'] += 2
            total['Tris'] += 2
            total['Faces'] += 1

      me_options = {}

      if me.mode and Mesh.Modes['TWOSIDED']:
          me_options['doublesided'] = 1
      if materials:
         for mat in materials:
             for tex in mat.getTextures():
                if not tex:
                    continue
                if not tex.tex:
                    continue
                if tex.tex.name.startswith('SI'):
                    me_options['texturestyle'] = get_Name(tex.tex)
                elif tex.tex.getType() == 'Image':
                    if tex.tex.image:
                        me_options['texturename'] = tex.tex.name
                        tex_list[tex.tex.name] = {}
                        me_options['texture'] = {}
                        tex_list[tex.tex.name]['name'] = tex.tex.name
                        tex_list[tex.tex.name]['file'] = tex.tex.image.filename
                        tex_list[tex.tex.name]['alpha'] = tex.tex.useAlpha
                        recol = 0
                        if tex.tex.mipmap:
                            recol = 7
                        #if tex.tex.mipmap:
                        #    recol = recol + 1
                        #if tex.tex.gauss:
                        #    recol = recol + 2
                        #if tex.tex.interpol:
                        #    recol = recol + 4
                        tex_list[tex.tex.name]['recol'] = recol


      #Open Geomobject
      if 'texturename' in me_options:
         file.write("%s<mesh name=\"%s\" texture=\"%s\">\n" % (Tab, nameMe['objName'], me_options['texturename']))
      else:
         file.write("%s<mesh name=\"%s\">\n" % (Tab, nameMe['objName']))
      print "Exporting Mesh %s" % nameMe['objName']

      me.calcNormals()
      idnt = 2

      if me_options:
          file.write("%s<metadata role=\"rct3\">\n" % (Tab*2))
          if 'doublesided' in me_options:
             file.write("%s<doubleSided/>\n" % (Tab*3))
          if 'texturestyle' in me_options:
             file.write("%s<textureStyle>%s</textureStyle>\n" % (Tab*3, me_options['texturestyle']))

          file.write("%s</metadata>\n" % (Tab*2))
      mesh_writeMeshData(file, idnt, me, data, current_container[0])
      #mesh_vertexList(file, idnt, verts, count)
      #idnt = 2
      #mesh_faceList(file, idnt, me, materials, sGroups, faces, matTable, hasTable, count)

      file.write("%s</mesh>\n" % (Tab))

      #free some memory
      me.materials = [None]
      if me.faces:
         me.faces.delete(1,[(f.index) for f in me.faces])
      me.verts.delete(me.verts)
      obj.fakeUser = False
      me.fakeUser = False
      scn.objects.unlink(obj)




def mesh_writeMeshData(file, idnt, me, orig_me, obj):
   faces = me.faces

   # To export quads it is needed to calculate all face and vertex normals new!
   #vec_null = Blender.Mathutils.Vector(0.0, 0.0, 0.0)
   #v_normals = dict([(v.index, vec_null) for v in me.verts])

   #-- Calculate new face and vertex normals

   #~ for i,f in enumerate(faces):

      #~ f_verts = f.verts

      #~ if len(f_verts) is 3: #tri
         #~ v0,v1,v2 = f_verts[:]
         #~ v0_i,v1_i,v2_i = f_verts[0].index, f_verts[1].index, f_verts[2].index
         #~ f_no = Blender.Mathutils.TriangleNormal(v0.co, v1.co, v2.co)
         #~ if f.smooth:
            #~ v_normals[v0_i] = v_normals[v0_i] + f_no
            #~ v_normals[v1_i] = v_normals[v1_i] + f_no
            #~ v_normals[v2_i] = v_normals[v2_i] + f_no

      #~ if len(f_verts) is 4: #quad
         #~ v0,v1,v2,v3 = f_verts[:]
         #~ v0_i,v1_i,v2_i,v3_i = f_verts[0].index, f_verts[1].index, f_verts[2].index,f_verts[3].index
         #~ f_no0 = Blender.Mathutils.TriangleNormal(v0.co, v1.co, v2.co)
         #~ f_no1 = Blender.Mathutils.TriangleNormal(v2.co, v3.co, v0.co)
         #~ if f.smooth:
            #~ v_normals[v0_i] = v_normals[v0_i] + f_no0
            #~ v_normals[v1_i] = v_normals[v1_i] + f_no0
            #~ v_normals[v2_i] = v_normals[v2_i] + f_no0

            #~ v_normals[v0_i] = v_normals[v2_i] + f_no1
            #~ v_normals[v2_i] = v_normals[v3_i] + f_no1
            #~ v_normals[v3_i] = v_normals[v0_i] + f_no1


   #-- Normalize vectors
   #for i,vec in v_normals.iteritems():
   #~ for vec in v_normals.itervalues():
      #~ vec.normalize()

   vertexMap = {}
   indexMap = {}
   p = find_boneParent(obj)
   warned = False
   supports_weights = (len(orig_me.verts) == len(me.verts))
   if guiTable['BWEIGHT'] and (not supports_weights):
       warningmsg("Mesh does not support bone weights (modifier(s) changed vertex count).", True)

   for current_face in faces:

      face_verts = current_face.verts

      for i in range(0, len(face_verts)):
         #no = v_normals[face_verts[i].index]
         no = face_verts[i].no
         bmap = [];
         if p:
            isarm = False
            name = ""
            if not isinstance(p, basestring):
                if p.getType() == "Armature":
                    isarm = True
                else:
                    name = get_Name(p)
            else:
                name = p
            if isarm:
                if not guiTable['BWEIGHT']:   # Use modified mesh
                    bmap = me.getVertexInfluences(face_verts[i].index)
                else:
                    if (len(orig_me.verts) == len(me.verts)):
                        bmap = orig_me.getVertexInfluences(face_verts[i].index)
                    else:
                        bmap = me.getVertexInfluences(face_verts[i].index)

            else:
                bmap.append([name, 1.0])
         vert = (face_verts[i].co[0], face_verts[i].co[1], face_verts[i].co[2], no[0], no[1], no[2], current_face.uv[i][0], current_face.uv[i][1])
         if not vertexMap.has_key(vert):
            vertexMap[vert] = bmap

   for index,current_vert in enumerate(vertexMap.iterkeys()):
      indexMap[current_vert] = index

      file.write("%s<vertex>\n" % ((Tab*idnt)))
      idnt += 1

      file.write("%s<position x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % ((Tab*idnt), current_vert[0], current_vert[1], current_vert[2]))
      file.write("%s<normal x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % ((Tab*idnt), current_vert[3], current_vert[4], current_vert[5]))
      file.write("%s<uv u=\"%.6f\" v=\"%.6f\"/>\n" % ((Tab*idnt), current_vert[6], current_vert[7]))
      # poor man's bone...

      bones = vertexMap[current_vert]
      bsum = 0.0
      for bn in bones:
         bsum += bn[1]
      if bsum > 1.0:
          if not warned:
            warningmsg("Bone weight sum > 1.0, you probably need to activate bone weights", True)
            warned = True
      if bsum > 0.0:
        for bn in bones:
            file.write("%s<bone name=\"%s\" weight=\"%f\"/>\n" % ((Tab*idnt), bn[0], bn[1]*100.0/bsum))

      idnt -= 1
      file.write("%s</vertex>\n" % ((Tab*idnt)))

   for current_face in me.faces:

      face_verts = current_face.verts

      if len(face_verts) is 3:
         #no = v_normals[face_verts[0].index]
         no = face_verts[0].no
         vert0 = (face_verts[0].co[0], face_verts[0].co[1], face_verts[0].co[2], no[0], no[1], no[2], current_face.uv[0][0], current_face.uv[0][1])
         #no = v_normals[face_verts[1].index]
         no = face_verts[1].no
         vert1 = (face_verts[1].co[0], face_verts[1].co[1], face_verts[1].co[2], no[0], no[1], no[2], current_face.uv[1][0], current_face.uv[1][1])
         #no = v_normals[face_verts[2].index]
         no = face_verts[2].no
         vert2 = (face_verts[2].co[0], face_verts[2].co[1], face_verts[2].co[2], no[0], no[1], no[2], current_face.uv[2][0], current_face.uv[2][1])

         file.write("%s<triangle a=\"%i\" b=\"%i\" c=\"%i\"/>\n" % ((Tab*idnt), indexMap[vert0], indexMap[vert1], indexMap[vert2]))

      elif len(face_verts) is 4:
         #no = v_normals[face_verts[0].index]
         no = face_verts[0].no
         vert0 = (face_verts[0].co[0], face_verts[0].co[1], face_verts[0].co[2], no[0], no[1], no[2], current_face.uv[0][0], current_face.uv[0][1])
         #no = v_normals[face_verts[1].index]
         no = face_verts[1].no
         vert1 = (face_verts[1].co[0], face_verts[1].co[1], face_verts[1].co[2], no[0], no[1], no[2], current_face.uv[1][0], current_face.uv[1][1])
         #no = v_normals[face_verts[2].index]
         no = face_verts[2].no
         vert2 = (face_verts[2].co[0], face_verts[2].co[1], face_verts[2].co[2], no[0], no[1], no[2], current_face.uv[2][0], current_face.uv[2][1])
         #no = v_normals[face_verts[3].index]
         no = face_verts[3].no
         vert3 = (face_verts[3].co[0], face_verts[3].co[1], face_verts[3].co[2], no[0], no[1], no[2], current_face.uv[3][0], current_face.uv[3][1])

         v0 = indexMap[vert0]
         v1 = indexMap[vert1]
         v2 = indexMap[vert2]
         v3 = indexMap[vert3]

         file.write("%s<triangle a=\"%i\" b=\"%i\" c=\"%i\"/>\n" % ((Tab*idnt), v0, v1, v2))
         file.write("%s<triangle a=\"%i\" b=\"%i\" c=\"%i\"/>\n" % ((Tab*idnt), v0, v2, v3))

def write_groups(file):
   lights = 0
   for group in Group.Get():
      if group.name[0] == '_':
         continue

      distance = RCT3GroupBag(group).lodDistance
      actions = ModxmlGroupBag(group).animations.split(",")

      objects = group.objects

      groupobjs = []
      for obj in objects:
         if (guiTable['SELO'] == 1) and (not obj.isSelected()):
            continue

         if obj.name[0] == '_':
            continue

         if obj.getType() == 'Mesh':
            groupobjs.append(obj)
         elif obj.getType() == 'Empty':
            groupobjs.append(obj)
         elif obj.getType() == 'Armature':
            groupobjs.append(obj)
         elif obj.getType() == 'Camera':
            groupobjs.append(obj)
         elif obj.getType() == 'Lamp':
            l = obj.getData()
            if l.type != Lamp.Types['Lamp']:
               continue

            fakelights = 0
            s = get_Name(obj);
            #if not (s.startswith("light") or s.startswith("nblight") or s.startswith("simplelight")):
            s = make_lampName(l, lights, s)
            if s == "":
               continue
            groupobjs.append(obj)

      if groupobjs:
         file.write("%s<group name=\"%s\">\n" % (Tab, get_Name(group)))
         if distance>0.0:
            file.write("%s<metadata role=\"rct3\">\n" % (Tab*2))
            file.write("%s<lodDistance>%f</lodDistance>\n" % (Tab*3, distance))
            file.write("%s</metadata>\n" % (Tab*2))

         for grobj in groupobjs:
            if grobj.getType() == 'Mesh':
               file.write("%s<mesh>%s</mesh>\n" % ((Tab*2), get_Name(grobj)))
            elif grobj.getType() == 'Armature':
               arm_data= grobj.getData()
               for bone in arm_data.bones.values():
                  if bone.name[0] != '_':
                     file.write("%s<bone>%s</bone>\n" % ((Tab*2), get_Name(bone)))
            elif grobj.getType() == 'Lamp':
               l = grobj.getData()
               s = get_Name(grobj);
               #if not (s.startswith("light") or s.startswith("nblight") or s.startswith("simplelight")):
               s = make_lampName(l, lights, s)
               file.write("%s<bone>%s</bone>\n" % ((Tab*2), s))
            else:
               file.write("%s<bone>%s</bone>\n" % ((Tab*2), get_Name(grobj)))

         for an in actions:
               file.write("%s<animation>%s</animation>\n" % ((Tab*2), an))

         file.write("%s</group>\n" % (Tab))




   #-------------------------End?----------------------


def write_ui(filename):

   global guiTable, gSilentMessages
   #, EXPORT_MOD, EXPORT_MTL, EXPORT_VC, EXPORT_SELO, EXPORT_UVI, EXPORT_MESHORI, EXPORT_SIMPROT, EXPORT_MAXTIME, EXPORT_FPS, EXPORT_KEYONLY, EXPORT_ROTFORM
   guiTable = {}

   curscene = Scene.GetCurrent()

   # Defaults
   guiTable['MOD'] = 1
   guiTable['BWEIGHT'] = 0
   guiTable['MTL'] = 1
   guiTable['VC'] = 1
   guiTable['SELO'] = 1
   guiTable['MESHORI'] = 0
   guiTable['SIMPROT'] = 1
   guiTable['MAXTIME'] = 0
   guiTable['FPS'] = 0
   guiTable['EXPFPS'] = 0
   guiTable['KEYONLY'] = 0
   guiTable['KEYONLYMODE'] = '0'
   guiTable['ROTFORM'] = 'Q'
   guiTable['STORE'] = 1
   guiTable['SHOWOPT'] = 1
   guiTable['WRITENONANIM'] = 1

   # Stuff from Goofos ASE exporter I don't dare touch :p
   guiTable['UV'] = 1
   guiTable['UVI'] = 0
   guiTable['VG2SG'] = 0
   guiTable['RECENTER'] = 0

   if 'modxml' in curscene.properties:
       mprop = curscene.properties['modxml']
       if 'export' in mprop:
           eprop = mprop['export']
           if 'MOD' in eprop:
                guiTable['MOD'] = eprop['MOD']
           if 'BWEIGHT' in eprop:
                guiTable['BWEIGHT'] = eprop['BWEIGHT']
           if 'MTL' in eprop:
                guiTable['MTL'] = eprop['MTL']
           if 'VC' in eprop:
                guiTable['VC'] = eprop['VC']
           if 'SELO' in eprop:
                guiTable['SELO'] = eprop['SELO']
           if 'MESHORI' in eprop:
                guiTable['MESHORI'] = eprop['MESHORI']
           if 'SIMPROT' in eprop:
                guiTable['SIMPROT'] = eprop['SIMPROT']
           if 'MAXTIME' in eprop:
                guiTable['MAXTIME'] = eprop['MAXTIME']
           if 'FPS' in eprop:
                guiTable['FPS'] = eprop['FPS']
           if 'EXPFPS' in eprop:
                guiTable['EXPFPS'] = eprop['EXPFPS']
           if 'KEYONLY' in eprop:
                guiTable['KEYONLY'] = eprop['KEYONLY']
           if 'KEYONLYMODE' in eprop:
                guiTable['KEYONLYMODE'] = eprop['KEYONLYMODE']
           if 'ROTFORM' in eprop:
                guiTable['ROTFORM'] = eprop['ROTFORM']
           if 'STORE' in eprop:
                guiTable['STORE'] = eprop['STORE']
           if 'SHOWOPT' in eprop:
                guiTable['SHOWOPT'] = eprop['SHOWOPT']
           if 'WRITENONANIM' in eprop:
                guiTable['WRITENONANIM'] = eprop['WRITENONANIM']


   if guiTable['SHOWOPT']:
       EXPORT_MOD = Draw.Create(guiTable['MOD'])
       EXPORT_BWEIGHT = Draw.Create(guiTable['BWEIGHT'])
       EXPORT_MTL = Draw.Create(guiTable['MTL'])
       #EXPORT_UV = Draw.Create(guiTable['UV'])
       EXPORT_VC = Draw.Create(guiTable['VC'])
       EXPORT_SELO = Draw.Create(guiTable['SELO'])
       #EXPORT_VG2SG = Draw.Create(guiTable['VG2SG'])
       #EXPORT_REC = Draw.Create(guiTable['RECENTER'])
       EXPORT_MESHORI = Draw.Create(guiTable['MESHORI'])
       EXPORT_SIMPROT = Draw.Create(guiTable['SIMPROT'])
       EXPORT_MAXTIME = Draw.Create(guiTable['MAXTIME'])
       EXPORT_FPS = Draw.Create(guiTable['FPS'])
       EXPORT_EXPFPS = Draw.Create(guiTable['EXPFPS'])
       EXPORT_KEYONLY = Draw.Create(guiTable['KEYONLY'])
       EXPORT_KEYONLYMODE = Draw.Create(guiTable['KEYONLYMODE'])
       EXPORT_ROTFORM = Draw.Create(guiTable['ROTFORM'])
       EXPORT_STORE = Draw.Create(guiTable['STORE'])
       EXPORT_SHOWOPT = Draw.Create(guiTable['SHOWOPT'])
       EXPORT_WRITENONANIM = Draw.Create(guiTable['WRITENONANIM'])

       # Get USER Options
       pup_block = []
       pup_block.append(('Options...'))
       pup_block.append(('Apply Modifiers', EXPORT_MOD, 'Use modified mesh data from each object.'))
       pup_block.append(('Bone Weights', EXPORT_BWEIGHT, 'Export bone weights instead of simple assignment (see readme).'))
       pup_block.append(('Materials', EXPORT_MTL, 'Export Materials.'))
       pup_block.append(('RCT3 Fixes', EXPORT_VC, 'Export effect points/bones appropriately for RCT3'))
       pup_block.append(('Simplify Rotations', EXPORT_SIMPROT, 'Simplify rotational keyframes'))
       pup_block.append(('Animation...'))
       pup_block.append(('Animation template length', EXPORT_MAXTIME, 0.0, 3600.0, 'Length for animation template. Zero deactivates it.'))
       pup_block.append(('Evaluation FPS', EXPORT_FPS, 0.0, 120.0, 'Evaluate animation as frames per second.'))
       pup_block.append(('Export FPS', EXPORT_EXPFPS, 0.0, 120.0, 'Export animation as frames per second.'))
       pup_block.append(('Keyframes only', EXPORT_KEYONLY, 'Export only defined keyframes without interpolation.'))
       pup_block.append(('  KO-Mode: ', EXPORT_KEYONLYMODE, 0, 1, 'Keyframe-only mode: ''0'' (Zero), ''3'' or ''5'' (see readme)'))
       pup_block.append(('Rot. format: ', EXPORT_ROTFORM, 0, 1, 'Rotation keyframe format: ''E''uler, ''A''xis or ''Q''uaternion'))
       pup_block.append(('Write non-animated bones', EXPORT_WRITENONANIM, 'Export non-animated bones in their rest-state.'))
       pup_block.append(('Context...'))
       pup_block.append(('Mesh Origins', EXPORT_MESHORI, 'Export mesh origins as bones'))
       pup_block.append(('Selection Only', EXPORT_SELO, 'Only export objects in visible selection, else export all mesh object.'))
       pup_block.append(('Export Script...'))
       pup_block.append(('Store Options', EXPORT_STORE, 'Store these options with your blend file.'))
       pup_block.append(('Show Options', EXPORT_SHOWOPT, 'Show these options.'))

       if not Draw.PupBlock('Export...', pup_block):
          return


       guiTable['MOD'] = EXPORT_MOD.val
       guiTable['BWEIGHT'] = EXPORT_BWEIGHT.val
       guiTable['MTL'] = EXPORT_MTL.val
       #guiTable['UV'] = EXPORT_UV.val
       guiTable['VC'] = EXPORT_VC.val
       guiTable['SELO'] = EXPORT_SELO.val
       #guiTable['VG2SG'] = EXPORT_VG2SG.val
       #guiTable['RECENTER'] = EXPORT_REC.val
       guiTable['MESHORI'] = EXPORT_MESHORI.val
       guiTable['SIMPROT'] = EXPORT_SIMPROT.val
       guiTable['MAXTIME'] = EXPORT_MAXTIME.val
       guiTable['FPS'] = EXPORT_FPS.val
       guiTable['EXPFPS'] = EXPORT_EXPFPS.val
       guiTable['KEYONLY'] = EXPORT_KEYONLY.val
       guiTable['KEYONLYMODE'] = EXPORT_KEYONLYMODE.val
       guiTable['ROTFORM'] = EXPORT_ROTFORM.val
       guiTable['STORE'] = EXPORT_STORE.val
       guiTable['SHOWOPT'] = EXPORT_SHOWOPT.val
       guiTable['WRITENONANIM'] = EXPORT_WRITENONANIM.val

       if (guiTable['ROTFORM'] != 'E') and (guiTable['ROTFORM'] != 'A') and (guiTable['ROTFORM'] != 'Q'):
           #Draw.PupMenu("Error!%t|Invalid rotation format given.")
           errormsg("Invalid rotation format given.")
           return

       if (guiTable['MOD']) and (guiTable['BWEIGHT']):
           warningmsg("Modifier application and bone weights enabled, this can lead to errors!")

       if guiTable['STORE']:
           if not ('modxml' in curscene.properties):
               curscene.properties['modxml'] = {}
           if not ('export' in curscene.properties['modxml']):
               curscene.properties['modxml']['export'] = {}
           curscene.properties['modxml']['export']['MOD'] = guiTable['MOD']
           curscene.properties['modxml']['export']['BWEIGHT'] = guiTable['BWEIGHT']
           curscene.properties['modxml']['export']['MTL'] = guiTable['MTL']
           curscene.properties['modxml']['export']['VC'] = guiTable['VC']
           curscene.properties['modxml']['export']['SELO'] = guiTable['SELO']
           curscene.properties['modxml']['export']['MESHORI'] = guiTable['MESHORI']
           curscene.properties['modxml']['export']['SIMPROT'] = guiTable['SIMPROT']
           curscene.properties['modxml']['export']['MAXTIME'] = guiTable['MAXTIME']
           curscene.properties['modxml']['export']['FPS'] = guiTable['FPS']
           curscene.properties['modxml']['export']['EXPFPS'] = guiTable['EXPFPS']
           curscene.properties['modxml']['export']['KEYONLY'] = guiTable['KEYONLY']
           curscene.properties['modxml']['export']['KEYONLYMODE'] = guiTable['KEYONLYMODE']
           curscene.properties['modxml']['export']['ROTFORM'] = guiTable['ROTFORM']
           curscene.properties['modxml']['export']['STORE'] = guiTable['STORE']
           curscene.properties['modxml']['export']['SHOWOPT'] = guiTable['SHOWOPT']
           curscene.properties['modxml']['export']['WRITENONANIM'] = guiTable['WRITENONANIM']

   Window.WaitCursor(1)

   if not filename.lower().endswith('.modxml'):
      filename += '.modxml'

   if guiTable['FPS'] == 0:
        guiTable['FPS'] = curscene.getRenderingContext().fps
   if guiTable['EXPFPS'] == 0:
        guiTable['EXPFPS'] = curscene.getRenderingContext().fps

   write(filename)

   Window.WaitCursor(0)

   if gSilentMessages:
       infomsg("There were messages during export, please check the console")


if __name__ == '__main__':
   Window.FileSelector(write_ui, 'Export XML Scene', sys.makename(ext='.modxml'))
