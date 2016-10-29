`timescale 1ns / 1ps
//
// PDP-11 processor control unit.
//
// Map for opcodes = xBCxxx (octal)
//    C-->
//         0          1       2        3        4        5         6          7
// B  -----------------------------------------------------------------------------
// |  |halt,wait, |  bne, |  bvc, |bhis,bcc,| jsr,  | clr,com,| ror,rol, |        |
// v  |reset,nop, |  beq, |  bvs, |blo,bcs, | trap, | inc,dec,| asr,asl, |        |
//   0|br,bpl,bmi,|  bhi, |  bge, |bgt,ble  | emt   | neg,adc,| sxt,mark,|        |
//    |jmp,rts,rti|  blos |  blt  |         |       | sbc,tst |mtps,mfps,|        |
//    |bpt,iot,rtt|       |       |         |       |         |mfpi,mtpi |        |
//    |---------------------------------------------------------------------------|
//   1|                                    mov                                    |
//    |---------------------------------------------------------------------------|
//   2|                                    cmp                                    |
//    |---------------------------------------------------------------------------|
//   3|                                    bit                                    |
//    |---------------------------------------------------------------------------|
//   4|                                    bic                                    |
//    |---------------------------------------------------------------------------|
//   5|                                    bis                                    |
//    |---------------------------------------------------------------------------|
//   6|                                 add, sub                                  |
//    |---------------------------------------------------------------------------|
//   7|   mul     |  div  |  ash  |   ashc  |  xor  |   FIS   |   CIS    |  sob   |
//    -----------------------------------------------------------------------------
//
`include "opcode.v"

//
// Select the source of ALU input
//
`define ALU_SRC_DST	2'b00	// src, dst
`define ALU_X_DST	2'b01	// x, dst
`define ALU_SRC_Y	2'b10	// src, y
`define ALU_X_Y		2'b11	// x, y

//
// Select the source of memory address
//
`define MEM_SRC		3'd0	// src
`define MEM_SRC_X	3'd1	// src + X
`define MEM_DST		3'd2	// dst
`define MEM_DST_Y	3'd3	// dst + Y
`define MEM_X		3'd4	// X
`define MEM_Y		3'd5	// Y
`define MEM_Z		3'd6	// Z

module control (
    input wire [15:0] cmd,	// instruction to execute
    input wire [2:0] cycle,	// cycle counter
    output reg [2:0] cnext,	// next cycle number
    output reg [2:0] reg_src,	// source register number
    output reg [2:0] reg_dst,	// destination register number
    output reg [1:0] alu_input,	// alu source from src or x, dst or y
    output reg [9:0] alu_op,	// alu operation code
    output reg [2:0] mem_addr,	// memory address src/dst/src+x/dst+y/x/y/z
    output reg mem_byte,	// memory byte mode
    output reg reg_from_mem,	// write to regfile from mem out
    output reg reg_we,		// register file write enable
    output reg x_we,		// x register write
    output reg y_we,		// y register write
    output reg z_we,		// z latch write
    output reg psw_we,		// psw register write
    output reg mem_we,		// memory write
    output reg ir_we		// instruction register write
);

`define DEFAULT_CONTROL		\
    reg_dst = 7;		\
    reg_src = 7;		\
    alu_input = 0;		\
    alu_op = `CLR;		\
    psw_we = 0;                 \
    reg_we = 0;                 \
    reg_from_mem = 0;           \
    mem_we = 0;                 \
    mem_byte = 0;		\
    mem_addr = 0;		\
    z_we = 0;                   \
    x_we = 0;                   \
    y_we = 0;                   \
    ir_we = 0

    always @(cmd or cycle) begin
        //
        // Cycle zero: fetch an instruction
        //
        casez ({ cycle, cmd })
        { 3'd0, 16'ozzzzzz }: begin `DEFAULT_CONTROL;
                cnext = 1;		// Next cycle 1
                reg_dst = 7;		// Use R7 (PC)
                alu_input = `ALU_SRC_DST; // ALU destination from PC
                alu_op = `INC2;		// Compute PC+2
                reg_we = 1;		// Write to PC...
                reg_from_mem = 0;	// ...from ALU
                mem_addr = `MEM_DST;	// Get M[PC]...
                ir_we = 1;		// ...latch instruction
                x_we = 1;		// ...write X
        end
        //
        // Unknown instruction.
        //
        default: begin `DEFAULT_CONTROL;
                cnext = 0;		// Next cycle 0
                $display ("%4d-%d) unknown opcode %o", $time, cycle, cmd);
                $finish;
        end

