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
#ifndef __gat_core_h__
#define __gat_core_h__

#include "gat_types.h"

/* define extern "C" { for C++ */
#ifdef __cplusplus
extern "C" {
#endif

void gat_init (gat *ga, gat_arch *arch);
void gat_set_callback (gat *ga, gat_callback callback, void *context);
int gat_engine (gat *ga);
void gat_cleanup (gat *ga);
void gat_init_pass (gat *ga);
void gat_print (gat *ga, const char *format, ...);
void gat_error (gat *ga, int, const char *format, ...);
void gat_warning (gat *ga, int, const char *format, ...);
void gat_fatal_error (gat *ga, int, const char *format, ...);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* !__gat_core_h__ */
