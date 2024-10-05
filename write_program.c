#include <stdio.h>
#include <inttypes.h>

#define MAX_VALUES 100

int main() {
    FILE *file = fopen("prog.bin", "wb");

    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    /*
    uint16_t values[] = {
        0x1110, 
        0x2210,
        0x3111,
        0x4121,
        0xF000
    };
    */

    uint16_t general_test[] = {
        0x1150,             // load
        0x4121,             // memw
        0x3212,             // memr
        0x2310,             // move
        0xD200,             // push
        0xE800,             // pop
        0x5280,             // and  
        0x25A0,             // move
        0xF000
    };

    uint16_t print_test[] = {
        // 48 65 6C 6C 6F = 'Hello'

        0xD480,         // push 0x48
        0xD650,         // push 0x65
        0xD6C0,         // push 0x6C
        0xD6C0,         // push 0x6C
        0xD6F0,         // push 0x6F

        0x1B50,         // load str length to arg register
        0xF100,         // syscall stdout
        0xF000,         // syscall exit
    };

    size_t count = sizeof(print_test) / sizeof(print_test[0]);

    // Write the binary data to the file
    fwrite(print_test, sizeof(uint16_t), count, file);

    return 0;
}
