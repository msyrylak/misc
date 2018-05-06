#ifndef CPU_H
#define CPU_H
#include <iostream>
#include <cstdint>


class CPU
{
public:
	// registers
	uint8_t A;
	uint8_t B;
	uint8_t C;

	// stack pointer
	uint8_t sp;

	// program counter
	uint16_t pc;

	// status register
	uint8_t status;

	// consumed clock cycles 
	uint32_t cycles;

	typedef void (CPU::*CodeExec)(uint16_t);
	typedef uint16_t(CPU::*AddrExec)();

	typedef struct Instr
	{
		AddrExec addr;
		CodeExec code;
	};

	Instr InstrTable[256];

	bool illegalOpcode;

	void Exec(Instr i);

	void Op_BRK(uint16_t src);
	void Op_STA(uint16_t src);
	void Op_LDA(uint16_t src);

	uint16_t Addr_IMP(); // IMPLIED
	uint16_t Addr_IMM(); // IMMEDIATE
	uint16_t Addr_ABS(); // ABSOLUTE


	// read/write callbacks
	typedef void(*BusWrite)(uint16_t, uint8_t);
	typedef uint8_t(*BusRead)(uint16_t);
	BusRead Read;
	BusWrite Write;

	// reset vectors
	static const uint16_t rstVectorH = 0xFFFD;
	static const uint16_t rstVectorL = 0xFFFC;

	CPU(BusRead read, BusWrite write);
	~CPU();
	void Reset();
	void Run(uint32_t n);

};


#endif