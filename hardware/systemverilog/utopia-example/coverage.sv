// ToDo: Copyright notice header

`ifndef COVERAGE__SV
`define COVERAGE__SV


class Coverage;

   bit [1:0] src;
   bit [NumTx-1:0] fwd;

   covergroup CG_Forward;

      coverpoint src
	{bins src[] = {[0:3]};
	 option.weight = 0;}
      coverpoint fwd
	{bins fwd[] = {[1:15]}; // Ignore fwd==0
	 option.weight = 0;}
      cross src, fwd;

   endgroup : CG_Forward

     // Instantiate the covergroup
     function new;
	CG_Forward = new;
     endfunction : new

   // Sample input data
   function void sample(input bit [1:0] src,
			input bit [NumTx-1:0] fwd);
      $display("@%0t: Coverage: src=%d. FWD=%b", $time, src, fwd);
      this.src = src;
      this.fwd = fwd;
      CG_Forward.sample();
   endfunction : sample

endclass : Coverage

`endif // COVERAGE__SV
