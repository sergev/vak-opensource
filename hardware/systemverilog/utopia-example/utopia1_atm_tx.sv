/**********************************************************************
 * Utopia ATM transmitter
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

module utopia1_atm_tx ( Utopia.CoreTransmit Tx );

  assign Tx.clk_out = Tx.clk_in;

  logic [0:5] PayloadIndex;  // 0 to 47
  enum bit [0:3] { reset, soc, vpi_vci, vci, vci_clp_pt, hec,
                   payload, ack, done } UtopiaStatus;

  always_ff @(posedge Tx.clk_in, posedge Tx.reset) begin: FSM
    if (Tx.reset) begin
      Tx.soc <= 0;
      Tx.en <= 1;
      Tx.ready <= 1;
      UtopiaStatus <= reset;
    end
    else begin: FSM_sequencer
      unique case (UtopiaStatus)
        reset: begin: reset_state
          Tx.en <= 1;
          Tx.ready <= 1;
          if (Tx.valid) begin
            Tx.ready <= 0;
            UtopiaStatus <= soc;
          end
        end: reset_state

        soc: begin: soc_state
          if (Tx.clav) begin
            Tx.soc <= 1;
            Tx.data <= Tx.ATMcell.nni.VPI[11:4];
            UtopiaStatus <= vpi_vci;
          end
          Tx.en <= !Tx.clav;
        end: soc_state

        vpi_vci: begin: vpi_vci_state
          Tx.soc <= 0;
          if (Tx.clav) begin
            Tx.data <= {Tx.ATMcell.nni.VPI[3:0],
                        Tx.ATMcell.nni.VCI[15:12]};
            UtopiaStatus <= vci;
          end
          Tx.en <= !Tx.clav;
        end: vpi_vci_state

        vci: begin: vci_state
          if (Tx.clav) begin
            Tx.data <= Tx.ATMcell.nni.VCI[11:4];
            UtopiaStatus <= vci_clp_pt;
          end
          Tx.en <= !Tx.clav;
        end: vci_state

        vci_clp_pt: begin: vci_clp_pt_state
          if (Tx.clav) begin
            Tx.data <= {Tx.ATMcell.nni.VCI[3:0],
                        Tx.ATMcell.nni.CLP, Tx.ATMcell.nni.PT};
            UtopiaStatus <= hec;
          end
          Tx.en <= !Tx.clav;
        end: vci_clp_pt_state

        hec: begin: hec_state
          if (Tx.clav) begin
            Tx.data <= Tx.ATMcell.nni.HEC;
            UtopiaStatus <= payload;
            PayloadIndex = 0;
          end
          Tx.en <= !Tx.clav;
        end: hec_state

        payload: begin: payload_state
          if (Tx.clav) begin
            Tx.data <= Tx.ATMcell.nni.Payload[PayloadIndex];
            if (PayloadIndex==47) UtopiaStatus <= ack;
            PayloadIndex++;
          end
          Tx.en <= !Tx.clav;
        end: payload_state

        ack: begin: ack_state
          Tx.en <= 1;
          if (!Tx.valid) begin
            Tx.ready <= 1;
            UtopiaStatus <= done;
          end
        end: ack_state

        done: begin: done_state
          if (!Tx.valid) begin
            Tx.ready <= 0;
            UtopiaStatus <= reset;
          end
        end: done_state
      endcase
    end: FSM_sequencer
  end: FSM
endmodule // utopia1_atm_tx

