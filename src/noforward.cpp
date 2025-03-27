#include <bits/stdc++.h>
#include "noforward.hpp"
using namespace std;
#include <string>
#include <bitset>

int binaryStringToInt(const string &binary) {
    int bitWidth = binary.length();
    int value = stoi(binary, nullptr, 2); // Convert binary string to int

    // Check if the number is negative (sign bit is 1)
    if (binary[0] == '1') {
        int signMask = (1 << bitWidth);   // Create a mask (e.g., 1000 for 3-bit)
        value -= signMask;  // Apply sign extension
    }

    return value;
}
string extract_B_imm(const string &inst) {
    string imm12  = inst.substr(0, 1);   // Bit 31 (imm[12])
    string imm11  = inst.substr(24, 1);   // Bit 7 (imm[11])
    string imm10_5 = inst.substr(1, 6); // Bits 30-25 (imm[10:5])
    string imm4_1 = inst.substr(20, 4);  // Bits 11-8 (imm[4:1])

    // Concatenating to form the full immediate in correct order
    string full_imm = imm12 + imm11 + imm10_5 + imm4_1 + "0";  // Shift left by 1
    // cout<<full_imm<<endl;
    return full_imm;
}
string extract_j_imm(const string &inst) {
    string imm20  = inst.substr(0, 1);   // Bit 31 (imm[12])
    string imm11  = inst.substr(11, 1);   // Bit 7 (imm[11])
    string imm10_1 = inst.substr(1, 10); // Bits 30-25 (imm[10:5])
    string imm19_12 = inst.substr(12, 8);  // Bits 11-8 (imm[4:1])

    // Concatenating to form the full immediate in correct order
    string full_imm = imm20 + imm19_12 + imm11 + imm10_1 + "0";  // Shift left by 1
    // cout<<full_imm<<endl;
    return full_imm;
}


vector<string> program;
vector<string> assembly;
vector<int> regs(32, 0);
map <int,int> memory;
struct IF_ID l2;
struct ID_EX l3;
struct EX_MEM l4;
struct MEM_WB l5;
int pc = 0;
void mem_write(bool mem_access, int start_address, int value) {
    if (mem_access) { // true implies word
        memory[start_address] = value % 256;
        memory[start_address + 1] = (value >> 8) % 256;
        memory[start_address + 2] = (value >> 16) % 256;
        memory[start_address + 3] = (value >> 24) % 256;
    } else {
        memory[start_address] = value % 256;
    }
}

int mem_read(bool mem_access, int start_address) {
    int value = 0;
    if (mem_access) { // true implies word
        value = memory[start_address + 3] << 24;
        value += memory[start_address + 2] << 16;
        value += memory[start_address + 1] << 8;
        value += memory[start_address];
    } else {
        value = memory[start_address];
    }
    return value;
}
string IF(int pc) {
    return program[pc];
}

