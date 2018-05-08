#include "CPU.h"

CPU::CPU(BusRead read, BusWrite write)
{
	Read = (BusRead)read;
	Write = (BusWrite)write;
	Instr instr;


	instr.addr = &CPU::Addr_IMP;
	instr.code = &CPU::Op_BRK;
	InstrTable[0x00] = instr;

	instr.addr = &CPU::Addr_IMM;
	instr.code = &CPU::Op_LDA;
	InstrTable[0xA9] = instr;

	instr.addr = &CPU::Addr_ABS;
	instr.code = &CPU::Op_STA;
	InstrTable[0x8D] = instr;

	instr.addr = &CPU::Addr_ABS;
	instr.code = &CPU::Op_ADD;
	InstrTable[0x01] = instr;

	Reset();
	return;
}


CPU::~CPU()
{
}

void CPU::SetCarry(uint8_t x)
{
	x ? (SR |= FLG_CARRY) : (SR &= (~FLG_CARRY));
}


void CPU::SetZero(uint8_t x)
{
	x ? (SR |= FLG_ZERO) : (SR &= (~FLG_ZERO));
}


void CPU::SetInterrupt(uint8_t x)
{
	x ? (SR |= FLG_INTERRUPT) : (SR &= (~FLG_INTERRUPT));
}


void CPU::SetBreak(uint8_t x)
{
	x ? (SR |= FLG_BREAK) : (SR &= (~FLG_BREAK));
}


void CPU::SetOverflow(uint8_t x)
{
	x ? (SR |= FLG_OVERFLOW) : (SR &= (~FLG_OVERFLOW));
}


void CPU::SetNegative(uint8_t x)
{
	x ? (SR |= FLG_NEGATIVE) : (SR &= (~FLG_NEGATIVE));
}

bool CPU::IfCarry()
{
	bool flagTest = SR & FLG_CARRY;
	return flagTest;
}


bool CPU::IfZero()
{
	bool flagTest = SR & FLG_ZERO;
	return flagTest;
}


bool CPU::IfInterrupt()
{
	bool flagTest = SR & FLG_INTERRUPT;
	return flagTest;
}


bool CPU::IfBreak()
{
	bool flagTest = SR & FLG_BREAK;
	return flagTest;
}


bool CPU::IfOverflow()
{
	bool flagTest = SR & FLG_OVERFLOW;
	return flagTest;
}


bool CPU::IfNegative()
{
	bool flagTest = SR & FLG_NEGATIVE;
	return flagTest;
}


void CPU::Reset()
{
	R1 = 0x00;
	R2 = 0x00;
	R3 = 0x00;

	PC = (Read(rstVectorH) << 8) + Read(rstVectorL); // load PC from reset vector

	SP = 0xFD;

	//status |= CONSTANT;

	cycles = 6; // according to the datasheet, the reset routine takes 6 clock cycles


	return;

}

void CPU::Run(uint32_t n)
{
	uint32_t start = cycles;
	uint8_t opcode;
	Instr instr;

	while(start + n > cycles && !illegalOpcode)
	{
		// fetch
		opcode = Read(PC++);

		// decode
		instr = InstrTable[opcode];

		// execute
		Exec(instr);

		cycles++;
	}

}


void CPU::Exec(Instr i)
{
	uint16_t src = (this->*i.addr)();
	(this->*i.code)(src);
}


uint16_t CPU::Addr_IMM()
{
	return PC++;
}

uint16_t CPU::Addr_IMP()
{
	return 0;
}

uint16_t CPU::Addr_ABS()
{
	uint16_t addrL;
	uint16_t addrH;
	uint16_t addr;

	addrL = Read(PC++);
	addrH = Read(PC++);

	addr = addrL + (addrH << 8);

	return addr;
}

void CPU::Op_BRK(uint16_t src)
{
	//pc++;
	//StackPush((pc >> 8) & 0xFF);
	//StackPush(pc & 0xFF);
	//StackPush(status | BREAK);
	//SET_INTERRUPT(1);
	//pc = (Read(irqVectorH) << 8) + Read(irqVectorL);
	//return;
}

void CPU::Op_LDA(uint16_t src)
{
	uint8_t m = Read(src);
	//SET_NEGATIVE(m & 0x80);
	//SET_ZERO(!m);
	R1 = m;
}

void CPU::Op_STA(uint16_t src)
{
	Write(src, R1);
	return;
}

void CPU::Op_ADD(uint16_t src)
{
	uint16_t tmp = Read(src);
	unsigned int tmp2 = tmp + R1;
	SetCarry(tmp2 > 0xFF);
	SetZero(!(tmp2 & 0xFF));
	return;
}