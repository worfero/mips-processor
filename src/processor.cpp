#include "processor.h"

std::mutex cout_mutex;

Processor::Processor(){
    registers = 
    {
        {0, 0x00, "$0"},     // $0 - constant 0
        {1, 0x00, "$at"},     // $at - assembler temporary
        {2, 0x00, "$v0"},     // $v0
        {3, 0x00, "$v1"},     // $v1
        {4, 0x00, "$a0"},     // $a0
        {5, 0x00, "$a1"},     // $a1
        {6, 0x00, "$a2"},     // $a2
        {7, 0x00, "$a3"},     // $a3
        {8, 0x00, "$t0"},     // $t0
        {9, 0x00, "$t1"},     // $t1
        {10, 0x00, "$t2"},    // $t2
        {11, 0x00, "$t3"},    // $t3
        {12, 0x00, "$t4"},    // $t4
        {13, 0x00, "$t5"},    // $t5
        {14, 0x00, "$t6"},    // $t6
        {15, 0x00, "$t7"},    // $t7
        {16, 0x00, "$s0"},    // $s0
        {17, 0x00, "$s1"},    // $s1
        {18, 0x00, "$s2"},    // $s2
        {19, 0x00, "$s3"},    // $s3
        {20, 0x00, "$s4"},    // $s4
        {21, 0x00, "$s5"},    // $s5
        {22, 0x00, "$s6"},    // $s6
        {23, 0x00, "$s7"},    // $s7
        {24, 0x00, "$t8"},    // $t8
        {25, 0x00, "$t9"},    // $t9
        {26, 0x00, "$k0"},    // $k0
        {27, 0x00, "$k1"},    // $k1
        {28, 0x00, "$gp"},    // $gp - global pounsigneder
        {29, 0x00, "$sp"},    // $sp - stack pounsigneder
        {30, 0x00, "$fp"},    // $fp - frame pounsigneder
        {31, 0x00, "$ra"}     // $ra - procedure return address
    };
    instStack.reserve(6);
    memory_space.reserve(MAX_MEM_SIZE);
}

