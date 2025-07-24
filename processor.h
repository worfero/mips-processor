#include <iostream>
#include <string>
#include <map>
#include <stdint.h>
#include <vector>
#include <bitset>

#define MAX_MEM_SIZE 65536
#define BUF_SIZE_FILE   65536    // Maximum buffer for a file
#define MAX_INST_NUM 1000 // Maximum number of instructions
#define DATA_MEM_START 1001 // starting address for data
#define MAX_NUM_REG 32 // Maximum number of registers

// ---- STEPS ----
#define FETCH 0
#define DECODE 1
#define EXECUTE 2
#define MEMORY 3
#define WRITEBACK 4

// saves the values of all registers available
typedef struct {
    unsigned address;
    unsigned value;
    std::string mnemonic;
} Register;

typedef struct {
    // declaring instruction variables
    int op;
    int rd;
    int rs;
    int rt;
    int imm;
    int sa;
    int funct;
    int stage;
    unsigned instBits;
} Instruction;

class Processor {
    private:
        //HazardDtc HazardUnit;
    public:
        Processor();

        uint32_t memory_space[MAX_MEM_SIZE];

        Register registers[MAX_NUM_REG];

        std::vector<Instruction> instStack;

        bool writeM;
        bool writeW;
        bool instructionEnd;
        // program counter
        unsigned pc;
        // opcode
        unsigned opcode;
        // number of instructions
        int program_size;
        int instCounter;
        // result of the ALU operation
        unsigned ALU_result;
        unsigned ALU_result_carry;
        // destination register
        Register *dest_reg;
        Register *dest_reg_carry;

        void run();

        void loadProgram(std::vector<unsigned> program);

        void fetch(Instruction &instruction);

        void decode(Instruction &instruction);

        void execute(Instruction &instruction);

        void memory(Instruction &instruction);

        void writeback(Instruction &instruction);

        void checkFwd(Register *reg);

        void op_add(Register rs, Register rt);
        
        void op_sub(Register rs, Register rt);
        
        void op_and(Register rs, Register rt);
        
        void op_or(Register rs, Register rt);
        
        void op_slt(Register rs, Register rt);
        
        void op_beq(Register rs, Register rt, unsigned offset, unsigned pc);

        void op_addi(Register rs, unsigned imm);
        
        void op_lw(Register rs, Register *rt, unsigned offset);
        
        void op_sw(Register rs, Register *rt, unsigned offset);
};

Processor::Processor() : registers 
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
} {}