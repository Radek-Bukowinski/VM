# Custom VM

A hobby project to make a VM emulator for a simple custom CPU

Notes:
- The CPU and the assembly language are toy ideas thought of by me
- Inspired by: https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c#implementing-the-vm
- Values are 16 bit
  - Registers hold 16 bit values
  - Memory has 16 bit addresses to 16 bit values

## Writing Programs
I've written an assembler to compile human readable assembly into machine code
```
g++-11 -o ./bin/as assembler.cpp

./bin/as [name_of_your_program].txt
```

## Compilation

``` 
gcc -o ./bin/a.out vm.c
./bin/a.out
```

## Registers

### Stack Pointer
Points to the top of the stack\
Moved when `pop` or `push` is used\
Value retrieved to register when `push` is used
```
stack_ptr
``` 

### Accumulator
Holds results of any calculations
```
acc
```
### Argument
Used for passing arguments to the VM
```
arg
```

### Program Counter
Pointer to the next instruction\
Cannot be accessed
```
pc
```

### Flag
Holds any flags raised\
System notifies user if flag raised 
```
flg

...

POSITIVE
OVERFLOW
ZERO
```

### General Purpose Registers
Used for any purpose\
Hold any unsigned 16 bit value
```
r0
...
r8
```

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
memr [address] [destination register]

e.g.

memr 0x23AB r1
```

#### `memw`
Write to an area of memory
```
memw [source register] [address]

e.g.

memw r1 0x1234
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

#### `jpo`
Branches to label if value stored in acc is positive
```
jpo [label]

e.g.

jpo .area

...

.area <- pc jumps to here
```

#### `jne`
Branches to label if value stored in acc is negative

#### `jump`
Jumps to label, regardless of anything

#### `nop`
No operation

## Memory layout
```
ui32 memory[MEMORY_MAX]
...
MEMORY_MAX 1024
```
- Holds 1024 values of size unsigned integer
- Divided into 3 sections: user, program, and system
### User Space
Holds user variables

### Program Space
Holds the users program

### System Reserved Space
Other memory reserved for system purposes
```
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
```
---
## Example program
Print 'Hello'
```
push 0x48
push 0x65
push 0x6C
push 0x6C
push 0x6F
load arg 0x5
syscall stdout
syscall exit
```