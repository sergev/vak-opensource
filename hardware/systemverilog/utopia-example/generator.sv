// ToDo: Copyright notice header

`ifndef GENERATOR__SV
 `define GENERATOR__SV

`include "atm_cell.sv"


/////////////////////////////////////////////////////////////////////////////
class UNI_generator;

   UNI_cell blueprint;	// Blueprint for generator
   mailbox  gen2drv;	// Mailbox to driver for cells
   event    drv2gen;	// Event from driver when done with cell
   int      nCells;	// Number of cells for this generator to create
   int	    PortID;	// Which Rx port are we generating?
   
   function new(input mailbox gen2drv,
		input event drv2gen,
		input int nCells,
		input int PortID);
      this.gen2drv = gen2drv;
      this.drv2gen = drv2gen;
      this.nCells  = nCells;
      this.PortID  = PortID;
      blueprint = new();
   endfunction : new

   task run();
      UNI_cell cell;
      repeat (nCells) begin
	 assert(blueprint.randomize());
	 $cast(cell, blueprint.copy());
	 cell.display($psprintf("@%0t: Gen%0d: ", $time, PortID));
	 gen2drv.put(cell);
	 @drv2gen;		// Wait for driver to finish with it
      end
   endtask : run

endclass : UNI_generator



/////////////////////////////////////////////////////////////////////////////
class NNI_generator;

   NNI_cell blueprint;	// Blueprint for generator
   mailbox  gen2drv;	// Mailbox to driver for cells
   event    drv2gen;	// Event from driver when done with cell
   int      nCells;	// Number of cells for this generator to create
   int	    PortID;	// Which Rx port are we generating?

   function new(input mailbox gen2drv,
		input event drv2gen,
		input int nCells,
		input int PortID);
      this.gen2drv = gen2drv;
      this.drv2gen = drv2gen;
      this.nCells  = nCells;
      this.PortID  = PortID;
      blueprint = new();
   endfunction : new


   task run();
      NNI_cell cell;
      repeat (nCells) begin
	 assert(blueprint.randomize());
	 $cast(cell, blueprint.copy());
	 cell.display($psprintf("Gen%0d: ", PortID));
	 gen2drv.put(cell);
	 @drv2gen;		// Wait for driver to finish with it
      end
   endtask : run

endclass : NNI_generator

`endif // GENERATOR__SV
