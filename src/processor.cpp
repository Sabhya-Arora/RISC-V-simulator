#include <bits/stdc++.h>
#include "processor.hpp"
using namespace std;
vector<string> program;
vector<string> assembly;
vector<int> regs(32, 0);
map <int,int> memory;

string IF(int pc) {
    return program[pc];
}

struct ID_EX ID(struct IF_ID inst) {
    struct ID_EX id_ex;
    string str = inst.instruction;
    int rs1 = stoi(str.substr(12, 5), nullptr, 2);
    int rs2 = stoi(str.substr(7, 5), nullptr, 2);
    int rd = stoi(str.substr(20, 5), nullptr, 2);
    int funct3 = stoi(str.substr(17, 3), nullptr, 2);
    int funct7 = stoi(str.substr(0, 7), nullptr, 2);
    int opcode = stoi(str.substr(25, 7), nullptr, 2);
    int imm = 0;
    id_ex.rs1 = rs1;
    id_ex.rs2 = rs2;
    id_ex.rd = rd;
    id_ex.pc = inst.pc;
    switch (opcode) {
        case 51: //R-type
            if (funct3 == 0) {
                if (funct7 == 0) {
                    id_ex.alu_op = 0;
                } else if (funct7 == 32) {
                    id_ex.alu_op = 1;
                }
            }
            break;
        case 3: //I-type
            if (funct3 == 2) { // lw
                imm = stoi(str.substr(0, 12), nullptr, 2);
                id_ex.alu_op = 2;
            }
            break;
        case 19: //I-type
            if (funct3 == 0) { // addi
                imm = stoi(str.substr(0, 12), nullptr, 2);
                id_ex.alu_op = 3;
            }
            break;
        case 35: //S-type
            if (funct3 == 2) { // sw, rs1 is address
                imm = stoi(str.substr(0, 7) + str.substr(20, 5), nullptr, 2);
                id_ex.alu_op = 4;
            }
            break;
        case 111: //rd, imm
            imm = stoi(str.substr(0, 20), nullptr, 2);
            id_ex.alu_op = 5;
            break;
    }
    
    return id_ex;
}

struct EX_MEM EX( struct ID_EX id_ex) {
    struct EX_MEM ex_mem;
    ex_mem.pc = id_ex.pc;
    ex_mem.rd = id_ex.rd;
    ex_mem.mem_read = id_ex.mem_read;
    ex_mem.mem_write = id_ex.mem_write;
    ex_mem.mem_to_reg = id_ex.mem_to_reg;
    ex_mem.wb_src = id_ex.wb_src;
    int operand1 = regs[id_ex.rs1];
    int operand2 = (id_ex.alu_src == RS2) ? regs[id_ex.rs2] : id_ex.imm;
    switch (id_ex.alu_op) {
        case ADD:
            ex_mem.alu_result = operand1 + operand2;
            break;
        case SUB: 
            ex_mem.alu_result = operand1 - operand2;
            break;
    }
    return ex_mem;
}

struct MEM_WB DM ( struct EX_MEM ex_mem) {
    struct MEM_WB mem_wb;
    mem_wb.rd = ex_mem.rd;
    mem_wb.alu_result = ex_mem.alu_result;
    mem_wb.mem_to_reg = ex_mem.mem_to_reg;
    mem_wb.wb_src = ex_mem.wb_src;
    if (ex_mem.mem_read) {
        mem_wb.mem_result = memory[ex_mem.alu_result];
    } else if (ex_mem.mem_write) {
        memory[ex_mem.alu_result] = ex_mem.rd;
    }
    return mem_wb;
}

void WB( struct MEM_WB mem_wb) {
    if (mem_wb.mem_to_reg && mem_wb.wb_src == MEM) {
        regs[mem_wb.rd] = mem_wb.mem_result;
    } 
    else if (mem_wb.mem_to_reg && mem_wb.wb_src == ALU) {
        regs[mem_wb.rd] = mem_wb.alu_result;
    }
}

string hexToBinary(string hex) {
    string binary = "";
    for (char c : hex) {
        switch (toupper(c)) {
            case '0': binary += "0000"; break;
            case '1': binary += "0001"; break;
            case '2': binary += "0010"; break;
            case '3': binary += "0011"; break;
            case '4': binary += "0100"; break;
            case '5': binary += "0101"; break;
            case '6': binary += "0110"; break;
            case '7': binary += "0111"; break;
            case '8': binary += "1000"; break;
            case '9': binary += "1001"; break;
            case 'A': binary += "1010"; break;
            case 'B': binary += "1011"; break;
            case 'C': binary += "1100"; break;
            case 'D': binary += "1101"; break;
            case 'E': binary += "1110"; break;
            case 'F': binary += "1111"; break;
        }
    }
    return binary;
}
int main(int argc, char * argv[]) {
    char *file;
    file = argv[1];
    int clk_cycle = atoi(argv[2]);
    FILE *fp = fopen(file, "r");
    char inp[100];
    while (fgets(inp, sizeof(inp), fp)) {
        int ind = 0;
        string mach_code = "";
        string assembly_code = "";
        while (inp[ind] != ':') {
            ind++;
        }
        ind++;
        while (inp[ind] == ' ') {
            ind++;
        }
        while (inp[ind] != ' ') {
            mach_code += inp[ind];
            ind++;
        }
        while (inp[ind] == ' ') {
            ind++;
        }
        while (inp[ind] != '\n') {
            assembly_code += inp[ind];
            ind++;
        }
        string str = hexToBinary(mach_code);
        program.push_back(str);
        assembly.push_back(assembly_code);
    }
    int pc = 0;
    for (int i = 0; i < clk_cycle; i++) {
        //IF
        struct IF_ID l2 = {pc, IF(pc)};
        //ID

        //EX

        //MEM

        //WB

    }
    
    
    return 0;
}