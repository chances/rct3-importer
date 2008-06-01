<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
								xmlns:rng="http://relaxng.org/ns/structure/1.0"
                version="1.0">

  <xsl:output method="xml" indent="yes"/>

  <xsl:preserve-space elements="*"/>

  <xsl:template match="rng:*">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:apply-templates select="rng:*|text()"/>
    </xsl:copy>
  </xsl:template>


  <xsl:template match="text()">
    <xsl:copy/>
  </xsl:template>

</xsl:stylesheet>
