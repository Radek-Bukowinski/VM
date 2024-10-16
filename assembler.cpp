#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

#include "def.h"

using namespace std;

string program_name;
ui32 program[PROGRAM_MAX] = {0x0000};

bool hexed = false;
bool label = false;
bool var = false;

ui16 var_count = 0x00; // range = 0x0000 -> 0x000F
ui16 label_count = 0x10; // range = 0x0010 -> 0x0020

unordered_map<string, ui16> vars;
unordered_map<string, ui16> labels;

ui16 lookup_hex(string word) {
    if (word == "nop") { return 0x0; } 
    else if (word == "load") { return 0x1; } 
    else if (word == "move") { return 0x2; } 
    else if (word == "shiftl") { return 0x3; } 
    else if (word == "shiftr") { return 0x4; } 
    else if (word == "and") { return 0x5; } 
    else if (word == "or") { return 0x6; } 
    else if (word == "not") { return 0x7; } 
    else if (word == "xor") { return 0x8; } 
    else if (word == "jpos") { return 0x9; } 
    else if (word == "jneg") { return 0xA; } 
    else if (word == "jzero") { return 0xB; } 
    else if (word == "jump") { return 0XC; } 
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
        ss << hex << word.substr(2, 4);
        ui16 x;
        ss >> x;
        hexed = true;
        return x;
    }
    else if(word[0] == '.') {
        label = true;
        auto it = labels.find(word);
        if (it != labels.end()) {
            return it -> second;
        } else {
            labels[word] = label_count;        
            return label_count;
        }
    }
    else { return 0x0; }
}

void write_program(ui32 program[], size_t count) {
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

    ifstream file("./programs/" + program_name);
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
        bool skip = false;
        
        while (iss >> word) {
            if (word[0] == ';') {
                skip = true;
                break;
            } else {
                result.push_back(word);
            }
        }
        if (!skip) {
            lines.push_back(result);
        }
    }

    file.close();

    int pc = 0;
    cout << "Output program" << '\n';
    for (const auto& row : lines) {
        ui16 buffer[8] = {0x0};
        int i = 0;
        for (const auto& word : row) {
            ui16 temp = lookup_hex(word);
            if (hexed) {
                buffer[i] = (temp >> 12) & 0xF;
                buffer[i + 1] = (temp >> 8) & 0xF;
                buffer[i + 2] = (temp >> 4) & 0xF;
                buffer[i + 3] = temp & 0xF;
                hexed = false;
            } else if (label) {
                //cout << "temp: " << hex << temp << '\n';
                buffer[6] = (temp >> 4) & 0xF;
                label = false;
            }else {
                buffer[i] = temp;
            }
            i++;
        }

        ui32 hexed;

        for (ui16 val : buffer) {
            //cout << "val: " << hex << val << '\n';
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