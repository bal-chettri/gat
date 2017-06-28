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

/* writes given bytes to the HEX output file */
static void gat_write_hex_bytes (gat *ga, const uint8_t *pbytes, unsigned count, gat_io *io) {
    unsigned i;
    for (i=0; i < count; i++)  {
        /* update the checksum first */
        ga->g_checksum^= pbytes[i];
        /* write the HEX bytes */
        if (pbytes[i] < 16) {
            fprintf (io->fp, "0%X", pbytes[i]);
        } else {
            fprintf (io->fp, "%X", pbytes[i]);
        }
    }       
}

/* writes EOF record to HEX file */
static void gat_end_hex_record (gat *ga, gat_io *io) {  
    fprintf (io->fp, ":00000001FF");
}

/* begins a new HEX record in HEX file */
static void gat_begin_hex_record (gat *ga, gat_io *io) {
    char buff[5];

    /* clear record chesksum */
    ga->g_checksum = 0;

    /* reset record length */
    ga->g_rec_length = 0;
    
    /* mark start of hex record; this is required later when updating the record */
    ga->g_offset_hexrec = (uint16_t) ftell( ga->ios[1].fp);
    
    fprintf (io->fp, ":00%s00", gat_chex16((uint16_t)ga->g_offset,buff));
}

/* updates HEX record in HEX file */
static void gat_update_hex_record (gat *ga, gat_io *io) {
    /* update checksum */
    ga->g_checksum^= ga->g_rec_length;
    ga->g_checksum^= (uint8_t )ga->g_offset;
    ga->g_checksum^= (ga->g_offset>>8)&0x0FF;
    ga->g_checksum = ~ga->g_checksum;

    /* write checksum */
    if (ga->g_checksum < 16) {
        fprintf ( io->fp, "%0X", ga->g_checksum);
    } else {
        fprintf ( io->fp, "%X", ga->g_checksum);
    }

    /* seek to start of HEX record */
    fseek ( io->fp, ga->g_offset_hexrec + sizeof(char), SEEK_SET );

    /* write updated length */
    if (ga->g_rec_length < 16) {
        fprintf (io->fp, "0%X", ga->g_rec_length);
    } else {
        fprintf (io->fp, "%X", ga->g_rec_length);
    }
    
    /* move to end of record */
    fseek (io->fp, 0, SEEK_END);

    /* write CR+LF */
    fprintf (io->fp, "\r\n");
}

/* emit routines */

static void masm85_hex_emit_begin_assembly (gat *ga, gat_io *io) {
    gat_begin_hex_record (ga, io);
}

static void masm85_hex_emit_end_assembly (gat *ga, gat_io *io) {
    gat_update_hex_record (ga, io);
    gat_end_hex_record (ga, io);
}

static void masm85_hex_emit_code (gat *ga, gat_io *io) {
    /* if record length >= INTEL_MAX_HEX_RECSIZE then we must terminate the 
           record forcefully */
    if ( (ga->g_rec_length + ga->g_bin_size) >= INTEL_MAX_HEX_RECSIZE ) {
        unsigned rem;

        /* find number of bytes that can't be put in the current record */
        rem = (ga->g_rec_length + ga->g_bin_size) - INTEL_MAX_HEX_RECSIZE;

        /* write new bytes in the current record */
        gat_write_hex_bytes (ga, ga->g_bin, ga->g_bin_size - rem, io);
        ga->g_rec_length+= (ga->g_bin_size - rem);

        gat_update_hex_record (ga, io);

        /* update the offset discarding the remaining bytes */
        ga->g_offset+= (ga->g_bin_size - rem);

        /* begin new record */
        gat_begin_hex_record (ga, io);

        /* write new bytes */
        gat_write_hex_bytes (ga, ga->g_bin + (ga->g_bin_size - rem), rem, io);
        ga->g_rec_length+= rem;

        ga->g_offset+= rem;
    } else {
        /* write new bytes */
        gat_write_hex_bytes (ga, ga->g_bin, ga->g_bin_size, io);
        ga->g_rec_length+= ga->g_bin_size;

        ga->g_offset+= ga->g_bin_size;
    }
}

static void masm85_hex_set_org (gat *ga, gat_io *io) {
    gat_update_hex_record (ga, io);
    gat_begin_hex_record (ga, io);
}

void masm85_hex_emitter (gat *ga, gat_io *io, gat_emitter_mode mode) {
    switch(mode) {
    case GAT_EMIT_BEGIN_ASSEMBLY:
        masm85_hex_emit_begin_assembly (ga, io);
        break;
    case GAT_EMIT_END_ASSEMBLY:
        masm85_hex_emit_end_assembly (ga, io);
        break;
    case GAT_EMIT_CODE: 
        masm85_hex_emit_code (ga, io);
        break;
    case GAT_EMIT_SET_ORG:
        masm85_hex_set_org (ga, io);
        break;
    default:
        break;  
    }
}
