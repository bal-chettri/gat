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
#include <ctype.h>

extern int masm85_filter (gat *ga);

/* checks is token is a 8 bit register id */
static int masm85_is_reg8 ( const char *strtext ) {
    if ( strlen(strtext) == 1 ) {
        if ( strchr ("abcdehlm", tolower(strtext[0]) ) ) {
            return 1; /* its a reg8 */
        }
    }
    return 0; /* not a reg8 */
}

/* checks if a token is a 16 bit register id */
static int masm85_is_reg16 ( const char *strtext ) {
    if ( strlen(strtext) == 1) {
        if ( strchr("bdh", tolower(strtext[0]) ) ) {
            return 1; /* its a reg16 */
        }
        return 0; /* not a reg16 */
    }
    if ( gat_strcmpi(strtext, "sp") || gat_strcmpi(strtext, "psw") ) {
        return 1; /* its a reg16 */
    }
    return 0; /* not a reg16 */
}

/* converts reg8 token to binary code */
static uint8_t masm85_creg8 (const char *strtext) {
    switch ( strtext[0]  ) {
    case 'a': return 7;
    case 'b': return 0;
    case 'c': return 1;
    case 'd': return 2;
    case 'e': return 3;
    case 'h': return 4;
    case 'l': return 5;
    default:
    /* case 'm': */
        return 6;
    }
}

/* convert register pair token to binary code */
static uint8_t masm85_creg16 ( const char *strtext ) {
    if ( gat_strcmpi (strtext, "b") ) {
        return 0;
    } else if ( gat_strcmpi (strtext, "d") ) {
        return 1;
    } else if ( gat_strcmpi (strtext, "h") ) {
        return 2;
    } else if ( gat_strcmpi (strtext, "sp") || gat_strcmpi (strtext, "psw") ) {
        return 3;
    } else {
        return 4;
    }
}

/* get binary / CPU code size for the instruction */
static void masm85_get_code_size (gat *ga, const gat_instr *instr) {
    int i;
    
    for (i = 0; i < instr->num_tokens; i++) {
        const int options = instr->type_operands[i];

        /* test token with operand and update g_bin_size */
        switch (options) {
        case GAT_OPRND_BYTE:
            if (options == 0) /* immediate value? */ {
                ++ga->g_bin_size; /* add byte to opcode */
            }
            break;

        case GAT_OPRND_DBL:
            ga->g_bin_size+= 2; /* always add a dbl to opcode */
            break;

        default:
            break;
        }
    }
}

/* generate binary / CPU code for the instruction */
static void masm85_gen_code (gat *ga, const gat_instr *instr) {
    int i;

    for (i = 0; i < instr->num_tokens; i++) {
        const uint8_t optype = instr->type_operands[i];
        const uint8_t options = instr->type_options[i];
        const uint16_t value = ga->g_arr_cooked_tokens[i + 1];

        switch (optype) {
        case GAT_OPRND_REG8:
        case GAT_OPRND_REG16:
            /* register code in low-order; options is shift mask */
            ga->g_bin[0]|=  (uint8_t)(value & 0x00FF) << options;
            break;

        case GAT_OPRND_BYTE:
            if ( options == 0 ) /* immediate? */ {
                /* byte in low-order; immediate value */
                ga->g_bin[ga->g_bin_size++] = (uint8_t)(value & 0x00FF);
            } else {
                /* byte in low-order; options is shift mask */
                ga->g_bin[0]|= (uint8_t)(value & 0x00FF) << options;
            }
            break;

        case GAT_OPRND_DBL:
            /* immediate word value */
            ga->g_bin[ga->g_bin_size] = (uint8_t)(value & 0x00FF);
            ga->g_bin[ga->g_bin_size + 1] = (uint8_t)((value >> 8) & 0x00FF);
            ga->g_bin_size+= 2;
        }
    }
}

/* masm85 arch interface */
gat_arch masm85_arch = {
    masm85_is_reg8,
    masm85_is_reg16,
    NULL,

    masm85_creg8,
    masm85_creg16,
    NULL,

    masm85_filter,
    masm85_get_code_size,
    masm85_gen_code
};