struct ID_EX ID(struct IF_ID inst, bool &stall, bool &branch) {
    struct ID_EX id_ex;
    if (inst.non_empty) id_ex.non_empty = true;
    else {
        id_ex.non_empty = false;
        return id_ex;
    }
    string str = inst.instruction;
    int rs1 = stoi(str.substr(12, 5), nullptr, 2);
    int rs2 = stoi(str.substr(7, 5), nullptr, 2);
    int rd = stoi(str.substr(20, 5), nullptr, 2);
    int funct3 = stoi(str.substr(17, 3), nullptr, 2);
    int funct7 = stoi(str.substr(0, 7), nullptr, 2);
    int opcode = stoi(str.substr(25, 7), nullptr, 2);
    id_ex.imm = 0;
    id_ex.rs1_val = regs[rs1];
    id_ex.rs2_val= regs[rs2];
    id_ex.rd = rd;
    id_ex.pc = inst.pc;
    switch (opcode) {
        case 51: //R-type
            id_ex.alu_src = RS2;
            id_ex.mem_read = 0;
            id_ex.mem_write = 0;
            id_ex.mem_to_reg = 1;
            id_ex.wb_src = ALU;
            if (funct3 == 0) {
                if (funct7 == 0) { //add rs1, rs2
                    id_ex.alu_op = ADD;
                } else if (funct7 == 32) {  //sub rs1, rs2
                    id_ex.alu_op = SUB;
                }
            }
            if (l3.non_empty && l3.rd != 0 && l3.mem_to_reg && (l3.rd == rs1 || l3.rd==rs2)) stall = true;
            if (l4.non_empty && l4.rd != 0 && l4.mem_to_reg && (l4.rd == rs1 || l4.rd==rs2)) stall = true;
            break;
        case 3: //I-type
        if (funct3 == 2) { // lw rs1
                id_ex.mem_read = 1;
                id_ex.mem_write = 0;
                id_ex.mem_to_reg = 1;
                id_ex.wb_src = MEM;
                id_ex.imm = binaryStringToInt(str.substr(0, 12));
                id_ex.alu_op = ADD;
                id_ex.alu_src = IMM;
                id_ex.memaccess = true;
            } else if (funct3 == 0) { // lb rs1
                id_ex.mem_read = 1;
                id_ex.mem_write = 0;
                id_ex.mem_to_reg = 1;
                id_ex.wb_src = MEM;
                id_ex.imm = binaryStringToInt(str.substr(0, 12));
                id_ex.alu_op = ADD;
                id_ex.alu_src = IMM;
                id_ex.memaccess = false;
            }
            if (l3.non_empty && l3.rd != 0 && l3.mem_to_reg && l3.rd == rs1) stall = true;
            if (l4.non_empty && l4.rd != 0 && l4.mem_to_reg && l4.rd == rs1) stall = true;
            break;
        case 19: //I-type
            if (funct3 == 0) { // addi rs1
                id_ex.mem_read = 0;
                id_ex.mem_write = 0;
                id_ex.mem_to_reg = 1;
                id_ex.wb_src = ALU;
                id_ex.imm = binaryStringToInt(str.substr(0, 12));
                id_ex.alu_src = IMM;
                id_ex.alu_op = ADD;
            }
            if (l3.non_empty && l3.rd != 0 && l3.mem_to_reg && l3.rd == rs1 ) stall = true;
            if (l4.non_empty && l4.rd != 0 && l4.mem_to_reg && l4.rd == rs1 ) stall = true;
            break;
        case 103: // jalr
            if ((l3.non_empty && l3.rd != 0 && l3.mem_to_reg && l3.rd == rs1 ) ||
            (l4.non_empty && l4.rd != 0 && l4.mem_to_reg && l4.rd == rs1 )) {
                stall = true;
                break;
            }
            if (funct3 == 0) {
                id_ex.imm = binaryStringToInt(str.substr(0, 12));
                id_ex.mem_read = 0;
                id_ex.mem_write = 0;
                id_ex.mem_to_reg = 1;
                id_ex.wb_src = NEXT_PC;
                id_ex.alu_op = NO_OP;
                int target_pc = id_ex.rs1_val + id_ex.imm;
                branch = true;
                pc = target_pc - 1;
            }
        break;
        case 35: //S-type
            if ((l3.non_empty && l3.rd != 0 && l3.mem_to_reg && (l3.rd == rs1 || l3.rd==rs2)) ||
             (l4.non_empty && l4.rd != 0 && l4.mem_to_reg && (l4.rd == rs1 || l4.rd==rs2))) {
                stall = true;
                break;
            }
            if (funct3 == 2) { // sw, rs1 is address, rs2 is the value
                id_ex.mem_read = 0;
                id_ex.mem_write = 1;
                id_ex.mem_to_reg = 0;
                id_ex.alu_op = ADD;
                id_ex.alu_src = IMM;
                id_ex.memaccess = true;
            } else if (funct3 == 0) { //sb
                id_ex.mem_read = 0;
                id_ex.mem_write = 1;
                id_ex.mem_to_reg = 0;
                id_ex.alu_op = ADD;
                id_ex.alu_src = IMM;
                id_ex.memaccess = false;
            }
            break;
        case 99: // B type
            if ((l3.non_empty && l3.rd != 0 && l3.mem_to_reg && (l3.rd == rs1 || l3.rd==rs2)) ||
             (l4.non_empty && l4.rd != 0 && l4.mem_to_reg && (l4.rd == rs1 || l4.rd==rs2))) {
                stall = true;
                break;
            }
            id_ex.imm = binaryStringToInt(extract_B_imm(inst.instruction));
            if (funct3 == 0) { // beq
                id_ex.mem_read = 0;
                id_ex.mem_write = 0;
                id_ex.mem_to_reg = 0;
                id_ex.alu_op = NO_OP;
                int target_pc = inst.pc + id_ex.imm/4;
                if (id_ex.rs1_val == id_ex.rs2_val) {
                    branch = true;
                    pc = target_pc - 1;
                    // cout<<"pc "<<pc<<endl;
                }
            } else if (funct3 == 5) { // bge
                id_ex.mem_read = 0;
                id_ex.mem_write = 0;
                id_ex.mem_to_reg = 0;
                id_ex.alu_op = NO_OP;
                int target_pc = inst.pc + id_ex.imm/4;
                if (id_ex.rs1_val >= id_ex.rs2_val) {
                    branch = true;
                    pc = target_pc - 1;
                    // cout<<"pc "<<pc<<endl;
                }
            }
            break;
        case 111: // jal type
            id_ex.imm = binaryStringToInt(extract_j_imm(inst.instruction));
            id_ex.mem_read = 0;
            id_ex.mem_write = 0;
            id_ex.mem_to_reg = 1;
            id_ex.wb_src = NEXT_PC;
            id_ex.alu_op = NO_OP;
            int target_pc = inst.pc + id_ex.imm/4;
            pc = target_pc - 1;
            branch = true;
    }
    return id_ex;
}

