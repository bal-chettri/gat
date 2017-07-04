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
/** gat_analyzer.cpp  MASM analyzer routines */
#include "gat_parser.h"
#include "gat_conv.h"
#include "gat_lexer.h"
#include "gat_table.h"
#include "gat_core.h"
#include "gat_str.h"
#include "gat_io.h"
#include "gat_err.h"
#include <assert.h>

#ifdef _DEBUG
#  define GAT_ASSERT(C)
#else
#  define GAT_ASSERT(C) assert(C)
#endif

#ifdef _DEBUG
#  define GAT_ASSERTE(C,M)
#else
#  define GAT_ASSERTE(C,M) if(!(C)) { \
   printf("masm assertion: %s\n",M); \
   assert(C); \
}
#endif

/* test routines; these are silent and do not generate any errors */

/* tests for a byte vlaue from literal or id */
int gat_test_byte (gat *ga, const char *strtoken) {
    return (
        gat_is_byte(strtoken) ||
        gat_is_id(strtoken)
        );
}

/* tests for a double value from literal, label or id */
int gat_test_dbl (gat *ga, const char *strtoken) {
    return (
        gat_is_dbl(strtoken) ||
        gat_is_label(strtoken) ||
        gat_is_id(strtoken)
        );
}

/* tests for specific token */
int gat_test_token (gat *ga, const char *strtoken, int optype) {
    switch (optype) {
    case GAT_OPRND_REG8:
        return ga->arch->is_reg8(strtoken);

    case GAT_OPRND_REG16:
        return ga->arch->is_reg16(strtoken);

    case GAT_OPRND_SYMBOL:
        return gat_is_symbol(strtoken[0]);

    case GAT_OPRND_BYTE:
        return gat_test_byte(ga, strtoken);
        
    case GAT_OPRND_DBL:
        return gat_test_dbl(ga, strtoken);

    default:
        GAT_ASSERTE(0,\
        "invalid operand type found in table");
    }

    return 0;
}

/* parse routines; these generate errors when input does not match */

int gat_parse_byte (gat *ga, const char *strtoken, uint8_t *value) {
    if (gat_is_byte(strtoken)) {
        *value = gat_cbyte(strtoken);
        return 1;
    } else if (gat_is_id(strtoken)) {
        /* search for id */
        int index = gat_search_id (ga, strtoken);
        if (index == -1) {
            /* not found */
            gat_error (ga, GAT_ERR_UNDEFINED_ID, "undefined identifier : %s", strtoken);
        } else {
            /* found; check id data type */
            if ( ga->arr_ids[index].idtype == GAT_IDTYPE_DBL ) {
                gat_error (ga, GAT_ERR_INVALID_CONVERSION, "cannot convert dbl to byte");
            } else /* its a byte */ {
                *value = (ga->arr_ids[index].data & 0x00FF);
                return 1;
            }
        }
    } else {
        gat_error (ga, GAT_ERR_BYTE_EXPECTED, "byte expected");
    }
    return 0; /* failed to parse a byte */
}

int gat_parse_dbl (gat *ga, const char *strtoken, uint16_t *value) {
    if (gat_is_dbl(strtoken)) {
        *value = gat_cdbl (strtoken);
        return 1;
    } else if (gat_is_id(strtoken)) {
        int index;
        /* search for label */
        index = gat_search_label (ga, strtoken);
        if (index == -1) {
            /* not found; search for id */
            index = gat_search_id (ga, strtoken);
            if (index == -1) {
                gat_error (ga, GAT_ERR_UNDEFINED_ID, "undefined label or identifier : %s", strtoken);
            } else {
                *value = ga->arr_ids[index].data;
                return 1;
            }
        } else {
            *value = ga->arr_labels[index].address;
            return 1;
        }
    } else {
        gat_error (ga, GAT_ERR_WORD_EXPECTED, "word expected");
    }
    return 0; /* failed to parse a dbl */
}

