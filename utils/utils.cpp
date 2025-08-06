#include "utils.h"

unsigned get_bits(unsigned num, unsigned lsbit, unsigned msbit)
{
    unsigned mask = ((1 << (msbit - lsbit + 1)) - 1) << lsbit;
    return (num & mask) >> lsbit;
}

std::vector<uint32_t> readFile()
{
    std::string filename = "mips-assembler/machine-code.bin";
    std::ifstream file(filename, std::ios::binary);

    std::vector<uint32_t> program;
    uint32_t buffer;

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        program = {0};
        return program;
    }

    while (file.read(reinterpret_cast<char *>(&buffer), sizeof(buffer)))
    {
        program.push_back(buffer);
    }

    file.close();

    return program;
}