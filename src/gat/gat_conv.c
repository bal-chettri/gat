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
/** gat_conv.cpp  gat data conversion routines. */
#include "gat_conv.h"
#include "gat_str.h" /* C4996 warning disable */
#include <ctype.h>

/* converts hex token to integer value */
uint32_t gat_hex_to_num (const char *strtext) {
    const char *ptr;
    uint32_t value = 0;
    uint32_t place = 1;

    for (ptr = strtext + strlen(strtext) - 1; ptr >= strtext; ptr--) {
        if ( isdigit (*ptr) ) {
            value+= place * (*ptr - '0');
        } else if ( isxdigit ( *ptr) ) {
            value+= place * (tolower(*ptr) - 'a' + 10);
        } else {
            break;
        }
        place*= 16;
    }
    return value;
}

/* converts dex token to integer value */
uint32_t gat_dec_to_num (const char *strtext) {
    const char *ptr;
    uint32_t value = 0;
    uint32_t place = 1;
    for (ptr = strtext + strlen(strtext)-1;  ptr >= strtext; ptr--) {
        if ( isdigit ( *ptr ) ) {
            value+= place * (*ptr - '0');
        } else {
            break;
        } 
        place*= 10;
    }
    return value;
}

/* converts string token to number */
uint32_t gat_cnum (const char *strtext) {
    char buff[GAT_MAX_TOKEN_LEN + 1];
    uint32_t num;
    char *ptr = (char *)strtext;
    size_t len = strlen(strtext);
    uint8_t ishex = 0;

    /* check for a hex notations: 0xFF, FFh */
    if ( strtext[0] == '0' && tolower(strtext[1]) == 'x' ) {        
        ishex = 1;
        ptr+= 2;
        len-= 2;
    } else if ( tolower(strtext[len-1]) == 'h' ) {
        ishex = 1;
        --len;
    }

    /* convert to number */
    if (ishex) {
        strncpy (buff, ptr, len);
        buff[len] = '\0';
        num = gat_hex_to_num ( buff );
    } else {
        num = gat_dec_to_num ( ptr );
    }

    return num;
}

/* converts byte value to hex string */
char *gat_chex8 (uint8_t value, char *buff) {
    const char *format = (value < 16 ? "0%X" : "%X");
    sprintf (buff, format, value);
    return buff;
}

/* converts word value to hex string */
char *gat_chex16 (uint16_t value, char *buff) {
    const char *format;
    if (value < 16) {
        format = "000%X";
    } else if (value < 256) {
        format = "00%X";
    } else if (value < 4096) {
        format = "0%X";
    } else {
        format = "%X";
    }
    sprintf (buff, format, value);
    return buff;
}
