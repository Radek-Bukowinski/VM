#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <string.h>

using namespace std;

string program_name;
uint16_t program[128] = {0x0000};

uint16_t str_to_hex(string word) {
    if (word == "nop") { return 0x0; } 
    else if (word == "load") { return 0x1; } 
    else if (word == "move") { return 0x2; } 
    else if (word == "memr") { return 0x3; } 
    else if (word == "memw") { return 0x4; } 
    else if (word == "and") { return 0x5; } 
    else if (word == "or") { return 0x6; } 
    else if (word == "not") { return 0x7; } 
    else if (word == "jmp") { return 0x8; } 
    else if (word == "add") { return 0x9; } 
    else if (word == "minus") { return 0xA; } 
    else if (word == "mult") { return 0xB; } 
    else if (word == "div") { return 0XC; } 
    else if (word == "push") { return 0xD; } 
    else if (word == "pop") { return 0xE; } 
    else if (word == "syscall") { return 0xF; } 
    else if (word == "exit") { return 0x0; } 
    else if (word == "stdout") { return 0x1; } 
    else if (word == "acc") { return 0xA; } 
    else if (word == "arg") { return 0xB; } 
    else if (word == "r1") { return 0x1; } 
    else if (word == "r2") { return 0x2; } 
    else if (word == "r3") { return 0x3; } 
    else if (word == "r4") { return 0x4; } 
    else if (word == "r5") { return 0x5; } 
    else if (word == "r6") { return 0x6; } 
    else if (word == "r7") { return 0x7; } 
    else if (word == "r8") { return 0x8; } 
    else if (word[0] == '0') {
        stringstream ss;
        ss << hex << word.substr(2, 2);
        uint16_t x;
        ss >> x;
        return x;
    } 
    else { return 0x0; }
}

void write_program(uint16_t program[], size_t count) {
    ofstream file("prog.bin", ios::binary | ios::out);

    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(program), sizeof(uint16_t) * count);
    } else {
        cerr << "Failed to open file | write_program" << endl;
    }
}

int main(int argc, char *argv[]) {

    if (argc > 2) {
        cout << "Too many arguments." << '\n';
        exit(1);
    } else {
        program_name = argv[1];
        cout << "Selected program: " << program_name << "\n\n";
    }

    ifstream file(program_name);
    if (!file) {
        cerr << "Failed to open file | read user input \n";
        return 1;
    }

    string line;
    vector<vector<string>> lines;

    while (getline(file, line)) {
        
        vector<string> result;
        istringstream iss(line);
        string word;
        
        while (iss >> word) {
            result.push_back(word);
        }
        lines.push_back(result);
    }

    file.close();

    int pc = 0;
    cout << "Output program" << '\n';
    for (const auto& row : lines) {
        uint16_t buffer[4] = {0x0000};
        int i = 0;
        for (const auto& word : row) {
            uint16_t temp = str_to_hex(word);
            if (temp > 0xF) {
                buffer[i] = (temp & 0xF0) >> 4;
                buffer[i + 1] = temp & 0x0F;
                i++;
            } else {
                buffer[i] = temp;
            }
            i++;
        }

        uint16_t hexed;

        for (uint16_t val : buffer) {
            hexed = (hexed << 4) | val;
        }

        program[pc] = hexed;
        cout << "pc[" << pc << "] = " << hex << program[pc] << "\n";
        pc++;

        fill(begin(buffer), end(buffer), 0x0000);
    }
    
    size_t count = sizeof(program) / sizeof(program[0]);
    write_program(program, count);

    return 0;
}