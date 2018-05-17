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

	const uint8_t FLG_CARRY = 1 << CARRY_SHIFT;
	const uint8_t FLG_ZERO = 1 << ZERO_SHIFT;
	const uint8_t FLG_INTERRUPT = 1 << INTERRUPT_SHIFT;
	const uint8_t FLG_BREAK = 1 << BREAK_SHIFT;
	const uint8_t FLG_OVERFLOW = 1 << OVERFLOW_SHIFT;
	const uint8_t FLG_NEGATIVE = 1 << NEGATIVE_SHIFT;

	// flag sets

	void SetCarry(uint8_t x);
	void SetZero(uint8_t);
	void SetInterrupt(uint8_t);
	void SetBreak(uint8_t);
	void SetOverflow(uint8_t);
	void SetNegative(uint8_t);

	// flag checks
	bool IfCarry();
	bool IfZero();
	bool IfInterrupt();
	bool IfBreak();
	bool IfOverflow();
	bool IfNegative();



public:
	// registers
	uint8_t R1;
	uint8_t R2;
	uint8_t R3;

	// stack pointer
	uint8_t SP;

	// program counter
	uint16_t PC;

	// status register
	uint8_t SR;

	// consumed clock cycles 
	uint32_t cycles;

	// instruction
	uint8_t instruction;

	// instructions table
	uint8_t InstructionSet[256];

	typedef void(CPU::*OpCode)(uint16_t);
	OpCode OpCodes[16];

	typedef uint16_t(CPU::*AddressMode)();
	AddressMode AddressModes[8];

	//typedef void (CPU::*CodeExec)(uint16_t);
	//typedef uint16_t(CPU::*AddrExec)();

	//typedef struct Instr
	//{
	//	AddrExec addr;
	//	CodeExec code;
	//};

	//Instr InstrTable[256];

	bool illegalOpcode;

	void Exec(uint8_t i);

	void Op_BRK(uint16_t src);
	void Op_STA(uint16_t src);
	void Op_LDA(uint16_t src);
	void Op_ADD(uint16_t src);
	void Op_MOV(uint16_t src);

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

	// stack operations
	void StackPush(uint8_t byte);
	uint8_t StackPop(uint8_t byte);

	uint8_t InstructionManager(uint8_t opCode, uint8_t address);
	void InstructionManager(uint8_t instruction);

	AddressMode GetAddress(uint8_t addressMd);
	OpCode GetOpCode(uint8_t opCode);
};


#endif