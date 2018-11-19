#ifndef SOC_H
#define SOC_H
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl2.h"
#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <map>


class SOC
{
	// flags
	static const uint8_t CARRY_SHIFT = 0;
	static const uint8_t ZERO_SHIFT = 1;
	static const uint8_t INTERRUPT_SHIFT = 2;
	static const uint8_t BREAK_SHIFT = 4;
	static const uint8_t OVERFLOW_SHIFT = 6;
	static const uint8_t NEGATIVE_SHIFT = 7;

	static const uint8_t FLG_CARRY = 1 << CARRY_SHIFT;
	static const uint8_t FLG_ZERO = 1 << ZERO_SHIFT;
	static const uint8_t FLG_INTERRUPT = 1 << INTERRUPT_SHIFT;
	static const uint8_t FLG_BREAK = 1 << BREAK_SHIFT;
	static const uint8_t FLG_OVERFLOW = 1 << OVERFLOW_SHIFT;
	static const uint8_t FLG_NEGATIVE = 1 << NEGATIVE_SHIFT;

	// flag sets
	void SetFlags(uint8_t x, const uint8_t flag);
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

	// memory
	uint8_t myMem[65536];
	std::map<uint16_t,uint16_t> changes;
	uint16_t highlightbyte = 1;

	// registers
	uint8_t R1; // code 0x00
	uint8_t R2; //code 0x01
	uint8_t R3; //code 0x02
	uint8_t *RegCodes[4];

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

	typedef void(SOC::*OpCode)(uint8_t, uint16_t);
	OpCode OpCodes[16];

	typedef uint16_t(SOC::*AddressMode)();
	AddressMode AddressModes[8];


	void Execute(uint8_t i);

	void Op_BRK(uint8_t reg, uint16_t src);
	void Op_ST(uint8_t reg, uint16_t src);
	void Op_LD(uint8_t reg, uint16_t src);
	void Op_ADD(uint8_t reg, uint16_t src);
	void Op_JMP(uint8_t reg, uint16_t src);
	void Op_JPC(uint8_t reg, uint16_t src);
	void Op_JPZ(uint8_t reg, uint16_t src);
	void Op_JPN(uint8_t reg, uint16_t src);
	void Op_PH(uint8_t reg, uint16_t src);
	void Op_PL(uint8_t reg, uint16_t src);
	void Op_AND(uint8_t reg, uint16_t src);
	void Op_XOR(uint8_t reg, uint16_t src);
	void Op_CLC(uint8_t reg, uint16_t src);


	uint16_t Addr_IMP(); // IMPLIED
	uint16_t Addr_IMM(); // IMMEDIATE
	uint16_t Addr_ABS(); // ABSOLUTE

	uint8_t Read(uint16_t addr);
	void Write(uint16_t addr, uint8_t val);

	// reset vectors
	static const uint16_t rstVectorH = 0xFFFD;
	static const uint16_t rstVectorL = 0xFFFC;
	static const uint16_t irqVectorH = 0xFFFF;
	static const uint16_t irqVectorL = 0xFFFE;

	SOC();
	~SOC();
	void Reset();
	void Run(uint32_t n);

	// stack operations
	void StackPush(uint8_t byte);
	uint8_t StackPop();

	uint8_t InstructionManager(uint8_t opCode, uint8_t address, uint8_t reg);

	AddressMode GetAddress(uint8_t addressMd);
	OpCode GetOpCode(uint8_t opCode);

	void SaveMemory(uint8_t* mem, int size);
	void LoadMemory(uint8_t* mem, int size);
};


#endif