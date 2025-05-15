#include <bitset>
#include "processor.h"

unsigned get_bits(unsigned num, unsigned lsbit, unsigned msbit) {
    unsigned mask = ((1 << (msbit - lsbit + 1)) - 1) << lsbit;
    return (num & mask) >> lsbit;
}

void Processor::run(){
    pc = 0;
    unsigned array_size = sizeof(inst_memory)/sizeof(inst_memory[0]);
    unsigned program_size = 0;
    for (int i = 0; i < array_size; i++){
        if (inst_memory[i] != 0){
            program_size++;
        }
    }

    for(pc; pc < program_size;){
        // stores the instruction in the program counter address to cur_inst
        unsigned cur_inst = inst_memory[pc];
        // prepare next instruction
        pc++;
        // gets instruction opcode
        opcode = get_bits(cur_inst, 26, 31);
        // For all R-Type instructions, opcode = 0
        if(opcode == 0){
            rTypeInstruction(cur_inst);
        }
        else{
            iTypeInstruction(cur_inst, opcode, &pc);
        }
    }
}

std::bitset<32> get_binary(int num){
    std::bitset<32> x(num);
    return x;
}

void Processor::add(Register *rd, Register rs, Register rt){
    // performs the arithmetic operation
    rd->value = rs.value + rt.value;
}

void Processor::sub(Register *rd, Register rs, Register rt){
    // performs the arithmetic operation
    rd->value = rs.value - rt.value;
}

void Processor::and(Register *rd, Register rs, Register rt){
    // performs the arithmetic operation
    rd->value = rs.value & rt.value;
}

void Processor::or(Register *rd, Register rs, Register rt){
    // performs the arithmetic operation
    rd->value = rs.value | rt.value;
}

void Processor::slt(Register *rd, Register rs, Register rt){
    // performs the arithmetic operation
    (rs.value < rt.value) ? rd->value = 1 : rd->value = 0;
}

void Processor::beq(Register rs, Register *rt, unsigned offset, unsigned *pc){
    // compares rs to rt, if they are equal, add the offset to the program counter
    (rs.value == rt->value) ? *pc = *pc + offset : *pc = *pc;
}

void Processor::addi(Register rs, Register *rt, unsigned imm, unsigned *pc){
    // sums rs to the immediate and stores it at rt
    rt->value = rs.value + imm;
}

void Processor::lw(Register rs, Register *rt, unsigned offset){
    // gets the value of the specified memory address
    unsigned mem_addr = rs.value + offset;
    unsigned mem_value = data_memory[mem_addr];

    // sets the destination register value to mem_value
    rt->value = mem_value;
}

void Processor::sw(Register rs, Register *rt, unsigned offset){
    // gets the value of the specified memory address
    unsigned mem_addr = rs.value + offset;

    // sets the destination register value to mem_value
    data_memory[mem_addr] = rt->value;
}

void Processor::rTypeInstruction(unsigned instruction){
    unsigned funct = get_bits(instruction, 0, 6);
    // parses the rd register from the instruction
    Register *rd = &registers[get_bits(instruction, 11, 15)];
    // parses the rt register from the instruction
    Register rt = registers[get_bits(instruction, 16, 20)];
    // parses the rs register from the instruction
    Register rs = registers[get_bits(instruction, 21, 25)];
    // executes current instruction
    switch(funct){
        case 32:
            add(rd, rs, rt);
            break;
        case 34:
            sub(rd, rs, rt);
            break;
        case 36:
            and(rd, rs, rt);
            break;
        case 37:
            or(rd, rs, rt);
            break;
        case 42:
            slt(rd, rs, rt);
            break;
    }
}

void Processor::iTypeInstruction(unsigned instruction, unsigned opcode, unsigned *pc){
    // parses the rs register from the instruction
    Register rs = registers[get_bits(instruction, 21, 25)];
    // parses the rt register from the instruction
    Register *rt = &registers[get_bits(instruction, 16, 20)];
    // parses the immediate from the instruction
    unsigned immediate = get_bits(instruction, 0, 15);
    // executes current instruction
    switch(opcode){
        case 4:
            beq(rs, rt, immediate, pc);
            break;
        case 8:
            addi(rs, rt, immediate, pc);
            break;
        case 35:
            lw(rs, rt, immediate);
            break;
        case 43:
            sw(rs, rt, immediate);
            break;
    }
}

unsigned main(){
    Processor processor;

    // stores instructions for testing
    processor.inst_memory[0] = 0x20110028;
    processor.inst_memory[1] = 0x20120032;
    processor.inst_memory[2] = 0x02328020;
    
    processor.run();
    std::cout << processor.registers[16].value << std::endl;
}