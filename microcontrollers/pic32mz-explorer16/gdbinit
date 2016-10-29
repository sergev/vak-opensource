set remote hardware-breakpoint-limit 6
set remote hardware-watchpoint-limit 2
#set breakpoint auto-hw on                  -- default
#set remote hardware-breakpoint-packet on   -- auto detected
#set breakpoint always-inserted on
#set can-use-hw-watchpoints 1

# ejtagproxy
target remote localhost:2000

# OpenOCD
#target remote localhost:3333
#monitor halt

#shell date --rfc-3339=ns
#dump binary memory z 0x80000000 0x80001000
#shell date --rfc-3339=ns
#restore z binary 0x80000000
#shell date --rfc-3339=ns
