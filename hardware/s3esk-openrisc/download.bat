@echo off
rem Run iMPACT to program the Spartan-3E device on the Starter Kit board.
impact -batch impact.run
del _impact.cmd
