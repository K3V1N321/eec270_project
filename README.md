# eec270_project
Using DRAMSim3, we evaluated the effectiveness of implementing permutation-based page interleacing, adjusting timine parameters, changing the address mapping scheme, and using a simultaneous refresh policy in improving the performance of DDR4 DRAM, with a size of 4 Gb, a 4 bit bus, and a megatransfer rate of 2400.  

## DRAMsim3
The simulator ran to evaluate the effectiveness of the optimizations.  

The simulator was ran with a given trace file by using a command like:  
```
./build/dramsim3main configs/DDR4_4Gb_x4_2400.ini -c 100000 -t trace.txt
```

Running the simulator on a trace gives various metrics of the run. For our research, we focused on the following metrics:  
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
format.py was create to format the generated trace file into the correct format for DRAMSim3.
