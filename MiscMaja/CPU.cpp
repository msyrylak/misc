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

}

void CPU::Run(uint32_t n)
{

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

void CPU::Op_BRK()