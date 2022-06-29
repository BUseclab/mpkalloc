/**
Copyright (c) 2022 BUSeclab

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include "pin.H"

using std::hex;
using std::cout;
using std::cerr;
using std::ofstream;
using std::ios;
using std::pair;
using std::string;
using std::vector;
using std::endl;

typedef vector<pair<uint64_t,uint64_t>> metadata_info;

static uint64_t bytes_requested = 0;
static metadata_info metadata;

ofstream trace;
ofstream hits;

static const char *whitelist[] = {
  "/path/to/chromium/src/out/Default/chrome",
  NULL 
};

VOID FuncRtnSearch(ADDRINT stackPtr, void *s)
{
    const char *name = (const char*)s;
    // Scan 0x100 Words on the stack for meta-data pointers.
    for (int i = 0; i < 0x100; i++) {
      uint64_t ndata = *(((uint64_t*)stackPtr)+i); 
      uint64_t sdata = *(((uint64_t*)stackPtr)-i);
      for (std::vector<pair<uint64_t,uint64_t> >::iterator it = metadata.begin(); it != metadata.end(); ++it) {
        uint64_t metadata_start = it->first;
        uint64_t size = it->second;
        if (metadata_start < ndata && ndata < (metadata_start + size)) {
           hits << name << ":" << i << endl;
        }
        if (metadata_start < sdata && sdata < (metadata_start + size)) {
           hits << name << ":" << (i*-1) << endl;
        }
      } 
    }
}

VOID FuncRtnBefore(ADDRINT stackPtr, void *s)
{
    const char *name = (const char*)s;
    cerr << "INVOKE:" << name << ":" << hex << stackPtr << ":" << metadata.size() << endl;
}

VOID FuncRtnAfter(ADDRINT stackPtr, void *s)
{
    const char *name = (const char*)s;
    cerr << "RETURN:" << name << ":" << hex << stackPtr << ":" << metadata.size() << endl;
}

VOID MetaDataEtr(ADDRINT bytes)
{
    bytes_requested = bytes;
    cerr << "Requested: " << ":" << hex << bytes << endl;
}

VOID MetaDataRtn(ADDRINT metaDataPtr)
{
    cerr << "MetaData Page: " << ":" << hex << metaDataPtr << ":" << hex << metaDataPtr + bytes_requested << endl;
    pair<uint64_t,uint64_t> p(metaDataPtr, bytes_requested);
    metadata.push_back(p);
}

VOID ImageLoad(IMG img, void *v)
{
    trace << "Loading " << IMG_Name(img) << ", Image id = " << IMG_Id(img) << endl;
    bool found = false;

    const char **w = whitelist;
    while (*w != NULL) {
       const char *img_name = IMG_Name(img).c_str();  
       if (strcmp(img_name, *w) == 0) {
           found = true;
           break;
       }
       trace << "Did not match " << *w << endl;
       w++;
    }

    if (!found) {
      trace << "  Skipping!" << endl;
      return;
    }

    for ( SYM sym = IMG_RegsymHead(img); SYM_Valid(sym); sym = SYM_Next(sym) )
    {
        string symPureName =  PIN_UndecorateSymbolName(SYM_Name(sym), UNDECORATION_NAME_ONLY);
        RTN funcRtn = RTN_FindByAddress(SYM_Address(sym));
        if (!RTN_Valid(funcRtn))
        {
            cerr << "Routine " << symPureName << " is not found at address 0x" 
                    << hex << SYM_Address(sym) << endl;
            exit(-1);
        }
        RTN_Open(funcRtn);
        const string symbolName = SYM_Name(sym);
        const char *s = symbolName.c_str();
        char *buf = (char*)malloc(strlen(s)+1);
        strcpy(buf, s);
        trace << "    " << buf << endl;
        if (symbolName == "_ZN8tcmalloc13MetaDataAllocEm") {
            RTN_InsertCall(funcRtn, IPOINT_BEFORE, AFUNPTR(MetaDataEtr), IARG_REG_VALUE, REG_RDI, IARG_END);
            RTN_InsertCall(funcRtn, IPOINT_AFTER, AFUNPTR(MetaDataRtn), IARG_REG_VALUE, REG_RAX, IARG_END);
        } else {
            RTN_InsertCall(funcRtn, IPOINT_AFTER, AFUNPTR(FuncRtnSearch), IARG_REG_VALUE, REG_STACK_PTR, IARG_PTR, buf, IARG_END);
        }
        RTN_Close(funcRtn); 
    }
}


/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char *argv[])
{
    trace.open("trace.txt");
    hits.open("hits.txt");
    PIN_InitSymbols();
    PIN_Init(argc, argv);

    IMG_AddInstrumentFunction(ImageLoad, 0);

    // Register Fini to be called when the application exits 
    // PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
