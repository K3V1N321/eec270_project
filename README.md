# eec270_project
Using DRAMSim3, we evaluated the effectiveness of implementing permutation-based page interleacing, adjusting timine parameters, changing the address mapping scheme, and using a simultaneous refresh policy in improving the performance of DDR4 DRAM, with a size of 4 Gb, a 4 bit bus, and a megatransfer rate of 2400.  

## DRAMsim3 [1]
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

## GAPBS [6]
The BFS algorithm given in GAPBS was used to generate trace files to be used as workloads for our research. The source code was modified slighlty to remove extra instructions when running BFS, such as logging instructions.
To prevent issues that would arise from tracking the executable when ran in parallel, the programs were compiled with make SERIAL=1 to make them run serially.

## PIN [5]
Using the PIN API, the source code of the MemTrace tool was modified to generate a trace file of specifically the portion of the BFS program, from GAPBS, that performed BFS. For each instruction, the memory address accessed, memory access type, and cycle the instruction was executed were collected.  

In `memtrace.cpp`, the function `LoadImage()` was used to ensure that only the instructions executed for the function that performs BFS was tracked. The line `RTN routine = RTN_FindByName(image, "_Z5DOBFSRK8CSRGraphIiiLb1EEibii");` was used to find the BFS function in the given executable to run. Then once the BFS function was encountered, a global variable was set to make PIN start tracking, and once the function finished, the global variable was set to make PIN stop tracking.  
With the BFS executable, the function that performed BFS was `DOBFS()`. In the gapbs directory, `nm -C bfs | grep DOBFS` was ran to get the memory address of where the function is located. Then `nm bfs | grep BFS` was ran to get the encoded name of the function, `_Z5DOBFSRK8CSRGraphIiiLb1EEibii`, checking that it is at the correct memory address. The encoded name might have the function name spread apart, which is why we first found the function's memory address and then used `grep` to search for a portion of the function name when finding the encoded name.

When in the MemTrace directory in pin, a trace file can be generated with a command like the following:  
```
../../../pin -t obj-intel64/memtrace.so -o testing_op.trace -emit 1 -- ../../../../gapbs/bfs -g 20 -k 20 -n 1
```
The options used in the command above are:  
- \-t [PIN tool to use]
- \-o [Name of output file]
- \-emit [Emit a trace in output file (0 = No, 1 = Yes)]
- \-\- [Executable to run]

The trace file generated from PIN, gives the memory access type as `R` or `W` for read and write, respectively. Also, in the genernated trace file, the cycle number for each instruction is the current cycle number stored in the Time Stamp Counter (TCP) register when the instruction was tracked by PIN. Since DRAMSim3 expects the memory access type to be given as `READ` or `WRITE`and to adjust the cycle number of each instruction, `format_trace.py` was created and used to format the trace file.  

In `format_trace.py`, replace the value of `inputFileName` with the name of the trace file to format and `outputFileName` with the name for the output file. Running `python3 format_trace.py` will output the formatted trace file.  

## References
[1] S. Li, Z. Yang, D. Reddy, A. Srivastava, and B. Jacob, “Dramsim3: A
cycle-accurate, thermal-capable dram simulator,” IEEE Computer Archi-
tecture Letters, vol. 19, no. 2, pp. 106–109, 2020.  
[2] R. Balasubramonian, Innovations In The Memory System. Springer
Nature, 2022.  
[3] O. Mutlu and T. Moscibroda, “Parallelism-aware batch scheduling: En-
hancing both performance and fairness of shared dram systems,” in 2008
International Symposium on Computer Architecture, 2008, pp. 63–74.  
[4] D. Lee, Y. Kim, G. Pekhimenko, S. Khan, V. Seshadri, K. Chang,
and O. Mutlu, “Adaptive-latency dram: Optimizing dram timing for the
common-case,” in 2015 IEEE 21st International Symposium on High
Performance Computer Architecture (HPCA). IEEE, 2015, pp. 489–501.  
[5] C.-K. Luk, R. Cohn, R. Muth, H. Patil, A. Klauser, G. Lowney,
S. Wallace, V. J. Reddi, and K. Hazelwood, “Pin: building customized
program analysis tools with dynamic instrumentation,” Acm sigplan
notices, vol. 40, no. 6, pp. 190–200, 2005.  
[6] S. Beamer, K. Asanovi´c, and D. Patterson, “The gap benchmark suite,”
arXiv preprint arXiv:1508.03619, 2015.  
[7] Z. Zhang, Z. Zhu, and X. Zhang, “A permutation-based page interleaving
scheme to reduce row-buffer conflicts and exploit data locality,” in
Proceedings of the 33rd annual ACM/IEEE international symposium on
Microarchitecture, 2000, pp. 32–41.  
[8] S.-K. Park, “Technology scaling challenge and future prospects of dram
and nand flash memory,” in 2015 IEEE international memory workshop
(IMW). IEEE, 2015, pp. 1–4
