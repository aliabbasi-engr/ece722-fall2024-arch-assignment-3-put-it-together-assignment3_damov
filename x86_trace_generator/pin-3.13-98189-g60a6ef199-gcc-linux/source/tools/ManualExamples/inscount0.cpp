/*
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

#include <iostream>
#include <fstream>
#include "pin.H"
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;
using std::cout;


bool instrumentRegion = false;


ofstream OutFile;

// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

// This function is called before every instruction is executed
VOID docount() { icount++; }
    
// Pin calls this function every time a new instruction is encountered
VOID Instruction(INS ins, VOID *v)
{
     // Insert a call to docount before every instruction, no arguments are passed
     if (instrumentRegion)
       
       INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)docount, IARG_END);
     
}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "inscount.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    // Write to a file since cout and cerr maybe closed by the application
    OutFile.setf(ios::showbase);
    OutFile << "Count " << icount << endl;
    OutFile.close();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}


/////////////////////////////////////////////////////////////////////////////////////////
/// manual simpoint
/////////////////////////////////////////////////////////////////////////////////////////
// Added by Lifeng
// start trace generation at SIM_BEGIN() function call

VOID RtnBegin(ADDRINT ret)
{
  if (ret == 0)
    return;

  // enable corresponding thread's instrumentation
    cout << " SIM BEGIN" << endl;
  instrumentRegion = true;
}
VOID RtnEnd(ADDRINT arg)
{
  if (arg == 0)
    return;

  // enable corresponding thread's instrumentation
  if (instrumentRegion==false)
    return;

  cout << " SIM END" << endl;
  instrumentRegion = false;
}

VOID Image(IMG img, VOID *v)
{

  
  RTN rtn = RTN_FindByName(img, "SIM_BEGIN");
  
  if (RTN_Valid(rtn))
  {
   
    RTN_Open(rtn);
    RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)RtnBegin,
                   IARG_FUNCRET_EXITPOINT_VALUE, IARG_END);
    RTN_Close(rtn);
  }
  RTN rtn2 = RTN_FindByName(img, "SIM_END");
  if (RTN_Valid(rtn2))
  {
    RTN_Open(rtn2);
    RTN_InsertCall(rtn2, IPOINT_BEFORE, (AFUNPTR)RtnEnd,
                   IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);
    RTN_Close(rtn2);
  }


}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    PIN_InitSymbols();

  
   if (PIN_Init(argc, argv)) return Usage();

   

    OutFile.open(KnobOutputFile.Value().c_str());

    cout<<"Instrumenting..."<<endl;
    IMG_AddInstrumentFunction(Image, 0);
    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
 
