#include <iostream>
#include <string>
#include <map>

#define MAX_INST_MEM_SIZE 1024
#define MAX_DATA_MEM_SIZE 65536

uint32_t inst_memory[MAX_INST_MEM_SIZE];
uint32_t data_memory[MAX_DATA_MEM_SIZE];

// saves the values of all registers available
typedef struct {
    int address;
    int value;
} Register;

static const Register registers[] = 
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
    {28, 0x00},    // $gp - global pointer
    {29, 0x00},    // $sp - stack pointer
    {30, 0x00},    // $fp - frame pointer
    {31, 0x00}     // $ra - procedure return address
};

int get_bits(int num, int lsbit, int msbit) {
    int mask = ((1 << (msbit - lsbit + 1)) - 1) << lsbit;
    return (num & mask) >> lsbit;
}

int main(){
    // stores a "lw $t1, 0($s0)" instruction as the first instruction for testing
    inst_memory[0] = 0x8e090008;
    
    data_memory[8] = 10;

    // declares program counter and sets it to 0
    int pc = 0;

    // stores the instruction in the program counter address to cur_inst
    int cur_inst = inst_memory[pc];

    // parses the rs register from the instruction
    int rs = registers[get_bits(cur_inst, 21, 25)].value;
    int offset = get_bits(cur_inst, 0, 15);
    int data_addr = rs + offset;
    int data_value = data_memory[data_addr];
    std::cout << data_value << std::endl;
}