#include "SOC.h"

SOC::SOC()
{
	OpCodes[0x00] = &SOC::Op_BRK;
	OpCodes[0x01] = &SOC::Op_ADD;
	OpCodes[0x02] = &SOC::Op_LD;
	OpCodes[0x03] = &SOC::Op_ST;
	OpCodes[0x04] = &SOC::Op_JMP;
	OpCodes[0x05] = &SOC::Op_JPC;
	OpCodes[0x06] = &SOC::Op_JPZ;
	OpCodes[0x07] = &SOC::Op_JPN;
	OpCodes[0x08] = &SOC::Op_PH;
	OpCodes[0x09] = &SOC::Op_PL;
	OpCodes[0x0A] = &SOC::Op_AND;
	OpCodes[0x0B] = &SOC::Op_XOR;
	OpCodes[0x0C] = &SOC::Op_CLC;

	AddressModes[0x00] = &SOC::Addr_ABS;
	AddressModes[0x01] = &SOC::Addr_IMM;
	AddressModes[0x02] = &SOC::Addr_IMP;

	RegCodes[0x00] = &R1;
	RegCodes[0x01] = &R2;
	RegCodes[0x02] = &R3;

	InstructionSet[0x00] = InstructionManager(0x00, 0x00, 0x00); // BRK
	InstructionSet[0x01] = InstructionManager(0x04, 0x00, 0x00); // JMP
	InstructionSet[0x02] = InstructionManager(0x05, 0x01, 0x00); // JPC
	InstructionSet[0x03] = InstructionManager(0x06, 0x01, 0x00); // JPZ
	InstructionSet[0x04] = InstructionManager(0x07, 0x01, 0x00); // JPN
	InstructionSet[0x05] = InstructionManager(0x01, 0x01, 0x00); // ADD
	InstructionSet[0x06] = InstructionManager(0x0B, 0x01, 0x00); // XOR
	InstructionSet[0x07] = InstructionManager(0x0C, 0x02, 0x00); // CLC
	InstructionSet[0x08] = InstructionManager(0x08, 0x02, 0x00); // PH
	InstructionSet[0x09] = InstructionManager(0x09, 0x02, 0x00); // PL
	InstructionSet[0x0A] = InstructionManager(0x0A, 0x02, 0x00); // AND
	InstructionSet[0xA9] = InstructionManager(0x02, 0x01, 0x00); // LD
	InstructionSet[0x8D] = InstructionManager(0x03, 0x00, 0x00); // ST

	//myMem[SOC::rstVectorH] = 0;
	//myMem[SOC::rstVectorL] = 0;
	//myMem[0] = 169; //load 
	//myMem[1] = 5;   //5 to R1
	//myMem[2] = 0x06; //XOR R1 
	//myMem[3] = 0xFF; //with 255
	//myMem[4] = 0x05; //ADD 
	//myMem[5] = 1; //one to make it negative (two's complement)
	//myMem[6] = 0x07; //clear carry flag
	//myMem[7] = 0x05; //add
	//myMem[8] = 10; //add 10 to -5
	//myMem[9] = 0x8D; //store it
	//myMem[10] = 12; //on 12 position in the memory
	//myMem[11] = 0;

	Reset();
	return;
}


SOC::~SOC()
{
}


void SOC::SetFlags(uint8_t x, const uint8_t flag)
{
	x ? (SR |= flag) : (SR &= (~flag));
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
	changes.insert(std::pair<uint16_t, uint16_t>(addr, myMem[addr]));
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

	changes.erase(changes.begin(), changes.end());

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
	uint8_t addressMd = i >> 6;
	uint8_t regBit = (i >> 4) & 3;

	uint16_t src = (*this.*(GetAddress(addressMd)))();
	(*this.*(GetOpCode(opCode)))(regBit, src);
}


// immediate
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


