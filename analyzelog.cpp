#include <bitset>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <set>
#include <sstream>
#include <queue>

#include "riscVInstruction.h"

typedef unsigned int Instruction;
typedef unsigned int Address;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " logfile" << std::endl;
        return 1;
    }
    std::ifstream infile(argv[1]);

    std::string line;
    char opChar, colon;
    unsigned int data, instrAddr, guess;
    unsigned int savedBAddr;
    unsigned int guessNotTakenC = 0;
    unsigned int guessNotTakenI = 0;
    unsigned int guessNotTakenTotal = 0;
    unsigned int guessTakenC = 0;
    unsigned int guessTakenI = 0;
	unsigned int guessTakenTotal = 0;
    // Instruction previous = 0xdeadbeef;
    RiscV32Instruction* instrData = nullptr;
    RiscV32Instruction* instrPrevious = nullptr;
    auto* dependentInstrs = new std::map<std::string, unsigned>();
	std::string instrPair = "";

	
    const int cache_size = 256;
    Address cache[cache_size];
    cache[0] = 0xcafebabe;
    unsigned int cache_hits = 0;
    unsigned int cache_misses = 0;
    int mask = 0x3FC;
    int index = 0;

    int numInstructions = 0;
    //Count of each type {I, R, B, L, S, U, J, Other}
    int instructionTypeCount[8] = {0, 0, 0, 0, 0, 0, 0, 0 };
	std::set<Instruction> otherOpCodes;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        if (!(iss >> opChar >> colon >> std::hex >> data)) {
            std::cerr << "Parse error: " << line << std::endl;
            break;
        }
        std::cout << std::hex << std::setfill('0') << std::setw(8) << std::showbase;

        
        switch(opChar)
    	{
        case 'i':
            instrData = new RiscV32Instruction(data);
        	//i: A 32 - bit RISC - V instruction(the trace first dumps the address then the instruction)
            // previous = data;    // for next time

            numInstructions++;
            switch (instrData->getInstrType())
            {
            case 'I':
                instructionTypeCount[0]++;
                break;
            case 'R':
                instructionTypeCount[1]++;
                break;
            case 'B':
                instructionTypeCount[2]++;
                savedBAddr = instrAddr;
                break;
            case 'L':
                instructionTypeCount[3]++;
                break;
            case 'S':
                instructionTypeCount[4]++;
                break;
            case 'U':
                instructionTypeCount[5]++;
                break;
            case 'J':
                instructionTypeCount[6]++;
                break;
            default:
                // std::cerr << "Something went wrong: I see op I with code " << std::bitset<7>(0x7F & data) << std::endl;
                // return -1;
                numInstructions--;
                instructionTypeCount[7]++;
                otherOpCodes.insert(instrData->getOp());
            }
        	

        	if (instrPrevious != nullptr && (instrData->getInstrType() == 'R' || instrData->getInstrType() == 'I' || 
                instrData->getInstrType() == 'L' || instrData->getInstrType() == 'S' || instrData->getInstrType() == 'B') &&
                (instrPrevious->getInstrType() == 'R' || instrPrevious->getInstrType() == 'I' || instrPrevious->getInstrType() == 'L'))
        	{
                instrPair = "";
				switch (instrData->getInstrType())
				{
				case 'R':
                case 'S':
                case 'B':
					if (instrPrevious->getInstrFieldVal("rd") == instrData->getInstrFieldVal("rs1") ||
                        instrPrevious->getInstrFieldVal("rd") == instrData->getInstrFieldVal("rs2"))
					{
                        instrPair = instrPrevious->getInstrName() + '-' + instrData->getInstrName();
					}

					break;
                case 'I':
                case 'L':
                    if (instrPrevious->getInstrFieldVal("rd") == instrData->getInstrFieldVal("rs1"))
                    {
                        instrPair = instrPrevious->getInstrName() + '-' + instrData->getInstrName();
                    }
                    break;
				}

        		if (!instrPair.empty())
        		{
        			if (dependentInstrs->find(instrPair) != dependentInstrs->end())
        				dependentInstrs->at(instrPair)++;
        			else
        				dependentInstrs->insert(std::pair<std::string, unsigned int>(instrPair, 1));
        		}
        	}

            delete instrPrevious;
            instrPrevious = new RiscV32Instruction(instrData->getInstr());
        	delete instrData;
			
            break;
        case 'I':
            //I: The address of an access into instruction memory
        	// 0x3FC
        	instrAddr = data;

               //decode the immediate, generate a guess address
            if (instrPrevious != NULL && instrPrevious->getInstrType() == 'B') {
            	// Get 32 bit of instruction. If its 1 then branch is backwards branch
            	// else branch is forward branch
                guess = (instrPrevious->getInstr() & 0x80000000) >> 31;

            	if (guess) //Backward branch: guess always-taken
            	{
                    guessTakenTotal++;
            		if (savedBAddr > instrAddr) //Went backward: Guess correct
						guessTakenC++;
            		else  //Went forward: Guess incorrect
						guessTakenI++;
            	}
            	else //Forward branch: guess always-not-taken
            	{
                    guessNotTakenTotal++;
                    if (instrAddr - savedBAddr > 4) //Lines were skipped: Guess incorrect
                        guessNotTakenI++;
                    else if (instrAddr - savedBAddr == 4)  //Went only to next line: Guess correct
                        guessNotTakenC++;
            		else
						return -1;
            	}
            }
            break;
        case 'L':
            //L: The address of a memory load into data memory
            index = data & mask; // index = 8 bits of memory for cache index
            index = index >> 2; // shift index over by 2 bits
            if ( cache[index] == data ) {
                cache_hits++;
            } else {
                cache_misses++;
                // now update the cache
                cache[index] = data;
            }
            break;
        case 'S':
            //S: The address of a memory store into data memory
            index = data & mask;
            index = index >> 2;
            if ( cache[index] == data ) {
                cache_hits++;
            } else {
                cache_misses++;
                // now update the cache
                cache[index] = data;
            }

            break;
        default:
            std::cerr << "Something went wrong: I see op " << opChar << std::endl;
            return -1;
        }

    }

	//Project 3 Part 2, 1
	//Determine Instruction Mix
	double instructionMix[8] = {static_cast<double>(instructionTypeCount[0]) / numInstructions,
                                static_cast<double>(instructionTypeCount[1]) / numInstructions,
                                static_cast<double>(instructionTypeCount[2]) / numInstructions,
                                static_cast<double>(instructionTypeCount[3]) / numInstructions,
                                static_cast<double>(instructionTypeCount[4]) / numInstructions,
                                static_cast<double>(instructionTypeCount[5]) / numInstructions,
                                static_cast<double>(instructionTypeCount[6]) / numInstructions,
                                static_cast<double>(instructionTypeCount[7])};
	
    std::cout << std::dec << std::setw(0) << std::noshowbase;
	std::cout << "Instruction Mix:" << std::endl;
	std::cout << "I: " << instructionMix[0] * 100 << "%\n";
    std::cout << "R: " << instructionMix[1] * 100 << "%\n";
    std::cout << "B: " << instructionMix[2] * 100 << "%\n";
    std::cout << "L: " << instructionMix[3] * 100 << "%\n";
    std::cout << "S: " << instructionMix[4] * 100 << "%\n";
    std::cout << "U: " << instructionMix[5] * 100 << "%\n";
    std::cout << "J: " << instructionMix[6] * 100 << "%\n";
    std::cout << "Amount of Other Instructions Run: " << instructionMix[7] << std::endl;
	std::cout << "Other OpCodes run: \n";
	for (auto it = otherOpCodes.begin(); it != otherOpCodes.end(); ++it)
		std::cout << std::bitset<7>(*it) << std::endl;


    //Project 3 Part 2, 2
	//Lost the top 5 most frequence dependent instruction pairs
	std::priority_queue< std::pair<unsigned, std::string> > sortedInstrPairs;
	int instrPairCount = 0;
    for (auto const& element : *dependentInstrs) {
        sortedInstrPairs.push(std::pair<unsigned, std::string>(element.second, element.first));
        instrPairCount += element.second;
    }
	// while (sortedInstrPairs.size() > 5) sortedInstrPairs.pop();
	std::cout << "\n\nInstruction Pairs\n";
    // for (int i = 0; i < 5; i++)
    while (!sortedInstrPairs.empty())
	{
		auto pair = sortedInstrPairs.top();
		sortedInstrPairs.pop();
		std::cout << std::setfill(' ') << std::setw(9) << pair.second << ": " << std::setw(7) << pair.first << " occurrences.\n";
	}

	
    //Project 3 Part 2, 3
	//What fraction of instructions executed have at least 1 register that will need to be forwarded?
	std::cout << "A total of " << numInstructions << " instruction were run with " << instrPairCount << " containing at least 1 register that needs forwarding.\n";
	std::cout << "The percentage being " << static_cast<double>(instrPairCount)/numInstructions * 100 << '%' << std::endl;

	
	//Part 2, 4
    long double guessCorrectTotal = static_cast<double>(guessNotTakenC + guessTakenC);
    long double guessIncorrectTotal = static_cast<double>(guessNotTakenI + guessTakenI);
	unsigned long totalGuesses = guessNotTakenTotal + guessTakenTotal;
    std::cout << "\n\nStatic Branch Prediction Analysis:\n";
    std::cout << "Always-Taken Correct guesses: " << guessTakenC << std::endl;
	std::cout << "Always-Taken Total guesses: " << guessTakenTotal << std::endl;
	std::cout << "Always-Taken Correct Percent: " << static_cast<double>(guessTakenC)/guessTakenTotal * 100 << '%' << std::endl;
    std::cout << "Always-Not-Taken Correct guesses: " << guessNotTakenC << std::endl;
    std::cout << "Always-Not-Taken Total guesses: " << guessNotTakenTotal << std::endl;
    std::cout << "Always-Not-Taken Correct Percent: " << static_cast<double>(guessNotTakenC)/guessNotTakenTotal * 100 << '%' << std::endl;
    std::cout << "Total Correct guesses: " << guessNotTakenC + guessTakenC << std::endl;
    std::cout << "Total Incorrect guesses: " << guessNotTakenI + guessTakenI << std::endl;
    std::cout << "Total guesses: " << totalGuesses << std::endl;
    std::cout << "Total Correct Predictions Percent: " << guessCorrectTotal / totalGuesses * 100 << "%\n";
    std::cout << "Total Incorrect Predictions Percent: " << guessIncorrectTotal / totalGuesses * 100 << "%\n";


    //cache system performance
    unsigned long totalCyclesNoCache = 0;
    totalCyclesNoCache = 50 * (cache_hits +  cache_misses); // adds L and S type instructions including the hits and misses
	
    unsigned long totalCyclesCache = 0;
    totalCyclesCache = cache_hits + 50 * cache_misses; // adds L and S type instructions including the hits and misses

    // std::cout << std::dec << "\n\nI saw " << cache_hits << " cache hits and " << cache_misses << " cache misses." << std::endl;
	std::cout << "\n\nTotal Cache Hits: " << cache_hits << std::endl;
	std::cout << "Total Cache Misses: " << cache_misses << std::endl;
    std::cout << "Total Cycles w/ cache: " << totalCyclesCache << std::endl;
    std::cout << "Total Cycles w/o cache: " << totalCyclesNoCache << std::endl;
    std::cout << "% of total cycles required using a cache (total cycles w/ cache)/(total cycles w/o cache) * 100): " << (static_cast<double>(totalCyclesCache) / totalCyclesNoCache) * 100  << '%' << std::endl;


    return 0;
}