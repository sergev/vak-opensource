/**********************************************************************
 * Definition of an ATM cell
 *
 * Author: Chris Spear
 * Revision: 1.00
 * Last modified: 7/7/2008
 *
 * (c) Copyright 2008, Chris Spear. *** ALL RIGHTS RESERVED ***
 * chris.spear.net
 *
 *  This source file may be used and distributed without restriction
 *  provided that this copyright statement is not removed from the file
 *  and that any derivative work contains this copyright notice.
 *
 * Used with permission in the book, "SystemVerilog for Verification"
 * By Chris Spear
 * Book copyright: 2008, Springer LLC, Norwell, MA, USA, Springer.com
 *********************************************************************/

`ifndef ATM_CELL__SV
`define ATM_CELL__SV

`include "definitions.sv"

virtual class BaseTr;
  static int count;  // Number of instance created
  int id;            // Unique transaction id

  function new();
    id = count++;
  endfunction

  // "pure" methods supported in VCS 2008.03 and later
  pure virtual function bit compare(input BaseTr to);
  pure virtual function BaseTr copy(input BaseTr to=null);
  pure virtual function void display(input string prefix="");
endclass // BaseTr

typedef class NNI_cell;

typedef virtual Utopia vUtopia;
typedef virtual Utopia.TB_Rx vUtopiaRx;
typedef virtual Utopia.TB_Tx vUtopiaTx;

/////////////////////////////////////////////////////////////////////////////
// UNI Cell Format
/////////////////////////////////////////////////////////////////////////////
class UNI_cell extends BaseTr;
   // Physical fields
   rand bit        [3:0]  GFC;
   rand bit        [7:0]  VPI;
   rand bit        [15:0] VCI;
   rand bit               CLP;
   rand bit        [2:0]  PT;
        bit        [7:0]  HEC;
   rand bit [0:47] [7:0]  Payload;

   // Meta-data fields
   static bit [7:0] syndrome[0:255];
   static bit syndrome_not_generated = 1;

   extern function new();
   extern function void post_randomize();
   extern virtual function bit compare(input BaseTr to);
   extern virtual function void display(input string prefix="");
   extern virtual function void copy_data(input UNI_cell copy);
   extern virtual function BaseTr copy(input BaseTr to=null);
   extern virtual function void pack(output ATMCellType to);
   extern virtual function void unpack(input ATMCellType from);
   extern function NNI_cell to_NNI();
   extern function void generate_syndrome();
   extern function bit [7:0] hec (bit [31:0] hdr);
endclass : UNI_cell


//-----------------------------------------------------------------------------
function UNI_cell::new();
   if (syndrome_not_generated)
     generate_syndrome();
endfunction : new


//-----------------------------------------------------------------------------
// Compute the HEC value after all other data has been chosen
function void UNI_cell::post_randomize();
   HEC = hec({GFC, VPI, VCI, CLP, PT});
endfunction : post_randomize



//-----------------------------------------------------------------------------

function bit UNI_cell::compare(input BaseTr to);
   UNI_cell cell;
   $cast(cell, to);
   if (this.GFC != cell.GFC)         return 0;
   if (this.VPI != cell.VPI)         return 0;
   if (this.VCI != cell.VCI)         return 0;
   if (this.CLP != cell.CLP)         return 0;
   if (this.PT != cell.PT)           return 0;
   if (this.HEC != cell.HEC)         return 0;
   if (this.Payload != cell.Payload) return 0;
   return 1;
endfunction : compare


function void UNI_cell::display(input string prefix="");
   ATMCellType p;

   $display("%sUNI id:%0d GFC=%x, VPI=%x, VCI=%x, CLP=%b, PT=%x, HEC=%x, Payload[0]=%x",
	    prefix, id, GFC, VPI, VCI, CLP, PT, HEC, Payload[0]);
   this.pack(p);
   $write("%s", prefix);
//   foreach (p.Mem[i]) $write("%x ", p.Mem[i]); $display;
   for (int i=0; i<p.Mem.size; i+=1) $write("%x ", p.Mem[i]); $display;
   //$write("%sUNI Payload=%x %x %x %x %x %x ...",
   //	  prefix, Payload[0], Payload[1], Payload[2], Payload[3], Payload[4], Payload[5]);
   //foreach(Payload[i]) $write(" %x", Payload[i]);
   $display;
endfunction : display


function void UNI_cell::copy_data(input UNI_cell copy);
   copy.GFC     = this.GFC;
   copy.VPI     = this.VPI;
   copy.VCI     = this.VCI;
   copy.CLP     = this.CLP;
   copy.PT      = this.PT;
   copy.HEC     = this.HEC;
   copy.Payload = this.Payload;
endfunction : copy_data


function BaseTr UNI_cell::copy(input BaseTr to=null);
   UNI_cell dst;
   if (to == null) dst = new();
   else            $cast(dst, to);
   copy_data(dst);
   return dst;
endfunction : copy


function void UNI_cell::pack(output ATMCellType to);
   to.uni.GFC     = this.GFC;
   to.uni.VPI     = this.VPI;
   to.uni.VCI     = this.VCI;
   to.uni.CLP     = this.CLP;
   to.uni.PT      = this.PT;
   to.uni.HEC     = this.HEC;
   to.uni.Payload = this.Payload;
   //$write("Packed: "); foreach (to.Mem[i]) $write("%x ", to.Mem[i]); $display;
endfunction : pack


function void UNI_cell::unpack(input ATMCellType from);
   this.GFC     = from.uni.GFC;
   this.VPI     = from.uni.VPI;
   this.VCI     = from.uni.VCI;
   this.CLP     = from.uni.CLP;
   this.PT      = from.uni.PT;
   this.HEC     = from.uni.HEC;
   this.Payload = from.uni.Payload;
endfunction : unpack


//---------------------------------------------------------------------------
// Generate a NNI cell from an UNI cell - used in scoreboard
function NNI_cell UNI_cell::to_NNI();
   NNI_cell copy;
   copy = new();
   copy.VPI     = this.VPI;   // NNI has wider VPI
   copy.VCI     = this.VCI;
   copy.CLP     = this.CLP;
   copy.PT      = this.PT;
   copy.HEC     = this.HEC;
   copy.Payload = this.Payload;
   return copy;
endfunction : to_NNI


//---------------------------------------------------------------------------
// Generate the syndome array, used to compute HEC
function void UNI_cell::generate_syndrome();
   bit [7:0] sndrm;
   for (int i = 0; i < 256; i = i + 1 ) begin
      sndrm = i;
      repeat (8) begin
         if (sndrm[7] === 1'b1)
           sndrm = (sndrm << 1) ^ 8'h07;
         else
           sndrm = sndrm << 1;
      end
      syndrome[i] = sndrm;
   end
   syndrome_not_generated = 0;
endfunction : generate_syndrome

//---------------------------------------------------------------------------
// Function to compute the HEC value
function bit [7:0] UNI_cell::hec (bit [31:0] hdr);
   hec = 8'h00;
   repeat (4) begin
      hec = syndrome[hec ^ hdr[31:24]];
      hdr = hdr << 8;
   end
   hec = hec ^ 8'h55;
endfunction : hec




/////////////////////////////////////////////////////////////////////////////
// NNI Cell Format
/////////////////////////////////////////////////////////////////////////////
class NNI_cell extends BaseTr;
   // Physical fields
   rand bit        [11:0] VPI;
   rand bit        [15:0] VCI;
   rand bit               CLP;
   rand bit        [2:0]  PT;
        bit        [7:0]  HEC;
   rand bit [0:47] [7:0]  Payload;

   // Meta-data fields
   static bit [7:0] syndrome[0:255];
   static bit syndrome_not_generated = 1;

   extern function new();
   extern function void post_randomize();
   extern virtual function bit compare(input BaseTr to);
   extern virtual function void display(input string prefix="");
   extern virtual function void copy_data(input NNI_cell copy);
   extern virtual function BaseTr copy(input BaseTr to=null);
   extern virtual function void pack(output ATMCellType to);
   extern virtual function void unpack(input ATMCellType from);
   extern function void generate_syndrome();
   extern function bit [7:0] hec (bit [31:0] hdr);
endclass : NNI_cell


function NNI_cell::new();
   if (syndrome_not_generated)
     generate_syndrome();
endfunction : new


//-----------------------------------------------------------------------------
// Compute the HEC value after all other data has been chosen
function void NNI_cell::post_randomize();
   HEC = hec({VPI, VCI, CLP, PT});
endfunction : post_randomize


function bit NNI_cell::compare(input BaseTr to);
   NNI_cell cell;
   $cast(cell, to);
   if (this.VPI != cell.VPI)         return 0;
   if (this.VCI != cell.VCI)         return 0;
   if (this.CLP != cell.CLP)         return 0;
   if (this.PT  != cell.PT)          return 0;
   if (this.HEC != cell.HEC)         return 0;
   if (this.Payload != cell.Payload) return 0;
   return 1;
endfunction : compare


function void NNI_cell::display(input string prefix="");
   ATMCellType p;

   $display("%sNNI id:%0d VPI=%x, VCI=%x, CLP=%b, PT=%x, HEC=%x, Payload[0]=%x",
	    prefix, id, VPI, VCI, CLP, PT, HEC, Payload[0]);
   this.pack(p);
   $write("%s", prefix);
// foreach (p.Mem[i]) $write("%x ", p.Mem[i]); $display;
   for (int i=0; i<p.Mem.size; i+=1) $write("%x ", p.Mem[i]); $display;
   //$write("%sUNI Payload=%x %x %x %x %x %x ...",
   $display;
endfunction : display

function void NNI_cell::copy_data(input NNI_cell copy);
   copy.VPI     = this.VPI;
   copy.VCI     = this.VCI;
   copy.CLP     = this.CLP;
   copy.PT      = this.PT;
   copy.HEC     = this.HEC;
   copy.Payload = this.Payload;
endfunction : copy_data

function BaseTr NNI_cell::copy(input BaseTr to=null);
   NNI_cell dst;
   if (to == null) dst = new();
   else            $cast(dst, to);
   copy_data(dst);
   return dst;
endfunction : copy

function void NNI_cell::pack(output ATMCellType to);
   to.nni.VPI     = this.VPI;
   to.nni.VCI     = this.VCI;
   to.nni.CLP     = this.CLP;
   to.nni.PT      = this.PT;
   to.nni.HEC     = this.HEC;
   to.nni.Payload = this.Payload;
endfunction : pack

function void NNI_cell::unpack(input ATMCellType from);
   this.VPI     = from.nni.VPI;
   this.VCI     = from.nni.VCI;
   this.CLP     = from.nni.CLP;
   this.PT      = from.nni.PT;
   this.HEC     = from.nni.HEC;
   this.Payload = from.nni.Payload;
endfunction : unpack

//---------------------------------------------------------------------------
// Generate the syndome array, used to compute HEC
function void NNI_cell::generate_syndrome();
   bit [7:0] sndrm;
   for (int i = 0; i < 256; i = i + 1 ) begin
      sndrm = i;
      repeat (8) begin
         if (sndrm[7] === 1'b1)
           sndrm = (sndrm << 1) ^ 8'h07;
         else
           sndrm = sndrm << 1;
      end
      syndrome[i] = sndrm;
   end
   syndrome_not_generated = 0;
endfunction : generate_syndrome

//---------------------------------------------------------------------------
// Function to compute the HEC value
function bit [7:0] NNI_cell::hec (bit [31:0] hdr);
   hec = 8'h00;
   repeat (4) begin
      hec = syndrome[hec ^ hdr[31:24]];
      hdr = hdr << 8;
   end
   hec = hec ^ 8'h55;
endfunction : hec


`endif // ATM_CELL__SV
