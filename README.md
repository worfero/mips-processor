# mips-processor
Basic MIPS processor emulator. 

It works as a single-cycle processor with some instructions defined.

It also has a built-in assembler where you can compile a .asm file into a .bin file. The processor is going to search for a "mips-assembler/machine-code.bin" file in order to work correctly so if you do not plan to use the built-in assembler, you must paste a .bin file with a compiled program in the same folder with the same name.

The "multi-cycle" branch features a multi-cycle version of the processor which is still under development and doesn't quite work properly. Feel free to take a look.
