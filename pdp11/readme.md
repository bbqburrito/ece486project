Thao Tran, Jonathan Anchel, Dai Ho, William Boyd
ECE 486 final project Winter 2018
I. File list
------------------------------------------------------
double_operand.cpp      Implements all functions in double_operand class - all functions 
                        for double operand instructions
pdp11.cpp               Implements all other functions, including all functions for classes 
                        other than double_operand and all file reader/writer and "helper" functions
main.cpp                Driver for pdp11
pdp11.h                 header for entire project
Makefile                builds pdp11

Program can be built using default make arguments

II. Command line
-----------------------------------------------------
usage: ./pdp11 [-v] <input file> <trace file> [-s startpoint (int octal)]
-v  verbose mode:
    displays condition codes and contents of each register before and after every instruction is 
    executed.

-s  specify start point:
    allows for entering start point on command line. Start point interpreted as octal int

example: ./pdp11 -sv addc.ascii tracefile.out 4

III. Design
----------------------------------------------------
    A. Program Design
    1. Data structures
    The program uses one dimensional arrays for all collections of data. Most are statically 
    allocated, but one is dynamically allocated. There is a uint16_t array of 8 members to 
    represent the registers and a 64K member array of i_cache, which is a structure consiting of 
    a char and a uint16_t, to represent the program memory. While the memory of the pdp11 is 
    byte-addressable, with an even byte corresponding to the low byte of a 16 bit word and an
    odd byte corresponding the the high byte, we used 64K array of 16 bit integers, with
    the high byte and low byte set equal to each other. This was to simplify management of word
    size data, since we were developing only functions dealing with word size data.

    2. Classes
    Each instruction type was represented by a class, and each of those classes derives from 
    a base class. Most functions were virtualized so that polymorphism could be used when 
    sorting instructions to functions. The base class holds data members that identify char *
    names of output files, and each class had data members corresponding to registers, register
    modes, etc. appropriate to the instruction type. For example, the single operand class has
    data members for the function code, the register, and the register mode. The branch class 
    has data members for the funciton code and the offset.

    3. Parsing instructions
    The program is first loaded line by line into a memory array, then the main function gets
    the start point, then steps through each command stored in memory as pointed to by register 7.
    The execution loop first sends the contents of the memory location at the PC to an interpreter
    function, which uses downcasting to create an object of the appropriate class. This object is
    then sent back to the execution loop and the main function calls class functions through the
    command variable. 

    4. Files created and read
    The program reads data from an ascii file, which is specified on the command line, and writes
    to a trace file, which is also specified on the command line. It also writes to a branch 
    trace file, called branch_trace.out unless specified during program execution. Before exiting,
    the program writes the contents of the memory to a file named memory.txt.

    
