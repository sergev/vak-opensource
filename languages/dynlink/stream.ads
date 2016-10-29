with System;

package Stream is

type Unsigned is mod 2 ** Integer'Size;

type Stream is interface;

procedure Put_String
	(S   : Stream;
	 Val : String) is abstract;

procedure Put_Integer
	(S     : Stream;
	 Val   : Integer;
	 Width : Integer := 0;
	 Base  : Integer := 10) is abstract;

procedure Put_Unsigned
	(S     : Stream;
	 Val   : Unsigned;
	 Width : Integer := 0;
	 Base  : Integer := 10) is abstract;

end Stream;
