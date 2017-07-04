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
#ifndef __gat_types_h__
#define __gat_types_h__

#include <stdio.h>

/* define fixed size types for cross platform compatibility */
#ifdef WIN32
/* basic int types */
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
#else
#  include <stdint.h>
#endif
typedef char * gat_char;

/* define gat version */
#define GAT_VERSION                 "0.0.1"

/* define token types */
#define GAT_EQU                     0
#define GAT_ORG                     1
#define GAT_END                     2
#define GAT_LABEL                   3

/* define constants */
#define GAT_WHITE                   "\r\n\f\t\v "
#define GAT_SYMBOLS                 "~~!@#$%^&*()+-={}[]:\";'<>?,./|\\"
#define GAT_COMMENT_CHAR            ';'

#define GAT_MAX_IO                  5
#define GAT_MAX_IDS                 1000
#define GAT_MAX_LABELS              GAT_MAX_IDS
#define GAT_MAX_TOKENS              4
#define GAT_MAX_TOKEN_LEN           16
#define GAT_MAX_DIRECTIVES          4
#define GAT_MAX_INSTRUCTIONS        80
#define GAT_MAX_MNEMONIC_LEN        4
#define GAT_MAX_ID_LEN              16
#define GAT_MAX_PATH                255
#define GAT_MAX_LINEBUFF_SIZE       255
#define GAT_MAX_ERRORS              100
#define GAT_MAX_BYTE                0xFF
#define GAT_MAX_DBL                 0xFFFF

#define \
GAT_MAX_NUMERIC_TOKEN_LENGTH        6

#define GAT_OPRND_REG8              1
#define GAT_OPRND_REG16             2
#define GAT_OPRND_BYTE              3
#define GAT_OPRND_DBL               4
#define GAT_OPRND_SYMBOL            5
#define GAT_OPRND_INVALID           0xFF
#define GAT_IDTYPE_BYTE             3
#define GAT_IDTYPE_DBL              4

/* constants for Intel HEX file format */
#define INTEL_HEX_RECTYPE_DATA      0
#define INTEL_HEX_RECTYPE_EOF       1
#define INTEL_MAX_HEX_RECSIZE       16

/* define bit mask for command line switches */
#define GAT_NUM_SWITCHES            5
#define GAT_CMDLN_SWITCH            '-'

/* gat assembler */
struct _gat;

/* enum _gat_token_type */
typedef enum _gat_token_type {
    GAT_TOK_INVALID,
    GAT_TOK_WORD,
    GAT_TOK_SYMBOL,
    GAT_TOK_WHITE,
    GAT_TOK_STRING
}gat_token_type; /* enum gat_token_type */

typedef enum _gat_callback_type {
    GAT_CALLBACK_TEXT,
    GAT_CALLBACK_ERROR
}gat_callback_type;

/* gat_callback type */
typedef void (*gat_callback) \
(struct _gat *ga, gat_callback_type type, void *data, void *ctx);

struct _gat_io;
typedef enum _gat_emitter_state {
    GAT_EMIT_BEGIN_ASSEMBLY,
    GAT_EMIT_END_ASSEMBLY,
    GAT_EMIT_SET_ORG,
    GAT_EMIT_CODE
}gat_emitter_state;

/* gat_emitter type */
typedef void (*gat_emitter) \
(struct _gat *, struct _gat_io *, gat_emitter_state);

/* define structure types */

/* io structure */
typedef struct _gat_io {
    char mode[4];
    char path[GAT_MAX_PATH];
    FILE *fp;
    gat_emitter emitter;
}gat_io;

/* error info structure */
typedef struct _gat_error_info {
    int errno;          /* error no */
    uint8_t warning;    /* is warning? */
    uint8_t fatal;      /* fatal error? */
    char *desc;         /* error description */
    char *file;         /* source/object file path */
    int line;           /* line no */
    int col;            /* col no */
}gat_error_info;

/* cmd line structure */
typedef struct _gat_cmdline {
    unsigned count;
    char **args;
}gat_cmdline;

/* LABEL */
typedef struct _gat_label {
    char name [GAT_MAX_ID_LEN + 1];
    uint16_t address;
}gat_label;

/* ID */
typedef struct _gat_id {
    char name [GAT_MAX_ID_LEN + 1];
    uint16_t data;
    uint8_t idtype;
    /* uint8_t assumed; */
}gat_id;

/* directive */
typedef struct _gat_dir {
    uint8_t token;
    char *command;
    uint8_t token_index;
    uint8_t num_tokens;
}gat_dirt;

/* INSTR */
typedef struct _gat_instr {
    char mnemonic [GAT_MAX_MNEMONIC_LEN + 1]; /* mnemonic */
    uint8_t opcode; /* preliminary opcode */
    uint8_t num_tokens; /* number of tokens including mnemonic */
    uint8_t type_operands[3];
    uint8_t type_options[3];
}gat_instr;

/* Intel HEX record */
typedef struct _INTEL_HEX_RECORD {
    char header;
    uint8_t length;
    uint16_t offset;
    uint8_t type;
}INTEL_HEX_RECORD;

/* struct gat_token */
typedef struct _gat_token {
    char *string;               /* pointer to actual string */
    size_t length;              /* length of string */
    gat_token_type type;        /* type of token */
}gat_token;

/* gat arch interface */
typedef struct _gat_arch {
    int (* is_reg8) (const char *);
    int (* is_reg16) (const char *);
    int (* is_reg32) (const char *);
    
    uint8_t (* creg8) (const char *);
    uint8_t (* creg16) (const char *);
    uint8_t (* creg32) (const char *);

    int (* filter) (struct _gat *);

    void (* get_code_size) (struct _gat *, const gat_instr *);
    void (* gen_code) (struct _gat *, const gat_instr *);
}gat_arch;

/* gat */
typedef struct _gat {
    gat_callback callback;
    void *context;
    gat_arch *arch;
    int fatal_error;
    unsigned num_ids;
    gat_id arr_ids[GAT_MAX_IDS];
    unsigned num_labels ;
    gat_label arr_labels[GAT_MAX_LABELS];
    gat_io ios [GAT_MAX_IO];
    unsigned num_ios;
    unsigned pass;
    unsigned err_count;
    unsigned warn_count;
    uint32_t line_num;
    uint32_t org;
    uint32_t offset;
    uint32_t size;
    char str_line [GAT_MAX_LINEBUFF_SIZE + 1];
    unsigned num_tokens;
    char *arr_tokens [GAT_MAX_TOKENS];
    gat_token arr_raw_tokens [GAT_MAX_TOKENS];
    uint16_t arr_cooked_tokens [GAT_MAX_TOKENS];
    uint8_t bin[3];
    unsigned bin_size;
    unsigned long cmdline_flags;
    uint16_t rec_length;
    uint32_t offset_hexrec;
    uint8_t checksum;
}gat;

/* externs */
extern gat_dirt g_dirt_table [GAT_MAX_DIRECTIVES];
extern gat_instr g_instr_table [GAT_MAX_INSTRUCTIONS];

#endif /* !__gat_types_h__ */
