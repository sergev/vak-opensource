`ifndef UART_PORTS_SV
`define UART_PORTS_SV

interface uart_ports (
    output  logic       reset,
    input   wire        txclk,
    output  logic       ld_tx_data,
    output  logic [7:0] tx_data,
    output  logic       tx_enable,
    output  logic       tx_out,
    input   wire        tx_empty,
    input   wire        rxclk,
    output  logic       uld_rx_data,
    input   wire  [7:0] rx_data,
    output  logic       rx_enable,
    output  logic       rx_in,
    input   wire        rx_empty,
    output  logic       loopback,
    output  logic       rx_tb_in
);
endinterface

`endif
