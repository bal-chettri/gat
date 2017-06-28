/*
 * Copyright 2017, Bal Chettri
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT 
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __gat_str_h__
#define __gat_str_h__

#include "gat_types.h"
#include <string.h>
#include <stdarg.h> /* va_list */

/* disable warnings */
#ifdef WIN32
#pragma warning (disable:4996) /* deprecated */
#endif

/* define extern "C" { for C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* gat string functions */
#if (!defined(WIN32) && !defined(DOS))
 int strcmpi (const char *, const char *);
#endif
#define gat_strcmp(_s1,_s2) (!strcmp(_s1,_s2))
#define gat_strcmpi(_s1,_s2) (!strcmpi(_s1,_s2))
int gat_rngcmp (char, const char *);
char *gat_unquote (const char *, char *buff);
char *gat_trim (const char *, char *);
char *gat_format_string (const char *format, va_list arg_list);

/* gat commandline parser functions */
uint32_t gat_cmdln_scan_switches (gat_cmdline *pcmd, const char *arr_switches[], 
                                uint32_t switch_masks[], int num_switches);
char *gat_cmdln_get_param (gat_cmdline *pcmd, const char *str_switch, char *buff);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* !__gat_str_h__ */
