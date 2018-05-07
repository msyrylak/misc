#ifndef CPU_H
#define CPU_H
#include <iostream>
#include <cstdint>


class CPU
{
	// flags
	const uint8_t CARRY_SHIFT = 0;
	const uint8_t ZERO_SHIFT = 1;
	const uint8_t INTERRUPT_SHIFT = 2;
	const uint8_t BREAK_SHIFT = 4;
	const uint8_t OVERFLOW_SHIFT = 6;
	const uint8_t NEGATIVE_SHIFT = 7;

	const uint8_t FLG_CARRY = 0;
	const uint8_t FLG_ZERO = 1;
	const uint8_t FLG_INTERRUPT = 2;
	const uint8_t FLG_BREAK = 4;
	const uint8_t FLG_OVERFLOW = 6;
	const uint8_t FLG_NEGATIVE = 7;





public:
	// registers
	uint8_t R1;
	uint8_t R2;
	uint8_t R3;

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
	void Op_ADD(uint16_t src);

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
	static const uint16_t irqVectorH = 0xFFFF;
	static const uint16_t irqVectorL = 0xFFFE;

	CPU(BusRead read, BusWrite write);
	~CPU();
	void Reset();
	void Run(uint32_t n);

};


#endif