/*
 * Copyright (C) 2007-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

/*
 * Collect an address trace
 *
 * During execution of the program, record values in a MLOG and use the
 * MLOG values to reconstuct the trace. The MLOG can contain the actual
 * addresses or register values that can be used to compute the
 * address. This tool is thread safe. Each thread writes to its own MLOG
 * and each MLOG is dumped to a separate file.
 *
 */

/*
 * We do TRACE based instrumentation. At the top of the TRACE, we allocate
 * the maximum amount of space that might be needed for this trace in the
 * log. At each memory instruction, we just record the address at a
 * pre-determined slot in the log. If there are early exits from a trace,
 * then their slots will be empty. We initialize all the empty slots to an
 * invalid address so we can tell later that this instruction did not
 * reference memory.
 *
 * We check if the log is full at the top of the trace. If it is full, we
 * empty the log and reset the log pointer.
 *
 */
#include <cassert>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <unistd.h>
#include "pin.H"
using std::endl;
using std::hex;
using std::ofstream;
using std::string;
using std::vector;

#define MEMTRACE_DEBUG 0
#define BATCH_SIZE 10000000

TLS_KEY mlog_key;
REG scratch_reg0;
REG scratch_reg1;
std::ofstream outputFile;
TLS_KEY key;
const char READ = 'R';
const char WRITE = 'W';
bool start = false;

// std::vector<TRACE_INFO> traces(10000000);
// size_t index = 0;


// uint64_t counts = 0;

/*
 *
 *
 *
 * Knobs for tool
 *
 *
 *
 */

/*
 * Name of the output file
 */
KNOB< string > KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool", "o", "memtrace.out", "output file");

/*
 * Emit the address trace to the output file
 */
KNOB< BOOL > KnobEmitTrace(KNOB_MODE_WRITEONCE, "pintool", "emit", "0", "emit a trace in the output file");

class TRACE_INFO
{
    public:
        TRACE_INFO() {
            __address = 0;
            __cycle = 0;
            __type = ' ';
        }

        TRACE_INFO(ADDRINT address, UINT64 cycle, const char type) {
            __address = address;
            __cycle = cycle;
            __type = type;
        }

        ADDRINT getAddress() {
            return __address;
        }

        UINT64 getCycle() {
            return __cycle;
        }

        char getType() {
            return __type;
        }
    
    private:
        ADDRINT __address;
        UINT64 __cycle;
        char __type;
};

class TRACE_LOGGER
{
    public:
        TRACE_LOGGER() {
            __traces.resize(BATCH_SIZE);
        }

        void addTrace(ADDRINT address, UINT64 cycle, const char type) {
            TRACE_INFO trace = TRACE_INFO(address, cycle, type);
            __traces[__index] = trace;
            __index += 1;
            if (__index == BATCH_SIZE) {
                for (size_t i = 0; i < __index; i++) {
                    ADDRINT address = __traces[i].getAddress();
                    UINT64 cycle = __traces[i].getCycle();
                    char type = __traces[i].getType();
                    __outputFile << std::showbase << std::hex << address << " " << type << " " << std::dec << cycle << "\n";
                }
                __index = 0;
            } 
        }

        void open(std::string fileName) {
            __outputFile.open(fileName);
        }

        void close() {
            // Write remaining traces
            for (size_t i = 0; i < __index; i++) {
                ADDRINT address = __traces[i].getAddress();
                UINT64 cycle = __traces[i].getCycle();
                char type = __traces[i].getType();
                __outputFile << std::showbase << std::hex << address << " " << type << " " << std::dec << cycle << "\n";
            }

            __outputFile.close();
        }
    private:
        std::ofstream __outputFile;
        std::vector<TRACE_INFO> __traces;
        size_t __index = 0;
};

TRACE_LOGGER logger;

void MemoryAccessInfo(ADDRINT address, UINT64 cycle, const char type)
{
    logger.addTrace(address, cycle, type);
}



void InstrumentFunction(INS ins, VOID* v) {
    // Log every memory references of the instruction
    if (start) {
        if (INS_IsMemoryRead(ins) && INS_IsStandardMemop(ins))
        {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemoryAccessInfo,
                        IARG_MEMORYREAD_EA, IARG_TSC, IARG_PTR, READ, IARG_END);
        }
        if (INS_IsMemoryWrite(ins) && INS_IsStandardMemop(ins))
        {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemoryAccessInfo,
                        IARG_MEMORYWRITE_EA, IARG_TSC, IARG_PTR, WRITE, IARG_END);
        }
        if (INS_HasMemoryRead2(ins) && INS_IsStandardMemop(ins))
        {
            INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)MemoryAccessInfo,
                        IARG_MEMORYREAD2_EA, IARG_TSC, IARG_PTR, READ, IARG_END);
        }
    }
}

void FinalFunction(INT32 code, VOID* v) {
    logger.close();
}

VOID StartTracking() {
    start = true;
}

VOID StopTracking() {
    start = false;
}

VOID LoadImage(IMG image, VOID *v) {    
    // bfs
    RTN routine = RTN_FindByName(image, "_Z5DOBFSRK8CSRGraphIiiLb1EEibii");

    // sssp
    // RTN routine = RTN_FindByName(image, "_Z9DeltaStepRK8CSRGraphIi10NodeWeightIiiELb1EEiib");

    // bc
    // RTN routine = RTN_FindByName(image, "_Z7BrandesRK8CSRGraphIiiLb1EER12SourcePickerIS0_Eib");

    // cc
    // RTN routine = RTN_FindByName(image, "_Z8AfforestRK8CSRGraphIiiLb1EEbi");
    // pr
    // RTN routine = RTN_FindByName(image, "_Z14PageRankPullGSRK8CSRGraphIiiLb1EEidb");

    // tc
    // RTN routine = RTN_FindByName(image, "_Z6HybridRK8CSRGraphIiiLb1EE");

    if (RTN_Valid(routine)) {
        // std::cout << "Benchmark found" << std::endl;
        RTN_Open(routine);
        RTN_InsertCall(routine, IPOINT_BEFORE, (AFUNPTR)StartTracking, IARG_END);
        RTN_InsertCall(routine, IPOINT_AFTER, (AFUNPTR)StopTracking, IARG_END);
        RTN_Close(routine);
    }
}

int main(int argc, char* argv[])
{
    PIN_InitSymbols();
    PIN_Init(argc, argv);
    const string fileName = KnobOutputFile.Value();
    logger.open(fileName);

    IMG_AddInstrumentFunction(LoadImage, 0);
    INS_AddInstrumentFunction(InstrumentFunction, 0);
    PIN_AddFiniFunction(FinalFunction, 0);

    PIN_StartProgram();

    return 0;
}