
all: xsl/iso_schematron_skeleton.xsl.h xsl/iso_simple.xsl.h

xsl/iso_schematron_skeleton.xsl.h:  xsl/iso_schematron_skeleton.xsl ../../xml/xsl/strip.xsl
	xsltproc ../../xml/xsl/strip.xsl $< | xmllint --noblanks - | ../../../Utilities/bin2c/bin2c -c -z - $@

xsl/iso_simple.xsl.h:  xsl/iso_simple.xsl ../../xml/xsl/strip.xsl
	xsltproc ../../xml/xsl/strip.xsl $< | xmllint --noblanks - | ../../../Utilities/bin2c/bin2c -c -z - $@

