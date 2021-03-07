# RiscV-MachineCode-Instruction-Analysis
A computer architecture project. This program will analyze a line separated list RISCV machine code instructions dumped from the RARs Risc IDE instruction dump tool. 
The most significant and useful portion of this project is the riscVInstruction class. 
It is capable of decoding a Risc machine code instruction down to its separate fields and values among other useful functions.


## Files
### riscVInstruction.h/.cpp
This class will decode a RiscV instruction numerical value passed to it. 
It has useful functions like getOp() which returns its opcode
getInstrType() which tells you if its an I-Type, R-Type, etc...
getInstrFieldNames(): Returns a list of the fields this instruction has based on its type
getInstrFieldValue(*fieldName*): Returns the value of the specified field
and more...


### Testing.cpp
A simple bash file that allows you to enter any RiscV instruction and them prints out all relavent information on that instruction


### analyzelog.cpp
The analysis program I wrote for my class. Its not documented too great, but runs an analysis on the whole log file you pass it and prints out some useful information on it.

![Example Printout](https://github.com/NoahTarr/RiscV-MachineCode-Instruction-Analysis/blob/main/example_analysis_printout.png)


### Sample instruction logs
A couple log dumps from my own projects. One of them is very small, only 200 lines or so, and the other is quite large, around 500k lines. The larger one is a dump from a merge sort and binary search project.


### Sample risc project
A full example project that contains merge sort, insert sort, binary search, file read, array compare, and a main function to run them. Note these only work on a file 
that contains exactly 1024 line seperated unsigned integers. You can alter the functions to account for different amounts relatively easily if you know risc.
