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
#ifndef __gat_conv_h__
#define __gat_conv_h__

#include "gat_types.h"

/* define extern "C" { for C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* conversion functions */
uint32_t gat_hex_to_num (const char *);
uint32_t gat_dec_to_num (const char *);
uint32_t gat_cnum (const char *);
#define gat_cbyte(_s) ((uint8_t)(gat_cnum(_s)&0x000000FF))
#define gat_cdbl(_s) ((uint16_t)(gat_cnum(_s)&0x0000FFFF))
char *gat_chex8 (uint8_t, char *buff);
char *gat_chex16 (uint16_t, char *buff);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* !__gat_conv_h__ */