/* parses token specified in optype parameter and returns the value */
int gat_parse_token (gat *ga, const char *strtoken, int optype, void *pvalue) {
    switch (optype) {
    /* check for 8 bit reg */
    case GAT_OPRND_REG8:
        if (ga->arch->is_reg8(strtoken)) {
            *((uint8_t *)pvalue) = ga->arch->creg8(strtoken);
        } else {
            gat_error (ga, GAT_ERR_8BIT_REG_EXPECTED, "8 bit register name expected");
            return 0;
        }
        break;

    /* check for 16 bit reg */
    case GAT_OPRND_REG16:
        if (ga->arch->is_reg16(strtoken)) { 
            *((uint8_t *)pvalue) = ga->arch->creg16(strtoken);           
        } else {
            gat_error (ga, GAT_ERR_16BIT_REG_EXPECTED, "16 bit register name expected");
            return 0;
        }
        break;

    /* check for symbol */
    case GAT_OPRND_SYMBOL:
        if ( !gat_is_symbol(strtoken[0]) || strtoken[0] != *((char *)pvalue) ) {
            gat_error (ga, GAT_ERR_SYMBOL_EXPECTED, "symbol %c expected", *((char *)pvalue) );
            return 0;
        }
        break;

    /* check for byte constant or id */
    case GAT_OPRND_BYTE:
        if (!gat_parse_byte (ga, strtoken, (uint8_t *)pvalue)) {
            gat_error (ga, GAT_ERR_BYTE_EXPECTED, "byte expected");
            return 0;
        }
        break;

    /* check for word constant, id or label */
    case GAT_OPRND_DBL:
        if (!gat_parse_dbl (ga, strtoken, (uint16_t *)pvalue)) {
            gat_error (ga, GAT_ERR_WORD_EXPECTED, "word expected");
            return 0;
        }
        break;

    default: 
        GAT_ASSERTE(0,\
        "invalid operand type found in table");
    }

    return 1;
}

int gat_is_reg (gat *ga, const char *token) {
    return ( 
        ga->arch->is_reg8(token) || 
        ga->arch->is_reg16(token) 
        );
}

/* scans an ORG directive -> ORG dbl | id */
int gat_parse_org (gat *ga) {
    if ( gat_is_dbl(ga->arr_tokens[1]) ) {
        /* set from word constant */
        ga->org = gat_cdbl(ga->arr_tokens[1]);
        ga->offset = ga->org;
    } else if ( gat_is_id(ga->arr_tokens[1])) {
        /* search for id */
        int index = gat_search_id (ga, ga->arr_tokens[1]);
        if (index == -1) {
            gat_error (ga, GAT_ERR_CONST_EXPECTED, "constant expected : org");
            return 0;
        } else if (ga->arr_ids[index].idtype == GAT_IDTYPE_BYTE) {
            ga->org = (ga->arr_ids[index].data & 0x00FF);
            ga->offset = ga->org;
        } else {
            ga->org = (ga->arr_ids[index].data & 0xFFFF);
            ga->offset = ga->org;
        }
    } else {
        gat_error (ga, GAT_ERR_CONST_EXPECTED, "constant expected : %s", ga->arr_tokens[0]);
        return 0;
    }

    return 1;
}

/* parse EQU directive -> id EQU (byte | word | id) */
int gat_parse_equ (gat *ga) {
    /* parse id at token 0 */
    if ( !gat_is_id (ga->arr_tokens[0]) ) {
        gat_error (ga, GAT_ERR_INVALID_ID, "invalid identifier : %s", 
                    ga->arr_tokens[0]);
        return 0;
    } 
    /* can't use register names for ids */
    if ( gat_is_reg (ga, ga->arr_tokens[0]) ) {
        gat_error (ga, GAT_ERR_RESERVED_NAME, "use of register name as identifier : %s", 
                    ga->arr_tokens[0]);
        return 0;
    }

    /* parse value at token 2 */
    if ( gat_is_byte(ga->arr_tokens[2]) ) {
        /* define byte id */
        gat_define_id (ga, ga->arr_tokens[0], 
                        gat_cdbl(ga->arr_tokens[2]), 
                        GAT_IDTYPE_BYTE);
    } else if ( gat_is_dbl (ga->arr_tokens[2]) ) {
        /* define dbl id */
        gat_define_id (ga, ga->arr_tokens[0], 
                        gat_cdbl(ga->arr_tokens[2]), 
                        GAT_IDTYPE_DBL);
    } else if  ( gat_is_id(ga->arr_tokens[2]) ) {
        /* search for id */
        int index = gat_search_id (ga, ga->arr_tokens[2]);        
        if (index == -1) {
            /* id not found */
            gat_error (ga, GAT_ERR_UNDEFINED_ID, "undefined label or identifier : %s", 
                        ga->arr_tokens[2]);            
            return 0;
        } else {
            /* define same type id */
            gat_define_id (ga, ga->arr_tokens[0], 
                            ga->arr_ids[index].data, 
                            ga->arr_ids[index].idtype);
        }
    } else {
        gat_error (ga, GAT_ERR_CONST_EXPECTED, "constant expected : %s", 
                    ga->arr_tokens[0]);
        return 0;
    }

    return 1;
}

/* parses a label -> id : */
int gat_parse_label (gat *ga) {
    /* parse id at token 0 */
    if ( !gat_is_id (ga->arr_tokens[0]) ) {
        gat_error (ga, GAT_ERR_INVALID_ID, "invalid label-identifier : %s", 
                    ga->arr_tokens[0]);
        return 0;
    } 

    /* can't use register names for ids */
    if ( gat_is_reg(ga, ga->arr_tokens[0]) ) {
        gat_error (ga, GAT_ERR_RESERVED_NAME, "use of register name as label-identifier : %s", 
                    ga->arr_tokens[0]);
        return 0;
    } 
    
    /* define label */
    gat_define_label (ga, ga->arr_tokens[0], (uint16_t)ga->offset);
    return 1;
}

