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
 * masmstr.cpp mams85 string functions.
 */
#include "gat_str.h"
#include "gat_lexer.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#if defined(__MACH__) || defined(__APPLE__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

/* define strcmpi for non microsoft and dos compilers */
#if !defined(WIN32) && !defined(DOS)
int strcmpi (const char *s1, const char *s2) {
    register const char *p1, *p2;
    for (p1 = s1, p2 = s2; *p1 && *p2; ++p1, ++p2) {
        if (tolower(*p1) != tolower(*p2)) {
            return *p1 - *p2;
        }
    }
    return 0;
}
#endif /* !defined(WIN32) & !defined(DOS) */

/* checks if a character is within array of character ranges */
int gat_rngcmp ( char ch , const char *strrng ) {
    unsigned i;
    char rng[2];
    for (i = 0 ; i < strlen(strrng) / 2; i++) {
        rng[0] = strrng [i * 2];
        rng[1] = strrng [i * 2 + 1];
        if ( ch >= rng[0] && ch <= rng[1] ) {
            return 1;
        }
    }
    return 0;
}

/* unquotes a string token */
char *gat_unquote ( const char *strtext, char *buff ) {
    char *ptr = (char *)strtext;
    while ( *ptr == '\'' || *ptr == '\"' ) {
        ptr++;
    }
    strcpy (buff, ptr);

    ptr = buff + strlen(buff) - 1;
    while ( ptr >= buff &&  (*ptr == '\'' || *ptr == '\"') ) {
        ptr--;
    }
    *(++ptr) = '\0';
    
    return buff;
}

/* trims string both sides */
char *gat_trim ( const char *strinput, char *buff ) {   
    char *ptr = (char *)strinput;

    /* trim left */
    while ( *ptr && gat_is_white (*ptr) ) {
        ptr++;
    }

    /* copy string into buffer */
    strcpy (buff, ptr);

    /* strip off comments */
    ptr = strchr (buff, GAT_COMMENT_CHAR);
    if (ptr) {
        *ptr = '\0';
    } else {
        ptr = buff + strlen(buff) - 1;
    }

    /* trim right */
    while (ptr >= buff && gat_is_white(*ptr)) {
        --ptr;
    }

    /* null terminate */
    *(++ptr) = '\0';
    return buff;
}

/* returns formatted string from variable argument list. The string must 
   be freed when done. */
char *gat_format_string (const char *format, va_list arg_list) {
    size_t size_buff;
    char *buff;
    int num_chars_copied;
    
    size_buff = 256;
    buff = (char *)malloc(size_buff * sizeof(char));

    do {
        num_chars_copied = vsnprintf (buff, size_buff, format, arg_list);
        if (num_chars_copied == -1) {
            size_buff*= 2;
            buff = (char *)realloc (buff, size_buff * sizeof(char));
            if (buff == NULL) {
                return NULL;
            }
        }
    } while (num_chars_copied == -1);

    return buff;
}

/* scan for commandline switches */
uint32_t gat_cmdln_scan_switches (
    gat_cmdline *pcmd, 
    const char *arr_switches[],
    uint32_t switch_masks[], 
    int num_switches)
{
    unsigned long switch_flags;
    unsigned iarg, isw;
    int flg_switch_found;
    char *ptr;

    if (pcmd->count == 0) {
        return 0;
    }

    switch_flags = 0;
    iarg = 0;
    ptr = pcmd->args[0];

    while (*ptr != '\0' || iarg < (pcmd->count-1) ) {
        if (*ptr == '\0') {
            ptr = pcmd->args[++iarg];  /* get next argument */
        }

        /* does it look like switch */
        if (*ptr == GAT_CMDLN_SWITCH) {
            flg_switch_found = 0;

            /* scan all switches */
            for (isw = 0; isw < (unsigned)num_switches; isw++) {
                if ( strstr(ptr,arr_switches[isw]) == ptr ) {
                    /* set flag to indicate switch was found */
                    flg_switch_found = 1;

                    /* check if switch is already set */
                    if (switch_flags & switch_masks[isw]) {
                        printf ("switch redefined : %s\n", arr_switches[isw]);
                        return 0;
                    }

                    /* set the switch flag */
                    switch_flags|= switch_masks[isw];
                    break;
                }
            }

            if (flg_switch_found) {
                ptr+= strlen (arr_switches[isw]); /* skip switch */
            } else {
                ++ptr; /* skip the character */
            }
        } else {
            /* skip the parameter */
            while (*ptr && *ptr != GAT_CMDLN_SWITCH) {
                ++ptr;
            }
        }
    } /* end while */

    return switch_flags;

} /* end cmdline_scan_switches() */

char *gat_cmdln_get_param (gat_cmdline *pcmd, const char *str_switch, char *buff) {
    char *ptr, *ptr_buff;
    int iarg;

    ptr_buff = buff;
    iarg = 0;
    ptr = pcmd->args[0];

    while (*ptr || iarg < (int)pcmd->count) {
        if (*ptr == '\0') {
            ptr = pcmd->args [++iarg];
        }

        if (*ptr == GAT_CMDLN_SWITCH) {
            if ( strstr (ptr, str_switch) == ptr ) {
                /* skip switch */
                ptr+= strlen(str_switch);

                /* copy switch param */
                while ( *ptr || iarg < (int)(pcmd->count-1) ) {
                    if (*ptr == '\0') {
                        if (ptr_buff == buff) {
                            ptr = pcmd->args[++iarg];
                        } else {
                            break;
                        }
                    }
                    if (*ptr == GAT_CMDLN_SWITCH) {
                        break;
                    }
                    *(ptr_buff++) = *(ptr++);
                }
                *ptr_buff = '\0';
                return buff;
            }
            ++ptr;
        } else {
            while (*ptr && *ptr != GAT_CMDLN_SWITCH) {
                ++ptr;
            }
        }
    }

    *ptr_buff = '\0';
    return buff;
}
