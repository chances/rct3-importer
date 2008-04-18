#!BPY

"""
Name: 'Model XML (.modxml) v0.1'
Blender: 244
Group: 'Export'
Tooltip: 'Model XML (.modxml)'
"""
__author__ = "Belgabor"
__version__ = "0.1"
__bpydoc__ = """\
-- Model XML (.xml) v0.1 for Blender 2.44 --<br>

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

import Blender, time, math, sys as osSys #os
from Blender import sys, Window, Draw, Scene, Mesh, Material, Texture, Image, Mathutils, Lamp, Curve, Group

# from BezierExport
def GlobaliseVector(vec, mat): return (Mathutils.Vector(vec) * mat)

#============================================
#           Write!
#============================================

def write(filename):
   start = time.clock()
   print_boxed('---------Start of Export------------')
   print 'Export Path: ' + filename

   global exp_list, Tab, idnt, imgTable, worldTable, guiTable
   global restanim
   restanim = []

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
   if (restanim):
      file.write("%s<animation name=\"Rest Animation\">\n" % (Tab))
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
      else:
         return find_boneParent(obj.parent)
   else:
      return None
   

def make_boneName(obj, type = None, name = None):
   n = obj.name
   if name:
      n = name
   s = "<bone name=\"%s\"" % n
   if type:
      s += " role=\"%s\"" % type
   p = find_boneParent(obj)
   if p:
      s += " parent=\"%s\"" % p.name
   s += ">"
   return s
      


def make_lampName(lamp, lights):
   s = ""
   if lamp.mode & Lamp.Modes['RayShadow']:
      # light should shine
      if lamp.mode & Lamp.Modes['OnlyShadow']:
         # no bulb
         s = "nblightstart"
      else:
         # bulb
         s = "lightstart"
   else:
      # light should not shine
      if lamp.mode & Lamp.Modes['OnlyShadow']:
         # no bulb, nothing to do
         return ""
      else:
         # bulb
         s = "simplelightstart"
      
   lights += 1
   
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
      
   s += "_radius%d" % lamp.dist
   return s
   

def normalizeRotation(m):
   global guiTable
   
   r = []
   # Deconstruct matrix
   t = -math.atan2(-m[1][2], m[2][2])
   r.append(t)
   t = -math.asin(m[0][2])
   r.append(t)
   t = -math.atan2(-m[0][1], m[0][0])
   r.append(t)
   
   if guiTable['SIMPROT']:
      r2 = []
      t = -math.atan2(m[1][2], -m[2][2])
      r2.append(t)
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
      container = []
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
            ra.append(current_obj.name)
            ra.append(current_obj.matrixLocal.translationPart())
            ra.append([0,0,0])
            restanim.append(ra)
            continue
            
         if len(mesh.faces) == 0:
            print 'Mesh Object '+current_obj.name+' not exported. No faces.'
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
         ra.append(current_obj.name)
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
         ra.append(current_obj.name)
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
               file.write("%s<spline name=\"%s\" cyclic=\"%d\">\n" % (Tab, current_obj.name, bezCurve.isCyclic()))
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

         s = current_obj.name;
         if not (s.startswith("light") or s.startswith("nblight") or s.startswith("simplelight")):
            s = make_lampName(l, lights)
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
         ra.append(current_obj.name)
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
   print "Write Header"

   file.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
   file.write("<model xmlns=\"http://rct3.sourceforge.net/rct3xml/model\">\n");
   file.write("%s<system handedness=\"right\" up=\"z\"/>\n" % (Tab))
   

def write_post(file, filename, scn, worldTable):
   print "Write Post"

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

   for current_container in exp_list:

      TransTable = {'SizeX': 1, 'SizeY': 1, 'SizeZ': 1}
      nameMe = {'objName': 'obj', 'meName': 'me'}
      sGroups = {}
      hasTable = {'hasMat': 0, 'hasSG': 0, 'hasUV': 0, 'hasVC': 0, 'matRef': 0}
      count = {'face': 0, 'vert': 0, 'UVs': 0, 'cVert': 0}

      obj = current_container[0]
      #mat_ref = current_container[1]
      data = obj.getData(0,1)
      nameMe['objName'] = obj.name
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
         nameMe['parent'] = obj.getParent().name
      
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
            ra.append(obj.name)
            ra.append(obj.matrixLocal.translationPart())
            # Deconstruct matrix
            m = obj.matrixLocal.rotationPart()
            ra.append(normalizeRotation(m))
            restanim.append(ra)

      tempObj = Blender.Object.New('Mesh', 'XML_export_temp_obj')
      tempObj.setMatrix(obj.matrix)
      tempObj.link(me)

      if guiTable['VG2SG']:
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
         print 'Error: ' + nameMe['meName'] + 'has 0 Verts'
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

      #Open Geomobject
      file.write("%s<mesh name=\"%s\">\n" % (Tab, nameMe['objName']))
      print "Exporting Mesh %s\n" % nameMe['objName']

      me.calcNormals()
      idnt = 2
      mesh_writeMeshData(file, idnt, me, current_container[0])
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


      

def mesh_writeMeshData(file, idnt, me, obj):
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
   
   for current_face in faces:

      face_verts = current_face.verts
      
      for i in range(0, len(face_verts)):
         #no = v_normals[face_verts[i].index]
         no = face_verts[i].no
         vert = (face_verts[i].co[0], face_verts[i].co[1], face_verts[i].co[2], no[0], no[1], no[2], current_face.uv[i][0], current_face.uv[i][1])
         if not vertexMap.has_key(vert):
            vertexMap[vert] = 0

   for index,current_vert in enumerate(vertexMap.iterkeys()):
      indexMap[current_vert] = index
      
      file.write("%s<vertex>\n" % ((Tab*idnt)))
      idnt += 1

      file.write("%s<position x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % ((Tab*idnt), current_vert[0], current_vert[1], current_vert[2]))
      file.write("%s<normal x=\"%.6f\" y=\"%.6f\" z=\"%.6f\"/>\n" % ((Tab*idnt), current_vert[3], current_vert[4], current_vert[5]))
      file.write("%s<uv u=\"%.6f\" v=\"%.6f\"/>\n" % ((Tab*idnt), current_vert[6], current_vert[7]))
      # poor mans bone...
      if p:
         file.write("%s<bone name=\"%s\" weight=\"100\"/>\n" % ((Tab*idnt), p.name))
      
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
      objects = group.objects
         
      groupobjs = []
      for object in objects:
         if (guiTable['SELO'] == 1) and (not object.isSelected()):
            continue
         
         if object.getType() == 'Mesh':
            groupobjs.append(object)
         elif object.getType() == 'Empty':
            groupobjs.append(object)
         elif object.getType() == 'Camera':
            groupobjs.append(object)
         elif object.getType() == 'Lamp':
            l = object.getData()
            if l.type != Lamp.Types['Lamp']:
               continue

            fakelights = 0
            s = object.name;
            if not (s.startswith("light") or s.startswith("nblight") or s.startswith("simplelight")):
               s = make_lampName(l, lights)
            if s == "":
               continue
            groupobjs.append(object)
            
      if groupobjs:
         file.write("%s<group name=\"%s\">\n" % (Tab, group.name))
         for grobj in groupobjs:
            if grobj.getType() == 'Mesh':
               file.write("%s<mesh>%s</mesh>\n" % ((Tab*2), grobj.name))
            elif grobj.getType() == 'Lamp':
               l = grobj.getData()
               s = grobj.name;
               if not (s.startswith("light") or s.startswith("nblight") or s.startswith("simplelight")):
                  s = make_lampName(l, lights)
               file.write("%s<bone>%s</bone>\n" % ((Tab*2), s))
            else:
               file.write("%s<bone>%s</bone>\n" % ((Tab*2), grobj.name))
         file.write("%s</group>\n" % (Tab))
         



   #-------------------------End?----------------------


def write_ui(filename):

   global guiTable, EXPORT_MOD, EXPORT_MTL, EXPORT_UV, EXPORT_VC, EXPORT_SELO, EXPORT_UVI, EXPORT_VG2SG, EXPORT_MESHORI, EXPORT_SIMPROT, EXPORT_MAXTIME
   guiTable = {'MOD': 1, 'MTL': 1, 'UV': 1, 'VC': 1, 'SELO': 1, 'UVI': 0, 'VG2SG': 0, 'RECENTER':0, 'MESHORI': 0, 'SIMPROT': 1, 'MAXTIME': 1.0}

   EXPORT_MOD = Draw.Create(guiTable['MOD'])
   EXPORT_MTL = Draw.Create(guiTable['MTL'])
   #EXPORT_UV = Draw.Create(guiTable['UV'])
   EXPORT_VC = Draw.Create(guiTable['VC'])
   EXPORT_SELO = Draw.Create(guiTable['SELO'])
   #EXPORT_VG2SG = Draw.Create(guiTable['VG2SG'])
   #EXPORT_REC = Draw.Create(guiTable['RECENTER'])
   EXPORT_MESHORI = Draw.Create(guiTable['MESHORI'])
   EXPORT_SIMPROT = Draw.Create(guiTable['SIMPROT'])
   EXPORT_MAXTIME = Draw.Create(guiTable['MAXTIME'])

   # Get USER Options
   pup_block = []
   pup_block.append(('Options...'))
   pup_block.append(('Apply Modifiers', EXPORT_MOD, 'Use modified mesh data from each object.'))
   pup_block.append(('Materials', EXPORT_MTL, 'Export Materials.'))
   pup_block.append(('RCT3 Fixes', EXPORT_VC, 'Export effect points/bones appropriately for RCT3'))
   pup_block.append(('Simplfy Rotations', EXPORT_SIMPROT, 'Simplify rotational keyframes for rest animation'))
   pup_block.append(('Rest animation length', EXPORT_MAXTIME, 0.0, 3600.0, 'Length of resting animation'))
   pup_block.append(('Context...'))
   pup_block.append(('Mesh Origins', EXPORT_MESHORI, 'Export mesh origins as bones'))
   pup_block.append(('Selection Only', EXPORT_SELO, 'Only export objects in visible selection, else export all mesh object.'))

   if not Draw.PupBlock('Export...', pup_block):
      return

   Window.WaitCursor(1)

   guiTable['MOD'] = EXPORT_MOD.val
   guiTable['MTL'] = EXPORT_MTL.val
   #guiTable['UV'] = EXPORT_UV.val
   guiTable['VC'] = EXPORT_VC.val
   guiTable['SELO'] = EXPORT_SELO.val
   #guiTable['VG2SG'] = EXPORT_VG2SG.val
   #guiTable['RECENTER'] = EXPORT_REC.val
   guiTable['MESHORI'] = EXPORT_MESHORI.val
   guiTable['SIMPROT'] = EXPORT_SIMPROT.val
   guiTable['MAXTIME'] = EXPORT_MAXTIME.val

   if not filename.lower().endswith('.modxml'):
      filename += '.modxml'

   write(filename)

   Window.WaitCursor(0)


if __name__ == '__main__':
   Window.FileSelector(write_ui, 'Export XML Scene', sys.makename(ext='.modxml'))
