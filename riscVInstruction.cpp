#include "riscVInstruction.h"

RiscV32Instruction::RiscV32Instruction(unsigned int instr)
{
	this->instr = instr;
	// this->instrInBinary = std::bitset<32>(instr);
	this->fieldMap = new std::map<std::string, unsigned int>();

	//Every instruction has its first 7 bits set to its opcode
	// fieldMap->insert(std::pair<std::string, unsigned int>("opcode", 0x7F & instr));

	switch (0x7F & instr)
	{
	case 0b0010011:
		//I
		this->instrType = 'I';

		fieldMap->insert(std::pair<std::string, unsigned int>("rd", (0xF80 & instr) >> 7));
		fieldMap->insert(std::pair<std::string, unsigned int>("funct3", (0x7000 & instr) >> 12));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs1", (0xF8000 & instr) >> 15));
		fieldMap->insert(std::pair<std::string, unsigned int>("imm", (0xFFF00000 & instr) >> 20));

		switch ((0x7000 & instr) >> 12) //funct3
		{
		case 0: this->instrName = "ADDI";
			break;
		case 2: this->instrName = "SLTI";
			break;
		case 3: this->instrName = "STLIU";
			break;
		case 4: this->instrName = "XORI";
			break;
		case 6: this->instrName = "ORI";
			break;
		case 7: this->instrName = "ANDI";
			break;
		case 1: this->instrName = "SLLI";
			break;
		case 5:
			(0xFE000000 & instr) == 0 ? this->instrName = "SRLI" : this->instrName = "SRAI";
			break;
		default:
			this->instrName = "UnKnown";
		}

		break;
	case 0b0110011:
		//R
		this->instrType = 'R';

		fieldMap->insert(std::pair<std::string, unsigned int>("rd", (0xF80 & instr) >> 7));
		fieldMap->insert(std::pair<std::string, unsigned int>("funct3", (0x7000 & instr) >> 12));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs1", (0xF8000 & instr) >> 15));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs2", (0x1F00000 & instr) >> 20));
		fieldMap->insert(std::pair<std::string, unsigned int>("funct7", (0xFE000000 & instr) >> 25));

		switch ((0x7000 & instr) >> 12) //funct3
		{
		case 0:
			//funct7
			(0xFE000000 & instr) == 0 ? this->instrName = "ADD" : this->instrName = "SUB";
			break;
		case 1: this->instrName = "SLL";
			break;
		case 2: this->instrName = "SLT";
			break;
		case 3: this->instrName = "SLTU";
			break;
		case 4: this->instrName = "XOR";
			break;
		case 5:
			//funct7
			(0xFE000000 & instr) == 0 ? this->instrName = "SRL" : this->instrName = "SRA";
			break;
		case 6: this->instrName = "OR";
			break;
		case 7: this->instrName = "AND";
			break;
		default:
			this->instrName = "UnKnown";
		}


		break;
	case 0b1100011:
		//B
		this->instrType = 'B';

		fieldMap->insert(std::pair<std::string, unsigned int>("imm0", (0xF80 & instr) >> 7));
		fieldMap->insert(std::pair<std::string, unsigned int>("funct3", (0x7000 & instr) >> 12));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs1", (0xF8000 & instr) >> 15));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs2", (0x1F00000 & instr) >> 20));
		fieldMap->insert(std::pair<std::string, unsigned int>("imm1", (0xFE000000 & instr) >> 25));

		//For testing - These are two attempts at getting the full immediate fields value
		//      fieldMap->insert(std::pair<std::string, unsigned int>("imm", 
		// ((instr >> 19) & 0b100000000000) + ((instr << 4) & 0b10000000000) + ((instr >> 20) & 0b1111100000) + ((instr >> 7) & 0b11110)));
		//      fieldMap->insert(std::pair<std::string, unsigned int>("imm",
		// (fieldMap->at("imm1") >> 6 << 12) + (fieldMap->at("imm0") & 1 << 11) + (fieldMap->at("imm1") & 0x3f << 5) + (fieldMap->at("imm0") & 0x1e << 1)));


		switch ((0x7000 & instr) >> 12) //funct3
		{
		case 0: this->instrName = "BEQ";
			break;
		case 1: this->instrName = "BNE";
			break;
		case 4: this->instrName = "BLT";
			break;
		case 5: this->instrName = "BGE";
			break;
		case 6: this->instrName = "BLTU";
			break;
		case 7: this->instrName = "BGEU";
			break;
		default:
			this->instrName = "UnKnown";
		}

		break;
	case 0b1100111:
	case 0b1101111:
		//J
		this->instrType = 'J';

		fieldMap->insert(std::pair<std::string, unsigned int>("rd", (0xF80 & instr) >> 7));
		fieldMap->insert(std::pair<std::string, unsigned int>("imm", (0xFFFFF000 & instr) >> 12));

		(instr & 8) == 0 ? this->instrName = "JAL" : this->instrName = "JALR";

		break;
	case 0b0010111:
	case 0b0110111:
		//U
		this->instrType = 'U';

		fieldMap->insert(std::pair<std::string, unsigned int>("rd", (0xF80 & instr) >> 7));
		fieldMap->insert(std::pair<std::string, unsigned int>("imm", (0xFFFFF000 & instr) >> 12));

		(instr & 32) == 0 ? this->instrName = "AUIPC" : this->instrName = "LUI";

		break;
	case 0b0000011:
		//L
		this->instrType = 'L';

		fieldMap->insert(std::pair<std::string, unsigned int>("rd", (0xF80 & instr) >> 7));
		fieldMap->insert(std::pair<std::string, unsigned int>("funct3", (0x7000 & instr) >> 12));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs1", (0xF8000 & instr) >> 15));
		fieldMap->insert(std::pair<std::string, unsigned int>("imm", (0xFFF00000 & instr) >> 20));

		switch ((0x7000 & instr) >> 12) //funct3
		{
		case 0: this->instrName = "LB";
			break;
		case 1: this->instrName = "LH";
			break;
		case 2: this->instrName = "LW";
			break;
		case 4: this->instrName = "LBU";
			break;
		case 5: this->instrName = "LHU";
			break;
		default:
			this->instrName = "UnKnown";
		}

		break;
	case 0b0100011:
		//S
		this->instrType = 'S';

		fieldMap->insert(std::pair<std::string, unsigned int>("imm0", (0xF80 & instr) >> 7));
		fieldMap->insert(std::pair<std::string, unsigned int>("funct3", (0x7000 & instr) >> 12));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs1", (0xF8000 & instr) >> 15));
		fieldMap->insert(std::pair<std::string, unsigned int>("rs2", (0x1F00000 & instr) >> 20));
		fieldMap->insert(std::pair<std::string, unsigned int>("imm1", (0xFE000000 & instr) >> 25));

		switch ((0x7000 & instr) >> 12) //funct3
		{
		case 0: this->instrName = "SB";
			break;
		case 1: this->instrName = "SH";
			break;
		case 2: this->instrName = "SW";
			break;
		default:
			this->instrName = "UnKnown";
		}

		break;
	default:
		//Other
		this->instrType = 'X';
		fieldMap->insert(std::pair<std::string, unsigned int>("UnKnown", (0xFFFFFF80 & instr) >> 4));
		this->instrName = "UnKnown";
	}
}

RiscV32Instruction::~RiscV32Instruction()
{
	delete this->fieldMap;
}

unsigned int RiscV32Instruction::getInstr()
{
	return this->instr;
}

// std::bitset<32> RiscV32Instruction::getInstrInBinary()
// {
// 	return this->instrInBinary;
// }

unsigned int RiscV32Instruction::getOp()
{
	return 0x7F & this->instr;
}

char RiscV32Instruction::getInstrType()
{
	return this->instrType;
}

std::string RiscV32Instruction::getInstrName()
{
	return this->instrName;
}

std::list<std::string> RiscV32Instruction::getInstrFieldNames() const
{
	// return this->fieldMap->key;
	std::list<std::string> retval;
	for (auto const& element : *this->fieldMap)
	{
		retval.push_back(element.first);
	}
	return retval;
}

unsigned RiscV32Instruction::getInstrFieldVal(std::string fieldName) const
{
	return this->fieldMap->at(fieldName);
}