struct EX_MEM EX( struct ID_EX id_ex) {
    struct EX_MEM ex_mem;
    if (id_ex.non_empty) ex_mem.non_empty = true;
    else {
        ex_mem.non_empty = false;
        return ex_mem;
    }
    ex_mem.pc = id_ex.pc;
    ex_mem.rd = id_ex.rd;
    ex_mem.mem_read = id_ex.mem_read;
    ex_mem.mem_write = id_ex.mem_write;
    ex_mem.mem_to_reg = id_ex.mem_to_reg;
    ex_mem.memaccess = id_ex.memaccess;
    ex_mem.wb_src = id_ex.wb_src;
    if(id_ex.alu_op == NO_OP) return ex_mem;
    int operand1 = id_ex.rs1_val;
    int operand2 = (id_ex.alu_src == RS2) ? id_ex.rs2_val : id_ex.imm;
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
    if (ex_mem.non_empty) mem_wb.non_empty = true;
    else {
        mem_wb.non_empty = false;
        return mem_wb;
    }
    mem_wb.rd = ex_mem.rd;
    mem_wb.alu_result = ex_mem.alu_result;
    mem_wb.mem_to_reg = ex_mem.mem_to_reg;
    mem_wb.wb_src = ex_mem.wb_src;
    mem_wb.pc = ex_mem.pc;
    if (ex_mem.mem_read) {
        mem_wb.mem_result = mem_read(ex_mem.memaccess, ex_mem.alu_result);
    } else if (ex_mem.mem_write) {
        mem_write(ex_mem.memaccess, ex_mem.alu_result, ex_mem.rs2_val);
    }
    return mem_wb;
}

