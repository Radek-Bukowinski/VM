#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "def.h"

/*
    TODO:

        -> memr/memw not memory safe, user can access other locations
        -> fix stack pointer
        -> implement other opcodes
        -> potential unsafe memcpy() in load_program?
        -> is there a need for free_vm()?
        -> figure how to convert hex to ascii
*/

bool running = false;
bool trap = false;

ui8 cycle = 0;

ui16 memory[MEMORY_MAX]; // 1 megabyte of memory?
ui16 program[PROGRAM_MAX] = {0x0000};
ui8 stack[STACK_MAX];

ui8 registers[REGISTER_COUNT] = {0}; // 8 8-bit general purpose registers

ui8* stack_ptr = &stack[STACK_START];
ui8 acc;                                // accumulator
ui16* pc = &memory[PC_START];           // program counter
ui8 flg;                                // flag
ui8 arg;                                // argument

ui16 instruction;
ui8 opcode;
str decoded; // stores string of decoded opcoded

size_t program_alloc_end = 0; // program size

enum flag {
    NEGATIVE,
    ZERO,
    OVERFLOW
};

int err_ex(char* error_msg) {
    running = false;
    printf("Error: %s\n", error_msg);
    return EXIT_FAILURE;
}

void load_program(str file_name) {

    FILE* file_ptr = fopen(file_name, READ_ONLY);  
    if (file_ptr == NULL) {
        err_ex("file open unsuccessful");
    } else {
        printf("File opened \n");
    }

    ui16 buffer[PROGRAM_MAX];

    fread(buffer, sizeof(ui16), PROGRAM_MAX, file_ptr);
    fclose(file_ptr);

    printf("File loaded \n");

    for (size_t i = 0; i < PROGRAM_MAX; i++) {
        if (buffer[i] == 0xf000) {
            program_alloc_end = i;
            memset(buffer + (i + 1), 0x0000, PROGRAM_MAX - (i + 1));
            break;
        }
    }

    size_t bytes = sizeof(program) / sizeof(program[0]);

    memcpy(program, buffer, bytes);
    printf("Program loaded \n");
}

ui8* lookup_reg(ui8 reg) {
    switch(reg) {
        case 0x1:
            return &registers[0];
        case 0x2:
            return &registers[1];
        case 0x3:
            return &registers[2];
        case 0x4:
            return &registers[3];
        case 0x5:
            return &registers[4];
        case 0x6:
            return &registers[5];
        case 0x7:
            return &registers[6];
        case 0x8:
            return &registers[7];
        case 0xA:
            return &acc;
        case 0xB:
            return &arg;
        default:
            err_ex("register lookup not found");
            return 0;
    }
}

void display_vm(ui16 i) {
    printf("--------------------------------------------\n");
    printf("cycle: %i\n", cycle);
    printf("--------------------------------------------\n");
    printf("instruction\n");
    printf("    fetched: %x \n", i);
    printf("    decoded: %s \n", decoded);
    printf("--------------------------------------------\n");
    printf("special registers \n");
    printf("    pc: %x\n", *pc);
    printf("    acc: %x\n", acc);
    printf("    flag: %x\n", flg);
    printf("    arg: %x\n", arg);
    printf("--------------------------------------------\n");
    printf("general purpose registers \n");
    printf("    r1: %x\n", registers[0]);
    printf("    r2: %x\n", registers[1]);
    printf("    r3: %x\n", registers[2]);
    printf("    r4: %x\n", registers[3]);
    printf("    r5: %x\n", registers[4]);
    printf("    r6: %x\n", registers[5]);
    printf("    r7: %x\n", registers[6]);
    printf("    r8: %x\n", registers[7]);
    printf("--------------------------------------------\n\n\n");
}

void free_vm() { }

void print_mem() {
    for (size_t i = 0; i < MEMORY_MAX; i++) {
        printf("%li: %x\n", i, memory[i]);
    }
}

void print_stack() {
    for (size_t i = 0; i < STACK_MAX; i++) {
        printf("%li: %x\n", i, stack[i]);
    }
}

