
all: xsl/iso_schematron_skeleton.xsl.h xsl/iso_simple.xsl.h

xsl/iso_schematron_skeleton.xsl.h:  xsl/iso_schematron_skeleton.xsl
	xsltproc xsl/strip.xsl $< | xmllint --noblanks - | bin2c -c -z - $@

xsl/iso_simple.xsl.h:  xsl/iso_simple.xsl
	xsltproc xsl/strip.xsl $< | xmllint --noblanks - | bin2c -c -z - $@

