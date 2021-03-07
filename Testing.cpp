#include <bitset>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <set>
#include <sstream>
#include <queue>

#include "riscVInstruction.h"

int main()
{
	while (1)
	{
		unsigned int in;
		std::cout << "enter: ";
		std::cin >> std::hex >> in;
		// auto instr = new RiscV32Instruction(0x1e0e13);
		auto instr = new RiscV32Instruction(in);
		// std::cout <<
		std::cout << instr->getInstrName() << std::endl;
		for (auto f : instr->getInstrFieldNames())
		{
			std::cout << f << ": " << std::hex << instr->getInstrFieldVal(f) << std::endl;
		}
	}
	return 0;
}