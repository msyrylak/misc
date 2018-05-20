#include "SOC.h"

SOC::SOC()
{
	OpCodes[0x00] = &SOC::Op_BRK;
	OpCodes[0x01] = &SOC::Op_ADD;
	OpCodes[0x02] = &SOC::Op_LD;
	OpCodes[0x03] = &SOC::Op_ST;

	AddressModes[0x00] = &SOC::Addr_ABS;
	AddressModes[0x01] = &SOC::Addr_IMM;
	AddressModes[0x02] = &SOC::Addr_IMP;

	InstructionSet[0x00] = InstructionManager(0x03, 0x00);
	InstructionSet[0xA9] = InstructionManager(0x02, 0x01);
	

	Reset();
	return;
}


SOC::~SOC()
{
}

void SOC::SetFlags(uint8_t x, const uint8_t flag)
{
	switch(flag)
	{
	case FLG_CARRY:
		x ? (SR |= FLG_CARRY) : (SR &= (~FLG_CARRY));
		break;
	case FLG_ZERO:
		x ? (SR |= FLG_ZERO) : (SR &= (~FLG_ZERO));
		break;
	case FLG_INTERRUPT:
		x ? (SR |= FLG_INTERRUPT) : (SR &= (~FLG_INTERRUPT));
		break;
	case FLG_BREAK:
		x ? (SR |= FLG_BREAK) : (SR &= (~FLG_BREAK));
		break;
	case FLG_OVERFLOW:
		x ? (SR |= FLG_OVERFLOW) : (SR &= (~FLG_OVERFLOW));
		break;
	case FLG_NEGATIVE:
		x ? (SR |= FLG_NEGATIVE) : (SR &= (~FLG_NEGATIVE));
		break;
	default:
		break;
	}
}

void SOC::SetCarry(uint8_t x)
{
	SetFlags(x, FLG_CARRY);
}


void SOC::SetZero(uint8_t x)
{
	SetFlags(x, FLG_ZERO);
}


void SOC::SetInterrupt(uint8_t x)
{
	SetFlags(x, FLG_INTERRUPT);
}


void SOC::SetBreak(uint8_t x)
{
	SetFlags(x, FLG_BREAK);
}


void SOC::SetOverflow(uint8_t x)
{
	SetFlags(x, FLG_OVERFLOW);
}


void SOC::SetNegative(uint8_t x)
{
	SetFlags(x, FLG_NEGATIVE);
}

bool SOC::IfCarry()
{
	bool flagCheck = SR & FLG_CARRY;
	return flagCheck;
}


bool SOC::IfZero()
{
	bool flagCheck = SR & FLG_ZERO;
	return flagCheck;
}


bool SOC::IfInterrupt()
{
	bool flagCheck = SR & FLG_INTERRUPT;
	return flagCheck;
}


bool SOC::IfBreak()
{
	bool flagCheck = SR & FLG_BREAK;
	return flagCheck;
}


bool SOC::IfOverflow()
{
	bool flagCheck = SR & FLG_OVERFLOW;
	return flagCheck;
}


bool SOC::IfNegative()
{
	bool flagCheck = SR & FLG_NEGATIVE;
	return flagCheck;
}


void SOC::Write(uint16_t addr, uint8_t val)
{
	myMem[addr] = val;
}


uint8_t SOC::Read(uint16_t addr)
{
	return myMem[addr];
}


void SOC::Reset()
{
	R1 = 0x00;
	R2 = 0x00;
	R3 = 0x00;

	PC = (Read(rstVectorH) << 8) + (Read(rstVectorL)); // load PC from reset vector

	SP = 0xFD;

	SR = 0x00;

	return;

}

void SOC::Run(uint32_t n)
{
	uint8_t opcode;
	uint8_t instr;

	for(uint32_t i = 0; i < n; i++)
	{
		// fetch
		opcode = Read(PC++);

		// decode
		instr = InstructionSet[opcode];

		// execute
		Exec(instr);
	}
}


void SOC::Exec(uint8_t i)
{

	// decode opcode and addressing mode
	uint8_t opCode = i & 0x0F;
	uint8_t addressMd = i >> 4;

	uint16_t src = (*this.*(GetAddress(addressMd)))();
	(*this.*(GetOpCode(opCode)))(src);
}


// imediate
uint16_t SOC::Addr_IMM()
{
	return PC++;
}

// implied
uint16_t SOC::Addr_IMP()
{
	return 0;
}

// absolute
uint16_t SOC::Addr_ABS()
{
	uint16_t addrL;
	uint16_t addrH;
	uint16_t addr;

	addrL = Read(PC++);
	addrH = Read(PC++);

	addr = addrL + (addrH << 8);

	return addr;
}


void SOC::Op_BRK(uint16_t src)
{
	//pc++;
	//StackPush((pc >> 8) & 0xFF);
	//StackPush(pc & 0xFF);
	//StackPush(status | BREAK);
	//SET_INTERRUPT(1);
	//pc = (Read(irqVectorH) << 8) + Read(irqVectorL);
	//return;
}

void SOC::Op_LD(uint16_t src)
{
	uint8_t m = Read(src);
	SetNegative(m & 0x80);
	SetZero(!m);
	R1 = m;
}

void SOC::Op_ST(uint16_t src)
{
	Write(src, R1);
	return;
}

void SOC::Op_ADD(uint16_t src)
{
	uint16_t m = Read(src);
	unsigned int tmp = m + R1 + (IfCarry() ? 1 : 0 );
	SetCarry(tmp > 0xFF);
	SetZero(!(tmp & 0xFF));
	R1 = tmp & 0xFF;
	return;
}


// stack operations
void SOC::StackPush(uint8_t byte)
{
	Write(0x0100 + SP, byte);
	SP--;
}


uint8_t SOC::StackPop(uint8_t byte)
{
	SP++;
	return Read(0x0100 + SP);
}


uint8_t SOC::InstructionManager(uint8_t opCode, uint8_t address)
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


SOC::AddressMode SOC::GetAddress(uint8_t addressMd)
{
	return AddressModes[addressMd];
}

SOC::OpCode SOC::GetOpCode(uint8_t opCode)
{
	return OpCodes[opCode];
}