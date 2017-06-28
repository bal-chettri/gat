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
/** masmcore.cpp  defines masm85 core functions. */
#include "gat_core.h"
#include "gat_sysutils.h"
#include "gat_str.h"
#include "gat_io.h"
#include "gat_parser.h"
#include "gat_tokenizer.h"
#include "gat_err.h"
#if defined(__MACH__) || defined(__APPLE__) || defined(__CYGWIN__)
#include <stdlib.h>
#else
#include <malloc.h>
#include <process.h> /* exit() */
#endif

/* initializes the assembler */
void gat_init (gat *ga, gat_arch *arch) {   
    ga->callback = NULL;
    ga->context = NULL;
    ga->arch = arch;

    ga->fatal_error = 0;
    ga->g_num_ids = 0;
    ga->g_num_labels = 0;

    ga->g_err_count = 0;
    ga->g_warn_count = 0;
    ga->g_offset = 0;
    ga->g_size = 0;

    ga->num_ios = 0;

    ga->g_pass = 0;
    ga->g_line_num = 0;
    ga->g_num_tokens = 0;

    memset (ga->g_arr_tokens, 0, sizeof(ga->g_arr_tokens));
    memset (ga->g_arr_raw_tokens, 0, sizeof(ga->g_arr_raw_tokens));
}

/* sets callback */
void gat_set_callback (gat *ga, gat_callback callback, void *context) {
    ga->callback = callback;
    ga->context = context;
}

/* gat engine function */
int gat_engine (gat *ga) {
    int exit_code;

    gat_open_files (ga);

    /* run PASS #1 (analysis phase) */  
    ga->g_pass = 1;
    gat_init_pass (ga);
    gat_scan (ga);
        
    /* run PASS #2 (assembly phase) */
    ga->g_pass = 2;
    gat_init_pass (ga); 
    gat_assemble (ga);

    /* print assembly / error report */
    if (ga->g_err_count == 0) {
        gat_print (ga, "written %u bytes to %s", ga->g_size, ga->ios[1].path);
    }
    gat_print (ga, "%u error(s) %u warning(s)", ga->g_err_count, ga->g_warn_count);

    exit_code = (ga->g_err_count == 0 ? 0 : -1);
    return exit_code;
}

/* cleanup assembly state */
void gat_cleanup (gat *ga) {
    gat_free_tokens (ga);

    gat_close_files (ga);

    /* delete ouput files on error */
    if (ga->g_err_count > 0) {
        unsigned i = 1; /* output begins at [1] */
        for (; i < ga->num_ios; i++) {
            gat_io *io = ga->ios + i;
            gat_kill_file (ga, io->path);
        }
    }
}

/* initializes a pass state */
void gat_init_pass (gat *ga) {
    gat_free_tokens (ga);
    ga->g_line_num = 0;
}

/* prints message on screen. masm85 doesn't directly print on the stdout or FILE.
   The message is sent to the registered callback. */
void gat_print (gat *ga, const char *format, ...) {
    if (ga->callback != NULL) {
        if (format == NULL) {
            ga->callback (ga, GAT_CALLBACK_TEXT, "", ga->context);
        } else {
            char *buff;
            va_list arg_list;

            va_start (arg_list, format);
            buff = gat_format_string  (format, arg_list);
            va_end (arg_list);

            /* redirect message to callback if it is set */
            /* ga->callback (ga, GAT_CALLBACK_TEXT, "masm85 : ", ga->context); */
            ga->callback (ga, GAT_CALLBACK_TEXT, buff, ga->context);
        
            free (buff);
        }       
    }   
}

/* prints non-fatal error message */
void gat_error (gat *ga, int err_no, const char *format, ...) {
    gat_error_info err;
    char *err_desc;
    va_list arg_list;

    /* increment error count */
    ++ga->g_err_count;

    if (ga->callback != NULL) {
        /* build error description string */    
        va_start (arg_list, format);
        err_desc = gat_format_string  (format, arg_list);
        va_end (arg_list);

        /* build error info */
        err.warning = 0; /* error */
        err.fatal = 0;
        err.errno = err_no;
        err.desc = err_desc;
        err.file = ga->ios[0].path;
        err.line = ga->g_line_num;
        err.col = -1;

        /* report error */
        ga->callback (ga, GAT_CALLBACK_ERROR, &err, ga->context);

        /* free error desccription string */
        free (err_desc);
    }
}

void gat_warning (gat *ga, int err_no, const char *format, ...) {
    gat_error_info err;
    char *err_desc;
    va_list arg_list;

    /* increment error count */
    ++ga->g_warn_count;

    if (ga->callback != NULL) {
        /* build error description string */    
        va_start (arg_list, format);
        err_desc = gat_format_string  (format, arg_list);
        va_end (arg_list);

        /* build error info */
        err.warning = 1;
        err.fatal = 0;
        err.errno = err_no;
        err.desc = err_desc;
        err.file =  ga->ios[0].path;
        err.line = ga->g_line_num;
        err.col = -1;

        /* report error */
        ga->callback (ga, GAT_CALLBACK_ERROR, &err, ga->context);

        /* free error desccription string */
        free (err_desc);
    }
}

/* gat_fatal_error does fatal error reporting, performs proper cleanup and exits 
   the assembler. Note that this sub routine does not return. */
void gat_fatal_error (gat *ga, int err_no, const char *format, ...) {
    gat_error_info err;
    char *err_desc;
    va_list arg_list;

    ga->fatal_error = 1;
    ++ga->g_err_count;

    if (ga->callback != NULL) {
        va_start (arg_list, format);
        err_desc = gat_format_string  (format, arg_list);
        va_end (arg_list);

        /* build error info */
        err.warning = 0; /* error */
        err.fatal = 1;
        err.errno = err_no;
        err.desc = err_desc;
        err.file =  ga->ios[0].path;
        err.line = ga->g_line_num;
        err.col = -1;

        ga->callback (ga, GAT_CALLBACK_ERROR, &err, ga->context);

        free (err_desc);
    }

    /* perform cleanup and exit */
    gat_cleanup (ga);

    /* terminate through exit */
    exit (err_no);
}
