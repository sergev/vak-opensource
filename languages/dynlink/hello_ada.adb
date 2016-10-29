with Stream;
with System;
with Ada.Unchecked_Conversion;

package body Hello_Ada is

x : Integer := 12345;

procedure Start (console : Stream.Stream'Class) is
	function Address_To_Unsigned is
		new Ada.Unchecked_Conversion (Source => System.Address,
					      Target => Stream.Unsigned);
begin
	console.Put_String ("Hello, World!" & ASCII.LF & ASCII.NUL);

	console.Put_String ("Loaded at " & ASCII.NUL);
	console.Put_Unsigned (Address_To_Unsigned (Start'Address), 8, 16);
	console.Put_String (ASCII.LF & ASCII.NUL);

	console.Put_String ("Global variable x = " & ASCII.NUL);
	console.Put_Integer (x);
	console.Put_String (ASCII.LF & ASCII.NUL);
end Start;

end Hello_Ada;
