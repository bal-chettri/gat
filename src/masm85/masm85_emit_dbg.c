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

/* emit rountines */

static void masm85_dbg_emit_begin_assembly (gat *ga, gat_io *io) {
    uint16_t codesize = 0;
    if ( fwrite(&codesize, sizeof(uint16_t), 1, io->fp) != 1 ) {
        gat_fatal_error (ga, 1, "failed to write DBG header");
    }
}

static void masm85_dbg_emit_end_assembly (gat *ga, gat_io *io) {
    rewind (io->fp); /* reset cursor pos to write code size */
    if ( fwrite(&ga->size, sizeof(uint32_t), 1, io->fp) != 1 ) {
        gat_fatal_error (ga, GAT_ERR_FILEIO_FAILED, "failed to update DBG header");
    }
}

static void masm85_dbg_emit_code (gat *ga, gat_io *io) {
    if ( (fwrite (&ga->offset, sizeof(uint16_t), 1, io->fp) != 1) ||
         (fwrite (&ga->line_num, sizeof(uint32_t), 1, io->fp) != 1) ) {
        gat_fatal_error (ga, GAT_ERR_FILEIO_FAILED, "gat_emit() failed to write debug file");
    }
}

void masm85_dbg_emitter (gat *ga, gat_io *io, gat_emitter_state state) {
    switch(state) {
    case GAT_EMIT_BEGIN_ASSEMBLY:
        masm85_dbg_emit_begin_assembly (ga, io);
        break;
    case GAT_EMIT_END_ASSEMBLY:
        masm85_dbg_emit_end_assembly (ga, io);
        break;
    case GAT_EMIT_CODE: 
        masm85_dbg_emit_code (ga, io);
        break;
    /* case GAT_EMIT_SET_ORG: */
    default:
        break;  
    }
}
