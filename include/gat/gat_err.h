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
#ifndef __gat_err_h__
#define __gat_err_h__

/* gat error class */
#define GAT_ERR_BASE_SYNTAX         1           /* 1 - 100 */
#define GAT_ERR_BASE_SEMANTIC       50          /* 101 - 200 */
#define GAT_ERR_BASE_FATAL          100         /* 201 - 300 */
#define GAT_ERR_BASE_WARNING        150         /* 301 - 400 */

typedef enum _gat_error_code {
    /* syntax errors */
    GAT_ERR_8BIT_REG_EXPECTED = GAT_ERR_BASE_SYNTAX,
    GAT_ERR_16BIT_REG_EXPECTED,
    GAT_ERR_32BIT_REG_EXPECTED,
    GAT_ERR_64BIT_REG_EXPECTED,
    GAT_ERR_SYMBOL_EXPECTED,
    GAT_ERR_BYTE_EXPECTED,
    GAT_ERR_WORD_EXPECTED,
    GAT_ERR_DWORD_EXPECTED,
    GAT_ERR_CONST_EXPECTED,
    GAT_ERR_INVALID_OPERANDS,
    GAT_ERR_TOO_MANY_TOKENS,

    /* semantic errors */
    GAT_ERR_INVALID_ID = GAT_ERR_BASE_SEMANTIC,
    GAT_ERR_RESERVED_NAME,
    GAT_ERR_REDEFINED,
    GAT_ERR_TOO_MANY_IDS,
    GAT_ERR_UNDEFINED_ID,   
    GAT_ERR_TYPE_MISMATCH,
    GAT_ERR_INVALID_INSTRUCTION,
    GAT_ERR_INVALID_CONVERSION,
    
    /* fatal errors */  
    GAT_ERR_OFFSET_OUT_OF_RANGE = GAT_ERR_BASE_FATAL,
    GAT_ERR_OUT_OF_MEMORY,
    GAT_ERR_FILE_OPEN,
    GAT_ERR_FILEIO_FAILED,
    GAT_ERR_INVALID_INPUT_PATH,
    GAT_ERR_INVALID_OUTPUT_PATH,

    /* warnings */
    GAT_WARN_DATA_TRUNCATATION = GAT_ERR_BASE_WARNING
}gat_error_code;

#endif /* !__gat_err_h__ */
