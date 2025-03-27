# 5-Stage Pipeline Processor Simulation

This project implements a **5-stage pipelined processor** with two variants:
- **With Forwarding**: Implements register forwarding to reduce stalls.
- **Without Forwarding**: Introduces stalls to handle data hazards.

## 1. Design Decisions
### **Pipeline Stages Implemented**
The processor follows the classic **5-stage pipeline model**:
1. **Instruction Fetch (IF)**: Loads instruction from memory.
2. **Instruction Decode (ID)**: Decodes instruction and fetches register values.
3. **Execution (EX)**: Performs ALU operations.
4. **Memory Access (MEM)**: Reads/writes data memory.
5. **Write Back (WB)**: Writes results back to the register file.

### **Handling Data Hazards**
- **With Forwarding:**
  - Implemented a **forwarding unit** that detects RAW (Read After Write) hazards.
  - Uses data from later pipeline stages (EX/MEM, MEM/WB) instead of stalling.
  - **NOPs** still required on a load-use case.
- **Without Forwarding:**
  - Uses pipeline stalls when a RAW dependency is detected.
  - Inserts **NOPs** (bubbles) to delay execution until data is available.

### **Instruction Set & ALU Operations**
- Supports basic arithmetic (`ADD`, `SUB`, `MUL`), bitwise (`AND`, `OR`), shift (`SLL`), and comparison (`SLT`).
- Also supports branch instructions (`BEQ`, `BGE`, `BLT`, `BNE`) and jump instructions (`JALR`, `JAL`)

### **Branch Resolution**
- Branches are resolved in the Decode (ID) stage.
- If a branch is taken, instruction fetched in the IF stage must be killed to prevent incorrect execution.
- If branch is not taken, the next instruction is not killed and continues as is.
- By default, branch is not taken and the immediate next instruction is fetched while the branch gets resolved

### **Memory and register data handling**
- **Register File:**
  - The processor maintains a **register file** in form of a 32 length vector regs, where regs[i] contains the value of the ith register that is accessed during the **Instruction Decode (ID)** and **Write Back (WB)** stages.
  - Register reads occur in the ID stage, while write operations happen in the WB stage.
  - **Forwarding logic** ensures that data is available even before the WB stage completes.

- **Memory Access:**
  - The memory is accessed in the **Memory Access (MEM)** stage.
  - A map data structure is used to represent the memory. The key in the (key, value) pair is the byte address and value is the 8 bit integer stored at that memory location.
  - Load (`LW`) instructions fetch data from memory and store it in a register during WB.
  - Store (`SW`) instructions write data from a register into memory.

## 2. Known Issues & Limitations
- **Branch Hazard Handling:** Currently, there is no **branch prediction** mechanism, so all branches introduce a **stall penalty**.
- **Load-Use Hazard Delay:** Even with forwarding, **load-use dependencies** still require **one stall cycle**.
- **Limited Instruction Set:** Does not include floating-point instructions or advanced RISC-V instructions.

## 3. Sources Consulted
### **Books & Documentation**
- "Computer Organization and Design" by David A. Patterson & John L. Hennessy
- RISC-V Instruction Set Manual: Referenced for instruction decoding.

### **Online References**
- [**Example RISC-V assembly programs**: Input file generation.](https://marz.utk.edu/my-courses/cosc230/book/example-risc-v-assembly-programs/#strlen)

### **Software used**
- **RIPES**: for generating the machine code from assembly 

## 4. How to Run the Code
### **Compilation & Execution**
```sh
# Compile without forwarding
 g++ -o noforward noforward.cpp
 ./noforward

# Compile with forwarding
 g++ -o forward forwarding.cpp
 ./forwarding
```

### **Expected Output**
The simulation outputs the **pipeline execution steps**, showing how instructions propagate through each stage. The forwarding version should show **fewer stalls** compared to the non-forwarding version.


---

