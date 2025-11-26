#include "processor.h"
#include <iostream>

int main()
{
    Processor processor;

    // reads instructions from bin file
    std::vector<uint32_t> program = readFile();
    // stores instructions
    processor.loadProgram(program);
    std::cout << "Starting..." << std::endl;
    processor.run();
    std::cout << std::endl;
    processor.print_registers();
}