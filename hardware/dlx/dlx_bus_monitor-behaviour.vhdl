--------------------------------------------------------------------------
--
--  Copyright (C) 1993, Peter J. Ashenden
--  Mail:       Dept. Computer Science
--              University of Adelaide, SA 5005, Australia
--  e-mail:     petera@cs.adelaide.edu.au
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
--  $RCSfile: dlx_bus_monitor-behaviour.vhdl,v $  $Revision: 2.1 $  $Date: 1993/10/31 22:36:40 $
--
--------------------------------------------------------------------------
--
--  Behavioural architecture of DLX bus monitor
--


use std.textio.all,
    work.dlx_instr.all,
    work.images.image_hex;
    

architecture behaviour of dlx_bus_monitor is
  
begin

  monitor : if enable generate
  
    enabled_monitor : process
  
      variable write_command, instr_fetch : boolean;
      variable L : line;
    
    begin
      monitor_loop : loop
        --
        -- wait for a command, valid on leading edge of phi2
        --
        wait until phi2 = '1' and mem_enable = '1';
        --
        -- capture the command information
        --
        write_command := write_enable = '1';
        instr_fetch := ifetch = '1';
        write(L, tag);
        write(L, string'(": "));
        if write_command then
          write(L, string'("D-write to "));
        elsif instr_fetch then
          write(L, string'("I-fetch from "));
        else
          write(L, string'("D-read from "));
        end if;
        write(L, image_hex(a));
        if verbose then
          case width is
            when width_word =>
              write(L, string'(", word"));
            when width_halfword =>
              write(L, string'(", halfword"));
            when width_byte =>
              write(L, string'(", byte"));
          end case;
          if burst = '1' then
            write(L, string'(", burst "));
          else
            write(L, string'(", single "));
          end if;
          writeline(output, L);
        else
          if not instr_fetch then
            write(L, string'(", data"));
          else
            writeline(output, L);
          end if;
        end if;
        --
        burst_loop : loop
          if write_command then
            if verbose then
              write(L, tag);
              write(L, string'(": Write data "));
              write(L, image_hex(d));
              writeline(output, L);
            else
              write(L, ' ');
              write(L, image_hex(d));
            end if;
          end if;
          -- wait for the response from memory
          loop 
            wait until phi2 = '0';
            if reset = '1' then
              exit monitor_loop;
            end if;
            exit when ready = '1';
          end loop;
          if write_command then
            if verbose then
              write(L, tag);
              write(L, string'(": Ready"));
              writeline(output, L);
            end if;
          elsif instr_fetch then
            if verbose then
              write(L, tag);
              write(L, string'(": Ready, instruction "));
              write(L, image_hex(d));
              write(L, string'(" [ "));
              write_instr(L, d);
              write(L, string'(" ]"));
              writeline(output, L);
            else                         --  brief instruction fetch
              write(L, tag);
              write(L, string'(":   "));
              write(L, image_hex(d));
              write(L, string'(" [ "));
              write_instr(L, d);
              write(L, string'(" ]"));
              writeline(output, L);
            end if;
          else                           --  data fetch
            if verbose then
              write(L, tag);
              write(L, string'(": Ready, data "));
              write(L, image_hex(d));
              writeline(output, L);
            else                         --  brief data fetch
              write(L, ' ');
              write(L, image_hex(d));
            end if;
          end if;
          exit burst_loop when burst = '0';
        end loop burst_loop;
        --
        if not verbose and not instr_fetch then
          writeline(output, L);
        end if;
      end loop monitor_loop;
      --
      -- get here when reset is asserted
      --
      assert reset = '1'
        report "reset code reached with reset = '0'" severity error;
      write(L, string'("DLX_bus_monitor: Reset"));
      writeline(output, L);
      wait until phi2 = '0' and reset = '0';
      write(L, string'("DLX_bus_monitor: End Reset"));
      writeline(output, L);
      --
      -- process monitor now starts again from beginning
      --
    end process enabled_monitor;

  end generate;

end behaviour;
