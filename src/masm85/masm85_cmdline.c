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
#include "gat_err.h"
#include <stdio.h>
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#endif

#define MASM85_SWITCH_HEX               1
#define MASM85_SWITCH_85                2
#define MASM85_SWITCH_O                 4
#define MASM85_SWITCH_DBG               8
#define MASM85_SWITCH_HELP              16

#define MASM85_VERSION                  "0.0.1"

/* define commandline switches */
static const char *arr_cmdline_switches [/*MASM_NUM_SWITCHES*/] = { 
    "-hex", 
    "-85", 
    "-o", 
    "-dbg", 
    "-help" 
};

/* define commandline switch flags */
static uint32_t arr_cmdline_switchflags [/*MASM_NUM_SWITCHES*/] = { 
    MASM85_SWITCH_HEX, 
    MASM85_SWITCH_85, 
    MASM85_SWITCH_O, 
    MASM85_SWITCH_DBG, 
    MASM85_SWITCH_HELP 
};

/* prototypes */
extern void masm85_hex_emitter (gat *, gat_io *, gat_emitter_state);
extern void masm85_bin_emitter (gat *, gat_io *, gat_emitter_state);
extern void masm85_dbg_emitter (gat *, gat_io *, gat_emitter_state);

static void masm85_usage (gat *ga);

/* process command line */  
void masm85_process_commandline (gat *ga, int argc, char *argv[]) {
    char g_input_path [GAT_MAX_PATH];
    char g_output_path [GAT_MAX_PATH];
    char g_debug_path [GAT_MAX_PATH];       

     /* [0] = command name, [1] first arg */
    gat_cmdline cmdinfo = { argc - 1, &argv[1] };

    *g_input_path = *g_output_path = *g_debug_path = '\0';

    /* chek for input path*/
    if ( (argc > 1) && (argv[1][0] != GAT_CMDLN_SWITCH) ) {
        strncpy ( g_input_path, argv[1], GAT_MAX_PATH );
        gat_attach_io (ga, "rt", g_input_path, NULL);
    }

    /* scan command line for switches */
    ga->g_cmdline_flags = gat_cmdln_scan_switches (&cmdinfo, arr_cmdline_switches, 
                                                    arr_cmdline_switchflags, 
                                                    GAT_NUM_SWITCHES );
    
    /* see if -hex and -85 are both specified */
    if ( (ga->g_cmdline_flags & MASM85_SWITCH_HEX) && 
         (ga->g_cmdline_flags & MASM85_SWITCH_85) ) {
        gat_fatal_error (ga, 1, "-hex and -85 switches can't be used together");
    }
    
    /* check for invalid switches */
    if (g_input_path[0] == '\0') {
        /* these switches can't be used without input-path */
        if ( (ga->g_cmdline_flags & MASM85_SWITCH_HEX) || 
             (ga->g_cmdline_flags & MASM85_SWITCH_85) ||
             (ga->g_cmdline_flags & MASM85_SWITCH_O) || 
             (ga->g_cmdline_flags & MASM85_SWITCH_DBG) ) 
        {
            gat_fatal_error (ga, GAT_ERR_INVALID_INPUT_PATH, "input-path not specified");
        }
    }

    /* set -hex if -hex and -85 are both unspecified */
    if ( !(ga->g_cmdline_flags & MASM85_SWITCH_HEX) && !(ga->g_cmdline_flags & MASM85_SWITCH_85) ) {
        ga->g_cmdline_flags|= MASM85_SWITCH_HEX;
    }

    /* show usage if -help is present */
    if (ga->g_cmdline_flags & MASM85_SWITCH_HELP) {
        masm85_usage (ga);
    }

    if (g_input_path[0] == '\0') {
        masm85_usage (ga);
    } else {
        /* make output path */
        if ( ga->g_cmdline_flags & MASM85_SWITCH_O ) {
            gat_cmdln_get_param ( &cmdinfo, "-o", g_output_path );
            if (g_output_path[0] == '\0') {
                gat_fatal_error (ga, GAT_ERR_INVALID_OUTPUT_PATH, "output-path not specified");
            }
        } else {
            ga->g_cmdline_flags|= MASM85_SWITCH_O;
            strcpy (g_output_path, g_input_path);
            /* attach .hex or .85 extension if required */
            gat_attach_extension ( ga, g_output_path, 
                                    (ga->g_cmdline_flags & MASM85_SWITCH_HEX) 
                                    ? ".hex" : ".85" );         
        }
        /* attach output */
        gat_attach_io (ga, 
                        (ga->g_cmdline_flags & MASM85_SWITCH_HEX) ? "wt" : "wb", 
                        g_output_path, 
                        (ga->g_cmdline_flags & MASM85_SWITCH_HEX) 
                            ? masm85_hex_emitter 
                            : masm85_bin_emitter
                        );
        
        /* make debug output path */
        if ( ga->g_cmdline_flags & MASM85_SWITCH_DBG ) {
            gat_cmdln_get_param ( &cmdinfo, "-dbg", g_debug_path );
            if (g_debug_path[0] == '\0') {
                strcpy (g_debug_path, g_input_path);
                /* attach .dbg extension if required */
                gat_attach_extension (ga, g_debug_path, ".dbg" );
                gat_attach_io (ga, "wb", g_debug_path, masm85_dbg_emitter);
            }
        }
    }
}

/* prints the help listing */
static void masm85_usage (gat *ga) {
    gat_print (ga, "masm85 - micro assembler for Intel 8085 micro processor");
    gat_print (ga, "Version : %s\n", MASM85_VERSION);
    gat_print (
        ga,
        "Usage: masm85 [<input-path>] [options]\n\n"
        "Options:\n"
        "  [-hex | -85]               : generate HEX or 85 file (default is -hex)\n"
        "  [-o<output-path>]          : specify output file path\n"
        "  [-dbg[<debug-ouput-path>]] : generate debug DBG file"
        );
}
