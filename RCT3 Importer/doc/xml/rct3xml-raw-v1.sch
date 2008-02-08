<?xml version="1.0" encoding="UTF-8"?>
<schema xmlns:sch="http://purl.oclc.org/dsdl/schematron" xmlns="http://purl.oclc.org/dsdl/schematron" queryBinding="xslt">
	<ns prefix="rct3" uri="http://rct3.sourceforge.net/rct3xml/raw" />
	<pattern>
		<!-- abstract rules -->
		<rule abstract="true" id="ref.ban.local">
			<assert test="ancestor::rct3:rawovl[1]//rct3:ban/@name[. = current()]">BoneAnimation &quot;<value-of select="."/>&quot; not found in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>
		<rule abstract="true" id="ref.bsh.local">
			<assert test="ancestor::rct3:rawovl[1]//rct3:bsh/@name[. = current()]">BoneShape &quot;<value-of select="."/>&quot; not found in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>
		<rule abstract="true" id="ref.ftx.local">
			<assert test="ancestor::rct3:rawovl[1]//rct3:ftx/@name[. = current()]">FlexiTexture &quot;<value-of select="."/>&quot; not found in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>		
		<rule abstract="true" id="ref.gsi.local">
			<assert test="ancestor::rct3:rawovl[1]//rct3:gsi/@name[. = current()]">GUISkinItem &quot;<value-of select="."/>&quot; not found in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>
		<rule abstract="true" id="ref.shs.local">
			<assert test="ancestor::rct3:rawovl[1]//rct3:shs/@name[. = current()]">StaticShape &quot;<value-of select="."/>&quot; not found in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>
		<rule abstract="true" id="ref.svd.global">
			<assert test="//rct3:svd/@name[. = current()]">SceneryItemVisual &quot;<value-of select="."/>&quot; not found in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>
		<rule abstract="true" id="ref.txt.local">
			<assert test="ancestor::rct3:rawovl[1]//rct3:txt/@name[. = current()]">Text &quot;<value-of select="."/>&quot; not found in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>
	</pattern>
	<pattern>
		<title>Check SceneryItems</title>
		<rule context="rct3:sid/@name">
      		<assert test="count(//rct3:sid/@name[. = current()]) = 1">Name &quot;<value-of select="."/>&quot; not unique.</assert>
    	</rule> 
		<rule context="rct3:sid/@nametxt">
			<extends rule="ref.txt.local" />
		</rule>
		<rule context="rct3:sid/@icon">
			<extends rule="ref.gsi.local" />
		</rule>
		<rule context="rct3:sid/@svd">
			<extends rule="ref.svd.global" />
		</rule>
		<rule context="rct3:sidgroup/@name">
			<extends rule="ref.txt.local" />
		</rule>
		<rule context="rct3:sidgroup/@icon">
			<extends rule="ref.gsi.local" />
		</rule>
		<rule context="rct3:sidvisual/@name">
			<extends rule="ref.svd.global" />
		</rule>
	</pattern>
	<pattern>
		<title>Check StaticShapes</title>
		<rule context="rct3:shs/@name">
      		<assert test="count(ancestor::rct3:rawovl[1]//rct3:shs/@name[. = current()]) = 1">Name &quot;<value-of select="."/>&quot; not unique.</assert>
    	</rule> 
		<rule context="rct3:shsmesh/@ftx">
			<extends rule="ref.ftx.local" />
		</rule>
	</pattern>
	<pattern>
		<title>Check SceneryItemVisuals</title>
		<rule context="rct3:svd/@name">
      		<assert test="count(//rct3:svd/@name[. = current()]) = 1">Name &quot;<value-of select="."/>&quot; not unique.</assert>
    	</rule> 
		<rule context="rct3:svdlod/@distance">
			<assert test="count(ancestor::rct3:svd[1]//rct3:svdlod/@distance[number(.) = number(current())]) = 1">Distance &quot;<value-of select="."/>&quot; occurs multiple times in <value-of select="name(ancestor::*[@name])"/> '<value-of select="ancestor::*[@name]/@name"/>'.</assert>
		</rule>
		<rule context="rct3:svdlod/@staticshape">
			<extends rule="ref.shs.local" />
		</rule>
		<rule context="rct3:svdlod/@boneshape">
			<extends rule="ref.bsh.local" />
		</rule>
		<rule context="rct3:svdlodanimation">
			<extends rule="ref.ban.local" />
		</rule>
		<rule context="rct3:svdlod/@ftx">
			<extends rule="ref.ftx.local" />
		</rule>
	</pattern>
	<pattern>
		<title>Check Textures</title>
		<rule context="rct3:tex/@name">
      		<assert test="count(//rct3:tex/@name[. = current()]) = 1">Name &quot;<value-of select="."/>&quot; not unique.</assert>
    	</rule> 
	</pattern>
	<pattern>
		<title>Check Texts</title>
		<rule context="rct3:txt/@name">
      		<assert test="count(//rct3:txt/@name[. = current()]) = 1">Name &quot;<value-of select="."/>&quot; not unique.</assert>
    	</rule> 
	</pattern>
	<pattern>
		<title>Check WildAnimalItems</title>
		<rule context="rct3:wai/@name">
      		<assert test="count(//rct3:wai/@name[. = current()]) = 1">Name &quot;<value-of select="."/>&quot; not unique.</assert>
    	</rule> 
		<rule context="rct3:wai/@nametxt">
			<extends rule="ref.txt.local" />
		</rule>
		<rule context="rct3:wai/@description">
			<extends rule="ref.txt.local" />
		</rule>
		<rule context="rct3:wai/@icon">
			<extends rule="ref.gsi.local" />
		</rule>
		<rule context="rct3:wai/@staticshape">
			<extends rule="ref.shs.local" />
		</rule>
	</pattern>
</schema>