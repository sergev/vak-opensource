package Pkg is

type Alpha is abstract tagged record
	data_alpha : Integer;
end record;
procedure func_alpha (x: in out Alpha) is abstract;

type Beta is interface;
procedure func_beta (x: in out Beta) is abstract;

type Gamma is new Alpha and Beta with record
	data_gamma : Integer;
end record;
procedure func_alpha (x: in out Gamma);
procedure func_beta (x: in out Gamma);
procedure func_gamma (x: in out Gamma);

end Pkg;