/* scans for directives */
void gat_scan_directives (gat *ga, int *dirt, int *end) {
    int i;

    *dirt = *end = 0;

    for (i = 0; i < sizeof(g_dirt_table) / sizeof(g_dirt_table[0]); i++) {
        if (g_dirt_table[i].token_index >= ga->num_tokens) {
            continue; /* not enough tokens to parse command */
        }

        /* test for command */
        if (!gat_strcmpi(ga->arr_tokens[g_dirt_table[i].token_index], 
                        g_dirt_table[i].command)) {
            continue;
        }

        /* set directive flag */
        *dirt = 1;

        /* check for number of tokens */
        if (ga->num_tokens != g_dirt_table[i].num_tokens) {
            const char *name = *g_dirt_table[i].command == ':' 
                                ? "label" : g_dirt_table[i].command;
            gat_error (ga, GAT_ERR_INVALID_OPERANDS, "invalid number of operands : %s", 
                        name);
            return;
        }

        /* parse directive */
        switch (g_dirt_table[i].token) {
        case GAT_END:
            *end = 1; break;
        case GAT_ORG:               
            gat_parse_org (ga); break;
        case GAT_LABEL:
            gat_parse_label (ga); break;
        case GAT_EQU:
            gat_parse_equ (ga); break;
        default:
            GAT_ASSERTE(0, \
            "unsupported directive found in g_dirt_table table.");
        }
    }
}

/* parse directives; silent on assembly */
void gat_parse_directives (gat *ga, int *dirt, int *end) {
    int i;

    *dirt = *end = 0;

    for (i = 0; i < sizeof(g_dirt_table) / sizeof(g_dirt_table[0]); i++) {
        if (g_dirt_table[i].token_index >= ga->num_tokens) {
            continue; /* not enough tokens to parse command */
        }

        /* test for command */
        if (!gat_strcmpi(ga->arr_tokens[g_dirt_table[i].token_index], 
                        g_dirt_table[i].command)) {
            continue;
        }

        /* set directive flag */
        *dirt = 1;

        /* check for number of tokens */
        if (ga->num_tokens != g_dirt_table[i].num_tokens) {
            return; /* error reporting already done in pass #1 */
        }

        /* parse directive */
        switch (g_dirt_table[i].token) {
        case GAT_END:
            *end = 1; break;

        case GAT_ORG:
            if (gat_parse_org (ga)) {
                unsigned i = 1; /* output at ios[1] */
                for (; i < ga->num_ios; i++) {
                    ga->ios[i].emitter (ga, ga->ios + i, GAT_EMIT_SET_ORG);
                }
            }
            break;
        }
    }
}

/* scans an instruction */
int gat_scan_instruction (gat *ga, const gat_instr *instr) {
    int i;

    ga->bin_size = 0;

    /* check for required number of operands */
    if (instr->num_tokens != ga->num_tokens - 1) {
        gat_error (ga, GAT_ERR_INVALID_OPERANDS, "invalid number of operands : %s", 
                    ga->arr_tokens[0]);
        return 0;
    }

    /* ignore opcode emition during analysis */
    ga->bin_size = 1;

    /* test all operands */
    for (i = 0; i < instr->num_tokens; i++) {
        const int options = instr->type_operands[i];
        /* test token with operand and update g_bin_size */
        if (!gat_test_token(ga, ga->arr_tokens[i + 1], options )) {
            gat_error ( ga, GAT_ERR_TYPE_MISMATCH, "operand type mismatch : %s", 
                        ga->arr_tokens[0] );
            return 0;
        }
    }

    /* get architecture code size */
    ga->arch->get_code_size (ga, instr);

    /* #TODO: check arch for opcode bin size limit */
    GAT_ASSERTE(ga->bin_size >= 1 && ga->bin_size <= 3, \
        "invalid opcode size");

    /* filter */
    if ( !ga->arch->filter (ga) ) {
        gat_error (ga, 
            GAT_ERR_INVALID_INSTRUCTION,
            "instruction not allowed; may be invalid operand(s) : %s", 
            ga->str_line);
        return 0;
    }

    return 1;
}

