.PHONY: all clean

all: rct3xml-raw-v1.rnc.bz2.h

rct3xml-raw-v1.rnc.bz2.h: rct3xml-raw-v1.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - | bzip2 -c > temp.bz2
	bin2c -c -z temp.bz2 $@
	touch -r $< $@
	rm temp.bz2

#rct3xml-raw-v1.rnc.bz2: rct3xml-raw-v1.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
#	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - | bzip2 > $@

clean:
	rm *.h *.bz2

