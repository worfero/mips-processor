#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <map>
#include <stdint.h>
#include <bitset>
#include <fstream>
#include <vector>
#include <iomanip>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "./utils/utils.h"

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
    unsigned address : 5;
    unsigned value;
    std::string mnemonic;
} Register;

typedef struct {
    // declaring instruction variables
    unsigned op : 6;
    unsigned rd : 5;
    unsigned rs : 5;
    unsigned rt: 5 ;
    unsigned imm : 16;
    unsigned sa : 5;
    unsigned funct : 6;
    unsigned stage : 3;
    unsigned instBits;
} Instruction;

class Processor {
    private:
    public:
        Processor();

        std::vector<Register> registers;
        std::vector<Instruction> instStack;
        std::vector<uint32_t> memory_space;

        bool writeM;
        bool writeW;
        bool instructionEnd;
        bool stall;
        bool stallReset;
        unsigned stallFlag;
        // program counter
        unsigned pc;
        // opcode
        unsigned opcode;
        // number of instructions
        unsigned program_size;
        unsigned instCounter;
        // result of the ALU operation
        unsigned ALU_resE;
        unsigned ALU_resM;
        unsigned ALU_resW;
        // destination register
        Register *d_regE;
        Register *d_regM;
        Register *d_regW;

        void print_registers();

        void run();

        void init_variables();

        void loadProgram(std::vector<uint32_t> program);

        void fetch(unsigned i);

        void decode(unsigned i);

        void execute(unsigned i);

        void memory(unsigned i);

        void writeback(unsigned i);

        void checkFwd(Register *reg);

        void op_add(Register rs, Register rt);
        
        void op_sub(Register rs, Register rt);
        
        void op_and(Register rs, Register rt);
        
        void op_or(Register rs, Register rt);
        
        void op_slt(Register rs, Register rt);
        
        void op_beq(Register rs, Register rt, unsigned offset);

        void op_addi(Register rs, unsigned imm);
        
        void op_lw(Register rs, Register *rt, unsigned offset);
        
        void op_sw(Register rs, Register *rt, unsigned offset);
};

#endif