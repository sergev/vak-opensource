--------------------------------------------------------------------------
--
--  Copyright (C) 1993, Peter J. Ashenden
--  Mail:	Dept. Computer Science
--		University of Adelaide, SA 5005, Australia
--  e-mail:	petera@cs.adelaide.edu.au
--
--  This program is free software; you can redistribute it and/or modify
--  it under the terms of the GNU General Public License as published by
--  the Free Software Foundation; either version 1, or (at your option)
--  any later version.
--
--  This program is distributed in the hope that it will be useful,
--  but WITHOUT ANY WARRANTY; without even the implied warranty of
--  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--  GNU General Public License for more details.
--
--  You should have received a copy of the GNU General Public License
--  along with this program; if not, write to the Free Software
--  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
--
--------------------------------------------------------------------------
--
--  $RCSfile: dlx_types-body.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 20:37:38 $
--
--------------------------------------------------------------------------
--
--  Package body for types used in dlx model
--


package body dlx_types is

  function resolve_dlx_word (values : in dlx_word_array) return dlx_word is

    variable result : dlx_word := X"0000_0000";

  begin
    for i in values'range loop
      result := result or values(i);
    end loop;
    return result;
  end resolve_dlx_word;

end dlx_types;