void Processor::init_variables()
{
    instructionEnd = false;
    writeM = false;
    writeW = false;
    d_regE = nullptr;
    d_regM = nullptr;
    d_regW = nullptr;
    pc = 0;
    ALU_resE = 0;
    ALU_resM = 0;
    ALU_resW = 0;
    stall = 0;
    stallFlag = 0;
    stallReset = 0;

    for (int i = 0; i < 6; i++)
    {
        instStack.push_back({0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
}

void Processor::loadProgram(std::vector<uint32_t> program)
{
    init_variables();
    for(int i=0; i < MAX_MEM_SIZE; i++){
        memory_space.push_back(0);
    }
    for(int i=0; (long long unsigned int)i < program.size(); i++){
        memory_space.at(i) = program.at(i);
    }

    if(memory_space.size() > 1){
        program_size = ( uint16_t ) ( program.size() );
    }
    else{
        program_size = 0;
    }
    instCounter = 1;
    instStack.at(0).stage = FETCH;
}

void Processor::print_registers()
{
    std::cout << "---- Registers ----" << std::endl;
    for (int i = 0; i < MAX_NUM_REG; i++)
    {
        cout_mutex.lock();
        std::cout << std::dec << registers[i].mnemonic << " - " << registers[i].value << std::endl;
        cout_mutex.unlock();
    }
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
    if(mem_addr >= memory_space.size()){
        memory_space[mem_addr] = 0;
    }
    uint32_t mem_value = memory_space[mem_addr];

    // sets the destination register value to mem_value
    rt->value = ( unsigned ) mem_value;
}

void Processor::op_sw(Register rs, Register *rt, unsigned offset)
{
    // gets the value of the specified memory_space address
    unsigned mem_addr = rs.value + offset + DATA_MEM_START;

    // sets the destination register value to mem_value
    memory_space.at(mem_addr) = rt->value;
}

void Processor::run()
{
    while (instCounter != 0)
    {
        std::cout << "Clock" << std::endl;
        std::cout << std::dec << pc+1 << "/" << program_size+1 << std::endl;
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
                print_registers();
                std::cout << std::endl;
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
            cout_mutex.lock();
            std::cout << "!!NOT STALL!!" << std::endl;
            cout_mutex.unlock();
        }
        if(stallFlag > 0){
            Instruction lwInstruction = instStack.at(stallFlag);
            Instruction nextInstruction = instStack.at(stallFlag+1);
            if(lwInstruction.rt == nextInstruction.rs){
                stall = 1;
                cout_mutex.lock();
                std::cout << "!!STALL!!" << std::endl;
                cout_mutex.unlock();
                stallReset = 1;
                stallFlag = 0;
            }
            else if(lwInstruction.rt == nextInstruction.rt && nextInstruction.op == 0){
                stall = 1;
                cout_mutex.lock();
                std::cout << "!!STALL!!" << std::endl;
                cout_mutex.unlock();
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
                instStack.push_back({ 0, 0, 0, 0, 0, 0, 0, FETCH, 0 });
                std::cout << "Instruction added to queue" << std::endl;
            }
        }
        //print_registers(*this);
    }
}

std::bitset<32> get_binary(unsigned num)
{
    std::bitset<32> x(num);
    return x;
}

void Processor::fetch(unsigned i)
{
    Instruction *instruction = &instStack[i];
    // stores the instruction in the program counter address to instBits
    instruction->instBits = memory_space[pc];
    // prepares pc for the next instruction
    pc++;
    cout_mutex.lock();
    std::cout << "FETCH: 0x" << std::hex << instruction->instBits << std::endl;
    cout_mutex.unlock();
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
    cout_mutex.lock();
    std::cout << "DECODE: 0x" << std::hex << instruction->instBits << std::endl;
    cout_mutex.unlock();
}

void Processor::execute(unsigned i)
{
    Instruction *instruction = &instStack[i];
    // parses the rt register from the instruction
    Register rt = registers[instruction->rt];
    // parses the rs register from the instruction
    Register rs = registers[instruction->rs];

    // check if forwarding is needed for rs register
    checkFwd(&rs);
    
    if(instruction->op == 35){
        if(instStack.size() > 1){
            stallFlag = i;
        }
    }

    // R-Type instructions
    if (instruction->op == 0)
    {
        // check if forwarding is needed for rt register, in case it's an R-Type instruction
        checkFwd(&rt);
        // disable forwarding flags
        if(writeM) writeM = false;
        if(writeW) writeW = false;
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
        // disable forwarding flags
        if(writeM) writeM = false;
        if(writeW) writeW = false;
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
    cout_mutex.lock();
    std::cout << "EXECUTE: 0x" << std::hex << instruction->instBits << std::endl;
    cout_mutex.unlock();
}

void Processor::memory(unsigned i)
{
    Instruction *instruction = &instStack[i];
    ALU_resM = ALU_resE;
    d_regM = d_regE;
    writeM = true;

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
            break;
        case 43:
            op_sw(rs, d_regM, offset);
            break;
    }
    cout_mutex.lock();
    std::cout << "MEMORY: 0x" << std::hex << instruction->instBits << std::endl;
    cout_mutex.unlock();
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
    else if(instruction->op < 32)
    {
        d_regW->value = ALU_resW;
    }
    cout_mutex.lock();
    std::cout << "WRITEBACK: 0x" << std::hex << instruction->instBits << std::endl;
    cout_mutex.unlock();
}

void Processor::checkFwd(Register *reg)
{
    if (reg->address != 0)
    {
        if(writeM){
            if(d_regM != nullptr && reg->address == d_regM->address){
                reg->value = ALU_resM;
            }
            writeM = false;
        }
        else if(writeW){
            if(d_regE != nullptr && reg->address == d_regE->address){
                reg->value = ALU_resE;
            }
            writeW = false;
        }
    }
}