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
        -> figure how to convert hex to ascii - done

        -> add ui
        -> option to load different programs
        -> option to load another program after one finishes
        -> run the program in a thread

        -> add flags functionality ( update_flags() )

        -> add variables written in program

            var test 0xFFAA
            var new 0x0090

            memory[0x0385] = 0xFFAA (test)
            memory[0x0386] = Ox0090 (new)

        -> add flag type for flg variable
*/

bool running = false;
bool trap = false;

ui8 cycle = 0;

ui32 memory[MEMORY_MAX] = {0x0000}; // 1 megabyte of memory?
ui32 program[PROGRAM_MAX] = {0x0000};
ui16 stack[STACK_MAX];

ui16 registers[REGISTER_COUNT] = {0}; // 8 8-bit general purpose registers

ui16* stack_ptr = &stack[STACK_START];
ui16 acc;                                // accumulator
ui32* pc = &memory[PC_START];           // program counter
ui16 flg;                                // flag
ui16 arg;                                // argument

ui32 instruction;
ui8 opcode;
c_str decoded; // stores string of decoded opcoded

size_t program_alloc_end = 0; // program size

enum flag {
    POSITIVE,
    NEGATIVE,
    ZERO,
    OVERFLOW
};

int err_ex(char* error_msg) {
    running = false;
    printf("Error: %s\n", error_msg);
    return EXIT_FAILURE;
}

void load_program(c_str file_name) {

    FILE* file_ptr = fopen(file_name, READ_ONLY);  
    if (file_ptr == NULL) {
        err_ex("file open unsuccessful");
    } else {
        printf("File opened \n");
    }

    ui32 buffer[PROGRAM_MAX];

    fread(buffer, sizeof(ui32), PROGRAM_MAX, file_ptr);
    fclose(file_ptr);

    printf("File loaded \n");

    for (size_t i = 0; i < PROGRAM_MAX; i++) {
        if (buffer[i] == 0xf0000000) {
            program_alloc_end = i;
            memset(buffer + (i + 1), 0x00000000, PROGRAM_MAX - (i + 1));
            break;
        }
    }

    size_t bytes = sizeof(program) / sizeof(program[0]);

    memcpy(program, buffer, bytes);
    printf("Program loaded \n");
}

ui16* lookup_reg(ui16 reg) {
    switch(reg) {
        case 0x1: return &registers[0];
        case 0x2: return &registers[1];
        case 0x3: return &registers[2];
        case 0x4: return &registers[3];
        case 0x5: return &registers[4];
        case 0x6: return &registers[5];
        case 0x7: return &registers[6];
        case 0x8: return &registers[7];
        case 0xA: return &acc;
        case 0xB: return &arg;
        default:
            err_ex("register lookup not found");
    }
}

