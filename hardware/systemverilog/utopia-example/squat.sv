/**********************************************************************
 * ATM squat top-level module
 *
 * To simulate this example with stimulus, invoke simulation on
 * 10.00.00_example_top.sv.  This top-level file includes all of the
 * example files in chapter 10.
 *
 * Author: Lee Moore, Stuart Sutherland
 *
 * (c) Copyright 2003, Sutherland HDL, Inc. *** ALL RIGHTS RESERVED ***
 * www.sutherland-hdl.com
 *
 * This example is based on an example from Janick Bergeron's
 * Verification Guild[1].  The original example is a non-synthesizable
 * behavioral model written in Verilog-1995 of a quad Asynchronous
 * Transfer Mode (ATM) user-to-network interface and forwarding node.
 * This example modifies the original code to be synthesizable, using
 * SystemVerilog constructs.  Also, the model has been made to be
 * configurable, so that it can be easily scaled from a 4x4 quad switch
 * to a 16x16 switch, or any other desired configuration.  The example,
 * including a nominal test bench, is partitioned into 8 files,
 * numbered 10.xx.xx_example_10-1.sv through 10-8.sv (where xx
 * represents section and subsection numbers in the book "SystemVerilog
 * for Design" (first edition).  The file 10.00.00_example_top.sv
 * includes all of the other files.  Simulation only needs to be
 * invoked on this one file.  Conditional compilation switches (`ifdef)
 * is used to compile the examples for simulation or for synthesis.
 *
 * [1] The Verification Guild is an independent e-mail newsletter and
 * moderated discussion forum on hardware verification.  Information on
 * the original Verification Guild example can be found at
 * www.janick.bergeron.com/guild/project.html.
 *
 * Used with permission in the book, "SystemVerilog for Design"
 *  By Stuart Sutherland, Simon Davidmann, and Peter Flake.
 *  Book copyright: 2003, Kluwer Academic Publishers, Norwell, MA, USA
 *  www.wkap.il, ISBN: 0-4020-7530-8
 *
 * Revision History:
 *   1.00 15 Dec 2003 -- original code, as included in book
 *   1.01 10 Jul 2004 -- cleaned up comments, added expected results
 *                       to output messages
 *   1.10 21 Jul 2004 -- corrected errata as printed in the book
 *                       "SystemVerilog for Design" (first edition) and
 *                       to bring the example into conformance with the
 *                       final Accellera SystemVerilog 3.1a standard
 *                       (for a description of changes, see the file
 *                       "errata_SV-Design-book_26-Jul-2004.txt")
 *
 * Caveat: Expected results displayed for this code example are based
 * on an interpretation of the SystemVerilog 3.1 standard by the code
 * author or authors.  At the time of writing, official SystemVerilog
 * validation suites were not available to validate the example.
 *
 * RIGHT TO USE: This code example, or any portion thereof, may be
 * used and distributed without restriction, provided that this entire
 * comment block is included with the example.
 *
 * DISCLAIMER: THIS CODE EXAMPLE IS PROVIDED "AS IS" WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED
 * TO WARRANTIES OF MERCHANTABILITY, FITNESS OR CORRECTNESS. IN NO
 * EVENT SHALL THE AUTHOR OR AUTHORS BE LIABLE FOR ANY DAMAGES,
 * INCLUDING INCIDENTAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF THE
 * USE OF THIS CODE.
 *********************************************************************/

