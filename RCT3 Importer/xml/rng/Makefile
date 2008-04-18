.PHONY: all clean Precompile cleanPrecompile

Precompile: all
cleanPrecompile: clean
all: rct3xml-raw-v1.rnc.gz.h relaxng.rnc.gz.h ms3d_comment.rnc.gz.h model.rnc.gz.h model.sch.gz.h

rct3xml-raw-v1.rng: rct3xml-shared-v1.rng rct3xml-installer-shared-v1.rng

%.rnc.gz.h: %.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - > temp.rnc
	bin2c -c -z -g 9 -n $< temp.rnc $@
	touch -r $< $@
	rm temp.rnc

%.rnc: %.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - > $@

%.rng: %.srng ../xsl/ShortRNG/ShortRNG.xsl
	xsltproc --novalid ../xsl/ShortRNG/ShortRNG.xsl $< > $@

%.sch: %.srng ../xsl/RNG2Schtrn.xsl
	xsltproc --novalid ../xsl/RNG2Schtrn.xsl $< > $@

%.sch.gz.h: %.sch
	bin2c -c -z -g 9 $< $@

#rct3xml-raw-v1.rnc.bz2.h: rct3xml-raw-v1.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
#	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - | bzip2 -c > temp.bz2
#	bin2c -c -z temp.bz2 $@
#	touch -r $< $@
#	rm temp.bz2

#rct3xml-raw-v1.rnc.bz2: rct3xml-raw-v1.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
#	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - | bzip2 > $@

clean:
	rm *.gz.h *.gz

