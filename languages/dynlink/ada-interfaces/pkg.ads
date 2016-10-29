with System;

package Pkg is

type Generic_Interface is interface;
function "=" (x, y: Generic_Interface) return Boolean is abstract;
function Type_Name (x: Generic_Interface) return String is abstract;
function Cast (x: Generic_Interface; iface: String) return System.Address is abstract;

type Sequence is abstract interface and Generic_Interface;
function Value (x: Sequence) return Integer is abstract;
procedure Next (x: in out Sequence) is abstract;

type Printable is abstract interface and Generic_Interface;
procedure Print (x: Printable) is abstract;

type Arithmetic is new Sequence and Printable with record
	val : Integer;
end record;

function "=" (x, y: Arithmetic) return Boolean;
function Type_Name (x: Arithmetic) return String;
function Cast (x: Arithmetic; iface: String) return System.Address;
function Value (n: Arithmetic) return Integer;
procedure Next (n: in out Arithmetic);
procedure Print (n: Arithmetic);

end Pkg;