void WB( struct MEM_WB mem_wb) {
    if (mem_wb.non_empty)
    {
        if (mem_wb.mem_to_reg && mem_wb.wb_src == MEM) {
            regs[mem_wb.rd] = mem_wb.mem_result;
        } 
        else if (mem_wb.mem_to_reg && mem_wb.wb_src == ALU) {
            regs[mem_wb.rd] = mem_wb.alu_result;
        } 
        else if (mem_wb.mem_to_reg && mem_wb.wb_src == NEXT_PC) {
            regs[mem_wb.rd] = mem_wb.pc+1;
        }  
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
    l2.non_empty = l3.non_empty = l4.non_empty = l5.non_empty = false;
    char *file;
    file = argv[1];
    int clk_cycle = atoi(argv[2]);
    FILE *fp = fopen(file, "r");
    char inp[100];
    while (fgets(inp, sizeof(inp), fp)) {
        int ind = 0;
        string mach_code = "";
        string assembly_code = "";
        
        while (inp[ind] == ' ') {
            ind++;
        }
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
    int n = program.size();
    // for (int i = 0; i < n; i++) {
    //     cout<<program[i]<<endl;
    // }
    bool finished = false;
    // cout<<pc<<endl;
    vector<int> fetch(clk_cycle, -1), decode(clk_cycle, -1), execute(clk_cycle, -1), mem(clk_cycle, -1), writeback(clk_cycle, -1);
    for (int i = 0; i < clk_cycle; i++) {
        // cout<<fetch[0]<<endl;
        // cout<<"PC "<<pc<<endl;
        bool stall = false;
        bool branch = false;
        //IF
        struct IF_ID nextl2;
        if (pc < n) {
            nextl2 = {pc, IF(pc), true};
            fetch[i] = pc;
        }
        else {
            nextl2 = {pc, "", false};
        }
        // cout<<fetch[i]<<" "<<i<<"ALJKDNS"<<endl;
        //WB
        WB(l5);
        // cout<<l5.non_empty<<endl;
        if (l5.non_empty)
        writeback[i] = l5.pc;
        //ID
        if (l2.non_empty)
        decode[i] = l2.pc;
        struct ID_EX nextl3 = ID(l2, stall, branch);
        // cout<<pc<<" "<<stall<<endl;
        // cout<<l2.instruction<<" "<<stall<<endl;
        
        //EX
        if (l3.non_empty)
        execute[i] = l3.pc;
        struct EX_MEM nextl4 = EX(l3);
        
        //MEM
        if (l4.non_empty)
        mem[i] = l4.pc;
        struct MEM_WB nextl5 = DM(l4);
        
        if (!stall) {
            pc++;
            l2 = nextl2;
            if (branch) l2.non_empty = false;
            l3 = nextl3;
            l5 = nextl5;
            l4 = nextl4;
        } else {
            l3.non_empty = false;
            l4 = nextl4;
            l5 = nextl5;
        }
        // if (pc >= n && !branch) {
        //     cout<<"HELO"<<pc<<endl;
        //     // finished = true;
        // };
    }
    // cout<<"OUT"<<endl;
    // cout<<n<<endl;
    vector<vector<string>> display(n, vector<string> (clk_cycle, " "));
    for (int i = 0; i < clk_cycle; i++) {
        // cout<<fetch[i]<<" "<<decode[i]<<" "<<execute[i]<<" "<<mem[i]<<" "<<writeback[i]<<endl;
        if (writeback[i] >= 0)
        display[writeback[i]][i] = "WB";
        if (mem[i] >= 0)
        display[mem[i]][i] = "MEM";
        if (execute[i] >= 0)
        display[execute[i]][i] = "EX";
        if (decode[i] >= 0)
        display[decode[i]][i] = "ID";
        if (fetch[i] >= 0)
        display[fetch[i]][i] = "IF";
    }
    cout<<setw(20)<<"Assembly";
    for (int i = 0; i < clk_cycle; i++) {
        cout<<";"<<setw(3)<<i;
    }
    for (int i = 0; i < n; i++) {
        cout<<setw(20)<<assembly[i];
        for (int j = 0; j < clk_cycle; j++) {
            if (j > 0) {
                if (display[i][j] == display[i][j-1] && display[i][j] != " ") {
                   cout<<";"<<setw(3)<<'-';
                } else 
                cout<<";"<<setw(3)<<display[i][j];
            } else cout<<";"<<setw(3)<<display[i][j];
        }
        cout<<endl;
    }
    return 0;
}