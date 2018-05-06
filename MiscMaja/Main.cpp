#include "CPU.h"
#include <iostream>

uint8_t myMem[65536];

void BusWrite(uint16_t addr, uint8_t val)
{
	myMem[addr] = val;
}


uint8_t BusRead(uint16_t addr)
{
	return myMem[addr];
}


int main()
{
	myMem[CPU::rstVectorH] = 0;
	myMem[CPU::rstVectorL] = 0;
	myMem[0] = 169;
	myMem[1] = 5;
	myMem[2] = 0x8D;
	myMem[3] = 10;
	CPU test(BusRead, BusWrite);
	test.Reset();
	test.Run(2);

	return 0;
}