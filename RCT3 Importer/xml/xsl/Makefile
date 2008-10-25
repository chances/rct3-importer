.PHONY: all clean Precompile cleanPrecompile

Precompile: all
cleanPrecompile: clean
sheets-incelim: rng-incelim-1.2/incelim.xsl.gz.h rng-incelim-1.2/inc.xsl.gz.h rng-incelim-1.2/elim.xsl.gz.h rng-incelim-1.2/strip.xsl.gz.h rng-incelim-1.2/clean.xsl.gz.h
sheets-rngtornc: RngToRnc-1_4/RngToRncClassic.xsl.gz.h
sheets-shortrng: ShortRNG/ShortRNG.xsl.gz.h
sheets-schematron: schematron/iso_simple.xsl.gz.h schematron/iso_schematron_skeleton.xsl.gz.h
sheets-other: RNG2Schtrn.xsl.gz.h examplotron.xsl.gz.h
all: sheets-incelim sheets-rngtornc sheets-shortrng sheets-schematron sheets-other

%.xsl.gz.h: %.xsl strip.xsl
	xsltproc --novalid strip.xsl $< > $<.temp
	bin2c -c -z -g 9 -n $< $<.temp $@
	#touch -r $< $@
	rm $<.temp

clean:
	rm -f *.gz.h 
	rm -f rng-incelim-1.2/*.gz.h 
	rm -f RngToRnc-1_4/*.gz.h
	rm -f ShortRNG/*.gz.h
	rm -f schematron/*.gz.h
