space :=  
space +=  
svn_rev := $(shell svn info --xml ../xml/rng | xmllint - | grep "<commit" | sed 's:.*"\(.*\)".*:\1:g')
schemata_name := rct3-schemata-svn$(svn_rev)
schemata_dir := pub/$(schemata_name)
importer_name := $(shell grep DIST_DIR ../src/RCT3\ Tool/version_importer.h | sed 's:.*DIST_DIR.*"\(.*\)".*//.*:\1:g')
importer_dir := pub/$(importer_name)
importer_bin := $(importer_dir)/RCT3\ Importer.exe $(importer_dir)/libOVL.dll
importer_doc := readme.txt readme.html COPYING changelog.txt credits.txt install.txt
blender_scripts_name := rct3-blender-scripts-svn$(svn_rev)
blender_scripts_dir := pub/$(blender_scripts_name)
ovlmake_name := $(shell grep DIST_DIR ../src/ovlmake/version_ovlmake.h | sed 's:.*DIST_DIR.*"\(.*\)".*//.*:\1:g')
ovlmake_dir := pub/$(ovlmake_name)
ovlmake_doc = ReadMe$(space)1st.txt ReadMe$(space)1st.html OvlCompiler$(space)Readme.txt License.txt changelog.txt

generated_docs := ../src/RCT3\ Tool/readme.html ../src/ovlmake/ReadMe\ 1st.html ../xml/readme.html scripts/Blender/readme.html komodo/readme.html ../doc/flatrides/flatrides.pdf ../doc/new_in_v18/notes_v18.pdf

UPX := upx --lzma
P7Z := 7z

.PHONY: all clean prepare doc

all: prepare schemata importer blender-scripts ovlmake

%.html: %.txt
	asciidoc -a numbered -a toc "$<"
	
%.pdf: %.xml ../doc/rct3dblatex/rct3imp.specs ../doc/new_in_v18/general.xml
	docbook2latex -V -t pdf -b pdftex -c ../doc/rct3dblatex/rct3imp.specs -i ../doc/rct3dblatex $<

doc: generated_docs

pub/%.zip:
	sh -c "cd pub && $(P7Z) a -r -tzip $*.zip $* && cd .."

prepare:
	mkdir -p pub
	mkdir -p pub/rct3-flatride-tutorial-svn$(svn_rev)
	mkdir -p pub/rct3-flatride-tutorial-svn$(svn_rev)/lists
	cp ../doc/flatrides/lists/*.pdf pub/rct3-flatride-tutorial-svn$(svn_rev)/lists
	mkdir -p pub/rct3-utility-notes-svn$(svn_rev)
	
schemata-prepare: prepare
	mkdir -p $(schemata_dir)
	mkdir -p $(schemata_dir)/komodo
	
schemata-copy: schemata-prepare ../xml/rng/readme.html komodo/readme.html
	cp ../xml/rng/rct3-catalog.xml $(schemata_dir)
	cp ../xml/rng/readme.txt $(schemata_dir)	
	cp ../xml/rng/readme.html $(schemata_dir)	
	cp komodo/* $(schemata_dir)/komodo

schemata-create: schemata-prepare schemata-copy ${schemata_dir}/rct3xml-ovlcompiler-v1.rng ${schemata_dir}/rct3xml-raw-v1.rng ${schemata_dir}/rct3xml-scenery-v1.rng ${schemata_dir}/model.rng

schemata: schemata-create $(schemata_dir).zip
	
${schemata_dir}/%.rng: ../xml/rng/%.rngi ../xml/xsl/stripstrip.xsl
	xsltproc --novalid ../xml/xsl/stripstrip.xsl $< > $@
	
importer-prepare: prepare
	mkdir -p $(importer_dir)
	
importer-copy: importer-prepare
	cp ../bin/*.mgk $(importer_dir)
	cp RCT3\ Recolorable\ Texture\ Info.zip $(importer_dir)
	cp $(foreach df,$(importer_doc),../src/RCT3\ Tool/$(df)) $(importer_dir)
	
${importer_dir}/RCT3\ Importer.exe: ../bin/RCT3_Importer.exe
	cp $< "$@"
	strip "$@"
	$(UPX) "$@"

${importer_dir}/libOVL.dll: ../bin/libOVL.dll
	cp $< $@
	strip "$@"
	$(UPX) $@

importer-create: importer-prepare importer-copy $(importer_bin)

importer: importer-create $(importer_dir).zip

blender-scripts-prepare: prepare
	mkdir -p $(blender_scripts_dir)

blender-scripts-copy: blender-scripts-prepare scripts/Blender/readme.html
	cp scripts/Blender/* $(blender_scripts_dir)
	
blender-scripts-create: blender-scripts-prepare blender-scripts-copy

blender-scripts: blender-scripts-create $(blender_scripts_dir).zip

ovlmake-prepare: prepare
	mkdir -p $(ovlmake_dir)
	mkdir -p $(ovlmake_dir)/doc
	mkdir -p $(ovlmake_dir)/doc/Italian
	
ovlmake-copy: ovlmake-prepare
	cp ../bin/*.mgk $(ovlmake_dir)
	cp $(foreach df,$(ovlmake_doc),../src/ovlmake/$(df)) $(ovlmake_dir)
	cp ../src/ovlmake/*.txt $(ovlmake_dir)/doc
	cp ../src/ovlmake/Italian/*.txt $(ovlmake_dir)/doc/Italian
	
${ovlmake_dir}/ovlmake.exe: ../bin/ovlmake.exe
	cp $< "$@"
	strip "$@"
	$(UPX) "$@"
	
ovlmake-create: ovlmake-prepare ovlmake-copy ${ovlmake_dir}/ovlmake.exe

ovlmake: ovlmake-create $(ovlmake_dir).zip


clean:
	rm -rf pub

