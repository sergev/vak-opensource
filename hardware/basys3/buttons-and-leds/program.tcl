set bitfile "result.bit"

open_hw
connect_hw_server
open_hw_target
set device [lindex [get_hw_devices] 0]
current_hw_device $device
refresh_hw_device -quiet -update_hw_probes false

set_property PROBES.FILE {} $device
set_property PROGRAM.FILE $bitfile $device

program_hw_devices $device
refresh_hw_device -quiet
