Komodo Edit Support Files
=========================
Belgabor
v1.0, Nov 2008

NOTE: If you're looking at the plain text version of this file, you may notice a few strange characters
or punctiation in the text. These allow automatic generation of a formatted HTML version. The engine used
for this (http://www.methods.co.nz/asciidoc/index.html[asciidoc]) allows to do that with the least impact
on readability in plain text. In short, just ignore the strange stuff :)


Introduction
------------

This directory contains a couple of files that you can use to conveniently edit RCT3-related xml files
in Komodo Edit. Komodo Edit is a free open-sourced stripped-down version of ActiveState's Komodo IDE
and is available here: http://www.activestate.com/Products/komodo_ide/komodo_edit.mhtml

It has a lot of nice features, one of them being code-completion based on xml RelaxNG-schemata. It can
also add code-snippets or even whole file templates and lead you through filling them in. Finally it
allows you to run external commands and view their output. Unforutantely it cannot validate xml, it
only does a basic syntax check. But for most RCT3-related xml files, ovlmake can serve as validator.


Prerequisites
-------------

- Installed Komodo Edit 5 (may work with earlier versions, but this is untested)
- ovlmake v0.5. The executable needs to be in your execution path (refer to windows tutorials about
  how to do that)
- The RelaxNG-schemata and catalog file in this pack.


Installation
------------

- Open the Komodo Edit preferences (Edit -> Preferences...), open the Languages -> XML Catalogs page
  and add the location of the `rct3-catalog.xml` file in this pack to the list. This gives you xml tag
  code-completion for RCT3-related xml files. You may need to restart Komodo Edit after this step to make
  it actually work. If you get a new release of this pack, you do not have to repeat this step provided
  you extract the new version to the same place (although a restart of Komodo might be necessary).
- Open the enclosed `flatride-template.xml` file in Komodo Edit and save it as a template (File -> Save As
  Template...). Just confirm the filename. It will now be available as a template (with tab guidance) in
  _My Templates_ when you create a new file. Since this is separately saved, you need to repeat this step
  (and overwrite the old one) when you get an updated version. Of course you can also make your own changes
  to it.
- Select Toolbox -> Import Package... and select the `RCT3.kpz`-file in this directory. Komodo Edit will now
  ask you for a directory. It doesn't matter what you choose, but Komodo Edit will create an empty folder
  called `RCT3` in there which you can safely delete afterwards (after making sure it wasn't there before and
  contains some unrelated files of course!). This adds RCT3-related abbreviations and commands to the Komodo
  Edit Toolbox (You can usually toggle display of the toolbox with the _Show/Hide Right Pane_ icon in Komodo's
  toolbar).


Usage
-----

The abbreviations are used like normal Komodo abbreviations (refer to Komodo's help) and are defined for xml
files.

There are also three different commands running ovlmake to validate (equivalent to `ovlmake --check file`),
make (create, equivalent to `ovlmake file`) and install (create in the proper RCT3-Folder, equivalent to `ovlmake
--install file`) your current file. The comands parse ovlmake's output, which since version 0.5 tries to give
as much information about errors or warnings as possible, i.e. when applicable with file and line number.
Komodo can jump to this if you click on the respective output line, but keep in mind that ovlmake uses zero as
a line number for general messages or when line number information is not applicable/available.
