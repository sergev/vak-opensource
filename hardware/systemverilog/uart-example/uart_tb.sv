`include "uart.v"
module testbench();

wire        reset;
wire        ld_tx_data;
wire [7:0]  tx_data;
wire        tx_enable;
wire        tx_out;
wire        tx_empty;
wire        uld_rx_data;
wire [7:0]  rx_data;
wire        rx_enable;
wire        rx_in;
wire        rx_empty;
wire        loopback;
wire        rx_tb_in;
reg         txclk;
reg         rxclk;

uart_ports ports (
    .reset          (reset),
    .txclk          (txclk),
    .ld_tx_data     (ld_tx_data),
    .tx_data        (tx_data),
    .tx_enable      (tx_enable),
    .tx_out         (tx_out),
    .tx_empty       (tx_empty),
    .rxclk          (rxclk),
    .uld_rx_data    (uld_rx_data),
    .rx_data        (rx_data),
    .rx_enable      (rx_enable),
    .rx_in          (rx_in),
    .rx_empty       (rx_empty),
    .loopback       (loopback),
    .rx_tb_in       (rx_tb_in)
);

uart_top tbtop(ports);

initial begin
    $dumpfile("uart.vcd");
    $dumpvars();
    txclk = 0;
    rxclk = 0;
end

// Loopback control logic
assign rx_in = (loopback) ? tx_out : rx_tb_in;

// RX and TX Clock generation
always #1 rxclk = ~rxclk;
always #16 txclk = ~txclk;

// DUT Connected here
uart U (
    .reset          (reset),
    .txclk          (txclk),
    .ld_tx_data     (ld_tx_data),
    .tx_data        (tx_data),
    .tx_enable      (tx_enable),
    .tx_out         (tx_out),
    .tx_empty       (tx_empty),
    .rxclk          (rxclk),
    .uld_rx_data    (uld_rx_data),
    .rx_data        (rx_data),
    .rx_enable      (rx_enable),
    .rx_in          (rx_in),
    .rx_empty       (rx_empty)
);

endmodule