/* assembles the instruction */
int gat_assemble_instruction (gat *ga, const gat_instr *instr) {
    int i;
        
    /* reset bin size */
    ga->bin_size = 0; 

    if (instr->num_tokens != ga->num_tokens - 1) {
        return 0; /* silent on assemble */
    }

    /* write opcode */
    ga->bin[0] = instr->opcode;
    ga->bin_size = 1;

    /* parse all token values */
    for (i = 0; i < instr->num_tokens; i++)  {
        const uint8_t optype = instr->type_operands[i];
        const uint8_t options = instr->type_options[i];

        /* set token parse options input */
        ga->arr_cooked_tokens[i + 1] = (uint16_t)options & 0x00FF;

        /* try to parse the token */
        if ( !gat_parse_token (ga, ga->arr_tokens[i + 1], 
                                optype, 
                                 (void *)&ga->arr_cooked_tokens[i + 1])
                                 ) {
            return 0; /* failed to parse token */
        }
    }
    
    /* generate architecture code */
    ga->arch->gen_code (ga, instr);

    /* Note: bin size is checked during analysis phase */
    
    if ( !ga->arch->filter (ga) ) {
        return 0; /* silent on assembly */
    }

    /* emit the code */
    gat_emit (ga);

    return 1;
}

/* scans assembly source; this corresponds to analysis phase. */
int gat_scan (gat *ga) {
    int flag_end = 0;
    int flag_dirt = 0;
    int index;
    
    /* reset vars */
    ga->org = ga->offset = 0;

    gat_print (ga, "scanning %s", ga->ios[0].path/*ga->input_path*/);
    
    /* begin assembly loop */
    while (!feof( ga->ios[0].fp) && !flag_end && !ga->fatal_error) {
        /* read and parse the next line in input */
        if (!gat_read_line(ga) || gat_tokenize_line (ga) == 0) {
            continue;
        }

        gat_scan_directives (ga, &flag_dirt, &flag_end);
        
        if (!flag_dirt) {
            index = gat_search_instr (ga, ga->arr_tokens[0]);
            if (index == -1) {
                gat_error (ga, GAT_ERR_INVALID_INSTRUCTION, "invalid instruction : %s", 
                            ga->arr_tokens[0]);
            } else if (gat_scan_instruction (ga, &g_instr_table[index])) {
                if (ga->offset + ga->bin_size > 65536){
                    gat_fatal_error (ga, GAT_ERR_OFFSET_OUT_OF_RANGE, "offset out of range");
                }
                ga->offset+= ga->bin_size;          
            }
        }
    }  /* end wile */

    return (ga->err_count == 0 ? 1 : 0);
}

/* assembles the source to machine code. */
int gat_assemble (gat *ga) {
    int flag_end = 0;
    int flag_dirt = 0;
    int index;
    
    /* reset vars */
    ga->org = ga->offset = 0;
    gat_print (ga, "assembling %s",  ga->ios[0].path);
    
    /* rewind the input file for assembly phase */
    rewind ( ga->ios[0].fp);

    /* emit begin */
    for (index = 1; index < (int)ga->num_ios; index++) {
        ga->ios[index].emitter (ga, &ga->ios[index], GAT_EMIT_BEGIN_ASSEMBLY);      
    }

    /* begin assembly loop */
    while (!feof( ga->ios[0].fp) && !flag_end && !ga->fatal_error) {
        /* read and parse the next line in input */
        if (!gat_read_line(ga) || gat_tokenize_line (ga) == 0) {
            continue;
        }

        gat_parse_directives (ga, &flag_dirt, &flag_end);
        
        if (!flag_dirt) {
            index = gat_search_instr (ga, ga->arr_tokens[0]);
            if (index != -1) {
                gat_assemble_instruction (ga, &g_instr_table[index]);
            }
        }
    }  /* end wile */

    if (ga->err_count == 0) {
        /* emit end */
        for (index = 1; index < (int)ga->num_ios; index++) {
            ga->ios[index].emitter (ga, &ga->ios[index], GAT_EMIT_END_ASSEMBLY);
        }
    }

    return (ga->err_count == 0 ? 1 : 0);
}

/* writes binary data to the output file */
void gat_emit (gat *ga) {
    unsigned i;
    for (i = 1; i < ga->num_ios; i++) {
        ga->ios[i].emitter (ga, &ga->ios[i], GAT_EMIT_CODE);
    }

    /* increment the program size count */
    ga->size+= ga->bin_size;
}

#if 0
/* reset the bin buffer */
void gat_bin_buffer_reset (gat *ga) {
    ga->bin_size = 0;
}

/* writes a byte to bin buffer */
void gat_bin_buffer_write (gat *ga, uint8_t val) {
    ga->bin[ga->bin_size++] = val;
}

/* masks a byte in bin buffer */
void gat_bin_buffer_mask_set (gat *ga, uint8_t pos, uint8_t val) {
    ga->bin[pos]|= val;
}

/* unmasks a byte in bin buffer */
void gat_bin_buffer_mask_unset (gat *ga, uint8_t pos, uint8_t val) {
    ga->bin[pos]&= ~val;
}
#endif
