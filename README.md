# misc
C++ Minimal Instruction Set Computer Simulator (MISC)

Inspired by MOS 6502 and other retro CPUs, with memory editor highlighting byte or bytes that are currently in use. It simulates an 8-bit processor capable of addressing 64KB of memory  three main registers (R1, R2, R3) and also a Status Register (SR), Program Counter (PC) and a Stack Pointer (SP) with a descending stack default value of 0xFD (253). The Instruction Set can fit up to 256 instructions, each of which consist of an addressing mode, the register it’s working on and the operation to perform - all potentially combined in different configurations. 

![GUI of the Simulator](https://github.com/msyrylak/misc/blob/master/miscGUI.PNG)