`include "definitions.sv"  // include external definitions

module squat
  #(parameter int NumRx = 4, parameter int NumTx = 4)
   (// NumRx x Level 1 Utopia ATM layer Rx Interfaces
    Utopia.TopReceive Rx[0:NumRx-1],

    // NumTx x Level 1 Utopia ATM layer Tx Interfaces
    Utopia.TopTransmit Tx[0:NumTx-1],

    // Utopia Level 2 parallel management interface
    // Intel-style Utopia parallel management interface
    CpuInterface.Peripheral mif,

    // Miscellaneous control interfaces
    input wire rst, clk
   );

  // Register file
  LookupTable #(.Asize(8), .dType(CellCfgType)) lut();

  //
  // Hardware reset
  //
  logic reset;
  always_ff @(posedge clk) begin
    reset <= rst;
  end

  const bit [2:0] WriteCycle = 3'b010;
  const bit [2:0] ReadCycle = 3'b001;
  always_latch begin  // configure look-up table
    if (mif.BusMode == 1'b1) begin
      /*unique*/ case ({mif.Sel, mif.Rd_DS, mif.Wr_RW})
        WriteCycle: lut.write(mif.Addr, mif.DataIn);
      endcase
    end
  end

  always_comb begin
    mif.Rdy_Dtack <= 1'bz;
    mif.DataOut <= 8'hzz;
    if (mif.BusMode == 1'b1) begin
      /*unique*/ case ({mif.Sel, mif.Rd_DS, mif.Wr_RW})
        WriteCycle: mif.Rdy_Dtack <= 1'b0;
        ReadCycle: begin
                     mif.Rdy_Dtack <= 1'b0;
                     mif.DataOut <= lut.read(mif.Addr);
                   end
      endcase
    end
  end

  //
  // ATM-layer Utopia interface receivers
  //
  genvar RxIter;
  generate
    for (RxIter=0; RxIter<NumRx; RxIter+=1) begin: RxGen
      assign Rx[RxIter].clk_in = clk;
      assign Rx[RxIter].reset  = reset;
      utopia1_atm_rx atm_rx(Rx[RxIter].CoreReceive);
    end
  endgenerate

  //
  // ATM-layer Utopia interface transmitters
  //
  genvar TxIter;
  generate
    for (TxIter=0; TxIter<NumTx; TxIter+=1) begin: TxGen
      assign Tx[TxIter].clk_in = clk;
      assign Tx[TxIter].reset  = reset;
      utopia1_atm_tx atm_tx(Tx[TxIter].CoreTransmit);
    end
  endgenerate

   //
   // Generate the CRC-8 syndrom table
   //
   bit [7:0] syndrom[0:255];
   initial begin
      bit [7:0] sndrm;

      for (int i=0; i<256; i++) begin
	 sndrm = i;
	 repeat (8) begin
	    if (sndrm[7] == 1'b1)
              sndrm = (sndrm << 1) ^ 8'h07;
	    else
              sndrm = sndrm << 1;
	 end
	 syndrom[i] = sndrm;
      end
   end


  //
  // Function to compute the HEC value
  //
  function bit [7:0] hec (input bit [31:0] hdr);
    bit [7:0] RtnCode;

    RtnCode = 8'h00;
    repeat (4) begin
      RtnCode = syndrom[RtnCode ^ hdr[31:24]];
      hdr = hdr << 8;
    end
    RtnCode = RtnCode ^ 8'h55;
    return RtnCode;
  endfunction

  //
  // Rewriting and forwarding process
  //

  logic [NumTx-1:0] forward; // Changed to be big-endian

  typedef enum bit [0:1] {wait_rx_valid,
                          wait_rx_not_valid,
                          wait_tx_ready,
                          wait_tx_not_ready } StateType;
  StateType SquatState;

  bit [0:NumTx-1] Txvalid;
  bit [0:NumTx-1] Txready;
  bit [0:NumTx-1] Txsel_in;
  bit [0:NumTx-1] Txsel_out;
  bit [0:NumRx-1] Rxvalid;
  bit [0:NumRx-1] Rxready;
  bit [0:NumRx-1] RoundRobin;
  ATMCellType [0:NumRx-1] RxATMcell;
  ATMCellType [0:NumTx-1] TxATMcell;

  generate
    for (TxIter=0; TxIter<NumTx; TxIter+=1) begin: GenTx
      assign Tx[TxIter].valid    = Txvalid[TxIter];
      assign Txready[TxIter]     = Tx[TxIter].ready;
      assign Txsel_in[TxIter]    = Tx[TxIter].selected;
      assign Tx[TxIter].selected = Txsel_out[TxIter];
      assign Tx[TxIter].ATMcell  = TxATMcell[TxIter];
    end
  endgenerate
  generate
    for (RxIter=0; RxIter<NumRx; RxIter+=1) begin: GenRx
      assign Rxvalid[RxIter]   = Rx[RxIter].valid;
      assign Rx[RxIter].ready  = Rxready[RxIter];
      assign RxATMcell[RxIter] = Rx[RxIter].ATMcell;
    end
  endgenerate

  ATMCellType ATMcell;
  always_ff @(posedge clk, posedge reset) begin: FSM
    bit breakVar;
    if (reset) begin: reset_logic
      Rxready <= '1;
      Txvalid <= '0;
      Txsel_out <= '0;
      SquatState <= wait_rx_valid;
      forward <= 0;
      RoundRobin = 1;
    end: reset_logic
    else begin: FSM_sequencer
      unique case (SquatState)

        wait_rx_valid: begin: rx_valid_state
          Rxready <= '1;
          breakVar = 1;
          for (int j=0; j<NumRx; j+=1) begin: loop1
            for (int i=0; i<NumRx; i+=1) begin: loop2
              if (Rxvalid[i] && RoundRobin[i] && breakVar)
                begin: match
                  ATMcell <= RxATMcell[i];
                  Rxready[i] <= 0;
                  SquatState <= wait_rx_not_valid;
                  breakVar = 0;
                end: match
            end: loop2
            if (breakVar)
              RoundRobin={RoundRobin[1:$bits(RoundRobin)-1],
                          RoundRobin[0]};
          end: loop1
        end: rx_valid_state

        wait_rx_not_valid: begin: rx_not_valid_state
          if (ATMcell.uni.HEC != hec(ATMcell.Mem[0:3])) begin
            SquatState <= wait_rx_valid;
            `ifndef SYNTHESIS // synthesis ignores this code
              $display("@%0t: Bad HEC: ATMcell.uni.HEC(0x%x) != (0x%x) hec(ATMcell.Mem[0:3])(0x%x)",
		       $time, ATMcell.uni.HEC, hec(ATMcell.Mem[0:3]), ATMcell.Mem[0:3]);
