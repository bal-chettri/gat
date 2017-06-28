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
#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include "gat_types.h"

#include <string.h>
#ifdef WIN32
#pragma warning (disable:4996) /* deprecated */
#endif

#define gat_is_white(_c) (strchr(GAT_WHITE,_c)?1:0)
#define gat_is_delim(_c) (strchr(GAT_SYMBOLS,(_c))?1:0)
#define gat_is_symbol(_c) (strchr(GAT_SYMBOLS,(_c))?1:0)

int gat_is_whitespace_token (const gat_token *token);

unsigned gat_tokenize_line (gat *ga);
size_t gat_tokenize (gat *ga);
void gat_free_tokens (gat *ga);

#if _DEBUG
void gat_dump_token (const gat_token *token);
void gat_dump_tokens (const gat_token *tokens, size_t length);
#endif

#endif /* __TOKENIZER_H__ */
