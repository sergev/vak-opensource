`include "uart_ports.sv"

program uart_top (uart_ports ports);

`include "uart_sb.sv"
`include "uart_txgen.sv"

    uart_txgen txgen = new(ports);

    initial begin
        fork
            txgen.goTxgen();
        join_none

        while (!txgen.isDone()) begin
            @ (posedge ports.txclk);
        end
        repeat (200) @ (posedge ports.txclk);
        $write("%dns : Termintating the simulation\n",$time);
    end
endprogram
