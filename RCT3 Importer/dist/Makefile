schemata_dir := pub/schemata-r$(shell svn info --xml ../xml/rng | xmllint - | grep "<commit" | sed 's:.*"\(.*\)".*:\1:g')
importer_dir := pub/$(shell grep DIST_DIR ../src/RCT3\ Tool/version_importer.h | sed 's:.*DIST_DIR.*"\(.*\)".*//.*:\1:g')
importer_bin := $(importer_dir)/RCT3\ Importer.exe $(importer_dir)/libOVL.dll
importer_doc := rct3imp.txt COPYING changelog.txt credits.txt install.txt

UPX := upx --lzma

.PHONY: all clean prepare

all: prepare schemata importer

prepare:
	mkdir -p pub
	
schemata-prepare: prepare
	mkdir -p $(schemata_dir)

schemata: schemata-prepare ${schemata_dir}/rct3xml-ovlcompiler-v1.rng ${schemata_dir}/rct3xml-raw-v1.rng ${schemata_dir}/rct3xml-scenery-v1.rng ${schemata_dir}/model.rng
	cp ../xml/rng/rct3-catalog.xml $(schemata_dir)
	
${schemata_dir}/%.rng: ../xml/rng/%.rngi ../xml/xsl/stripstrip.xsl
	xsltproc --novalid ../xml/xsl/stripstrip.xsl $< > $@
	
importer-prepare:
	mkdir -p $(importer_dir)
	
${importer_dir}/RCT3\ Importer.exe: ../bin/RCT3_Importer.exe
	cp $< "$@"
	strip "$@"
	$(UPX) "$@"

${importer_dir}/libOVL.dll: ../bin/libOVL.dll
	cp $< $@
	strip "$@"
	$(UPX) $@

importer: importer-prepare $(importer_bin)
	cp ../bin/*.mgk $(importer_dir)
	cp RCT3\ Recolorable\ Texture\ Info.zip $(importer_dir)
	cp $(foreach df,$(importer_doc),../src/RCT3\ Tool/$(df)) $(importer_dir)
	
clean:
	rm -rf pub

