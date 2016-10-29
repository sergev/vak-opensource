with System.Machine_Code; use System.Machine_Code;

package body Pkg is

procedure func_gamma (x: in out Gamma) is
begin
	asm ("# func_gamma");
	x.data_gamma := 1;
end func_gamma;

procedure func_beta (x: in out Gamma) is
begin
	asm ("# func_beta");
end func_beta;

procedure func_alpha (x: in out Gamma) is
begin
	asm ("# func_alpha");
	x.data_alpha := 1;
end func_alpha;

end Pkg;
