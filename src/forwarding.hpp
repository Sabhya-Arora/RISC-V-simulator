#include<bits/stdc++.h>
using namespace std;

enum ALU_OP {
    ADD = 0,
    SUB,
    AND,
    OR,
    XOR,
    SLL,
    SRL,
    SRA,
    SLT,
    SLTU,
    MUL,
    MULH,
    MULHSU,
    MULHU,
    DIV,
    DIVU,
    REM,
    REMU,
    NO_OP
};

enum ALU_SRC {
    RS2 = 0,
    RS1,
    L4,
    L5_ALU,
    L5_MEM,
    L4_PC,
    L5_PC,
    IMM
};

enum WB_SRC {
    ALU = 0,
    MEM,
    NEXT_PC
};

struct IF_ID {
    int pc;
    string instruction;
    bool non_empty;
};

struct ID_EX {
    int alu_op;
    int rs1_val; 
    int rs2_val;
    int rd;
    int imm;
    int pc;
    int alu_src1;
    int alu_src2;
    int mem_read, mem_write, mem_to_reg, mem_src;
    int wb_src;
    bool non_empty;
    bool mem_access;
};

struct EX_MEM {
    int alu_result;
    int rs2_val;
    int rd;
    int pc;
    int mem_read;
    int mem_write;
    int mem_to_reg;
    int mem_src;
    int wb_src;
    bool non_empty;
    bool mem_access;
};

struct MEM_WB {
    int alu_result;
    int mem_result;
    int pc;
    int wb_src;
    int rd;
    int mem_to_reg;
    bool non_empty;
};
