dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.52)
AC_INIT(vos, 0.12.0, vos-bugs@interreality.org)
AC_CONFIG_SRCDIR([ASElexer.l])
AM_INIT_AUTOMAKE(libASE, 1.0)
AM_CONFIG_HEADER(libASEconfig.h)
AC_LANG(C)

dnl Checks for programs.
AC_PROG_CC

AM_PROG_LEX
AC_ISC_POSIX
AC_LIBTOOL_WIN32_DLL
AM_PROG_LIBTOOL

CFLAGS=-g

AC_SUBST(CFLAGS)
AC_SUBST(DEFINES)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
