#ifndef DEF_H
#define DEF_H

#define READ_ONLY "rb"

#define MEMORY_MAX 1024
#define PROGRAM_MAX 128
#define STACK_MAX 128
#define REGISTER_COUNT 8
#define PC_START 0x100
#define STACK_START 0x0

#define I instruction

#define NYBL_1(i) (i & 0xF000) >> 12
#define NYBL_2(i) (i & 0xF00) >> 8
#define NYBL_3(i) (i & 0xF0) >> 4
#define NYBL_4(i) (i & 0xF)

#define ui32 uint32_t
#define ui16 uint16_t
#define ui8  uint8_t
#define str char*

#endif