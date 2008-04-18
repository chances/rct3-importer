.PHONY: all clean Precompile cleanPrecompile

Precompile: all
cleanPrecompile: clean
all: rng-incelim-1.2/incelim.xsl.gz.h rng-incelim-1.2/inc.xsl.gz.h rng-incelim-1.2/elim.xsl.gz.h rng-incelim-1.2/strip.xsl.gz.h rng-incelim-1.2/clean.xsl.gz.h RngToRnc-1_4/RngToRncClassic.xsl.gz.h ShortRNG/ShortRNG.xsl.gz.h RNG2Schtrn.xsl.gz.h examplotron.xsl.gz.h

%.xsl.gz.h: %.xsl strip.xsl
	xsltproc --novalid strip.xsl $< > temp.xsl
	bin2c -c -z -g 9 -n $< temp.xsl $@
	touch -r $< $@
	rm temp.xsl

clean:
	rm *.gz.h

