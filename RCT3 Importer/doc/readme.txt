Documentation is in docbook format.

A good editor can be found at http://www.xmlmind.com/products.html
They also have a FO Converter that can convert docbook to several formats
To get the html version I uploaded you need to define the following properties for the DocBook to HTML (one page) transformation:

toc.section.depth 3
section.autolabel 1
html.stylesheet docbook.css

This directory contains a preliminary css for it. Also the images subdir contains QBullets by Matterform Media (see readme.htm).