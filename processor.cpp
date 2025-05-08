#include <iostream>
#include <string>
#include <map>
#include <bitset>

#define MAX_INST_MEM_SIZE 1024
#define MAX_DATA_MEM_SIZE 65536

uint32_t inst_memory[MAX_INST_MEM_SIZE];
uint32_t data_memory[MAX_DATA_MEM_SIZE];

// saves the values of all registers available
typedef struct {
    unsigned address;
    unsigned value;
} Register;

static Register registers[] = 
{
    {0, 0x00},     // $0 - constant 0
    {1, 0x00},     // $at - assembler temporary
    {2, 0x00},     // $v0
    {3, 0x00},     // $v1
    {4, 0x00},     // $a0
    {5, 0x00},     // $a1
    {6, 0x00},     // $a2
    {7, 0x00},     // $a3
    {8, 0x00},     // $t0
    {9, 0x00},     // $t1
    {10, 0x00},    // $t2
    {11, 0x00},    // $t3
    {12, 0x00},    // $t4
    {13, 0x00},    // $t5
    {14, 0x00},    // $t6
    {15, 0x00},    // $t7
    {16, 0x00},    // $s0
    {17, 0x00},    // $s1
    {18, 0x00},    // $s2
    {19, 0x00},    // $s3
    {20, 0x00},    // $s4
    {21, 0x00},    // $s5
    {22, 0x00},    // $s6
    {23, 0x00},    // $s7
    {24, 0x00},    // $t8
    {25, 0x00},    // $t9
    {26, 0x00},    // $k0
    {27, 0x00},    // $k1
    {28, 0x00},    // $gp - global pounsigneder
    {29, 0x00},    // $sp - stack pounsigneder
    {30, 0x00},    // $fp - frame pounsigneder
    {31, 0x00}     // $ra - procedure return address
};

unsigned get_bits(unsigned num, unsigned lsbit, unsigned msbit) {
    unsigned mask = ((1 << (msbit - lsbit + 1)) - 1) << lsbit;
    return (num & mask) >> lsbit;
}

void add(Register *rd, Register rs, Register rt){
    // performs the arithmetic operation
    rd->value = rs.value + rt.value;
}

void sub(Register *rd, Register rs, Register rt){
    // performs the arithmetic operation
    rd->value = rs.value - rt.value;
}

void lw(Register rs, Register *rt, unsigned offset){
    // gets the value of the specified memory address
    unsigned mem_addr = rs.value + offset;
    unsigned mem_value = data_memory[mem_addr];

    // sets the destination register value to mem_value
    rt->value = mem_value;
}

void sw(Register rs, Register *rt, unsigned offset){
    // gets the value of the specified memory address
    unsigned mem_addr = rs.value + offset;

    // sets the destination register value to mem_value
    data_memory[mem_addr] = rt->value;
}

void rTypeInstruction(unsigned instruction){
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
    }
}

void iTypeInstruction(unsigned instruction, unsigned opcode){
    // parses the rs register from the instruction
    Register rs = registers[get_bits(instruction, 21, 25)];
    // parses the rt register from the instruction
    Register *rt = &registers[get_bits(instruction, 16, 20)];
    // parses the immediate from the instruction
    unsigned immediate = get_bits(instruction, 0, 15);
    // executes current instruction
    switch(opcode){
        case 35:
            lw(rs, rt, immediate);
            break;
        case 43:
            sw(rs, rt, immediate);
            break;
    }
}

unsigned main(){
    // stores a instruction as the first instruction for testing
    inst_memory[0] = 0x8d49000a;

    data_memory[10] = 8;
    data_memory[9] = 8;
    data_memory[11] = 8;

    // declares program counter and sets it to 0
    unsigned pc = 0;
    // stores the instruction in the program counter address to cur_inst
    unsigned cur_inst = inst_memory[pc];
    // prepare next instruction
    pc++;
    // gets instruction opcode
    unsigned opcode = get_bits(cur_inst, 26, 31);
    // Use std::bitset to represent the number in binary

    // As all R-Type instructions, opcode = 0
    if(opcode == 0){
        rTypeInstruction(cur_inst);
    }
    else{
        iTypeInstruction(cur_inst, opcode);
    }
}