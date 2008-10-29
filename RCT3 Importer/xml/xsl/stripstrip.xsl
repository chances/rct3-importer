<?xml version="1.0" ?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:strip="http://strip.xml/strip"
    >

    <xsl:template match="*[@strip:strip]" />

    <!-- IdentityTransform -->
    <xsl:template match="/ | @* | node()">
	<xsl:copy>
	    <xsl:apply-templates select="@* | node()" />
	</xsl:copy>
    </xsl:template>

</xsl:stylesheet>
