#pragma once
#include <bitset>
#include <list>
#include <map>

class RiscV32Instruction
{
private:
	unsigned int instr;
	char instrType;
	std::string instrName;
	// std::bitset<32> instrInBinary;
	std::map<std::string, unsigned>* fieldMap;

public:
	RiscV32Instruction(unsigned int instr);

	~RiscV32Instruction();
	
	//Returns the full 32-bit instruction
	unsigned int getInstr();

	//Returns the instruction in a binary representation 
	// std::bitset<32> getInstrInBinary();
	
	//Returns the OpCode of the instruction
	unsigned int getOp();
	
	//Returns either I, R, B, L, S, U, J, O(Other)
	//Other is usually System but can be a custom or future type
	char getInstrType();

	//Return the name of the instruction.
	//e.g. add, sub, addi, lw, etc...
	std::string getInstrName();

	//Returns all the fields names required for this instructions
	//e.g. {funct7, rs2, rs1, funct3, rd, opcode} if this was an R-Type
	std::list<std::string> getInstrFieldNames() const;

	//Returns all the field values required for this instructions
	//e.g. {0, 31, 5, 0, 31, 51} if instr == 0x1f28fb3 == add t6, t0, t6
	unsigned getInstrFieldVal(std::string fieldName) const;
};