//	      $write("Bad HEC for: "); foreach (ATMcell.Mem[i]) $write("%x ", ATMcell.Mem[i]); $display;
	      $write("Bad HEC for: "); for (int i=0; i<ATMcell.Mem.size; i+=1) $write("%x ", ATMcell.Mem[i]); $display;

            `endif
          end
          else begin
            // Get the forward ports & new VPI
            {forward, ATMcell.nni.VPI} <=
              lut.read(ATMcell.uni.VPI);
            // Recompute the HEC
            ATMcell.nni.HEC <= hec(ATMcell.Mem[0:3]);
            SquatState <= wait_tx_ready;
          end
        end: rx_not_valid_state

        wait_tx_ready: begin: tx_valid_state
          if (forward) begin
            for (int i=0; i<NumTx; i+=1) begin
              if (forward[i] && Txready[i]) begin
                TxATMcell[i] <= ATMcell;
                Txvalid[i] <= 1;
                Txsel_out[i] <= 1;
              end
            end
            SquatState <= wait_tx_not_ready;
          end
          else begin
            SquatState <= wait_rx_valid;
          end
        end: tx_valid_state

        wait_tx_not_ready: begin: tx_not_valid_state
          for (int i=0; i<NumTx; i+=1) begin
            if (forward[i] && !Txready[i] && Txsel_in[i]) begin
              Txvalid[i] <= 0;
              Txsel_out[i] <= 0;
              forward[i] <= 0;
            end
          end
          if (forward)
            SquatState <= wait_tx_ready;
          else
            SquatState <= wait_rx_valid;
        end: tx_not_valid_state

        default: begin: unknown_state
          SquatState <= wait_rx_valid;
          `ifndef SYNTHESIS // synthesis ignores this code
            $display("Unknown condition"); $finish();
          `endif
        end: unknown_state
      endcase
    end: FSM_sequencer
  end: FSM
endmodule : squat
