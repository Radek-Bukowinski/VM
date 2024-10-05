# 16 bit VM

A hobby project to make a VM emulator for a simple custom CPU

## Registers
#### stack_ptr (Stack Pointer)
> pointer to the top of the stack

#### acc (Accumulator)
> holds results of any calculations

#### pc (Program Counter)
> pointer to the next instruction

#### flg (Flag)
> holds any flags raised

#### r0 - r8 (General Purpose Registers)
> used for any purpose

---
## Instructions
#### load
> loads integer into a register

#### move
> move contents of one register to another

#### memw
> write to an area of memory

#### memr
> read from an area of memory

#### syscall
> performs a system call

## Memory layout
     0 
     |
    ...     USER SPACE
     |
    255 

    256
     | 
    ...     PROGRAM SPACE
     |
    384

    385
     |
    ...     SYSTEM RESERVED SPACE
     |
    1023

---
## Example program
```
load r1 5

memw &0x12 r1

memr r2 &0x12

move r3 r1

syscall exit
```
Is interpreted by the VM as:

```
0x1150
0x4121
0x3212
0x2310
0xF000
```