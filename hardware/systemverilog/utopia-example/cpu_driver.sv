// ToDo: Copyright notice header

`ifndef CPU_DRIVER__SV
 `define CPU_DRIVER__SV

`include "atm_cell.sv"
//`include "CpuInterface.sv"
//`include "CPUMethod.sv"

typedef virtual CpuInterface vCPU;
typedef virtual CpuInterface.Test vCPU_T;

class CPU_driver;
   vCPU_T mif;
   CellCfgType lookup [255:0]; // copy of look-up table
   Config cfg;
   bit [NumTx-1:0] fwd;

   extern function new(vCPU_T mif, Config cfg);
   extern task Initialize_Host ();
   extern task HostWrite (int a, CellCfgType d); // configure
   extern task HostRead (int a, output CellCfgType d);
   extern task run();
endclass : CPU_driver


function CPU_driver::new(vCPU_T mif, Config cfg);
   this.mif = mif;
   this.cfg = cfg;
endfunction : new


task CPU_driver::Initialize_Host ();
   mif.BusMode <= 1;
   mif.Addr <= 0;
   mif.DataIn <= 0;
   mif.Sel <= 1;
   mif.Rd_DS <= 1;
   mif.Wr_RW <= 1;
endtask : Initialize_Host


task CPU_driver::HostWrite (int a, CellCfgType d); // configure
   #10 mif.Addr <= a; mif.DataIn <= d; mif.Sel <= 0;
   #10 mif.Wr_RW <= 0;
   while (mif.Rdy_Dtack!==0) #10;
   #10 mif.Wr_RW <= 1; mif.Sel <= 1;
   while (mif.Rdy_Dtack==0) #10;
endtask : HostWrite


task CPU_driver::HostRead (int a, output CellCfgType d);
   #10 mif.Addr <= a; mif.Sel <= 0;
   #10 mif.Rd_DS <= 0;
   while (mif.Rdy_Dtack!==0) #10;
   #10 d = mif.DataOut; mif.Rd_DS <= 1; mif.Sel <= 1;
   while (mif.Rdy_Dtack==0) #10;
endtask : HostRead


task CPU_driver::run();
   CellCfgType CellFwd;
   Initialize_Host();

   // Configure through Host interface
   repeat (10) @(negedge clk);
   $write("Memory: Loading ... ");
   for (int i=0; i<=255; i++) begin
      CellFwd.FWD = $urandom();
`ifdef FWDALL
      CellFwd.FWD = '1
`endif
      $display("CellFwd.FWD[%0d]=%0d", i, CellFwd.FWD);
      CellFwd.VPI = i;
      HostWrite(i, CellFwd);
      lookup[i] = CellFwd;
   end

   // Verify memory
   $write("Verifying ...");
   for (int i=0; i<=255; i++) begin
      HostRead(i, CellFwd);
      if (lookup[i] != CellFwd) begin
         $display("FATAL, Mem Location 0x%x contains 0x%x, expected 0x%x",
                  i, CellFwd, lookup[i]);
         $finish;
      end
   end
   $display("Verified");

endtask : run


`endif // CPU_DRIVER__SV
