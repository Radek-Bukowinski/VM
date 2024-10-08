# 16 bit VM

A hobby project to make a VM emulator for a simple custom CPU

Notes:
- The CPU and the assembly language are toy ideas thought of by me
- Inspired by: https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c#implementing-the-vm
- Values are 16 bit
  - Registers hold 16 bit values
  - Memory has 16 bit addresses to 16 bit values

## Compilation

``` 
gcc -o ./bin/a.out vm.c
./bin/a.out
```

## Registers
#### `stack_ptr (Stack Pointer)`
> pointer to the top of the stack

#### `acc (Accumulator)`
> holds results of any calculations

#### `arg (Argument)`
> Used for passing arguments to the VM

#### `pc (Program Counter)`
> pointer to the next instruction

#### `flg (Flag)`
> holds any flags raised

#### `r0 - r8 (General Purpose Registers)`
> used for any purpose

---
## Instructions
#### `load`
Loads integer into a register
```
load [target register] [value]

e.g.

load r1 0x5
```

#### `move`
Move contents of one register to another
```
move [destination register] [source register]

e.g.

move r2 acc
```

#### `memr`
Read from an area of memory
```
memr 0x23AB r1
```

#### `memw`
Write to an area of memory
```

```

#### `syscall`
Performs a system call
```
syscall [call code]

e.g.

syscall exit
```

#### `and`
Performs the and operation on two values
```
and [register1] [register 2]

e.g.

and r1 r2
```

#### `or`
Performs the or operation on two values

#### `not`
Performs the not operation on one value

#### `xor`
Performs the xor operation one two values

#### `push`
Pushes a value to the stack from a register

#### `pop`
Pops a value from the stack to a register

#### `brp`
Branches to label if value stored in acc is positive

#### `brn`
Branches to label if value stored in acc is negative

#### `jump`
Jumps to label

#### `nop`
No operation

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