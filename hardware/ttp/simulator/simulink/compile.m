%
% Compile S-functions for Simulink
%
mex -I.. -DS_FUNCTION_NAME=node0 -output node0 cpu-sfun.c ../node0.c ../node_common.c
mex -I.. -DS_FUNCTION_NAME=node1 -output node1 cpu-sfun.c ../node1.c ../node_common.c
mex -I.. -output ctlr ctlr-sfun.c ../ctlr.c
mex -I.. -output bus bus-sfun.c ../bus.c
