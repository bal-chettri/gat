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
/* masm85_main.cpp */
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#endif
#include <stdio.h>
#include "gat.h"
#include "gat_err.h"

/* externs */
extern void masm85_callback (gat *, gat_callback_type, void *, void *);
extern void masm85_process_commandline (gat *ga, int argc, char *argv[]);
extern gat_arch masm85_arch;

#ifdef WINDOWS
int _tmain(int argc, _TCHAR* argv[]) {
#else
int main (int argc, char *argv[]) {
#endif
    /* invoke the gat assembler */
    int exitcode = 0;
    gat _ga, *ga = &_ga;
    
    gat_init (ga, &masm85_arch);
    gat_set_callback ( ga, masm85_callback, NULL );

    masm85_process_commandline ( ga, argc, argv );
    if (ga->num_ios > 0) {
        exitcode = gat_engine (ga);
    }
    
    gat_cleanup (ga);

    return exitcode;
}
