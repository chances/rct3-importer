<?xml version="1.0"?>
<!--
	RELAX NG Shorthand Processor
	$Id: ShortRNG.xsl,v 1.2 2001-08-17 11:30:00-07 bear Exp bear $
                      Copyright (C) 2001, Kohsuke KAWAGUCHI
                         e-mail: kohsukekawaguchi@yahoo.com

	$Log: ShortRNG.xsl,v $
	Revision 1.4  2008-03-30 Tobias Minich
	Fixed occurs/minOccurs/maxOccurs appearing in output when used with type
	Added ref attribute. Works like type, adds a ref child node with the respective name attribute.
	
	Revision 1.2  2001-08-17 11:30:00-07  bear
	supported <element> with @type.

	Revision 1.1  2001-08-16 22:13:51-07  bear
	implemented @minOccurs/@maxOccurs support.

-->
<x:stylesheet
	xmlns:x="http://www.w3.org/1999/XSL/Transform"
	xmlns="http://relaxng.org/ns/structure/1.0"
	xmlns:s="http://relaxng.sourceforge.net/ns/shorthand"
	exclude-result-prefixes="s"
	version="1.0">
	
	<x:output encoding="utf-8" indent="yes"/>
	
	<!-- pattern with @maxOccurs/@minOccurs -->
	<x:template match="s:*[@maxOccurs|@minOccurs]">
		<!-- minOccurs first -->
		<x:choose>
			<x:when test="@minOccurs">
				<x:call-template name="copyNtimes">
					<x:with-param name="howMany" select="number(@minOccurs)"/>
					<x:with-param name="exp" select="."/>
				</x:call-template>
			</x:when>
			<x:otherwise><!-- when @minOccurs is not present -->
				<x:apply-templates select="." mode="pass2"/>
			</x:otherwise>
		</x:choose>
		<!-- maxOccurs -->
		<x:choose>
			<x:when test="@maxOccurs='unbounded'">
				<zeroOrMore>
					<x:apply-templates select="." mode="pass2"/>
				</zeroOrMore>
			</x:when>
			<x:when test="@maxOccurs and @minOccurs">
				<x:call-template name="copyNtimes.optional">
					<x:with-param name="howMany" select="number(@maxOccurs)-number(@minOccurs)"/>
					<x:with-param name="exp" select="."/>
				</x:call-template>
			</x:when>
			<x:when test="@maxOccurs">
				<x:call-template name="copyNtimes.optional">
					<x:with-param name="howMany" select="number(@maxOccurs)-1"/>
					<x:with-param name="exp" select="."/>
				</x:call-template>
			</x:when>
			<x:otherwise><!-- when @maxOccurs is not present -->
				<x:call-template name="copyNtimes.optional">
					<x:with-param name="howMany" select="1-number(@minOccurs)"/>
					<x:with-param name="exp" select="."/>
				</x:call-template>
			</x:otherwise>
		</x:choose>
	</x:template>
	
	<!-- repeatedly copy the expression by using the tail recursion -->
	<x:template name="copyNtimes">
		<x:param name="howMany"/>
		<x:param name="exp"/>
		
		<x:if test="$howMany > 0 ">
			<x:apply-templates select="$exp" mode="pass2"/>
			<x:call-template name="copyNtimes">
				<x:with-param name="howMany" select="$howMany - 1"/>
				<x:with-param name="exp" select="$exp"/>
			</x:call-template>
		</x:if>
	</x:template>
	
	<!-- repeatedly copy the expression by using the tail recursion -->
	<x:template name="copyNtimes.optional">
		<x:param name="howMany"/>
		<x:param name="exp"/>
		
		<x:if test="$howMany > 0 ">
			<optional>
				<x:apply-templates select="$exp" mode="pass2"/>
				<x:call-template name="copyNtimes.optional">
					<x:with-param name="howMany" select="$howMany - 1"/>
					<x:with-param name="exp" select="$exp"/>
				</x:call-template>
			</optional>
		</x:if>
	</x:template>
	
	
	<!-- pattern with @occurs -->
	<x:template match="s:*[@occurs]">
		<x:choose>
			<x:when test="@occurs='?'">
				<optional>
					<x:apply-templates select="." mode="pass2"/>
				</optional>
			</x:when>
			<x:when test="@occurs='+'">
				<oneOrMore>
					<x:apply-templates select="." mode="pass2"/>
				</oneOrMore>
			</x:when>
			<x:when test="@occurs='*'">
				<zeroOrMore>
					<x:apply-templates select="." mode="pass2"/>
				</zeroOrMore>
			</x:when>
			<x:otherwise>
				<x:message terminate="yes">
					Unknown occurs type: <x:value-of select="@occurs"/>
				</x:message>
			</x:otherwise>
		</x:choose>
	</x:template>
	<x:template match="s:*">
		<x:apply-templates select="." mode="pass2"/>
	</x:template>
	
	
	<!-- pattern with @ref -->
	<x:template match="s:*[@ref]" mode="pass2">
		<x:element name="{local-name()}">
			<x:apply-templates select="@*[(name() != 'minOccurs') and (name() != 'maxOccurs') and (name() != 'occurs')]|*|text()|comment()" mode="pass3"/>
			<ref name="{@ref}"/>
		</x:element>
	</x:template>
	<!-- attribute with @type -->
	<x:template match="s:attribute[@type]" mode="pass2">
		<attribute>
			<x:apply-templates select="@*[(name() != 'minOccurs') and (name() != 'maxOccurs') and (name() != 'occurs')]|*|text()|comment()" mode="pass3"/>
			<data type="{@type}"/>
		</attribute>
	</x:template>
	<!-- element with @type -->
	<x:template match="s:element[@type]" mode="pass2">
		<element>
			<x:apply-templates select="@*[(name() != 'minOccurs') and (name() != 'maxOccurs') and (name() != 'occurs')]|*|text()|comment()" mode="pass3"/>
			<data type="{@type}"/>
		</element>
	</x:template>
	<x:template match="s:*" mode="pass2">
		<x:apply-templates select="." mode="pass3"/>
	</x:template>
	<!-- ignore @type of the attribute -->
	<x:template match="s:attribute/@type" mode="pass3"/>
	<!-- ignore @type of the element -->
	<x:template match="s:element/@type" mode="pass3"/>
	<!-- default element processing rule -->
	<x:template match="s:*" mode="pass3">
		<x:element name="{local-name(.)}"><x:apply-templates select="*|@*[(name() != 'minOccurs') and (name() != 'maxOccurs') and (name() != 'occurs')]|text()|comment()"/></x:element>
	</x:template>
	
	
	<!-- ignore @ref -->
	<x:template match="@ref" mode="pass3"/>
	<!-- ignore @occurs -->
	<x:template match="@occurs"/>
	<x:template match="@maxOccurs" mode="pass3"/>
	<x:template match="@minOccurs" mode="pass3"/>
	<!-- default rule -->
	<x:template match="*|@*|comment()|text()">
		<x:copy>
			<x:apply-templates select="*|@*|text()|comment()"/>
		</x:copy>
	</x:template>
	<x:template match="*|@*|comment()|text()" mode="pass3">
		<x:copy>
			<x:apply-templates select="*|@*|text()|comment()"/>
		</x:copy>
	</x:template>
	
</x:stylesheet>
