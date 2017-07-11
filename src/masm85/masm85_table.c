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
 * gat_table.cpp  defines instruction table
 */
#include "gat.h"

/* assemble directive table */
gat_dirt g_dirt_table [] = {
    /*{ token, command, token_index, num_tokens }*/
    { GAT_EQU, "equ", 1, 3}, /* id equ byte|dbl|id|label */
    { GAT_ORG, "org", 0, 2}, /* org dbl */
    { GAT_END, "end", 0, 1}, /* end */
    { GAT_LABEL, ":", 1, 2 }, /* id: */
};

/* Length of directive table. */
const int g_len_dirt_table = sizeof(g_dirt_table) / sizeof(g_dirt_table[0]);

/* instruction table */
gat_instr g_instr_table [] =
{
    /*{ mnemonic, opcode, num_operands, type_operands[3], type_options[3] }*/
    { "aci", 0xCE, 1, { GAT_OPRND_BYTE }, { 0 } },
    { "adc", 0x88, 1, { GAT_OPRND_REG8 }, { 0 } },
    { "add", 0x80, 1, { GAT_OPRND_REG8 }, { 0 } },
    { "adi", 0xc6, 1, { GAT_OPRND_BYTE }, { 0 } },
    { "ana", 0xA0, 1, { GAT_OPRND_REG8 }, { 0 } },
    { "ani", 0xE6, 1, { GAT_OPRND_BYTE }, { 0 } },
    
    { "call",0xCD, 1, { GAT_OPRND_DBL }, { 0 } },
    { "cc", 0xDC, 1, { GAT_OPRND_DBL }, { 0 } },
    { "cm", 0xFC, 1, { GAT_OPRND_DBL }, { 0 } },
    { "cma", 0x2F, 0 },
    { "cmc", 0x3F, 0 }, 
    { "cmp", 0xB8, 1, { GAT_OPRND_REG8 }, { 0 } },
    { "cnc", 0xD4, 1, { GAT_OPRND_DBL }, { 0 } },   
    { "cnz", 0xC4, 1, { GAT_OPRND_DBL }, { 0 } },
    { "cp", 0xF4, 1, { GAT_OPRND_DBL }, { 0 } },
    { "cpe", 0xEC, 1, { GAT_OPRND_DBL }, { 0 } },
    { "cpi", 0xFE, 1, { GAT_OPRND_BYTE }, { 0 } },
    { "cpo", 0xE4, 1, { GAT_OPRND_DBL }, { 0 } },
    { "cz",  0xCC, 1, { GAT_OPRND_DBL }, { 0 } },

    { "daa", 0x27, 0 },
    { "dad", 0x09, 1, { GAT_OPRND_REG16 }, { 4 } },
    { "dcr", 0x05, 1, { GAT_OPRND_REG8 }, { 3 } },
    { "dcx", 0x0B, 1, { GAT_OPRND_REG16 }, { 4 } }, 
    { "di", 0xF3, 0 },
    
    { "ei", 0xFB, 0 },
    
    { "hlt", 0x76, 0 },

    { "in", 0xDB, 1, { GAT_OPRND_BYTE }, { 0 } },
    { "inr", 0x04, 1, { GAT_OPRND_REG8 }, { 3 } },
    { "inx", 0x03, 1, { GAT_OPRND_REG16 }, { 4 } },
    
    { "jc", 0xDA, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jm", 0xFA, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jmp", 0xC3, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jnc", 0xD2, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jnz", 0xC2, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jp", 0xF2, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jpe", 0xEA, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jpo", 0xE2, 1, { GAT_OPRND_DBL }, { 0 } },
    { "jz",  0xCA, 1, { GAT_OPRND_DBL }, { 0 } },

    { "lda", 0x3A, 1, { GAT_OPRND_DBL }, { 0 } },
    { "ldax", 0x0A, 1, { GAT_OPRND_REG16 }, { 4 } },
    { "lhld", 0x2A, 1, { GAT_OPRND_DBL }, { 0 } },
    { "lxi", 0x01, 3, { GAT_OPRND_REG16, GAT_OPRND_SYMBOL, GAT_OPRND_DBL }, { 4, ',', 0 } },
    
    { "mov", 0x40, 3, { GAT_OPRND_REG8, GAT_OPRND_SYMBOL, GAT_OPRND_REG8 }, { 3, ',', 0 } },
    { "mvi", 0x06, 3, { GAT_OPRND_REG8, GAT_OPRND_SYMBOL, GAT_OPRND_BYTE }, { 3, ',', 0 } },    

    { "nop", 0x00, 0 },
    
    { "ora", 0xB0, 1, { GAT_OPRND_REG8 }, { 0 } },
    { "ori", 0xF6, 1, { GAT_OPRND_BYTE }, { 0 } },
    { "out", 0xD3, 1, { GAT_OPRND_BYTE }, { 0 } },

    { "pchl", 0xE9, 0 },
    { "pop", 0xC1, 1, { GAT_OPRND_REG16 }, { 4 } },
    { "push", 0xC5, 1, { GAT_OPRND_REG16 }, { 4 } },
    
    { "ral", 0x17, 0 },
    { "rar", 0x1F, 0 },
    { "rc", 0xD8, 0 },
    { "ret", 0xC9, 0 }, 
    { "rim", 0x20, 0 },
    { "rlc", 0x07, 0 },
    { "rm", 0xF8, 0 },
    { "rnc", 0xD0, 0 },     
    { "rnz", 0xC0, 0 },
    { "rp", 0xF0, 0 },  
    { "rpe", 0xE8, 0 },
    { "rpo", 0xE0, 0 },
    { "rrc", 0x0F, 0 },
    { "rst", 0xC7, 1, { GAT_OPRND_BYTE }, { 3 } },  
    { "rz",  0xC8, 0 },

    { "sbb", 0x98, 1, { GAT_OPRND_REG8 }, { 0 } },      
    { "sbi", 0xDE, 1, { GAT_OPRND_BYTE }, { 0 } },
    { "shld", 0x22, 1, { GAT_OPRND_DBL }, { 0 } },  
    { "sim", 0x30, 0 },
    { "sphl", 0xF9, 0 },
    { "sta", 0x32, 1, { GAT_OPRND_DBL }, { 0 } },
    { "stax", 0x02, 1 , { GAT_OPRND_REG16 }, { 4 } },
    { "stc", 0x37, 0 },
    { "sub", 0x90, 1, { GAT_OPRND_REG8 }, { 0 } },      
    { "sui", 0xD6, 1, { GAT_OPRND_BYTE }, { 0 } },
    
    { "xchg", 0xEB, 0 },
    { "xra", 0xA8, 1, { GAT_OPRND_REG8 }, { 0 } },  
    { "xri", 0xEE, 1, { GAT_OPRND_BYTE }, { 0 } },
    { "xthl", 0xE3, 0 },        
};

/* Length of instruction table. */
const int g_len_instr_table = sizeof(g_instr_table) / sizeof(g_instr_table[0]);
