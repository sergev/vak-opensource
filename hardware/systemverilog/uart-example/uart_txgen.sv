class uart_txgen;
    uart_sb sb;
    virtual uart_ports ports;
    bit tx_done;
    bit rx_done;

    // Connects the transmitter output to recevier input
    bit loopback;

    // Number of frames to send to transmitter
    integer no_tx_cmds;

    // Number of frames to send to receiver
    integer no_rx_cmds;

    // Delay the reading of data from receiver
    bit rx_over_flow;

    // Send frame to transmitter before it has sent out last frame
    bit tx_over_flow;

    // Insert framming error (stop bit) in frame sent to receiver
    bit rx_frame_err;

    function new (virtual uart_ports ports);
    begin
        this.ports = ports;
        sb = new();
        tx_done = 0;
        rx_done = 0;
        no_tx_cmds = 5;
        no_rx_cmds = 5;
        rx_over_flow = 0;
        rx_frame_err = 0;
        ports.rx_tb_in = 1;
        ports.uld_rx_data = 0;
    end
    endfunction

    // Main method, which starts rest of methods
    task goTxgen();
    begin
        tx_done = 0;
        rx_done = 0;
        assertReset();
        fork
            txDriver();
            rxDriver();
            txMonitor();
            rxMonitor();
        join_none
    end
    endtask
    // This method asserts method
    task assertReset();
    begin
        @ (posedge ports.rxclk);
        ports.reset = 1;
        $write("%dns : Asserting reset to Uart\n",$time);
        repeat (5) @ (posedge ports.rxclk);
        ports.reset = 0;
    end
    endtask

    task txDriver();
    begin
        integer i = 0;
        integer tx_timeout = 0;
        bit [7:0] tx_data = 0;
        ports.tx_enable = 1;
        for (i = 0; i < no_tx_cmds; i ++)  begin
            tx_data = $random();
            sb.txAdd(tx_data);
            if (loopback == 1) begin
                sb.rxAdd(tx_data);
            end
            // Check if uart is ready to accept data for transmission
            while (ports.tx_empty == 0) begin
                @ (posedge ports.txclk);
                tx_timeout ++ ;
                if (tx_timeout > 10) begin
                    $write("%dns : txDriver : Warning : tx_empty is 0 for more then 10 clocks\n",
                        $time);
                end
            end
            tx_timeout = 0;
            // Drive the data in UART for transmitting
            @ (posedge ports.txclk);
            ports.ld_tx_data = 1;
            ports.tx_data    = tx_data;
            $write("%dns : txDriver  : Transmitting data %x\n",$time, tx_data);
            @ (posedge ports.txclk);
            ports.ld_tx_data = 0;
            ports.tx_data    = 0;
            while (ports.tx_empty == 1) begin
                @ (posedge ports.txclk);
                tx_timeout ++ ;
                if (tx_timeout > 10) begin
                    $write("%dns : txDriver : Warning : tx_empty is 1 for more then 10 clocks\n",
                        $time);
                end
            end
            tx_timeout = 0;
        end
        tx_done = 1;
    end
    endtask

    task rxDriver();
    begin
        bit [7:0] rx_data = 0;
        integer i,j = 0;
        ports.rx_enable = 1;
        if (loopback == 1) begin
            ports.loopback = 1;
        end else begin
            ports.loopback = 0;
            for (i = 0; i < no_rx_cmds; i++)  begin
                rx_data = $random();
                sb.rxAdd(rx_data);
                $write("%dns : rxDriver  : Transmitting data %x\n",$time, rx_data);
                @ (posedge ports.txclk);
                ports.rx_in = 0;
                for (j = 0; j < 8; j ++) begin
                    @ (posedge ports.txclk);
                    ports.rx_in = rx_data[j];
                end
                @ (posedge ports.txclk);
                ports.rx_in = 1;
                @ (posedge ports.txclk);
            end
        end
        rx_done = 1;
    end
    endtask

    task txMonitor();
    begin
        bit [7:0] tx_data = 0;
        integer i = 0;
        while (1) begin
            @ (posedge ports.txclk);
            if (ports.tx_out == 0) begin
                $write("%dns : txMonitor : Found start of frame\n",$time);
                for (i = 0; i < 8; i ++)  begin
                    @ (posedge ports.txclk);
                    tx_data[i] = ports.tx_out;
                end
                @ (posedge ports.txclk);
                if (ports.tx_out == 0) begin
                    $write("%dns : txMonitor Error : Framing error detecting\n",$time);
                    sb.txCompare(8'b0);
                end else begin
                    $write("%dns : txMonitor : Sampled data %x\n",$time, tx_data);
                    sb.txCompare(tx_data);
                end
            end
        end
    end
    endtask

    task rxMonitor() ;
    begin
        bit [7:0] rx_data = 0;
        while (1) begin
            @ (posedge ports.txclk);
            if (ports.rx_empty == 0) begin
                ports.uld_rx_data = 1;
                @ (posedge ports.txclk);
                rx_data = ports.rx_data;
                ports.uld_rx_data = 0;
                $write("%dns : rxMonitor : Sampled data %x\n",$time, rx_data);
                sb.rxCompare(rx_data);
                @ (posedge ports.txclk);
            end
        end
    end
    endtask

    function bit isDone();
    begin
        if (tx_done == 1 && rx_done == 1) begin
            isDone =  1;
        end else begin
            isDone = 0;
        end
    end
    endfunction
endclass
