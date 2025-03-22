# eec270_project
Using DRAMSim3, we evaluated the effectiveness of implementing permutation-based page interleacing, adjusting timine parameters, changing the address mapping scheme, and using a simultaneous refresh policy in improving the performance of DDR4 DRAM, with a size of 4 Gb, a 4 bit bus, and a megatransfer rate of 2400.  

## DRAMsim3
The simulator ran to evaluate the effectiveness of the optimizations.  

The simulator was ran with a given trace file by using a command like:  
```
./build/dramsim3main configs/DDR4_4Gb_x4_2400.ini -c 100000 -t trace.txt
```
The options used in the command above are:  
- \-c [Number of cycles to simulate]
- \-t [Trace file to use]

The command will generate 3 files:  
- dramsim3.json
- dramsim3.txt
- dramsim3epoch.json

Various metrics will be given in dramsim3.txt. For our research, we focused on the following metrics:  
- num_writes_done  
- num_reads_done  
- num_write_row_hits  
- num_read_row_hits  
- average_read_latency  
- average_power  
- average_bandwidth

## GAPBS
The BFS algorithm given in GAPBS was used to generate trace files to be used as workloads for our research. The source code was modified slighlty to remove extra instructions when running BFS, such as logging instructions.

## PIN
Using the PIN API, the source code of the MemTrace tool was modified to generate a trace file of specifically the portion of the BFS program, from GAPBS, that performed BFS. For each instruction, the memory address accessed, memory access type, and cycle the instruction was executed were collected.  

When in the MemTrace directory in pin, a trace file can be generated with a command like the following:  
```
../../../pin -t obj-intel64/memtrace.so -o testing_op.trace -emit 1 -- ../../../../gapbs/bfs -g 20 -k 20 -n 1
```
The options used in the command above are:  
- \-t [PIN tool to use]
- \-o [Name of output file]
- \-emit [Emit a trace in output file (0 = No, 1 = Yes)]
- \-\- [Executable to run]

The trace file generated from PIN, gives the memory access type as `R` or `W` for read and write, respectively. Also, in the genernated trace file, the cycle number for each instruction is the current cycle number stored in the Time Stamp Counter (TCP) register when the instruction was tracked by PIN. Since DRAMSim3 expects the memory access type to be given as `READ` or `WRITE`and to adjust the cycle number of each instruction, `format_trace.py` was created and used.  

In `format_trace.py`, replace the value of `inputFileName` with the name of the trace file to format and `outputFileName` with the name for the output file. Running `python3 format_trace.py` will output the formatted trace file. 
