#include<bits/stdc++.h>
using namespace std;

struct IF_ID {
    int pc;
    string instruction;
} ;

struct ID_EX {
    int alu_op;
    int rs1; 
    int rs2;
    int rd;
    int imm;
    int pc;
    int alu_src;
    int branch;
};

struct EX_MEM {
    int alu_result;
    int rd;
    int pc;
    int mem_read;
    int mem_write;
};

struct MEM_WB {
    int alu_result;
    int mem_result;
    int wb_src;
    int rd;
    int mem_to_reg;
};
