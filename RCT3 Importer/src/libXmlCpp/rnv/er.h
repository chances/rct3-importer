/* $Id: er.h 315 2004-01-20 00:22:29Z dvd $ */

#ifndef ER_H
#define ER_H 1

#include <stdarg.h>

extern int (*er_printf)(const char *format,...);
extern int (*er_vprintf)(const char *format,va_list ap);

extern int er_default_printf(const char *format,...);
extern int er_default_vprintf(const char *format,va_list ap);

#endif
