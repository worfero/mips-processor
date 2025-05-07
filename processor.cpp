#include <iostream>
#include <string>
#include <map>

#define MAX_INST_SIZE 1024

uint32_t inst_memory[MAX_INST_SIZE];

// saves the values of all registers available
typedef struct {
    int address;
    int value;
} Register;

static const Register registers[] = 
{
    {0, 0x00},      // $0 - constant 0
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
    {17, 0x00},    // $s0
    {18, 0x00},    // $s0
    {19, 0x00},    // $s0
    {20, 0x00},    // $s0
    {21, 0x00},    // $s0
    {22, 0x00},    // $s0
    {23, 0x00},    // $s0
    {24, 0x00},    // $t8
    {25, 0x00},    // $t9
    {26, 0x00},    // $k0
    {27, 0x00},    // $k1
    {28, 0x00},    // $gp - global pointer
    {29, 0x00},    // $sp - stack pointer
    {30, 0x00},    // $fp - frame pointer
    {31, 0x00}     // $ra - procedure return address
};

int main(){
    
}