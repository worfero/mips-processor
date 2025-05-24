#include <iostream>
#include <string>
#include <map>

#define MAX_MEM_SIZE 65536
#define BUF_SIZE_FILE   65536    // Maximum buffer for a file
#define MAX_INST_NUM 1000 // Maximum number of instructions
#define DATA_MEM_START 1001 // starting address for data

#define FETCH 0
#define DECODE 1
#define EXECUTE 2
#define MEMORY 3
#define WRITEBACK 4

// saves the values of all registers available
typedef struct {
    unsigned address;
    unsigned value;
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
} Instruction;

class Processor {
    public:
        Processor();

        uint32_t memory_space[MAX_MEM_SIZE];

        Register registers[32];

        Instruction instruction;

        // program counter
        unsigned pc;
        // current instruction
        unsigned inst_index;
        // opcode
        unsigned opcode;
        // state of the machine
        int state;
        // number of instructions
        int program_size;
        // result of the ALU operation
        unsigned ALU_result;
        // destination register
        Register *dest_reg;

        void run();

        void loadProgram(std::vector<unsigned> program);

        void fetch();

        void decode();

        void execute();

        void memory();

        void writeback();

        void add(Register rs, Register rt);
        
        void sub(Register rs, Register rt);
        
        void and(Register rs, Register rt);
        
        void or(Register rs, Register rt);
        
        void slt(Register rs, Register rt);
        
        void beq(Register rs, Register rt, unsigned offset, unsigned pc);

        void addi(Register rs, unsigned imm);
        
        void lw(Register rs, Register *rt, unsigned offset);
        
        void sw(Register rs, Register *rt, unsigned offset);
};

Processor::Processor() : registers 
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
} {}