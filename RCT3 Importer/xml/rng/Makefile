.PHONY: all clean Precompile cleanPrecompile

Precompile: all
cleanPrecompile: clean
rct3xml-raw-v1-all: rct3xml-raw-v1.rnc.gz.h rct3xml-raw-v1.sch.gz.h rct3xml-raw-v1.rngi.gz.h 
rct3xml-ovlcompiler-v1-all: rct3xml-ovlcompiler-v1.rnc.gz.h rct3xml-ovlcompiler-v1.rngi.gz.h
rct3xml-scenery-v1-all: rct3xml-scenery-v1.rnc.gz.h rct3xml-scenery-v1.rngi.gz.h
ms3d_comment-all: ms3d_comment.rnc.gz.h ms3d_comment.rngi.gz.h
model-all: model.rnc.gz.h model.sch.gz.h model.rngi.gz.h
relaxng-all: relaxng.rnc.gz.h
all: rct3xml-raw-v1-all rct3xml-ovlcompiler-v1-all rct3xml-scenery-v1-all ms3d_comment-all model-all relaxng-all

rct3xml-raw-v1.rng: rct3xml-shared-v1.rng rct3xml-installer-shared-v1.rng

#%.rnc.gz.h: %.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
#	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - > temp.rnc
#	bin2c -c -z -g 9 -n $< temp.rnc $@
#	touch -r $< $@
#	rm temp.rnc

%.rnc: %.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/filterrng.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/filterrng.xsl - | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - > $@
	#touch -r $< $@

%.rngi: %.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl
	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - > $@
	#touch -r $< $@

%.rng: %.srng ../xsl/ShortRNG/ShortRNG.xsl
	xsltproc --novalid ../xsl/ShortRNG/ShortRNG.xsl $< > $@
	#touch -r $< $@

%.sch: %.srng ../xsl/RNG2Schtrn.xsl
	xsltproc --novalid ../xsl/RNG2Schtrn.xsl $< > $@
	#touch -r $< $@

%.gz.h: %
	bin2c -c -z -g 9 $< $@
	#touch -r $< $@

#%.sch.gz.h: %.sch
#	bin2c -c -z -g 9 $< $@

#rct3xml-raw-v1.rnc.bz2.h: rct3xml-raw-v1.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
#	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - | bzip2 -c > temp.bz2
#	bin2c -c -z temp.bz2 $@
#	touch -r $< $@
#	rm temp.bz2

#rct3xml-raw-v1.rnc.bz2: rct3xml-raw-v1.rng ../xsl/rng-incelim-1.2/incelim.xsl ../xsl/strip.xsl ../xsl/RngToRnc-1_4/RngToRncClassic.xsl
#	xsltproc --novalid ../xsl/rng-incelim-1.2/incelim.xsl $< | xsltproc --novalid ../xsl/strip.xsl - | xsltproc --novalid ../xsl/RngToRnc-1_4/RngToRncClassic.xsl - | bzip2 > $@

clean:
	rm -f *.gz.h
	rm -f *.gz
	rm -f *.rnc 
	rm -f *.rng
	rm -f *.sch