void display_vm(ui32 i) {
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

void print_mem() {
    for (size_t i = 0; i < MEMORY_MAX; i++) {
        printf("%li: %x\n", i, memory[i]);
    }
}

void print_stack() {
    for (size_t i = 0; i < STACK_MAX; i++) {
        if (stack[i] == 0) {
            break;
        }
        printf("%li: %x\n", i, stack[i]);
    }
}

void clear_stack() {
    memset(stack, 0x0, sizeof(stack));
}
void clear_memory() {
    memset(memory, 0x0, sizeof(memory));
}

ui32* goto_label(ui32 label) {
    printf("checking label\n");
    ui32* t_pc = &memory[PC_START];
    for (int i = 0; i < PROGRAM_MAX; i++) {
        ui32 h = LABEL_NUM(*t_pc);
        if (h == label) {
            printf("label found\n");
            return t_pc;
        }
        t_pc++;
    }
    return pc;
}

void op_nop() {
    decoded = "nop";
    //ui8 label = ((I >> 4) & 0xF);
    //printf("%x\n", label);
}
void op_load() {
    decoded = "load";
    ui16* reg = lookup_reg(SECOND_HEX(I));
    ui16 val = HEX_VAL(I);
    *reg = val; 
}
void op_move() {
    decoded = "move";
    ui16* dest = lookup_reg(SECOND_HEX(I));
    ui16* src = lookup_reg(THIRD_HEX(I));
    *dest = *src;
    *src = 0;
}
void op_shiftl() {
    decoded = "shift left";
    ui16* src = lookup_reg(SECOND_HEX(I));
    acc = *src << 1;
}
void op_shiftr() {
    decoded = "shift right";
    ui16* src = lookup_reg(SECOND_HEX(I));
    acc = *src >> 1;
}
void op_and() {
    decoded = "and";
    ui16* reg1 = lookup_reg(SECOND_HEX(I));
    ui16* reg2 = lookup_reg(THIRD_HEX(I));
    acc = *reg1 & *reg2;
}
void op_or() {
    decoded = "or";
    ui16* reg1 = lookup_reg(SECOND_HEX(I));
    ui16* reg2 = lookup_reg(THIRD_HEX(I));
    acc = *reg1 | *reg2;
}
void op_not() {
    decoded = "not";
    ui16* reg = lookup_reg(SECOND_HEX(I));
    acc = ~(*reg);
}
void op_xor() {
    decoded = "xor";
    ui16* reg1 = lookup_reg(SECOND_HEX(I));
    ui16* reg2 = lookup_reg(THIRD_HEX(I));
    acc = *reg1 ^ *reg2;
}
void op_jpos() {
    decoded = "jpos";
    ui16* check = lookup_reg(SECOND_HEX(I));
    printf("%x\n", *check);
    if (*check > 0x0000) {
        printf("is positive\n");
        pc = goto_label(LABEL_NUM(I));
    } else {
        return;
    }
}
void op_jneg() {
    decoded = "jneg";
    if (acc < 0x0000) {
        pc = goto_label(LABEL_NUM(I));
    } else {
        return;
    }
}
void op_jzero() {
    decoded = "jzero";
    ui16* check = lookup_reg(SECOND_HEX(I));
    if (*check == 0x0000) {
        pc = goto_label(LABEL_NUM(I));
    } else {
        return;
    }
}
void op_jump() {
    decoded = "jump";
    pc = goto_label(LABEL_NUM(I));
}
void op_push() {
    decoded = "push";
    ui16 src = HEX_VAL_ALT(I);
    *stack_ptr = src;
    stack_ptr++;
}
void op_pop() {
    decoded = "pop";
    stack_ptr--;
    ui16* dest = lookup_reg(SECOND_HEX(I));
    *dest = *stack_ptr;
    *stack_ptr = 0;
}
 
void op_syscall() {
    printf("syscall\n");
    switch(SECOND_HEX(I)) {
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
                *stack_ptr = 0;

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
        opcode = FIRST_HEX(I);        
        pc++;
        
        switch (opcode) {

            case 0x0: op_nop(); break;
            case 0x1: op_load(); break;
            case 0x2: op_move(); break;
            case 0x3: op_shiftl(); break;
            case 0x4: op_shiftr(); break;
            case 0x5: op_and(); break;
            case 0x6: op_or(); break;
            case 0x7: op_not(); break;
            case 0x8: op_xor(); break;
            case 0x9: op_jpos(); break;
            case 0xA: op_jneg(); break;
            case 0xB: op_jzero(); break;
            case 0xC: op_jump(); break;
            case 0xD: op_push(); break;
            case 0xE: op_pop(); break;
            case 0xF: op_syscall(); break;

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
                        
    if (program[program_alloc_end] != 0xf0000000) {
        err_ex("program loaded incorrectly");
    } else {
        size_t bytes = sizeof(program) / sizeof(program[0]);

        if(memcpy(memory + 256, program, bytes) == NULL) {
            err_ex("Program not loaded in memory");
        } else {
            printf("Program copied to memory\n");
        }

        running = true;
        run();
    }
    
    printf("VM has finished running\n");
    return 0;
}
