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
/** masmtbl.cpp  defines masm85 table lookup functions */
#include "gat_table.h"
#include "gat_str.h"
#include "gat_core.h"
#include "gat_err.h"

/* searches for instruction record in the instruction table using binary 
   search; table is already sorted! */
int gat_search_instr (gat *ga, const char *strtext) {
    int start_index, end_index, mid_index, rescmp;    
    start_index = 0;
    end_index = ga->len_instr_table - 1;
    
    while (end_index >= start_index) {
        mid_index = start_index + (end_index - start_index) / 2;
        rescmp = strcmpi (ga->instr_table[mid_index].mnemonic, strtext);
        if (  rescmp > 0 ) {
            end_index = mid_index - 1;
        } else if ( rescmp < 0 ) {
            start_index = mid_index + 1;
        } else {
            return mid_index;
        }
    }

    /* not found! */
    return -1;
}

/* searches and returns index of id */
int gat_search_id (gat *ga, const char *id) {
    unsigned i;
    for (i = 0; i < ga->num_ids; i++) {
        if ( gat_strcmp (ga->arr_ids[i].name, id) ) {
            return i; /* found at i */
        }
    } 
    return -1; /* id not found */
}

/* defines a new id */
int gat_define_id (gat *ga, const char *id, uint16_t data, uint8_t id_type) {   
    int flag_dup  = 0;

    /* search for exsiting id */
    if ( (gat_search_id (ga, id) != -1) || (gat_search_label (ga, id) != -1) ) {
        flag_dup = 1;
    }

    if (flag_dup) {
        gat_error (ga, GAT_ERR_REDEFINED, "redefinition : %s", id);
    } else if (ga->num_ids == GAT_MAX_IDS) {
        gat_error (ga, GAT_ERR_TOO_MANY_IDS, "too many ids");
    } else if (strlen(id) > GAT_MAX_ID_LEN) {
        gat_error (ga, GAT_ERR_INVALID_ID, "identifier too long : %s", id);
    } else {
        strcpy (ga->arr_ids[ga->num_ids].name, id);
        ga->arr_ids[ga->num_ids].idtype = id_type;
        ga->arr_ids[ga->num_ids++].data = data;
        return 1; /* new id defined */
    }

    return 0; /* error defining id! */
}

/* searches and returns index of label */
int gat_search_label (gat *ga, const char *id) {
    unsigned i;
    for (i = 0; i < ga->num_labels; i++) {
        if ( gat_strcmp (ga->arr_labels[i].name, id) ) {
            return i; /* found at i */
        }
    }
    return -1; /* label not found! */
}

/* defines a new label */
int gat_define_label (gat *ga, const char *id, uint16_t address) {
    int flag_dup = 0;

    /* search for exsiting label */
    if ( (gat_search_label(ga, id) != -1) || (gat_search_id(ga, id) != -1) ) {
        flag_dup = 1;
    }

    if (flag_dup) {
        gat_error (ga, GAT_ERR_REDEFINED, "redefinition : %s", id);
    } else if (ga->num_labels == GAT_MAX_LABELS)
        gat_error (ga, GAT_ERR_TOO_MANY_IDS, "too many labels");    
    else if (strlen(id) > GAT_MAX_ID_LEN) {
        gat_error (ga, GAT_ERR_INVALID_ID, "identifier too long : %d", id);
    } else {
        /* add new label to label list */
        strcpy (ga->arr_labels [ga->num_labels].name, id);
        ga->arr_labels [ga->num_labels++].address = address;        
        return 1; /* new label defined */
    }

    return 0; /* error defining label! */
}
