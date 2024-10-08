#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

uint16_t h_buffer[3];

vector<string> split_into_vec(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    stringstream ss(str);

    // Split the string by the delimiter
    while (getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

auto str_to_hex(string word) -> uint16_t {

    if (word == "nop") {
        return 0x0;
    } else if (word == "load") {
        return 0x1;
    } else if (word == "move") {
        return 0x2;
    } else if (word == "memr") {
        return 0x3;
    } else if (word == "memw") {
        return 0x4;
    } else if (word == "and") {
        return 0x5;
    } else if (word == "or") {
        return 0x6;
    } else if (word == "not") {
        return 0x7;
    } else if (word == "jmp") {
        return 0x8;
    } else if (word == "add") {
        return 0x9;
    } else if (word == "minus") {
        return 0xA;
    } else if (word == "mult") {
        return 0xB;
    } else if (word == "div") {
        return 0XC;
    } else if (word == "push") {
        return 0xD;
    } else if (word == "pop") {
        return 0xE;
    } else if (word == "syscall") {
        return 0xF;
    } else if (word[0] == '0') {
        
        // 0x0 -> l=3
        // 0x00 -> l=4
        // 0x000 -> l=5
        // 0x0000 -> l=6

        stringstream ss;

        if (word.length() == 3) {
            ss << hex << word[2];
            ss >> h_buffer[0];
            ss.clear();
            ss.str("");
        } else if (word.length() == 4) {
            for (size_t i = 0; i < 2; i++) {
                ss << hex << word[2 + i];
                ss >> h_buffer[i];
                ss.clear();
                ss.str("");
            }
        } else if (word.length() == 5) {

        } else {
            cerr << "Invalid hex";
        }
        return 0;
    } else if (word == "exit") {
        return 0x0;
    } else if (word == "stdout") {
        return 0x1;
    } else if (word == "acc") {
        return 0xA;
    } else if (word == "arg") {
        return 0xB;
    } else if (word == "r1") {
        return 0x1;
    } else if (word == "r2") {
        return 0x2;
    } else if (word == "r3") {
        return 0x3;
    } else if (word == "r4") {
        return 0x4;
    } else if (word == "r5") {
        return 0x5;
    } else if (word == "r6") {
        return 0x6;
    } else if (word == "r7") {
        return 0x7;
    } else if (word == "r8") {
        return 0x8;
    } else {
        return 0x0;
    }
}

auto write_program(uint16_t program[], size_t count) {
    ofstream file("prog.bin", ios::binary | ios::out);

    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(program), sizeof(uint16_t) * count);
    } else {
        cerr << "Failed to open file | write_program" << endl;
    }
}

auto main() -> int {
    ifstream file("program.txt");
    if (!file) {
        cerr << "Failed to open file | read user input \n";
        return 1;
    }

    string line;
    vector<vector<string>> lines;

    // Read each line and store it in the vector
    while (getline(file, line)) {

        vector<string> line_vector = {line};

        lines.push_back(split_into_vec(line_vector[0], ' '));
    }

    file.close();

    uint16_t program[128];

    vector<uint16_t> current_line = {0x0, 0x0, 0x0, 0x0};

    int pc = 0;
    for (const auto& row : lines) {
        int i = 0;
        for (const auto& word : row) {

            if (word[0] == '0') {
                str_to_hex(word);

                if (word.length() == 3) {
                    current_line[i] = h_buffer[0];
                } else if (word.length() == 4) {
                    current_line[i] = h_buffer[0];
                    i++;
                    current_line[i] = h_buffer[1];
                } else if (word.length() == 5) {
                    current_line[i] = h_buffer[0];
                    i++;
                    current_line[i] = h_buffer[1];
                    i++;
                    current_line[i] = h_buffer[2];
                } else {
                    cerr << "Invalid hex";
                }

            } else {
                current_line[i] = str_to_hex(word);
            }
            i++;
        }

        h_buffer[0] = 0x0;
        h_buffer[1] = 0x0;
        h_buffer[2] = 0x0;

        uint16_t hexed;
        for (uint16_t val : current_line) {
            hexed = (hexed << 4) | val; // Shift left by 4 bits and add the next value
        }

        program[pc] = hexed;
        cout << "pc: " << pc << " = " << hex << program[pc] << "\n";
        pc++;

        current_line = {0x0, 0x0, 0x0, 0x0};

        cout << "\n";
    }
    
    size_t count = sizeof(program) / sizeof(program[0]);
    write_program(program, count);
}