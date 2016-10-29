with System;
use System;

package body Pkg is

function "=" (x, y: Arithmetic) return Boolean is
begin
	return (x'Address = y'Address);
end "=";

function Type_Name (x: Arithmetic) return String is
begin
	return "PKG.ARITHMETIC";
end Type_Name;

function Cast (x: in Arithmetic; iface: String) return System.Address is
begin
	if iface = "PKG.SEQUENCE" then
		return Sequence(x)'Address;
	end if;
	if iface = "PKG.PRINTABLE" then
		return Printable(x)'Address;
	end if;
	if iface = "PKG.ARITHMETIC" then
		return Arithmetic(x)'Address;
	end if;
	return Null_Address;
end Cast;

function Value (n: Arithmetic) return Integer is
begin
	return n.val;
end Value;

procedure Next (n: in out Arithmetic) is
begin
	n.val := n.val + 1;
end Next;

procedure Print (n: Arithmetic) is
begin
	return;
end Print;

end Pkg;
