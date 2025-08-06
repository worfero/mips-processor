# mips-processor
Basic MIPS processor emulator. 

It works as a pipelined processor with some but not all instructions defined.

Work is still in progress. So far it is capable of port forwarding and stall for hazards but the branching instructions are still yet to be done.

There are two other branches called single-cycle and multi-cycle with better working versions and branching instructions, but they are not pipelined.

It also has a built-in assembler where you can compile a .asm file into a .bin file. The processor is going to search for a "mips-assembler/machine-code.bin" file in order to work correctly so if you do not plan to use the built-in assembler, you must paste a .bin file with a compiled program in the same folder with the same name.