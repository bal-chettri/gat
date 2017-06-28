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
/**
 * gat_lex.cpp  gat lexical routines. lexer is silent.
 */
#include "gat_lexer.h"
#include "gat_str.h"
#include "gat_conv.h"
#include "gat_core.h"
#include "gat_tokenizer.h"
#include <ctype.h>
#include <malloc.h>

/* checks if token is a number */
int gat_is_num (const char *strtext) {
    const size_t token_length = strlen(strtext);
    if ( token_length >= 1 && token_length <= GAT_MAX_NUMERIC_TOKEN_LENGTH ) {
        size_t i, j, k;
        int bhex;
        uint8_t ch;

        /* check for nubmer system */
        if ( strtext[0] == '0' && tolower(strtext[1]) == 'x' ) {
            i = 2; 
            j = token_length - 1;
            bhex = 1;
        } else if ( tolower (strtext[token_length - 1]) == 'h' ) {
            i = 0;
            j = token_length - 2;
            bhex = 1;
        } else {
            i = 0;
            j = token_length - 1;
            bhex = 0;
        }

        /* do the conversion now */
        if (bhex == 1) {
            if ( tolower(strtext[token_length - 1])  == 'h' ) {
                ch = tolower (strtext [i++] ); 
                if (ch >= 'a' && ch <= 'f') {
                    return 0; /* NAN */
                }
            }
            for ( k = i; k <= j; k++ ) {
                if ( !isxdigit ( strtext [k] ) ) {
                    return 0; /* NAN */
                }
            }
        } else {
            for ( k = i; k <= j; k++ ) {
                if ( !isdigit ( strtext [k] ) ) {
                    return 0; /* NAN */
                }
            }
        }
        return 1; /* its a number */
    }
    return 0; /* NAN */
}

/* checks if token is an id */
int gat_is_id ( const char *strtext ) {
    const size_t token_length = strlen(strtext);
    if ( token_length > 0 ) {
        unsigned i;
        /* id can contain only characters [_a-zA-Z0-9] */
        for ( i = 0; i < token_length; i++ ) {
            if ( !gat_rngcmp ( strtext[i], "__azAZ09" ) )
                return 0; /* not an id */
        }
        /* id can't start with a numeric digit */
        if ( isdigit ( strtext [0] ) ) {
            return 0; /* not an id */
        }
        /* id can't have more than 2 continuous _ symbol */
        if ( strstr ( strtext, "___" ) ) {
            return 0; /* not an id */
        }
        for ( i = 0; i < token_length; i++ ) {
            if ( strtext [i] != '_') {
                return 1; /* its an id */
            }
        }
    }
    return 0; /* not an id */
}

/* checks if token is a label */
int gat_is_label (const char *strtext ) {
    const size_t token_length = strlen(strtext);
    /* label name must contain at least 2 characters <id> and <:> */
    if ( token_length >= 2 ) {
        /* label name must end with a colon <:> */
        if ( strtext[token_length - 1] == ':' ) {        
            /* label name must be a valid id name */
            char label[GAT_MAX_TOKEN_LEN + 1];
            strncpy (label, strtext, token_length - 1);
            if ( gat_is_id ( label ) ) {
                return 1; /* its a label */
            }
        }
    }
    return 0; /* not a label */
}

/* checks if token is a byte value (0xFF) */
int gat_is_byte ( const char * strtext ) {
    if ( gat_is_num (strtext) ) {
        uint32_t value = gat_cnum ( strtext );
        if ( value <= GAT_MAX_BYTE ) {
            return 1; /* its a byte value */
        }
    }
    return 0; /* not a byte value */
}

/* checks if token is a double byte value (0xFFFF) */
int gat_is_dbl ( const char *strtext ) {
    if ( gat_is_num (strtext) ) {
          uint32_t value = gat_cnum ( strtext );
          if ( value <= GAT_MAX_DBL ) {
            return 1; /* its a dbl byte value */
          }
    }
    return 0; /* not a dbl value */
}
