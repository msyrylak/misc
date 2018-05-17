#include "CPU.h"

CPU::CPU(BusRead read, BusWrite write)
{
	Read = (BusRead)read;
	Write = (BusWrite)write;
	//Instr instr;


	//instr.addr = &CPU::Addr_IMP;
	//instr.code = &CPU::Op_BRK;
	//InstrTable[0x00] = instr;

	//instr.addr = &CPU::Addr_IMM;
	//instr.code = &CPU::Op_LDA;
	//InstrTable[0xA9] = instr;

	//instr.addr = &CPU::Addr_ABS;
	//instr.code = &CPU::Op_STA;
	//InstrTable[0x8D] = instr;

	//instr.addr = &CPU::Addr_ABS;
	//instr.code = &CPU::Op_ADD;
	//InstrTable[0x01] = instr;


	OpCodes[0x00] = &CPU::Op_BRK;
	OpCodes[0x01] = &CPU::Op_ADD;
	OpCodes[0x02] = &CPU::Op_LDA;
	OpCodes[0x03] = &CPU::Op_STA;
	OpCodes[0x04] = &CPU::Op_MOV;

	AddressModes[0x00] = &CPU::Addr_ABS;
	AddressModes[0x01] = &CPU::Addr_IMM;
	AddressModes[0x02] = &CPU::Addr_IMP;

	InstructionSet[0x00] = InstructionManager(3, 0);
	InstructionSet[0xA9] = InstructionManager(2, 1);
	



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
	bool flagCheck = SR & FLG_CARRY;
	return flagCheck;
}


bool CPU::IfZero()
{
	bool flagCheck = SR & FLG_ZERO;
	return flagCheck;
}


bool CPU::IfInterrupt()
{
	bool flagCheck = SR & FLG_INTERRUPT;
	return flagCheck;
}


bool CPU::IfBreak()
{
	bool flagCheck = SR & FLG_BREAK;
	return flagCheck;
}


bool CPU::IfOverflow()
{
	bool flagCheck = SR & FLG_OVERFLOW;
	return flagCheck;
}


bool CPU::IfNegative()
{
	bool flagCheck = SR & FLG_NEGATIVE;
	return flagCheck;
}


void CPU::Reset()
{
	R1 = 0x00;
	R2 = 0x00;
	R3 = 0x00;

	PC = (Read(rstVectorH) << 8) + Read(rstVectorL); // load PC from reset vector

	SP = 0xFD;

	SR &= 0;

	cycles = 6; // according to the datasheet, the reset routine takes 6 clock cycles


	return;

}

void CPU::Run(uint32_t n)
{
	uint32_t start = cycles;
	uint8_t opcode;
	uint8_t instr;

	while(start + n > cycles && !illegalOpcode)
	{
		// fetch
		opcode = Read(PC++);

		// decode
		instr = InstructionSet[opcode];

		// execute
		Exec(instr);

		cycles++;
	}

}


void CPU::Exec(uint8_t i)
{
	//uint16_t src = (this->*i.addr)();
	//(this->*i.code)(src);

	// decode opcode and address mode
	uint8_t opCode = i & 0x0F;
	uint8_t addressMd = i >> 4;

	(*this.*(GetAddress(addressMd)))();
	(*this.*(GetOpCode(opCode)))(0);
}


// imediate
uint16_t CPU::Addr_IMM()
{
	return PC++;
}

// implied
uint16_t CPU::Addr_IMP()
{
	return 0;
}

// absolute
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

void CPU::Op_MOV(uint16_t src)
{
	Write(src, R1);
}

// stack operations
void CPU::StackPush(uint8_t byte)
{
	Write(0x0100 + SP, byte);

	if(SP == 0x00)
	{
		SP = 0xFF;
	}
	else
	{
		SP--; 
	}
}


uint8_t CPU::StackPop(uint8_t byte)
{
	if(SP = 0xFF)
	{
		SP = 0x00;
	}
	else
	{
		SP++;
	}

	return Read(0x0100 + SP);
}

uint8_t CPU::InstructionManager(uint8_t opCode, uint8_t address)
{
	uint8_t lowBits;
	uint8_t highBits;
	uint8_t instruction;

	// write bit
	lowBits = opCode;
	highBits = address << 4;

	instruction = lowBits | highBits;

	return instruction;

}


void CPU::InstructionManager(uint8_t instruction)
{
	// read bit
	uint8_t lowBits = instruction & 0x0F;
	uint8_t highBits = instruction >> 4;
	
}

CPU::AddressMode CPU::GetAddress(uint8_t addressMd)
{
	return AddressModes[addressMd];
}

CPU::OpCode CPU::GetOpCode(uint8_t opCode)
{
	return OpCodes[opCode];
}