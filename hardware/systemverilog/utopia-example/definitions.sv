/**********************************************************************
 * External typdefs included by most of the Utopia ATM model files
 *
 * To simulate this example with stimulus, invoke simulation on
 * 10.00.00_example_top.sv.  This top-level file includes all of the
 * example files in chapter 10.
 *
 * Author: Lee Moore, Stuart Sutherland
 *
 * (c) Copyright 2003, Sutherland HDL, Inc. *** ALL RIGHTS RESERVED ***
 * www.sutherland-hdl.com
 *
 * This example is based on an example from Janick Bergeron's
 * Verification Guild[1].  The original example is a non-synthesizable
 * behavioral model written in Verilog-1995 of a quad Asynchronous
 * Transfer Mode (ATM) user-to-network interface and forwarding node.
 * This example modifies the original code to be synthesizable, using
 * SystemVerilog constructs.  Also, the model has been made to be
 * configurable, so that it can be easily scaled from a 4x4 quad switch
 * to a 16x16 switch, or any other desired configuration.  The example,
 * including a nominal test bench, is partitioned into 8 files,
 * numbered 10.xx.xx_example_10-1.sv through 10-8.sv (where xx
 * represents section and subsection numbers in the book "SystemVerilog
 * for Design" (first edition).  The file 10.00.00_example_top.sv
 * includes all of the other files.  Simulation only needs to be
 * invoked on this one file.  Conditional compilation switches (`ifdef)
 * is used to compile the examples for simulation or for synthesis.
 *
 * [1] The Verification Guild is an independent e-mail newsletter and
 * moderated discussion forum on hardware verification.  Information on
 * the original Verification Guild example can be found at
 * www.janick.bergeron.com/guild/project.html.
 *
 * Used with permission in the book, "SystemVerilog for Design"
 *  By Stuart Sutherland, Simon Davidmann, and Peter Flake.
 *  Book copyright: 2003, Kluwer Academic Publishers, Norwell, MA, USA
 *  www.wkap.il, ISBN: 0-4020-7530-8
 *
 * Revision History:
 *   1.00 15 Dec 2003 -- original code, as included in book
 *   1.01 10 Jul 2004 -- cleaned up comments, added expected results
 *                       to output messages
 *
 * Caveat: Expected results displayed for this code example are based
 * on an interpretation of the SystemVerilog 3.1 standard by the code
 * author or authors.  At the time of writing, official SystemVerilog
 * validation suites were not available to validate the example.
 *
 * RIGHT TO USE: This code example, or any portion thereof, may be
 * used and distributed without restriction, provided that this entire
 * comment block is included with the example.
 *
 * DISCLAIMER: THIS CODE EXAMPLE IS PROVIDED "AS IS" WITHOUT WARRANTY
 * OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED
 * TO WARRANTIES OF MERCHANTABILITY, FITNESS OR CORRECTNESS. IN NO
 * EVENT SHALL THE AUTHOR OR AUTHORS BE LIABLE FOR ANY DAMAGES,
 * INCLUDING INCIDENTAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF THE
 * USE OF THIS CODE.
 *********************************************************************/

`ifndef _INCL_DEFINITIONS
`define _INCL_DEFINITIONS

`define TxPorts 4  // set number of transmit ports
`define RxPorts 4  // set number of receive ports

/*
  Cell Formats
*/
/* UNI Cell Format */
typedef struct packed {
  bit        [3:0]  GFC;
  bit        [7:0]  VPI;
  bit        [15:0] VCI;
  bit               CLP;
  bit        [2:0]  PT;
  bit        [7:0]  HEC;
  bit [0:47] [7:0]  Payload;
} uniType;

/* NNI Cell Format */
typedef struct packed {
  bit        [11:0] VPI;
  bit        [15:0] VCI;
  bit               CLP;
  bit        [2:0]  PT;
  bit        [7:0]  HEC;
  bit [0:47] [7:0]  Payload;
} nniType;

/* Test View Cell Format (Payload Section) */
typedef struct packed {
  bit [0:4]  [7:0] Header;
  bit [0:3]  [7:0] PortID;
  bit [0:3]  [7:0] CellID;
  bit [0:39] [7:0] Padding;
} tstType;

/*
  Union of UNI / NNI / Test View / ByteStream
*/
typedef union packed {
  uniType uni;
  nniType nni;
  tstType tst;
  bit [0:52] [7:0] Mem;
} ATMCellType;

/*
  Cell Rewriting and Forwarding Configuration
*/
typedef struct packed {
  bit [`TxPorts-1:0] FWD;
  bit [11:0] VPI;
} CellCfgType;

`endif // _INCL_DEFINITIONS