void op_load() {
    ui8* reg = lookup_reg(NYBL_2(I));
    ui8 val = NYBL_3(I);
    *reg = val;
}
void op_move() {
    ui8* dest = lookup_reg(NYBL_2(I));
    ui8* src = lookup_reg(NYBL_3(I));
    *dest = *src;
    *src = 0;
}
void op_memr() {
    ui8* dest = lookup_reg(NYBL_2(I));
    ui8 mem_address = I & 0xFF;
    *dest = memory[(mem_address)];
}
void op_memw() {
    ui8 dest = (I >> 4) & 0xFF;
    ui8* src = lookup_reg(NYBL_4(I));
    memory[dest] = *src;
}
void op_and() {
    ui8* reg1 = lookup_reg(NYBL_2(I));
    ui8* reg2 = lookup_reg(NYBL_3(I));
    acc = *reg1 & *reg2;
}
void op_or() {
    ui8* reg1 = lookup_reg(NYBL_2(I));
    ui8* reg2 = lookup_reg(NYBL_3(I));
    acc = *reg1 | *reg2;
}
void op_not() {
    ui8* reg = lookup_reg(NYBL_2(I));
    acc = ~(*reg);
}
void op_jmp() { }
void op_add() { }
void op_minus() { }
void op_mult() { }
void op_div() { }
void op_push() { 
    //ui8* src = lookup_reg(NYBL_2(I));
    // (hex_value >> 4) & 0xFF
    ui8 src = (I >> 4) & 0xFF; // get the next middle two hex values
    *stack_ptr = src;
    stack_ptr++;
}
void op_pop() {
    stack_ptr--;
    ui8* dest = lookup_reg(NYBL_2(I));
    *dest = *stack_ptr;
    *stack_ptr = 0;
}
 
void op_syscall() {
    printf("syscall\n");
    switch(NYBL_2(I)) {
        case 0x0: ; // exit program
            decoded = "syscall exit";
            running = false;
            break;
        
        case 0x1: ; // stdout
        {
            decoded = "syscall stdout";
            char printable[arg + 1];
            memset(printable, '\0', sizeof(printable));
            for (size_t i = 0; i < arg; i++) {
                stack_ptr--;
                char temp = (char) *stack_ptr;
                printable[i] = temp;
            }            

            printf("\nstdout\n");
            printf("--------------------------------------------\n ");
            for (int i = arg; i >= 0; i--) {
                printf("%c", printable[i]);
            }
            printf("\n--------------------------------------------\n\n");

            break;
        }
        case 0x2: ; //stdin
            break;
        
        case 0x3: ; // zero-out
            break;

        default: ;
            err_ex("unrecognized instruction in syscall");
            break;
    }
}


void run() { 
    printf("VM running...\n");

    while (running) {
        getchar();
        cycle++;

        instruction = *pc;
        opcode = NYBL_1(I);        
        pc++;

        switch (opcode) {

            case 0x0: ; // nop
                decoded = "nop";
                break;

            case 0x1: ; // load
                decoded = "load";
                op_load();
                break;

            case 0x2: ; // move
                decoded = "move";
                op_move();
                break;

            case 0x3: ; // memory read
                decoded = "memory read";
                op_memr();
                break;
            
            case 0x4: ; // memory write
                decoded = "memory write";
                op_memw();
                break;

            case 0x5: ; // and
                decoded = "and";
                op_and();
                break;
            
            case 0x6: ; // or
                decoded = "or";
                op_or();
                break;

            case 0x7: ; // not
                decoded = "not";
                op_not();
                break;
            
            case 0x8: ; // jmp
                decoded = "jmp";
                op_jmp();
                break;
            
            case 0x9: ; // add
                decoded = "add";
                op_add();
                break;
            
            case 0xA: ; // minus
                decoded = "minus";
                op_minus();
                break;
            
            case 0xB: ; // mult
                decoded = "mult";
                op_mult();
                break;
            
            case 0xC: ; // div
                decoded = "div";
                op_div();
                break;
            
            case 0xD: ; // push
                decoded = "push";
                op_push();
                break;
            
            case 0xE: ; // pop
                decoded = "pop";
                op_pop();
                break;

            case 0xF: ; // syscall
                decoded = "syscall";
                op_syscall();
                break;

            default:
                err_ex("unrecognized instruction");
                break;
        }
        display_vm(I);
        decoded = "";
    }
}

int main(int argc, char** argv) {

    load_program("./prog.bin");

    if (program[program_alloc_end] != 0xf000) {
        err_ex("program loaded incorrectly");
    } else {
        size_t bytes = sizeof(program) / sizeof(program[0]);

        if(memcpy(memory + 256, program, bytes) == NULL) {
            err_ex("Program not loaded in memory");
        } else {
            printf("Program loaded in memory\n");
        }

        running = true;
        run();
    }
    
    free_vm();
    printf("VM has finished running\n");
    return 0;
}