void SOC::Op_BRK(uint8_t reg, uint16_t src)
{
	//pc++;
	//StackPush((pc >> 8) & 0xFF);
	//StackPush(pc & 0xFF);
	//StackPush(status | BREAK);
	//SET_INTERRUPT(1);
	//pc = (Read(irqVectorH) << 8) + Read(irqVectorL);
	//return;
}

void SOC::Op_LD(uint8_t reg, uint16_t src)
{
	uint8_t m = Read(src);
	SetNegative(m & 0x80);
	SetZero(!m);
	*RegCodes[reg] = m;
}

void SOC::Op_ST(uint8_t reg, uint16_t src)
{
	Write(src, *RegCodes[reg]);
}

void SOC::Op_ADD(uint8_t reg, uint16_t src)
{
	uint16_t m = Read(src);
	unsigned int tmp = m + *RegCodes[reg] + (IfCarry() ? 1 : 0 );
	SetCarry(tmp > 0xFF);
	SetZero(!(tmp & 0xFF));
	*RegCodes[reg] = tmp & 0xFF;
}

void SOC::Op_JMP(uint8_t reg, uint16_t src)
{
	PC = src;
}

void SOC::Op_JPC(uint8_t reg, uint16_t src)
{
	if(IfCarry())
	{
		PC += src;
	}
}

void SOC::Op_JPZ(uint8_t reg, uint16_t src)
{
	if(IfZero())
	{
		PC += src;
	}
}

void SOC::Op_JPN(uint8_t reg, uint16_t src)
{
	if(IfNegative())
	{
		PC += src;
	}
}

void SOC::Op_PH(uint8_t reg, uint16_t src)
{
	StackPush(*RegCodes[reg]);
}

void SOC::Op_PL(uint8_t reg, uint16_t src)
{
	*RegCodes[reg] = StackPop();
	SetNegative(*RegCodes[reg] & 0x80);
	SetZero(!(*RegCodes[reg]));
}

void SOC::Op_AND(uint8_t reg, uint16_t src)
{
	uint8_t mem = Read(src);
	uint8_t result = *RegCodes[reg] & mem;
	SetNegative(result & 0x80);
	SetZero(!result);
	*RegCodes[reg] = result;
}

void SOC::Op_XOR(uint8_t reg, uint16_t src)
{
	uint8_t mem = Read(src);
	uint8_t result = *RegCodes[reg] ^ mem;
	SetNegative(result & 0x80);
	SetZero(!result);
	*RegCodes[reg] = result;
}

void SOC::Op_CLC(uint8_t reg, uint16_t src)
{
	SetCarry(0);
}


// stack operations
void SOC::StackPush(uint8_t byte)
{
	Write(0x0100 + SP, byte);
	SP--;
}


uint8_t SOC::StackPop()
{
	SP++;
	return Read(0x0100 + SP);
}


uint8_t SOC::InstructionManager(uint8_t opCode, uint8_t address, uint8_t reg)
{
	uint8_t opBits;
	uint8_t addrBits;
	uint8_t regBits;
	uint8_t instruction;

	// write bit
	opBits = opCode;
	addrBits = address << 6;
	regBits = reg << 4;

	instruction = addrBits| regBits | opBits;

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

void SOC::SaveMemory(uint8_t* mem, int size)
{
	std::fstream outfile;
	outfile.open("memory.dat", std::ios::binary | std::ios::out);
	if(outfile)
	{
		for(int i = 0; i < size; i++)
		{
			outfile.write((char *)&mem[i], sizeof(uint8_t));
		}
	}
	else
	{
		std::cout << "No file found!";
	}
}

void SOC::LoadMemory(uint8_t* mem, int size)
{
	std::fstream infile;
	infile.open("memory.dat", std::ios::binary | std::ios::in);
	if(infile)
	{
		for(int i = 0; i < size; i++)
		{
			infile.read((char *)&mem[i], sizeof(uint8_t));
		}
	}
	else
	{
		std::cout << "No file found!";
	}

}
