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


	Reset();
	return;
}


CPU::~CPU()
{
}

void CPU::Reset()
{
	A = 0x00;
	B = 0x00;
	C = 0x00;

	pc = (Read(rstVectorH) << 8) + Read(rstVectorL); // load PC from reset vector

	sp = 0xFD;

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
		opcode = Read(pc++);

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
	return pc++;
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

	addrL = Read(pc++);
	addrH = Read(pc++);

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
	A = m;
}

void CPU::Op_STA(uint16_t src)
{
	Write(src, A);
	return;
}