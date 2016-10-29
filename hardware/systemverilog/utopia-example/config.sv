// ToDo: Copyright header notice

`ifndef CONFIG__SV
`define CONFIG__SV


/////////////////////////////////////////////////////////////////////////////
// Configuration descriptor for ATM testbench
/////////////////////////////////////////////////////////////////////////////
class Config;
   int nErrors, nWarnings;		// Number of errors, warnings during simulation

   bit [31:0] numRx, numTx;		// Copy of parameters

   constraint c_numRxTx_valid
     {numRx inside {[1:16]};
      numTx inside {[1:16]};}

   rand bit [31:0] nCells;	// Total number of cells to transmit / receive
   constraint c_nCells_valid
     {nCells > 0; }
   constraint c_nCells_reasonable
     {nCells < 1000; }

   rand bit in_use_Rx[];	// Input / output channel enabled
   constraint c_in_use_valid
     {in_use_Rx.sum > 0; }	// Make sure at least one RX is enabled

   rand bit [31:0] cells_per_chan[];
   constraint c_sum_ncells_sum		// Split cells over all channels
     {cells_per_chan.sum == nCells;	// Total number of cells
      }

   // Set the cell count to zero for any channel not in use
   constraint c_zero_unused_channels
     {foreach (cells_per_chan[i])
       {
//	solve in_use_Rx[i] before cells_per_chan[i];  // Needed for even dist of in_use
	if (in_use_Rx[i])
	     cells_per_chan[i] inside {[1:nCells]};
	else cells_per_chan[i] == 0;
	}
      }

   extern function new(input bit [31:0] numRx, numTx);
   extern virtual function void display(input string prefix="");

endclass : Config


//---------------------------------------------------------------------------
   function Config::new(input bit [31:0] numRx, numTx);
   if (numRx < 1 || numRx > 16) begin
      $display("FATAL %m numRx %0d out of bounds 1..16", numRx);
      $finish;
   end
   this.numRx = numRx;
   in_use_Rx = new[numRx];

   if (numTx < 1 || numTx > 16) begin
      $display("FATAL %m numTx %0d out of bounds 1..16", numTx);
      $finish;
   end
   this.numTx = numTx;

   cells_per_chan = new[numRx];
endfunction : new


//---------------------------------------------------------------------------
function void Config::display(input string prefix="");
    $write("%sConfig: numRx=%0d, numTx=%0d, nCells=%0d (", prefix, numRx, numTx, nCells);
   foreach (cells_per_chan[i])
      $write("%0d ", cells_per_chan[i]);
   $write("), enabled RX: ", prefix);
   foreach (in_use_Rx[i]) if (in_use_Rx[i]) $write("%0d ", i);
   $display;
 endfunction : display

`endif // CONFIG__SV
