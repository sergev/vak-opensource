//
// MESM-6 processor
//
// Copyright (c) 2019 Serge Vakulenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
`define RESET_VECTOR            'o00001     // reset vector
`define INTERRUPT_VECTOR        'o00010     // interrupt & exception vectors

// Accumulator source selector
`define SEL_ACC_ALU             0   // from ALU
`define SEL_ACC_MEM             1   // from memory
`define SEL_ACC_REG             2   // M[r]
`define SEL_ACC_RR              3   // R register

// M[r] read index selector
`define SEL_MR_REG              0   // opcode[24:21]
`define SEL_MR_IMM              1   // immediate constant
`define SEL_MR_VA               2   // addr + C
`define SEL_MR_UA               3   // addr + C + M[i]

// M[] write index selector
`define SEL_MW_REG              0   // opcode[24:21]
`define SEL_MW_IMM              1   // immediate constant
`define SEL_MW_VA               2   // addr + C
`define SEL_MW_UA               3   // addr + C + M[i]

// M[] write data selector
`define SEL_MD_PC               0   // PC
`define SEL_MD_A                1   // accumulator
`define SEL_MD_REG              2   // M[r]
`define SEL_MD_REG_PLUS1        3   // M[r] + 1
`define SEL_MD_REG_MINUS1       4   // M[r] - 1
`define SEL_MD_VA               5   // addr + C
`define SEL_MD_UA               6   // addr + C + M[i]

// PC source selector
`define SEL_PC_UA               0   // addr + C + M[i]
`define SEL_PC_VA               1   // addr + C
`define SEL_PC_REG              2   // M[i]
`define SEL_PC_IMM              3   // immediate constant
`define SEL_PC_PLUS1            4   // pc + 1

// R register source selector
`define SEL_RR_UA               0   // from Uaddr
`define SEL_RR_MEM              1   // from memory
`define SEL_RR_REG              2   // M[r]

// ALU operations
`define ALU_NOP                 0   // result = a
`define ALU_AND                 1   // result = a & b
`define ALU_OR                  2   // result = a | b
`define ALU_XOR                 3   // result = a ^ b
`define ALU_SHIFT               4   // result = a << b
`define ALU_ADD_CARRY_AROUND    5   // result = a48 + b48 + carry
`define ALU_PACK                6   // result = a packed by mask b
`define ALU_UNPACK              7   // result = a unpacked by mask b
`define ALU_COUNT               8   // result = count ones in a
`define ALU_CLZ                 9   // result = count leading zeroes in a
`define ALU_FADD                10  // result = a + b (float)
`define ALU_FSUB                11  // result = a - b (float)
`define ALU_FREVSUB             12  // result = b - a (float)
`define ALU_FSUBABS             13  // result = |a| - |b| (float)
`define ALU_FSIGN               14  // result = a with sign(b) (float)
`define ALU_FADDEXP             15  // result = add exponents (float)
`define ALU_FSUBEXP             16  // result = subtract exponents (float)
`define ALU_FMUL                17  // result = a * b (float)
`define ALU_FDIV                18  // result = a / b (float)
`define ALU_YTA                 19  // result = y

`define ALU_OP_WIDTH            6   // alu operation is 6 bits

// Microcode memory settings
`define UPC_BITS                9   // microcode address width
`define UOP_BITS                57  // microcode opcode width

// Dedicated microcode addresses
`define UADDR_RESET             0   // start from zero
`define UADDR_NOP               23  // no operation
`define UADDR_INTERRUPT         24  // defined by mesm6_microcode.sv at runtime

// Micro-instruction fields
`define P_IMM                   0   // microcode address (9 bits) or constant to be used at microcode level
`define P_ALU                   9   // alu operation (6 bits)
`define P_SEL_ACC               15  // accumulator multiplexor (2 bits)
`define P_unused_17             17  // unused
`define P_SEL_MD                18  // M[i] write data multiplexor (3 bits)
`define P_SEL_MW                21  // M[i] write address multiplexor (2 bits)
`define P_W_M                   23  // write M[i] (from alu-out)
`define P_SEL_MR                24  // M read address multiplexor (2 bits)
`define P_M_NONZERO             26  // microcode branch if M[i] is non-zero
`define P_SEL_PC                27  // PC multiplexor (3 bits)
`define P_SEL_ADDR              30  // addr-out multiplexor between Uaddr and M[i]
`define P_R_ADD                 31  // use Mr for Uaddr instead of Vaddr
`define P_SEL_C_MEM             32  // use memory output for C instead of Uaddr
`define P_OP_NOT_CACHED         33  // microcode branch if byte[pc] is not cached at opcode
`define P_FETCH                 34  // request instruction fetch
`define P_W_OPCODE              35  // write opcode
`define P_DECODE                36  // jump to microcode entry point based on current opcode
`define P_MEM_R                 37  // request memory read
`define P_MEM_W                 38  // request memory write
`define P_W_A                   39  // write accumulator (from alu-out)
`define P_W_C                   40  // write C register
`define P_A_NONZERO             41  // microcode branch if acc is zero
`define P_unused_42             42  // unused
`define P_A_ZERO                43  // microcode branch if acc is zero
`define P_BRANCH                44  // microcode inconditional branch to address
`define P_C_ACTIVE              45  // clear C register flag
`define P_ENTER_INT             46  // set interrupt flag (enter interrupt)
`define P_EXIT_INT              47  // clear interrupt flag (exit from interrupt)
`define P_W_PC                  48  // write pc (from alu-out)
`define P_M_ZERO                49  // microcode branch if M[i] is zero
`define P_SEL_ALU_MEM           50  // use memory output for ALU input B instead of Uaddr
`define P_SEL_RR                51  // R register multiplexor (2 bits)
`define P_W_RR                  53  // write R register
`define P_G_LOG                 54  // set logical group
`define P_G_MUL                 55  // set multiplicative group
`define P_G_ADD                 56  // set additive group
