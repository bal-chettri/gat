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
#include "gat.h"

#define MASM85_PRINT_TEXT_MESSAGE

void masm85_callback (gat *ga, gat_callback_type type, void *data, void *context) {
    (void)ga;
    (void)context;
    
    switch (type) {
#ifdef MASM85_PRINT_TEXT_MESSAGE
    case GAT_CALLBACK_TEXT:
        printf ("%s\n", (const char *)data);
        break;
#endif

    case GAT_CALLBACK_ERROR: 
      {
        gat_error_info *err = (gat_error_info *)data;
        if (err->fatal) {           
            printf ("\"%s\": fatal error %d: line %d -> %s\n", err->file, err->errno, 
                    err->line, err->desc);
        } else {
            printf ("\"%s\": error %d: line %d -> %s\n", err->file, err->errno, 
                    err->line, err->desc);
        }
      }
      break;
    }
}
