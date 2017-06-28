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
#ifndef __gat_parser_h__
#define __gat_parser_h__

#include "gat_types.h"

/* define extern "C" { for C++ */
#ifdef __cplusplus
extern "C" {
#endif

int gat_test_byte (gat *ga, const char *token);
int gat_test_dbl (gat *ga, const char *token);
int gat_test_token (gat *ga, const char *token, int optype);

int gat_parse_byte (gat *ga, const char *token, uint8_t *value);
int gat_parse_dbl (gat *ga, const char *token, uint16_t *value);
int gat_parse_token (gat *ga, const char *token, int optype, void *value);

int gat_parse_org (gat *ga);
int gat_parse_equ (gat *ga);
int gat_parse_label (gat *ga);

void gat_scan_directives (gat *ga, int *dirt, int *end);
void gat_parse_directives (gat *ga, int *dirt, int *end);

int gat_scan_instruction (gat *ga, const gat_instr *instr);
int gat_assemble_instruction (gat *ga, const gat_instr *instr);
int gat_scan (gat *ga);
int gat_assemble (gat *ga);
void gat_emit (gat *ga);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* !__gat_parser_h__ */
