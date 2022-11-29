#!/bin/sh
nasm -f elf64 -g -F dwarf edit_distance.asm -o edit_distance.o && \
gcc  -DASM_VER -g lev-asm.c edit_distance.o -no-pie -fno-pic -o la
#gcc -DLINEAR_VER -g lev-asm.c -no-pie -fno-pic -o la
