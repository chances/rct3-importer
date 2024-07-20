<?xml version="1.0" encoding="UTF-8"?>
<!-- 

$Header: /home/xmlschemata/cvs/downloads/examplotron/compile.xsl,v 1.14 2004/09/21 14:25:09 vdv Exp $


Purpose: 
*******

This compiler is a XSLT transformation that compiles an 
examplotron schema into a XSLT transformation that can be used to 
validate documents that are conform to this schema.

Warning:
*******
The compilation is known to suceed with Saxon. It should be able to 
run the compiler using any XSLT processor, however this transformation 
happens to be quite challenging for the XSLT processor and problems 
are known with XT (transformation aborted for lack of support of the 
namespace axis), libxslt (bug 105116) and 4xslt (bugs 679360 and 679374). 

The resulting schema must use a Relax NG processor (with support for 
embedded Schematron rules to support eg:assert).


This Version: V 0.7 (21 September 2004) 
   http://examplotron.org/0/7/compile.xsl

Latest Version: 
   http://examplotron.org/compile.xsl

Latest version for this namespace: 
   http://examplotron.org/0/compile.xsl

Previous Version: V 0.6 (29 January 2003) 
   http://examplotron.org/0/6/compile.xsl


History:

V0.1: creation
V0.2: 
 - Added version and history information.
V0.3:
 - Added support for eg:assert
V0.4:
 - Added support for eg:import and eg:placeHolder
V0.5:
 - Major architectural change: the generated document is no longer
   a XSLT transformation but a Relax NG schema.
V0.6:
 - Need to add an empty pattern within empty elements (bug #26)
V0.7:
 - Added annotations in the RNG schema to keep the samples from the schema
 - Attributes are now optional.
 - Support for mixed content.
 - Iconic type support (token, boolean, integer, decimal, double, date, time and dateTime)
 - eg:content attribute (values eg:group, eg:interleave, eg:mixed)
V0.7-1 : bug #31
 - bug #33
 - bug #39

Copyright (c) 2001-2003 Eric van der Vlist
                   
Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
``Software''), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

The name of the authors when specified in the source files shall be 
kept unmodified.

THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL 4XT.ORG BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
USE OR OTHER DEALINGS IN THE SOFTWARE.
-->
<xsl:stylesheet 
	version="1.0" 
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
	xmlns:eg="http://examplotron.org/0/" 
	xmlns:ega="http://examplotron.org/annotations/" 
	xmlns = "http://relaxng.org/ns/structure/1.0"
	xmlns:sch ="http://www.ascc.net/xml/schematron"
	exclude-result-prefixes = "eg"
	>
  <xsl:output method="xml" version="1.0" encoding="UTF-8" indent="yes"/>
	<xsl:strip-space elements="*"/>
	<xsl:variable name="eg-uri" select="'http://examplotron.org/0/'"/>
	<xsl:variable name="Header" select="'$Header: /home/xmlschemata/cvs/downloads/examplotron/compile.xsl,v 1.14 2004/09/21 14:25:09 vdv Exp $'"/>

	<xsl:template match="/">
		<xsl:comment>
			<xsl:text>&#xa;Relax NG schema generated from an Examplotron schema.</xsl:text>
			<xsl:text>&#xa;&#xa;Version of the compiler: "</xsl:text>
			<xsl:value-of select="$Header"/>
			<xsl:text>"&#xa;&#xa;About Examplotron: http://examplotron.org </xsl:text>
			<xsl:text>&#xa;Bug reports: http://bugzilla.xmlschemata.org/cgi-bin/bugzilla/buglist.cgi?product=Examplotron</xsl:text>
			<xsl:text>&#xa;Mailing list: http://lists.xmlschemata.org/examplotron/</xsl:text>
			<xsl:text>&#xa;CVS: http://cvs.xmlschemata.org/cvsweb.cgi/downloads/examplotron/</xsl:text>
			<xsl:text>&#xa;&#xa;</xsl:text>
		</xsl:comment>
		<grammar datatypeLibrary="http://www.w3.org/2001/XMLSchema-datatypes" xmlns:ega="http://examplotron.org/annotations/">
			<start>
				<xsl:apply-templates select="*|text()|comment()"/>
			</start>
			<xsl:apply-templates select="//*[@eg:define]" mode="eg:def"/>
		</grammar>
	</xsl:template>

	<xsl:template match="comment()">
		<xsl:copy-of select="."/>
	</xsl:template>
	
	<xsl:template match="*">
		<xsl:apply-templates select="." mode="eg:elt-occurs"/>
	</xsl:template>
	
	<xsl:template match="eg:*"/>
	<xsl:template match="eg:attribute/@*"/>
	
	<xsl:template match="eg:attribute">
		<xsl:choose>
			<xsl:when test="@eg:occurs='+' or @eg:occurs='*'">
				<optional>
					<xsl:apply-templates select="." mode="att-elt"/>
				</optional>
			</xsl:when>
			<xsl:when test="@eg:occurs='-'">
				<ega:annotation>
					<xsl:copy-of select="."/>
				</ega:annotation>
			</xsl:when>
			<xsl:otherwise>
					<xsl:apply-templates select="." mode="eg:att-elt"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template match="eg:attribute" mode="eg:att-elt">
		<attribute name="{@name}">
			<xsl:apply-templates select="." mode="eg:elt-ref"/>
		</attribute>
	</xsl:template>
	
	<xsl:template match="*" mode="eg:elt-occurs">
		<xsl:choose>
			<xsl:when test="preceding-sibling::*[1][local-name(.)=local-name(current()) and namespace-uri(.) = namespace-uri(current()) and not(@eg:occurs)]">
				<ega:skipped>
					<xsl:copy-of select="."/>
				</ega:skipped>
			</xsl:when>
			<xsl:when test="following-sibling::*[1][local-name(.)=local-name(current()) and namespace-uri(.) = namespace-uri(current()) and not(@eg:occurs)]">
				<oneOrMore>
					<xsl:apply-templates select="." mode="eg:elt"/>
				</oneOrMore>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="." mode="eg:elt"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template match="*[@eg:occurs='+']" mode="eg:elt-occurs" priority="1">
		<oneOrMore>
			<xsl:apply-templates select="." mode="eg:elt"/>
		</oneOrMore>
	</xsl:template>
	
	<xsl:template match="*[@eg:occurs='-']" mode="eg:elt-occurs" priority="1">
		<ega:annotation>
			<xsl:copy-of select="."/>
		</ega:annotation>
	</xsl:template>
	
	<xsl:template match="*[@eg:occurs='?']" mode="eg:elt-occurs" priority="1">
		<optional>
			<xsl:apply-templates select="." mode="eg:elt"/>
		</optional>
	</xsl:template>
	
	<xsl:template match="*[@eg:occurs='*']" mode="eg:elt-occurs" priority="1">
		<zeroOrMore>
			<xsl:apply-templates select="." mode="eg:elt"/>
		</zeroOrMore>
	</xsl:template>
	
	<xsl:template match="*[@eg:occurs='.']" mode="eg:elt-occurs" priority="1">
		<xsl:apply-templates select="." mode="eg:elt"/>
	</xsl:template>
	
	<xsl:template match="*" mode="eg:elt">
		<element name="{local-name()}">
			<xsl:if test="not(namespace-uri()=namespace-uri(parent::*))">
				<xsl:attribute name="ns">
					<xsl:value-of select="namespace-uri()"/>
				</xsl:attribute>
			</xsl:if>
			<xsl:apply-templates select="comment()"/>
			<xsl:if test="@eg:assert">
				<sch:assert>
					<xsl:copy-of select="namespace::*"/>
					<xsl:attribute name="test">
						<xsl:value-of select="@eg:assert"/>
					</xsl:attribute>
				</sch:assert>
			</xsl:if>
			<xsl:apply-templates select="." mode="eg:elt-ref"/>
		</element>
	</xsl:template>

	<xsl:template match="*" mode="eg:elt-ref">
			<xsl:apply-templates select="." mode="eg:elt-model"/>
	</xsl:template>
	
	<xsl:template match="*[@eg:define]" mode="eg:elt-ref" priority="3">
		<ref name="{@eg:define}" ega:def="true"/>
	</xsl:template>
	
	<xsl:template match="*[@eg:content and not(contains(@eg:content, ':'))]" mode="eg:elt-ref" priority="2">
		<ref name="{@eg:content}">
			<ega:skipped>
				<xsl:copy-of select="."/>
			</ega:skipped>
		</ref>
	</xsl:template>
	
	<xsl:template match="*" mode="eg:elt-model">
			<xsl:apply-templates select="." mode="eg:elt-content"/>
	</xsl:template>
	
	
	<xsl:template match="*" mode="eg:def">
		<define name="{@eg:define}">
			<xsl:apply-templates select="." mode="eg:elt-model"/>
		</define>
	</xsl:template>
	
	<xsl:template match="*[@eg:content = 'eg:group']" mode="eg:elt-model" priority="1">
		<xsl:apply-templates select="." mode="eg:elt-content"/>
	</xsl:template>
	
	<xsl:template match="*[@eg:content='eg:interleave']" mode="eg:elt-model" priority="1">
		<interleave>
			<xsl:apply-templates select="." mode="eg:elt-content"/>
		</interleave>
	</xsl:template>
	
	<xsl:template match="*[@eg:content='eg:mixed']" mode="eg:elt-model" priority="1">
		<mixed>
			<xsl:apply-templates select="." mode="eg:elt-content"/>
		</mixed>
	</xsl:template>
	
	<xsl:template match="*[count(*[not(self::eg:attribute)]) > 1 and not(text())]" mode="eg:elt-model">
			<xsl:apply-templates select="." mode="eg:elt-content"/>
	</xsl:template>
	
	
	<xsl:template match="*[(*[not(self::eg:attribute)] and text())]" mode="eg:elt-model">
		<mixed>
			<xsl:apply-templates select="." mode="eg:elt-content"/>
		</mixed>
	</xsl:template>
	
	<xsl:template match="*" mode="eg:elt-content">
			<xsl:apply-templates select="@*"/>
			<xsl:apply-templates select="*|text()"/>
			<xsl:if test="not(*|text()|@*[namespace-uri() != $eg-uri])">
				<empty/>
			</xsl:if>
	</xsl:template>
	
	<xsl:template match="*[@eg:content and not(starts-with(@eg:content, 'eg:'))]" mode="eg:elt-model" priority="1">
			<xsl:apply-templates select="@*"/>
			<xsl:apply-templates select="eg:*"/>
			<xsl:if test="*[namespace-uri(.) != $eg-uri]|text()">
				<ega:skipped>
					<xsl:copy-of select="*[namespace-uri(.) != $eg-uri]|text()"/>
				</ega:skipped>
			</xsl:if>
			<xsl:apply-templates select="@eg:content" mode="eg:elt-model"/>
	</xsl:template>

	<xsl:template match="@eg:content[starts-with(., 'xsd:')]" mode="eg:elt-model" priority="2">
		<data type="{substring(., 5	)}"/>
	</xsl:template>

	<xsl:template match="@eg:content[starts-with(., 'dtd:')]" mode="eg:elt-model" priority="2">
		<data type="{substring(normalize-space(.), 5, string-length(normalize-space(.)) - 4 )}" datatypeLibrary="http://relaxng.org/ns/compatibility/datatypes/1.0"/>
	</xsl:template>


	<xsl:template match="@eg:content[contains(., ':')]" mode="eg:elt-model">
		<xsl:variable name="type" select="."/>
		<xsl:variable name="prefix" select="substring-before(., ':')"/>
		<data>
			<xsl:for-each select="../namespace::*">
			  <!--<ega:debug name="{name(.)}" prefix="{$prefix}"/>-->
				<xsl:if test="name(.) = $prefix">
					<xsl:copy-of select="."/>
					<xsl:attribute name="type">
						<xsl:value-of select="$type"/>
					</xsl:attribute>
				</xsl:if>
			</xsl:for-each>
		</data>
	</xsl:template>

	<xsl:template match="@eg:*"/>
	
	<xsl:template match="@*">
		<optional>
			<attribute name="{local-name()}">
				<xsl:if test="namespace-uri()">
					<xsl:attribute name="ns">
						<xsl:value-of select="namespace-uri()"/>
					</xsl:attribute>
				</xsl:if>
				<xsl:apply-templates select="." mode="eg:checkType"/>
			</attribute>
		</optional>
	</xsl:template>

	<xsl:template match="*[@eg:content='eg:mixed']/text()" priority="1">
		<ega:example>
			<xsl:copy-of select="."/>
		</ega:example>
	</xsl:template>

	<xsl:template match="*[@eg:content='eg:group']/text()" priority="1">
		<text>
			<ega:example>
				<xsl:copy-of select="."/>
			</ega:example>
		</text>
	</xsl:template>

	<xsl:template match="*[@eg:content='eg:interleave' and not(preceding-sibling::text())]/text()" priority="1">
		<text>
			<ega:example>
				<xsl:copy-of select="."/>
			</ega:example>
		</text>
	</xsl:template>

	<xsl:template match="text()[parent::*/*[not(self::eg:attribute)]]">
		<ega:example>
			<xsl:copy-of select="."/>
		</ega:example>
	</xsl:template>
	
	<xsl:template match="text()">
		<xsl:apply-templates select="." mode="eg:checkType"/>
	</xsl:template>
	
	<xsl:template name="eg:isDecimal">
		<xsl:param name="val" select="normalize-space(.)"/>
		<xsl:variable name="leading" select="translate(substring($val, 1, 1), '-', '+')"/>
		<xsl:variable name="no-digits" select="translate($val, '0123456789', '')"/>
		<xsl:variable name="no-digits2" select="translate($no-digits, '-', '+')"/>
		<xsl:choose>
			<xsl:when test="$no-digits = '' or ($no-digits2='+' and $leading='+')">
				<xsl:text>int</xsl:text>
			</xsl:when>
			<xsl:when test="$no-digits = '.' or ($no-digits2='+.' and $leading='+')">
				<xsl:text>decimal</xsl:text>
			</xsl:when>
			<xsl:otherwise>
				<xsl:text>no</xsl:text>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template match="@*[starts-with(normalize-space(.), '{xsd:') and contains(normalize-space(.), '}') and substring-after(normalize-space(.), '}') = '']" mode="eg:checkType" priority="1">
		<data type="{substring(normalize-space(.), 6, string-length(normalize-space(.)) - 6 )}"/>
	</xsl:template>
	
	<xsl:template match="@*[starts-with(normalize-space(.), '{dtd:') and contains(normalize-space(.), '}') and substring-after(normalize-space(.), '}') = '']" mode="eg:checkType" priority="1">
		<data type="{substring(normalize-space(.), 6, string-length(normalize-space(.)) - 6 )}" datatypeLibrary="http://relaxng.org/ns/compatibility/datatypes/1.0"/>
	</xsl:template>
	
	<xsl:template match="@*[starts-with(normalize-space(.), '{')  and contains(., '}') and substring-after(normalize-space(.), '}') = '' and contains(., ':')]" mode="eg:checkType">
		<xsl:variable name="type" select="substring(normalize-space(.), 2, string-length(normalize-space(.)) - 2)"/>
		<xsl:variable name="prefix" select="substring-before($type, ':')"/>
		<data>
			<xsl:for-each select="../namespace::*">
			  <!--<ega:debug name="{name(.)}" prefix="{$prefix}"/>-->
				<xsl:if test="name(.) = $prefix">
					<xsl:copy-of select="."/>
					<xsl:attribute name="type">
						<xsl:value-of select="$type"/>
					</xsl:attribute>
				</xsl:if>
			</xsl:for-each>
		</data>
	</xsl:template>
	
	<xsl:template match="text()[normalize-space(.)='true' or normalize-space(.)='false']|@*[normalize-space(.)='true' or normalize-space(.)='false']" mode="eg:checkType">
		<data type="boolean">
			<ega:example>
				<xsl:copy-of select="."/>
			</ega:example>
		</data>
	</xsl:template>
	
	<xsl:template match="text()|@*" mode="eg:checkType">
		<xsl:variable name="isDecimal">
			<xsl:call-template name="eg:isDecimal"/>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$isDecimal = 'int'">
				<data type="integer">
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
				</data>
			</xsl:when>
			<xsl:when test="$isDecimal = 'decimal'">
				<data type="decimal">
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
				</data>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="." mode="eg:checkDouble"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>
	
	<xsl:template match="text()|@*" mode="eg:checkDouble">
		<xsl:apply-templates select="." mode="eg:checkDate"/>
	</xsl:template>

	<xsl:template match="text()[contains(., 'E')]|@*[contains(., 'E')]" mode="eg:checkDouble">
		<xsl:variable name="n">
			<xsl:call-template name="eg:isDecimal">
				<xsl:with-param name="val" select="substring-before(., 'E')"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:variable name="m">
			<xsl:call-template name="eg:isDecimal">
				<xsl:with-param name="val" select="substring-after(., 'E')"/>
			</xsl:call-template>
		</xsl:variable>
		<xsl:choose>
			<xsl:when test="$n != 'no' and $m = 'int'">
				<data type="double">
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
				</data>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="." mode="eg:checkDate"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>


	<xsl:template match="text()|@*" name="eg:checkDate" mode="eg:checkDate">
		<xsl:param name="val" select="normalize-space(.)"/>
		<xsl:variable name="no-digits" select="translate(translate($val, '#', '!'), '0123456789', '##########')"/>
		<xsl:choose>
			<xsl:when test="$no-digits = '####-##-##' or $no-digits = '####-##-##Z' or $no-digits = '####-##-##+##:##' or $no-digits = '####-##-##-##:##'">
				<data type="date">
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
				</data>
			</xsl:when>
			<xsl:when test="$no-digits = '####-##-##T##:##:##' or $no-digits = '####-##-##T##:##:##Z' or $no-digits = '####-##-##T##:##:##+##:##' or $no-digits = '####-##-##T##:##:##-##:##'">
				<data type="dateTime">
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
				</data>
			</xsl:when>
			<xsl:when test="$no-digits = '##:##:##' or $no-digits = '##:##:##Z' or $no-digits = '##:##:##+##:##' or $no-digits = '##:##:##-##:##'">
				<data type="time">
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
				</data>
			</xsl:when>
			<xsl:otherwise>
				<xsl:apply-templates select="." mode="eg:noTypeFound"/>
			</xsl:otherwise>
		</xsl:choose>
	</xsl:template>

	<xsl:template match="@*" mode="eg:noTypeFound">
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
	</xsl:template>
	
	<xsl:template match="text()" mode="eg:noTypeFound">
				<text>
					<ega:example>
						<xsl:copy-of select="."/>
					</ega:example>
				</text>
	</xsl:template>

</xsl:stylesheet>
