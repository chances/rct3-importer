lib_LTLIBRARIES = libASE.la
bin_PROGRAMS = libASEtest

LEX = @LEX@ -b -8 -PASE_ -oASElexer.c -olex.yy.c
INCLUDES = @DEFINES@
include_HEADERS = libASE.h

libASE_la_SOURCES = ASElexer.l libASE.h loader.c symbols.h

libASEtest_SOURCES = libASEtest.c
libASEtest_LDADD = -lASE

EXTRA_DIST = libASE.vcproj visualstudio/ASElexer.c