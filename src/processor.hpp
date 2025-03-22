#include<bits/stdc++.h>
using namespace std;

enum operations {
    ADD = 1,
    SUB,
    LW,
    SW,
    ADDI,
};
struct R_type {
    unsigned int rs1;
    unsigned int rs2;
    unsigned int rd;
    unsigned int operation;
};

struct I_type {
    unsigned int rs1;
    unsigned int rd;
    unsigned int imm;
    unsigned int operation;
};

struct S_type {
    unsigned int rs1;
    unsigned int rs2;
    unsigned int imm;
    unsigned int operation;
};