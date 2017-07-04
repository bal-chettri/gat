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
#include "gat_io.h"
#include "gat_core.h"
#include "gat_str.h"
#include "gat_err.h"

void gat_open_files (gat *ga) {
    unsigned i;
    for (i = 0; i < ga->num_ios; i++) {     
        gat_io *io = ga->ios + i;
        if (i > 0) {
            gat_kill_file (ga, io->path);
        }
        io->fp = fopen (io->path, io->mode);
        if (io->fp == NULL) {
            gat_fatal_error (ga, 
                GAT_ERR_FILE_OPEN, "error opening %s file: '%s'", 
                i == 0 ? "input" : "output", 
                io->path);
        }
    }
}

void gat_close_files (gat *ga) {
    unsigned i;
    for (i = 0; i < ga->num_ios; i++) {     
        gat_io *io = ga->ios + i;
        if (io->fp) {
            fclose (io->fp);
            io->fp = NULL;
        }
    }
}

int gat_read_line (gat *ga) {
    FILE *fpin = ga->ios[0].fp;
    int flag_done;
    
    /* clear buffer */
    ga->str_line[0] = '\0';
    flag_done = 0;
    
    while (!feof(fpin) && !flag_done) {
        /* temp line buffer */
        char buff [GAT_MAX_LINEBUFF_SIZE + 1];
        buff[0] = '\0';

        /* read line */
        fgets (buff, GAT_MAX_LINEBUFF_SIZE, fpin);
        ++ga->line_num;
        
        /* trim */
        gat_trim (buff, ga->str_line);
        if (ga->str_line[0] != '\0') {
            flag_done = 1;
        }
    }

    if (strlen(ga->str_line) > 0) {
        return 1;
    }
    return 0;
}

/* removes file at specified path from the filesystem */
void gat_kill_file (gat *ga, const char *path) {
    remove (path);
}

void gat_attach_io (gat *ga, const char *mode, const char *path, gat_emitter emitter) {
    gat_io *io; 
    if (ga->num_ios == GAT_MAX_IO) {
        gat_fatal_error (ga, GAT_ERR_OUT_OF_MEMORY, "too many io channels");
    }
    io = &ga->ios[ga->num_ios++];
    strncpy (io->path, path, GAT_MAX_PATH);
    strncpy (io->mode, mode, sizeof(io->mode));
    io->fp = NULL;
    io->emitter = emitter;
}
