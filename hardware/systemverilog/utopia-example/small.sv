`timescale 1ns/1ns

// Define this so Utopia won't include Utopia methods
`define SYNTHESIS
`define TxPorts 4  // set number of transmit ports
`define RxPorts 4  // set number of receive ports

parameter NumRx = 4;
parameter NumTx = 4;

`include "Utopia.sv"

module top;
  logic rst;
  logic clk;

  // System Clock and Reset
  initial begin
    #0 rst = 0; clk = 0;
    #5 rst = 1;
    #5 clk = 1;
    #5 rst = 0; clk = 0;
    forever begin
      #5 clk = 1;
      #5 clk = 0;
    end
  end

   Utopia Rx[0:NumRx-1]();   // NumRx x Level 1 Utopia Rx Interfaces
   Utopia Tx[0:NumTx-1]();   // NumTx x Level 1 Utopia Tx Interfaces

   testp t1();
endmodule : top




program automatic testp;

`include "environment.sv"
   Environment env;

   initial begin
      env = new(top.Rx, top.Tx, NumRx, NumTx);
      env.gen_cfg;
      env.cfg.nCells = 2;
      env.cfg.cells_per_chan[0] = 2; env.cfg.in_use_Rx[0] = 1;
      env.cfg.cells_per_chan[1] = 0; env.cfg.in_use_Rx[1] = 0;
      env.cfg.cells_per_chan[2] = 0; env.cfg.in_use_Rx[2] = 0;
      env.cfg.cells_per_chan[3] = 0; env.cfg.in_use_Rx[3] = 0;
      env.cfg.display("Env: ");
      env.build();
      env.run();
      env.wrap_up;
   end
endprogram : testp