--
-- echo [string ...]
--
-- Write arguments to the standard output
--
-- The echo utility writes any specified operands, separated by single blank
-- (`` '') characters and followed by a newline (``\n'') character, to the
-- standard output.
--
with Ada.Command_Line;
with Ada.Text_IO;
use Ada;

procedure Echo is
begin
	if Command_Line.Argument_Count > 0 then
		Text_IO.Put (Command_Line.Argument (1));
		for Arg in 2 .. Command_Line.Argument_Count loop
			Text_IO.Put (' ');
			Text_IO.Put (Command_Line.Argument (Arg));
		end loop;
	end if;
	Text_IO.New_Line;
end Echo;
