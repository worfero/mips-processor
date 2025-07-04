#include <bitset>
#include <fstream>
#include <vector>
#include <iomanip>
#include "processor.h"

unsigned get_bits(unsigned num, unsigned lsbit, unsigned msbit) {
    unsigned mask = ((1 << (msbit - lsbit + 1)) - 1) << lsbit;
    return (num & mask) >> lsbit;
}

void print_registers(Processor processor){
    std::cout << "---- Registers ----" << std::endl;
    for(int i=0; i < MAX_NUM_REG; i++){
        std::cout << processor.registers[i].mnemonic << " - " << processor.registers[i].value << std::endl;
    }
}

std::vector<unsigned> readFile() {
    std::string filename = "mips-assembler/machine-code.bin";
    std::ifstream file(filename, std::ios::binary);

    std::vector<unsigned> program;
    unsigned buffer;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        program = {0};
        return program;
    }

    while (file.read(reinterpret_cast<char*>(&buffer), sizeof(buffer))) {
        program.push_back(buffer);
    }

    file.close();

    return program;
}

void Processor::loadProgram(std::vector<unsigned> program){
    for(int i = 0; i < program.size(); i++){
        memory_space[i] = program[i];
    }
    pc = 0;
    unsigned array_size = sizeof(memory_space)/sizeof(memory_space[0]);
    program_size = 0;
    for (int i = 0; i < 1001; i++){
        if (memory_space[i] != 0){
            program_size++;
        }
    }
    instCounter = 1;
    instStack[0].stage = FETCH;
}

void Processor::run(){
    while(instCounter != 0){
        for(int i=0; i < instCounter; i++){
            switch(instStack[i].stage){
                case FETCH:
                    fetch(instStack[i]);
                    instStack[i].stage = DECODE;
                    break;
                case DECODE:
                    decode(instStack[i]);
                    instStack[i].stage = EXECUTE;
                    break;
                case EXECUTE:
                    execute(instStack[i]);
                    instStack[i].stage = MEMORY;
                    break;
                case MEMORY:
                    memory(instStack[i]);
                    instStack[i].stage = WRITEBACK;
                    break;
                case WRITEBACK:
                    writeback(instStack[i]);
                    instCounter--;
                    break;
            }
        }
        if(pc < program_size){
            instStack[instCounter].stage = FETCH;
            instCounter++;
        }
    }
}

std::bitset<32> get_binary(int num){
    std::bitset<32> x(num);
    return x;
}

void Processor::op_add(Register rs, Register rt){
    // performs the arithmetic operation
    ALU_result = rs.value + rt.value;
}

void Processor::op_sub(Register rs, Register rt){
    // performs the arithmetic operation
    ALU_result = rs.value - rt.value;
}

void Processor::op_and(Register rs, Register rt){
    // performs the arithmetic operation
    ALU_result = rs.value & rt.value;
}

void Processor::op_or(Register rs, Register rt){
    // performs the arithmetic operation
    ALU_result = rs.value | rt.value;
}

void Processor::op_slt(Register rs, Register rt){
    // performs the arithmetic operation
    (rs.value < rt.value) ? ALU_result = 1 : ALU_result = 0;
}

void Processor::op_beq(Register rs, Register rt, unsigned offset, unsigned pc){
    // compares rs to rt, if they are equal, add the offset to the program counter
    (rs.value == rt.value) ? ALU_result = pc + offset : ALU_result = pc;
}

void Processor::op_addi(Register rs, unsigned imm){
    // sums rs to the immediate and stores it at rt
    ALU_result = rs.value + imm;
}

void Processor::op_lw(Register rs, Register *rt, unsigned offset){
    // gets the value of the specified memory_space address
    unsigned mem_addr = rs.value + offset + DATA_MEM_START;
    unsigned mem_value = memory_space[mem_addr];

    // sets the destination register value to mem_value
    rt->value = mem_value;
    std::cout << "Value " << memory_space[mem_addr] << " from memory address " << mem_addr << 
            " loaded to register " << rt->mnemonic << std::endl;
}

void Processor::op_sw(Register rs, Register *rt, unsigned offset){
    // gets the value of the specified memory_space address
    unsigned mem_addr = rs.value + offset + DATA_MEM_START;

    // sets the destination register value to mem_value
    memory_space[mem_addr] = rt->value;
    std::cout << "Value " << rt->value << " from register " << rt->mnemonic << 
            " written to memory address " << mem_addr << std::endl;
}

void Processor::fetch(Instruction &instruction){
    // stores the instruction in the program counter address to instBits
    instruction.instBits = memory_space[pc];
    // prepares pc for the next instruction
    pc++;
}

void Processor::decode(Instruction &instruction){
    // gets instruction opcode
    instruction.op = get_bits(instruction.instBits, 26, 31);
    if(instruction.op == 0){
        instruction.rs = get_bits(instruction.instBits, 21, 25);
        instruction.rt = get_bits(instruction.instBits, 16, 20);
        instruction.rd = get_bits(instruction.instBits, 11, 15);
        instruction.sa = get_bits(instruction.instBits, 6, 10);
        instruction.funct = get_bits(instruction.instBits, 0, 5);
    } 
    else{
        instruction.rs = get_bits(instruction.instBits, 21, 25);
        instruction.rt = get_bits(instruction.instBits, 16, 20);
        instruction.imm = get_bits(instruction.instBits, 0, 15);
    }
}

void Processor::execute(Instruction &instruction){
    // R-Type instructions
    if(instruction.op == 0){
        // parses the rd register from the instruction
        dest_reg = &registers[instruction.rd];
        // parses the rt register from the instruction
        Register rt = registers[instruction.rt];
        // parses the rs register from the instruction
        Register rs = registers[instruction.rs];
        switch(instruction.funct){
            case 32:
                op_add(rs, rt);
                break;
            case 34:
                op_sub(rs, rt);
                break;
            case 36:
                op_and(rs, rt);
                break;
            case 37:
                op_or(rs, rt);
                break;
            case 42:
                op_slt(rs, rt);
                break;
            default:
                break;
        }
    }
    // I-Type instructions
    else{
        dest_reg = &registers[instruction.rt];
        // parses the rs register from the instruction
        Register rs = registers[instruction.rs];
        // parses the rt register from the instruction
        Register rt = registers[instruction.rt];
        // parses the immediate from the instruction
        unsigned immediate = instruction.imm;
        // executes current instruction
        switch(instruction.op){
            case 4:
                //op_beq(rs, rt, immediate, pc);
                break;
            case 8:
                op_addi(rs, immediate);
                break;
            default:
                break;
        }
    }
}

void Processor::memory(Instruction &instruction){
    // parses the rs register from the instruction
    Register rs = registers[instruction.rs];
    // parses the rt register from the instruction
    Register *rt = &registers[instruction.rt];
    // parses the offset from the instruction
    unsigned offset = instruction.imm;
    switch(instruction.op){
        case 35:
            op_lw(rs, rt, offset);
            break;
        case 43:
            op_sw(rs, rt, offset);
            break;
    }
}

void Processor::writeback(Instruction &instruction){
    if(instruction.op > 0 && instruction.op < 8){
        pc = ALU_result;
    }
    else{
        dest_reg->value = ALU_result;
    }
}

int main(){
    Processor processor;

    // reads instructions from bin file
    std::vector<unsigned> program = readFile();
    // stores instructions
    processor.loadProgram(program);

    processor.run();
    std::cout << std::endl;
    print_registers(processor);
}