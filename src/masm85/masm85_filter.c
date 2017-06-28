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
/** masm85_filter.cpp  MASM85 filter routines. */
#include "gat.h"

/* returns 1 if instruction is allowed otherwise 0 */
int masm85_filter (gat *ga) {
    #define CHECK(_I,_S) \
    (gat_strcmpi(ga->g_arr_tokens[(_I)],(_S)))
    
    int invalid = 0;

    /* check for mov m, m */
    if ( CHECK(0,"mov") ) {
        invalid = CHECK(1,"m") && CHECK(3,"m");
    }
    else if ( CHECK(0,"push") || CHECK(0,"pop") ) {
        invalid = CHECK(1,"sp");
    }
    else if ( CHECK(0,"ldax") || CHECK(0,"stax") ) {
        invalid = CHECK(1,"h") || CHECK(1,"psw") || CHECK(1,"sp");
    }
    else if ( CHECK(0,"dad") || CHECK(0,"inx") || 
              CHECK(0,"dcx") || CHECK(0,"lxi") ) {
        invalid = CHECK(1,"psw");
    }
    else if ( CHECK(0,"rst") ) {
        invalid = (gat_cbyte(ga->g_arr_tokens[1]) > 7);
    }

    return !invalid;
    #undef CHECK
}
