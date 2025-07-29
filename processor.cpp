#include <bitset>
#include <fstream>
#include <vector>
#include <iomanip>
#include <thread>
#include <functional>
#include <mutex>
#include "processor.h"

std::mutex cout_mutex;

unsigned get_bits(unsigned num, unsigned lsbit, unsigned msbit)
{
    unsigned mask = ( uint16_t ) ( ((1 << (msbit - lsbit + 1)) - 1) << lsbit );
    return (num & mask) >> lsbit;
}

void print_registers(Processor processor)
{
    std::cout << "---- Registers ----" << std::endl;
    for (int i = 0; i < MAX_NUM_REG; i++)
    {
        std::cout << std::dec << processor.registers[i].mnemonic << " - " << processor.registers[i].value << std::endl;
    }
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

void Processor::loadProgram(std::vector<uint32_t> program)
{
    instructionEnd = false;
    writeM = false;
    writeW = false;
    d_regE = &registers[0];
    d_regM = &registers[0];
    stall = 0;
    stallFlag = 0;
    stallReset = 0;
    for (int i = 0; i < 6; i++)
    {
        instStack.push_back({0, 0, 0, 0, 0, 0, 0, 0, 0});
    }

    memory_space = program;

    pc = 0;
    if(memory_space.size() > 1){
        program_size = ( uint16_t ) ( memory_space.size() );
    }
    else{
        program_size = 0;
    }
    instCounter = 1;
    instStack.at(0).stage = FETCH;
}

void Processor::run()
{
    while (instCounter != 0)
    {
        std::cout << "Clock" << std::endl;
        std::cout << std::dec << instCounter << std::endl;
        std::cout << pc+1 << "/" << program_size+1 << std::endl;
        std::vector<std::thread> threads;
        for (unsigned i = 0; (std::vector<Instruction>::size_type) i < instCounter; i++)
        {
            switch (instStack.at(i).stage)
            {
            case FETCH:
                if(!stall){
                    threads.emplace_back(&Processor::fetch, this, i);
                    instStack.at(i).stage = DECODE;
                }
                break;
            case DECODE:
                if(!stall){
                    threads.emplace_back(&Processor::decode, this, i);
                    instStack.at(i).stage = EXECUTE;
                }
                break;
            case EXECUTE:
                if(!stall){
                    threads.emplace_back(&Processor::execute, this, i);
                    instStack.at(i).stage = MEMORY;
                }
                break;
            case MEMORY:
                threads.emplace_back(&Processor::memory, this, i);
                instStack.at(i).stage = WRITEBACK;
                break;
            case WRITEBACK:
                threads.emplace_back(&Processor::writeback, this, i);
                instructionEnd = true;
                break;
            }
        }
        for (std::thread &t : threads)
        {
            if (t.joinable())
            { // Check if the thread is joinable
                t.join();
            }
        }
        if(stallReset){
            stall = 0;
            stallReset = false;
            std::lock_guard<std::mutex> lock(cout_mutex);
            std::cout << "!!NOT STALL!!" << std::endl;
        }
        if(stallFlag > 0){
            if(instStack.at(stallFlag).rt == instStack.at(stallFlag+1).rt || instStack.at(stallFlag).rt == instStack.at(stallFlag+1).rs){
                stall = 1;
                std::lock_guard<std::mutex> lock(cout_mutex);
                std::cout << "!!STALL!!" << std::endl;
                stallReset = 1;
                stallFlag = 0;
            }
            else{
                stall = false;
                stallFlag = 0;
            }
        }
        // if an instruction made through the Writeback stage, it is removed from the array
        if (instructionEnd)
        {
            instStack.erase(instStack.begin());
            std::cout << "Instruction removed from queue" << std::endl;
            instCounter--;
            instructionEnd = false;
        }
        if (pc < program_size)
        {
            if(!stall){
                instCounter++;
                instStack[instCounter].stage = FETCH;
                std::cout << "Instruction added to queue" << std::endl;
            }
        }
    }
}

std::bitset<32> get_binary(unsigned num)
{
    std::bitset<32> x(num);
    return x;
}

void Processor::op_add(Register rs, Register rt)
{
    // performs the arithmetic operation
    ALU_resE = rs.value + rt.value;
}

void Processor::op_sub(Register rs, Register rt)
{
    // performs the arithmetic operation
    ALU_resE = rs.value - rt.value;
}

void Processor::op_and(Register rs, Register rt)
{
    // performs the arithmetic operation
    ALU_resE = rs.value & rt.value;
}

void Processor::op_or(Register rs, Register rt)
{
    // performs the arithmetic operation
    ALU_resE = rs.value | rt.value;
}

void Processor::op_slt(Register rs, Register rt)
{
    // performs the arithmetic operation
    (rs.value < rt.value) ? ALU_resE = 1 : ALU_resE = 0;
}

void Processor::op_beq(Register rs, Register rt, unsigned offset)
{
    // compares rs to rt, if they are equal, add the offset to the program counter
    (rs.value == rt.value) ? ALU_resE = pc + offset : ALU_resE = pc;
}

void Processor::op_addi(Register rs, unsigned imm)
{
    // sums rs to the immediate and stores it at rt
    ALU_resE = rs.value + imm;
}

void Processor::op_lw(Register rs, Register *rt, unsigned offset)
{
    // gets the value of the specified memory_space address
    unsigned mem_addr = rs.value + offset + DATA_MEM_START;
    unsigned mem_value = memory_space[mem_addr];

    // sets the destination register value to mem_value
    rt->value = mem_value;
}

void Processor::op_sw(Register rs, Register *rt, unsigned offset)
{
    // gets the value of the specified memory_space address
    unsigned mem_addr = rs.value + offset + DATA_MEM_START;

    // sets the destination register value to mem_value
    memory_space[mem_addr] = rt->value;
}

void Processor::fetch(unsigned i)
{
    Instruction *instruction = &instStack[i];
    // stores the instruction in the program counter address to instBits
    instruction->instBits = memory_space[pc];
    // prepares pc for the next instruction
    pc++;
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "FETCH: 0x" << std::hex << instruction->instBits << std::endl;
}

void Processor::decode(unsigned i)
{
    Instruction *instruction = &instStack[i];
    // gets instruction opcode
    instruction->op = get_bits(instruction->instBits, 26, 31);
    if (instruction->op == 0)
    {
        instruction->rs = get_bits(instruction->instBits, 21, 25);
        instruction->rt = get_bits(instruction->instBits, 16, 20);
        instruction->rd = get_bits(instruction->instBits, 11, 15);
        instruction->sa = get_bits(instruction->instBits, 6, 10);
        instruction->funct = get_bits(instruction->instBits, 0, 5);
    }
    else
    {
        instruction->rs = get_bits(instruction->instBits, 21, 25);
        instruction->rt = get_bits(instruction->instBits, 16, 20);
        instruction->imm = get_bits(instruction->instBits, 0, 15);
    }
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "DECODE: 0x" << std::hex << instruction->instBits << std::endl;
}

void Processor::execute(unsigned i)
{
    Instruction *instruction = &instStack[i];
    // parses the rt register from the instruction
    Register rt = registers[instruction->rt];
    // parses the rs register from the instruction
    Register rs = registers[instruction->rs];

    // check if forwarding is needed for both source registers
    checkFwd(&rs);
    
    if(instruction->op == 35){
        if(instStack.size() > 1){
            stallFlag = i;
        }
    }

    // R-Type instructions
    if (instruction->op == 0)
    {
        checkFwd(&rt);
        // parses the rd register from the instruction
        d_regE = &registers[instruction->rd];
        switch (instruction->funct)
        {
        case 32:
            op_add(rs, rt);
            writeW = true;
            break;
        case 34:
            op_sub(rs, rt);
            writeW = true;
            break;
        case 36:
            op_and(rs, rt);
            writeW = true;
            break;
        case 37:
            op_or(rs, rt);
            writeW = true;
            break;
        case 42:
            op_slt(rs, rt);
            writeW = true;
            break;
        default:
            break;
        }
    }
    // I-Type instructions
    else
    {
        d_regE = &registers[instruction->rt];
        // parses the immediate from the instruction
        unsigned immediate = instruction->imm;
        // executes current instruction
        switch (instruction->op)
        {
        case 4:
            // op_beq(rs, rt, immediate, pc);
            break;
        case 8:
            op_addi(rs, immediate);
            writeW = true;
            break;
        default:
            break;
        }
    }
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "EXECUTE: 0x" << std::hex << instruction->instBits << std::endl;
}

void Processor::memory(unsigned i)
{
    Instruction *instruction = &instStack[i];
    ALU_resM = ALU_resE;
    d_regM = d_regE;

    if(stall){
        d_regE = nullptr;
        ALU_resE = 0;
    }

    // parses the rs register from the instruction
    Register rs = registers[instruction->rs];
    // parses the offset from the instruction
    unsigned offset = instruction->imm;
    switch (instruction->op)
    {
    case 35:
        op_lw(rs, d_regM, offset);
        writeM = true;
        break;
    case 43:
        op_sw(rs, d_regM, offset);
        break;
    }
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "MEMORY: 0x" << std::hex << instruction->instBits << std::endl;
}

void Processor::writeback(unsigned i)
{
    Instruction *instruction = &instStack[i];
    ALU_resW = ALU_resM;
    d_regW = d_regM;
    if (instruction->op > 0 && instruction->op < 8)
    {
        pc = ALU_resW;
    }
    else
    {
        d_regW->value = ALU_resW;
    }
    std::lock_guard<std::mutex> lock(cout_mutex);
    std::cout << "WRITEBACK: 0x" << std::hex << instruction->instBits << std::endl;
}

void Processor::checkFwd(Register *reg)
{
    if (reg->address != 0)
    {
        if (reg->address == d_regM->address && writeM)
        {
            reg->value = d_regM->value;
            writeM = false;
        }
        else if (d_regE != nullptr && reg->address == d_regE->address && writeW)
        {
            reg->value = ALU_resE;
            writeW = false;
        }
    }
}

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
    print_registers(processor);
}