//-----------------------------
// Single-operand instructions.
// XOR instruction.
//
        //
        // Single operand, register mode: R
        // 1 cycle.
        //
        { 3'd1, 16'oz05z0z },		// clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z0z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0030z },           // swab
        { 3'd1, 16'o074z0z }:           // xor
        begin `DEFAULT_CONTROL;
                // Use data from Rd and execute an operation.
                // Store a result to Rd.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST; // ALU destination from dst
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                reg_we = 1;		// Write to dst...
                mem_byte = cmd[15];	// ...byte/word...
                reg_from_mem = 0;	// ...from ALU
        end
        //
        // Single operand, register-deferred mode: M [ R ]
        // 2 cycles.
        //
        { 3'd1, 16'oz05z1z },           // clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z1z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0031z },           // swab
        { 3'd1, 16'o074z1z }:           // xor
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST;	// Get M[dst]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ..write Y
                z_we = 1;		// Remember address in Z
        end
        { 3'd2, 16'oz05z1z },		// Cycle #2
        { 3'd2, 16'oz06z1z },
        { 3'd2, 16'oz0031z },
        { 3'd2, 16'o074z1z }: begin `DEFAULT_CONTROL;
                // Use data from Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end
        //
        // Single operand, autoincrement mode: M [ R++ ]
        // 2 cycles.
        //
        { 3'd1, 16'oz05z2z },           // clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z2z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0032z },           // swab
        { 3'd1, 16'o074z2z }:           // xor
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                // Increment the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = cmd[15] && (cmd[2:0] <= 5) ? `INC : `INC2;
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[dst]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        { 3'd2, 16'oz05z2z },		// Cycle #2
        { 3'd2, 16'oz06z2z },
        { 3'd2, 16'oz0032z },
        { 3'd2, 16'o074z2z }: begin `DEFAULT_CONTROL;
                // Use data from Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end
        //
        // Single operand, autoincrement-deferred mode: M [ M [ R++ ] ]
        // 3 cycles.
        //
        { 3'd1, 16'oz05z3z },		// clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z3z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0033z },           // swab
        { 3'd1, 16'o074z3z }:           // xor
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in Y.
                // Increment the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;		// Increment Rdst by 2
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[dst]
                y_we = 1;		// ...write Y
        end
        { 3'd2, 16'oz05z3z },		// Cycle #2
        { 3'd2, 16'oz06z3z },
        { 3'd2, 16'oz0033z },
        { 3'd2, 16'o074z3z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 3;		// Next cycle 3
                mem_addr = `MEM_Y;	// Get M[Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        { 3'd3, 16'oz05z3z },		// Cycle #3
        { 3'd3, 16'oz06z3z },
        { 3'd3, 16'oz0033z },
        { 3'd3, 16'o074z3z }: begin `DEFAULT_CONTROL;
                // Use data from Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end
        //
        // Single operand, autodecrement mode: M [ --R ]
        // 3 cycles.
        //
        { 3'd1, 16'oz05z4z },		// clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z4z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0034z },           // swab
        { 3'd1, 16'o074z4z }:           // xor
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = cmd[15] && (cmd[2:0] <= 5) ? `DEC : `DEC2;
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'oz05z4z },		// Cycle #2
        { 3'd2, 16'oz06z4z },
        { 3'd2, 16'oz0034z },
        { 3'd2, 16'o074z4z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 3;		// Next cycle 3
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST;	// Get M[dst]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        { 3'd3, 16'oz05z4z },		// Cycle #3
        { 3'd3, 16'oz06z4z },
        { 3'd3, 16'oz0034z },
        { 3'd3, 16'o074z4z }: begin `DEFAULT_CONTROL;
                // Use data from Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end
        //
        // Single operand, autodecrement-deferred mode: M [ M [ --R ] ]
        // 4 cycles.
        //
        { 3'd1, 16'oz05z5z },           // clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z5z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0035z },           // swab
        { 3'd1, 16'o074z5z }:           // xor
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = `DEC2;		// Decrement Rdst by 2
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'oz05z5z },		// Cycle #2
        { 3'd2, 16'oz06z5z },
        { 3'd2, 16'oz0035z },
        { 3'd2, 16'o074z5z }: begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in Y.
                cnext = 3;		// Next cycle 3
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST;	// Get M[dst]...
                y_we = 1;		// ...write Y
        end
        { 3'd3, 16'oz05z5z },		// Cycle #3
        { 3'd3, 16'oz06z5z },
        { 3'd3, 16'oz0035z },
        { 3'd3, 16'o074z5z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 4;		// Next cycle 4
                mem_addr = `MEM_Y;	// Get M[Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        { 3'd4, 16'oz05z5z },		// Cycle #4
        { 3'd4, 16'oz06z5z },
        { 3'd4, 16'oz0035z },
        { 3'd4, 16'o074z5z }: begin `DEFAULT_CONTROL;
                // Use data from Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end
        //
        // Single operand, index mode: M [ R + M [ PC++ ] ]
        // 3 cycles.
        //
        { 3'd1, 16'oz05z6z },		// clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z6z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0036z },           // swab
        { 3'd1, 16'o074z6z }:           // xor
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in Y.
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                y_we = 1;		// ...write Y
        end
        { 3'd2, 16'oz05z6z },		// Cycle #2
        { 3'd2, 16'oz06z6z },
        { 3'd2, 16'oz0036z },
        { 3'd2, 16'o074z6z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 3;		// Next cycle 3
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST_Y;	// Get M[dst+Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        { 3'd3, 16'oz05z6z },		// Cycle #3
        { 3'd3, 16'oz06z6z },
        { 3'd3, 16'oz0036z },
        { 3'd3, 16'o074z6z }: begin `DEFAULT_CONTROL;
                // Use data from Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end
        //
        // Single operand, index-deferred mode:
        //	M [ M [ R + M [ PC++ ] ] ]
        // 4 cycles.
        //
        { 3'd1, 16'oz05z7z },		// clr, com, inc, dec, neg, adc, sbc, tst
        { 3'd1, 16'oz06z7z },           // ror, rol, asr, asl, sxt
        { 3'd1, 16'oz0037z },           // swab
        { 3'd1, 16'o074z7z }:           // xor
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in Y.
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                y_we = 1;		// ...write Y
        end
        { 3'd2, 16'oz05z7z },		// Cycle #2
        { 3'd2, 16'oz06z7z },
        { 3'd2, 16'oz0037z },
        { 3'd2, 16'o074z7z }: begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in Y.
                cnext = 3;		// Next cycle 3
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST_Y;	// Get M[dst+Y]...
                y_we = 1;		// ...write Y
        end
        { 3'd3, 16'oz05z7z },		// Cycle #3
        { 3'd3, 16'oz06z7z },
        { 3'd3, 16'oz0037z },
        { 3'd3, 16'o074z7z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 4;		// Next cycle 4
                mem_addr = `MEM_Y;	// Get M[Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        { 3'd4, 16'oz05z7z },		// Cycle #4
        { 3'd4, 16'oz06z7z },
        { 3'd4, 16'oz0037z },
        { 3'd4, 16'o074zzz }: begin `DEFAULT_CONTROL;
                // Use data from Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end
//-----------------------------
// One-and-a-half-operand instructions: ASH.
// TODO: MUL, DIV, ASHC
//
        //
        // 1.5-operand, register mode: R
        // 1 cycle.
        //
        { 3'd1, 16'o072z0z }:		// ash
        begin `DEFAULT_CONTROL;
                // Use data from Rd and execute an operation.
                // Store a result to Rd.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[2:0];	// Use Rsrc
                reg_dst = cmd[8:6];	// Use Rdst
                alu_input = `ALU_SRC_DST; // ALU destination from dst
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                reg_we = 1;		// Write to dst from ALU
        end
        //
        // 1.5-operand, register-deferred src: M [ R ]
        // 2 cycles.
        //
        { 3'd1, 16'o072z1z }:		// ash
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 5;		// Next cycle 7
                reg_src = cmd[2:0];	// Use Rsrc
                mem_addr = `MEM_SRC;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        { 3'd5, 16'o072zzz }:		// Cycle #7 - common
        begin `DEFAULT_CONTROL;
                // Use data from X and execute an operation.
                // Store a result to Rd.
                cnext = 0;		// Next cycle 0
                reg_dst = cmd[8:6];	// Use Rdst
                alu_input = `ALU_X_DST; // ALU destination from dst
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                reg_we = 1;		// Write to dst from ALU
        end
        //
        // 1.5-operand, autoincrement src: M [ R++ ]
        // 2 cycles.
        //
        { 3'd1, 16'o072z2z }:		// ash
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                // Increment the register value.
                cnext = 5;		// Next cycle 5
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        //
        // 1.5-operand, autoincrement-deferred src: M [ M [ R++ ] ]
        // 3 cycles.
        //
        { 3'd1, 16'o072z3z }:		// ash
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X.
                // Increment the register value.
                cnext = 6;		// Next cycle 6
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;		// Increment Rsrc by 2
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        { 3'd6, 16'o072zzz }:		// Cycle #6 - common
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 5;		// Next cycle 5
                mem_addr = `MEM_X;	// Get M[X]...
                x_we = 1;		// ...write X
        end
        //
        // 1.5-operand, autodecrement src: M [ --R ]
        // 3 cycles.
        //
        { 3'd1, 16'o072z4z }:		// ash
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `DEC2;
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'o072z4z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 5;		// Next cycle 5
                reg_src = cmd[2:0];	// Use Rsrc
                mem_addr = `MEM_SRC;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        //
        // 1.5-operand, autodecrement-deferred src: M [ M [ --R ] ]
        // 4 cycles.
        //
        { 3'd1, 16'o072z5z }:		// ash
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `DEC2;		// Decrement Rsrc 2
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'o072z5z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X
                cnext = 6;		// Next cycle 6
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        //
        // 1.5-operand, index src: M [ R + M [ PC++ ] ]
        // 3 cycles.
        //
        { 3'd1, 16'o072z6z }:		// ash
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in X.
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                x_we = 1;		// ...write X
        end
        { 3'd2, 16'o072z6z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 5;		// Next cycle 5
                reg_src = cmd[2:0];	// Use Rsrc
                mem_addr = `MEM_SRC_X;	// Get M[src+X]...
                x_we = 1;		// ...write X
        end
        //
        // 1.5-operand, index-deferred src:
        //	M [ M [ R + M [ PC++ ] ] ]
        // 4 cycles.
        //
        { 3'd1, 16'o072z7z }:		// ash
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in X
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                x_we = 1;		// ...write X
        end
        { 3'd2, 16'o072z7z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X.
                cnext = 6;		// Next cycle 6
                reg_src = cmd[2:0];	// Use Rsrc
                mem_addr = `MEM_SRC_X;	// Get M[src+X]...
                x_we = 1;		// ...write X
        end
//-----------------------------
// Double-operand instructions.
// Cycles 1..3 - fetch source operand to X register.
// Cycles 4..7 - execute the rest of operation.
//
        //
        // Double operand, register mode: R, R
        // 1 cycle.
        //
        { 3'd1, 16'oz10z0z },		// mov
        { 3'd1, 16'oz20z0z },           // cmp
        { 3'd1, 16'oz30z0z },           // bit
        { 3'd1, 16'oz40z0z },           // bic
        { 3'd1, 16'oz50z0z },           // bis
        { 3'd1, 16'oz60z0z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Use data from Rsrc and execute an operation.
                // Store a result to Rdst.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[8:6];	// Use Rsrc...
                reg_dst = cmd[2:0];	// ...and Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                reg_we = 1;		// Write to dst...
                reg_from_mem = 0;	// ...from ALU
        end
//-----------------------------
// Source operand.
        //
        // Double operand, register-deferred src: M [ R ]
        // 1 + Ndst cycles.
        //
        { 3'd1, 16'oz11zzz },		// mov
        { 3'd1, 16'oz21zzz },           // cmp
        { 3'd1, 16'oz31zzz },           // bit
        { 3'd1, 16'oz41zzz },           // bic
        { 3'd1, 16'oz51zzz },           // bis
        { 3'd1, 16'oz61zzz }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 4;		// Next cycle 4
                reg_src = cmd[8:6];	// Use Rsrc
                mem_addr = `MEM_SRC;	// Get M[src]...
                mem_byte = cmd[15];	// ...byte/word...
                x_we = 1;		// ...write X
        end
        //
        // Double operand, autoincrement src: M [ R++ ]
        // 1 + Ndst cycles.
        //
        { 3'd1, 16'oz12zzz },		// mov
        { 3'd1, 16'oz22zzz },           // cmp
        { 3'd1, 16'oz32zzz },           // bit
        { 3'd1, 16'oz42zzz },           // bic
        { 3'd1, 16'oz52zzz },           // bis
        { 3'd1, 16'oz62zzz }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                // Increment the register value.
                cnext = 4;		// Next cycle 4
                reg_dst = cmd[8:6];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = cmd[15] && (cmd[8:6] <= 5) ? `INC : `INC2;
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[src]...
                mem_byte = cmd[15];	// ...byte/word...
                x_we = 1;		// ...write X
        end
        //
        // Double operand, autoincrement-deferred src: M [ M [ R++ ] ]
        // 2 + Ndst cycles.
        //
        { 3'd1, 16'oz13zzz },		// mov
        { 3'd1, 16'oz23zzz },           // cmp
        { 3'd1, 16'oz33zzz },           // bit
        { 3'd1, 16'oz43zzz },           // bic
        { 3'd1, 16'oz53zzz },           // bis
        { 3'd1, 16'oz63zzz }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X.
                // Increment the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[8:6];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;		// Increment Rsrc by 2
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        { 3'd2, 16'oz13zzz },		// Cycle #2
        { 3'd2, 16'oz23zzz },
        { 3'd2, 16'oz33zzz },
        { 3'd2, 16'oz43zzz },
        { 3'd2, 16'oz53zzz },
        { 3'd2, 16'oz63zzz }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 4;		// Next cycle 4
                mem_addr = `MEM_X;	// Get M[X]...
                mem_byte = cmd[15];	// ...byte/word...
                x_we = 1;		// ...write X
        end
        //
        // Double operand, autodecrement src: M [ --R ]
        // 2 + Ndst cycles.
        //
        { 3'd1, 16'oz14zzz },		// mov
        { 3'd1, 16'oz24zzz },           // cmp
        { 3'd1, 16'oz34zzz },           // bit
        { 3'd1, 16'oz44zzz },           // bic
        { 3'd1, 16'oz54zzz },           // bis
        { 3'd1, 16'oz64zzz }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[8:6];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = cmd[15] && (cmd[2:0] <= 5) ? `DEC : `DEC2;
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'oz14zzz },		// Cycle #2
        { 3'd2, 16'oz24zzz },
        { 3'd2, 16'oz34zzz },
        { 3'd2, 16'oz44zzz },
        { 3'd2, 16'oz54zzz },
        { 3'd2, 16'oz64zzz }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 4;		// Next cycle 4
                reg_src = cmd[8:6];	// Use Rsrc
                mem_addr = `MEM_SRC;	// Get M[src]...
                mem_byte = cmd[15];	// ...byte/word...
                x_we = 1;		// ...write X
        end
        //
        // Double operand, autodecrement-deferred src: M [ M [ --R ] ]
        // 3 + Ndst cycles.
        //
        { 3'd1, 16'oz15zzz },		// mov
        { 3'd1, 16'oz25zzz },           // cmp
        { 3'd1, 16'oz35zzz },           // bit
        { 3'd1, 16'oz45zzz },           // bic
        { 3'd1, 16'oz55zzz },           // bis
        { 3'd1, 16'oz65zzz }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[8:6];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `DEC2;		// Decrement Rsrc 2
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'oz15zzz },		// Cycle #2
        { 3'd2, 16'oz25zzz },
        { 3'd2, 16'oz35zzz },
        { 3'd2, 16'oz45zzz },
        { 3'd2, 16'oz55zzz },
        { 3'd2, 16'oz65zzz }: begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X
                cnext = 3;		// Next cycle 3
                reg_dst = cmd[8:6];	// Put Rsrc on dst bus
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        { 3'd3, 16'oz15zzz },		// Cycle #3
        { 3'd3, 16'oz25zzz },
        { 3'd3, 16'oz35zzz },
        { 3'd3, 16'oz45zzz },
        { 3'd3, 16'oz55zzz },
        { 3'd3, 16'oz65zzz }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 4;		// Next cycle 4
                mem_addr = `MEM_X;	// Get M[X]...
                mem_byte = cmd[15];	// ...byte/word...
                x_we = 1;		// ...write X
        end
        //
        // Double operand, index src: M [ R + M [ PC++ ] ]
        // 2 + Ndst cycles.
        //
        { 3'd1, 16'oz16zzz },		// mov
        { 3'd1, 16'oz26zzz },           // cmp
        { 3'd1, 16'oz36zzz },           // bit
        { 3'd1, 16'oz46zzz },           // bic
        { 3'd1, 16'oz56zzz },           // bis
        { 3'd1, 16'oz66zzz }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in X.
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                x_we = 1;		// ...write X
        end
        { 3'd2, 16'oz16zzz },		// Cycle #2
        { 3'd2, 16'oz26zzz },
        { 3'd2, 16'oz36zzz },
        { 3'd2, 16'oz46zzz },
        { 3'd2, 16'oz56zzz },
        { 3'd2, 16'oz66zzz }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 4;		// Next cycle 4
                reg_src = cmd[8:6];	// Use Rsrc
                mem_addr = `MEM_SRC_X;	// Get M[src+X]...
                mem_byte = cmd[15];	// ...byte/word...
                x_we = 1;		// ...write X
        end
        //
        // Double operand, index-deferred src:
        //	M [ M [ R + M [ PC++ ] ] ]
        // 3 + Ndst cycles.
        //
        { 3'd1, 16'oz17zzz },		// mov
        { 3'd1, 16'oz27zzz },           // cmp
        { 3'd1, 16'oz37zzz },           // bit
        { 3'd1, 16'oz47zzz },           // bic
        { 3'd1, 16'oz57zzz },           // bis
        { 3'd1, 16'oz67zzz }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in X
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                x_we = 1;		// ...write X
        end
        { 3'd2, 16'oz17zzz },		// Cycle #2
        { 3'd2, 16'oz27zzz },
        { 3'd2, 16'oz37zzz },
        { 3'd2, 16'oz47zzz },
        { 3'd2, 16'oz57zzz },
        { 3'd2, 16'oz67zzz }: begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X.
                cnext = 3;		// Next cycle 3
                reg_src = cmd[8:6];	// Use Rsrc
                mem_addr = `MEM_SRC_X;	// Get M[src+X]...
                x_we = 1;		// ...write X
        end
        { 3'd3, 16'oz17zzz },		// Cycle #3
        { 3'd3, 16'oz27zzz },
        { 3'd3, 16'oz37zzz },
        { 3'd3, 16'oz47zzz },
        { 3'd3, 16'oz57zzz },
        { 3'd3, 16'oz67zzz }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                cnext = 4;		// Next cycle 4
                mem_addr = `MEM_X;	// Get M[X]...
                mem_byte = cmd[15];	// ...byte/word...
                x_we = 1;		// ...write X
        end
//-----------------------------
// Destination operand.
        //
        // Double operand, register dst: R
        // Nsrc + 1 cycle.
        //
        { 3'd4, 16'oz1zz0z },		// mov
        { 3'd4, 16'oz2zz0z },           // cmp
        { 3'd4, 16'oz3zz0z },           // bit
        { 3'd4, 16'oz4zz0z },           // bic
        { 3'd4, 16'oz5zz0z },           // bis
        { 3'd4, 16'oz6zz0z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Use data from X and Rdst and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_X_DST;	// ALU from X and Rdst
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                reg_we = 1;		// Write to dst...
                reg_from_mem = 0;	// ...from ALU
        end
        //
        // Double operand, register-deferred dst: M [ R ]
        // Nsrc + 2 cycles.
        //                              // Cycle #4
        { 3'd4, 16'oz1zz1z },		// mov
        { 3'd4, 16'oz2zz1z },           // cmp
        { 3'd4, 16'oz3zz1z },           // bit
        { 3'd4, 16'oz4zz1z },           // bic
        { 3'd4, 16'oz5zz1z },           // bis
        { 3'd4, 16'oz6zz1z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 7;		// Next cycle 7
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST;	// Get M[dst]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        //
        // Double operand, autoincrement dst: M [ R++ ]
        // Nsrc + 2 cycles.
        //                              // Cycle #4
        { 3'd4, 16'oz1zz2z },		// mov
        { 3'd4, 16'oz2zz2z },           // cmp
        { 3'd4, 16'oz3zz2z },           // bit
        { 3'd4, 16'oz4zz2z },           // bic
        { 3'd4, 16'oz5zz2z },           // bis
        { 3'd4, 16'oz6zz2z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                // Increment the register value.
                cnext = 7;		// Next cycle 7
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = cmd[15] && (cmd[2:0] <= 5) ? `INC : `INC2;
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[dst]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        //
        // Double operand, autoincrement-deferred dst: M [ M [ R++ ] ]
        // Nsrc + 3 cycles.
        //                              // Cycle #4
        { 3'd4, 16'oz1zz3z },		// mov
        { 3'd4, 16'oz2zz3z },           // cmp
        { 3'd4, 16'oz3zz3z },           // bit
        { 3'd4, 16'oz4zz3z },           // bic
        { 3'd4, 16'oz5zz3z },           // bis
        { 3'd4, 16'oz6zz3z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in Y.
                // Increment the register value.
                cnext = 5;		// Next cycle 5
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;		// Increment Rdst by 2
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[dst]...
                y_we = 1;		// ...write Y
        end
        { 3'd5, 16'oz1zz3z },		// Cycle #5
        { 3'd5, 16'oz2zz3z },
        { 3'd5, 16'oz3zz3z },
        { 3'd5, 16'oz4zz3z },
        { 3'd5, 16'oz5zz3z },
        { 3'd5, 16'oz6zz3z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 7;		// Next cycle 7
                mem_addr = `MEM_Y;	// Get M[Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        //
        // Double operand, autodecrement dst: M [ --R ]
        // Nsrc + 3 cycles.
        //                              // Cycle #4
        { 3'd4, 16'oz1zz4z },		// mov
        { 3'd4, 16'oz2zz4z },           // cmp
        { 3'd4, 16'oz3zz4z },           // bit
        { 3'd4, 16'oz4zz4z },           // bic
        { 3'd4, 16'oz5zz4z },           // bis
        { 3'd4, 16'oz6zz4z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 5;		// Next cycle 5
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = cmd[15] && (cmd[2:0] <= 5) ? `DEC : `DEC2;
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd5, 16'oz1zz4z },		// Cycle #5
        { 3'd5, 16'oz2zz4z },
        { 3'd5, 16'oz3zz4z },
        { 3'd5, 16'oz4zz4z },
        { 3'd5, 16'oz5zz4z },
        { 3'd5, 16'oz6zz4z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 7;		// Next cycle 7
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST;	// Get M[dst]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        //
        // Double operand, autodecrement-deferred dst: M [ M [ --R ] ]
        // Nsrc + 4 cycles.
        //                              // Cycle #4
        { 3'd4, 16'oz1zz5z },		// mov
        { 3'd4, 16'oz2zz5z },           // cmp
        { 3'd4, 16'oz3zz5z },           // bit
        { 3'd4, 16'oz4zz5z },           // bic
        { 3'd4, 16'oz5zz5z },           // bis
        { 3'd4, 16'oz6zz5z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 5;		// Next cycle 5
                reg_dst = cmd[2:0];	// Use Rdst
                alu_input = `ALU_SRC_DST;
                alu_op = `DEC2;		// Decrement Rdst by 2
                reg_we = 1;		// Write Rdst back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd5, 16'oz1zz5z },		// Cycle #5
        { 3'd5, 16'oz2zz5z },
        { 3'd5, 16'oz3zz5z },
        { 3'd5, 16'oz4zz5z },
        { 3'd5, 16'oz5zz5z },
        { 3'd5, 16'oz6zz5z }: begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in Y.
                cnext = 6;		// Next cycle 6
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST;	// Get M[dst]...
                y_we = 1;		// ...write Y
        end
        { 3'd6, 16'oz1zz5z },		// Cycle #6
        { 3'd6, 16'oz2zz5z },
        { 3'd6, 16'oz3zz5z },
        { 3'd6, 16'oz4zz5z },
        { 3'd6, 16'oz5zz5z },
        { 3'd6, 16'oz6zz5z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 7;		// Next cycle 7
                mem_addr = `MEM_Y;	// Get M[Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        //
        // Double operand, index dst: M [ R + M [ PC++ ] ]
        // Nsrc + 3 cycles.
        //                              // Cycle #4
        { 3'd4, 16'oz1zz6z },		// mov
        { 3'd4, 16'oz2zz6z },           // cmp
        { 3'd4, 16'oz3zz6z },           // bit
        { 3'd4, 16'oz4zz6z },           // bic
        { 3'd4, 16'oz5zz6z },           // bis
        { 3'd4, 16'oz6zz6z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in Y.
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                y_we = 1;		// ...write Y
        end
        { 3'd5, 16'oz1zz6z },		// Cycle #5
        { 3'd5, 16'oz2zz6z },
        { 3'd5, 16'oz3zz6z },
        { 3'd5, 16'oz4zz6z },
        { 3'd5, 16'oz5zz6z },
        { 3'd5, 16'oz6zz6z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 7;		// Next cycle 7
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST_Y;	// Get M[dst+Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        //
        // Double operand, index-deferred dst:
        //	M [ M [ R + M [ PC++ ] ] ]
        // Nsrc + 4 cycles.
        //                              // Cycle #4
        { 3'd4, 16'oz1zz7z },		// mov
        { 3'd4, 16'oz2zz7z },           // cmp
        { 3'd4, 16'oz3zz7z },           // bit
        { 3'd4, 16'oz4zz7z },           // bic
        { 3'd4, 16'oz5zz7z },           // bis
        { 3'd4, 16'oz6zz7z }:           // add, sub
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in Y.
                // Increment the PC register value.
                cnext = 5;		// Next cycle 5
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write PC back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[PC]...
                y_we = 1;		// ...write Y
        end
        { 3'd5, 16'oz1zz7z },		// Cycle #5
        { 3'd5, 16'oz2zz7z },
        { 3'd5, 16'oz3zz7z },
        { 3'd5, 16'oz4zz7z },
        { 3'd5, 16'oz5zz7z },
        { 3'd5, 16'oz6zz7z }: begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in Y.
                cnext = 6;		// Next cycle 6
                reg_dst = cmd[2:0];	// Use Rdst
                mem_addr = `MEM_DST_Y;	// Get M[dst+Y]...
                y_we = 1;		// ...write Y
        end
        { 3'd6, 16'oz1zz7z },		// Cycle #6
        { 3'd6, 16'oz2zz7z },
        { 3'd6, 16'oz3zz7z },
        { 3'd6, 16'oz4zz7z },
        { 3'd6, 16'oz5zz7z },
        { 3'd6, 16'oz6zz7z }: begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in Y.
                cnext = 7;		// Next cycle 7
                mem_addr = `MEM_Y;	// Get M[Y]...
                mem_byte = cmd[15];	// ...byte/word...
                y_we = 1;		// ...write Y
                z_we = 1;		// Remember address in Z
        end
        //
        // Cycle #7 - common for dst modes 1..7.
        //
        { 3'd7, 16'ozzzzzz }: begin `DEFAULT_CONTROL;
                // Use data from X and Y and execute an operation.
                // Store a result to memory.
                cnext = 0;		// Next cycle 0
                alu_input = `ALU_X_Y;	// ALU from X and Y
                alu_op = cmd[15:6];	// Compute result
                psw_we = 1;		// New PSW
                mem_we = 1;		// Memory write...
                mem_byte = cmd[15];	// ...byte/word...
                mem_addr = `MEM_Z;	// ...to M[Z]
        end

//-----------------------------
// Program control.
//
        { 3'd1, 16'o000000 }:           // halt
        begin `DEFAULT_CONTROL;
                cnext = 0;		// Next cycle 0
                $finish;
        end
        { 3'd1, 16'o000240 }:           // nop
        begin `DEFAULT_CONTROL;
                cnext = 0;		// Next cycle 0
        end
        { 3'd1, 16'b00000001zzzzzzzz }, // br
	{ 3'd1, 16'b0000001zzzzzzzzz }, // bne, beq
	{ 3'd1, 16'b000001zzzzzzzzzz }, // bge, blt, bgt, ble
	{ 3'd1, 16'b10000zzzzzzzzzzz }: // bpl, bmi, bhi, blos, bvc, bvs, bhis, blo
        begin `DEFAULT_CONTROL;
                // Opcode is latched in X register.
                // Add it to PC as a word offset.
                cnext = 0;		// Next cycle 0
                reg_dst = 7;		// Use R7 (PC)
                alu_input = `ALU_X_DST; // ALU from X and PC
                alu_op = {cmd[15:8], 2'b0}; // Compute PC + (X << 1)
                reg_we = 1;		// Write to PC...
                reg_from_mem = 0;	// ...from ALU
        end
//-----------------------------
// JMP instruction.
//jmp loop1           // 00167 177770
//jmp $loop1          // 00127 000530
//jmp *$loop1         // 000137 000530
//jmp *loop1          // 000177 177754
//
        //
        // Register mode: JMP R
        // 1 cycle.
        //
        { 3'd1, 16'o00010z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Use data from Rs and store it to PC.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[2:0];	// Use Rsrc
                reg_dst = 7;            // Use Rdst
                alu_input = `ALU_SRC_DST; // ALU destination from dst
                alu_op = cmd[15:6];	// Compute result
                reg_we = 1;		// Write to dst from ALU
        end
        //
        // Register-deferred src: JMP (R)
        // 1 cycle.
        //
        { 3'd1, 16'o00011z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it to PC.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[2:0];	// Use Rsrc
                reg_dst = 7;		// Use R7 (PC)
                mem_addr = `MEM_SRC;	// Get M[src]...
                reg_we = 1;		// Write to PC...
                reg_from_mem = 1;	// ...from mem.
        end
        //
        // Autoincrement src: JMP (R)+
        // 2 cycles.
        //
        { 3'd1, 16'o00012z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory and store it in X.
                // Increment the register value.
                cnext = 5;		// Next cycle 5
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        { 3'd5, 16'o0001zz }:		// Cycle #5 - common
        begin `DEFAULT_CONTROL;
                // Use data from X and store it to PC.
                cnext = 0;		// Next cycle 0
                reg_dst = 7;            // Use R7 (PC)
                alu_input = `ALU_X_DST; // ALU destination from dst
                alu_op = cmd[15:6];	// Compute result
                reg_we = 1;		// Write to dst from ALU
        end
        //
        // Autoincrement-deferred src: JMP *(R)+
        // 2 cycles.
        //
        { 3'd1, 16'o00013z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X.
                // Increment the register value.
                cnext = 6;		// Next cycle 6
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;		// Increment Rsrc by 2
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        { 3'd6, 16'o0001zz }:		// Cycle #6 - common
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory[X] and store it to PC.
                cnext = 0;		// Next cycle 0
                reg_dst = 7;            // Use R7 (PC)
                mem_addr = `MEM_X;	// Get M[X]...
                reg_we = 1;		// Write to dst...
                reg_from_mem = 1;	// ...from mem
        end
        //
        // Autodecrement src: JMP -(R)
        // 2 cycles.
        //
        { 3'd1, 16'o00014z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `DEC2;
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'o00014z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Fetch an operand from memory[Rs] and store it to PC.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[2:0];	// Use Rsrc
                reg_dst = 7;            // Use R7 (PC)
                mem_addr = `MEM_SRC;	// Get M[src]...
                reg_we = 1;		// Write to dst from ALU
        end
        //
        // Autodecrement-deferred src: JMP *-(R)
        // 3 cycles.
        //
        { 3'd1, 16'o00015z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Decrement the register value.
                cnext = 2;		// Next cycle 2
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                alu_input = `ALU_SRC_DST;
                alu_op = `DEC2;		// Decrement Rsrc 2
                reg_we = 1;		// Write Rsrc back...
                reg_from_mem = 0;	// ...from alu
        end
        { 3'd2, 16'o00015z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory and store it in X
                cnext = 6;		// Next cycle 6
                reg_dst = cmd[2:0];	// Put Rsrc on dst bus
                mem_addr = `MEM_DST;	// Get M[src]...
                x_we = 1;		// ...write X
        end
        //
        // Index src: JMP (R + M [ PC++ ])
        // 2 cycles.
        //
        { 3'd1, 16'o00016z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in X.
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write to PC from ALU
                mem_addr = `MEM_DST;	// Get M[PC]...
                y_we = 1;		// ...write Y
        end
        { 3'd2, 16'o00016z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Add Rs and Y, and store result to PC.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[2:0];	// Use Rsrc
                reg_dst = 7;            // Use R7 (PC)
                alu_input = `ALU_SRC_Y;	// ALU destination from Y
                alu_op = `JMP2;         // Compute result
                reg_we = 1;		// Write to PC from ALU
        end
        //
        // Index-deferred src: JMP M [ R + M [ PC++ ] ]
        // 2 cycles.
        //
        { 3'd1, 16'o00017z }:		// jmp
        begin `DEFAULT_CONTROL;
                // Fetch an index from memory at PC and store it in Y
                // Increment the PC register value.
                cnext = 2;		// Next cycle 2
                reg_dst = 7;		// Use PC
                alu_input = `ALU_SRC_DST;
                alu_op = `INC2;
                reg_we = 1;		// Write to PC from ALU
                mem_addr = `MEM_DST;	// Get M[PC]...
                x_we = 1;		// ...write X
        end
        { 3'd2, 16'o00017z }:		// Cycle #2
        begin `DEFAULT_CONTROL;
                // Fetch an address from memory[Rs+X] and store it in X.
                cnext = 0;		// Next cycle 0
                reg_src = cmd[2:0];	// Use Rsrc
                reg_dst = 7;            // Use R7 (PC)
                mem_addr = `MEM_SRC_X;	// Get M[src+X]...
                reg_we = 1;		// Write to PC...
                reg_from_mem = 1;	// ...from mem
        end

`ifdef NOTDEF
        { 3'd1, 16'o004zzz },           // jsr
        { 3'd1, 16'oz0020z },           // rts

        { 3'd1, 16'oz00001 },           // wait
        { 3'd1, 16'oz00002 },           // rti, rtt
        { 3'd1, 16'oz00003 },           // bpt
        { 3'd1, 16'oz00004 },           // iot
        { 3'd1, 16'oz00005 },           // reset

        { 3'd1, 16'oz0023z },           // spl
        { 3'd1, 16'oz0024z },           // clc, clv, clz
        { 3'd1, 16'oz0025z },           // cln, ccc
        { 3'd1, 16'oz0026z },           // sec, sev, sez
        { 3'd1, 16'oz0027z },           // sen, scc

        { 3'd1, 16'o104zzz },           // trap, emt

        { 3'd1, 16'oz70zzz }:		// mul
        { 3'd1, 16'oz71zzz }:		// div
        { 3'd1, 16'oz73zzz }:		// ashc
        { 3'd1, 16'oz77zzz },           // sob
`endif
        endcase
    end

endmodule
