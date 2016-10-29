#!/usr/bin/python
# -*- encoding: utf-8 -*-
#
# Print the sequence of function calls from the Imperas trace file.
#
import sys, string, subprocess, bisect

if len(sys.argv) != 3:
    print "Usage: simtrace file.trace vmunix.elf"
    sys.exit (1)

# Extract the list of symbols from the binary executable.
nm_command = subprocess.Popen ("nm "+sys.argv[2], shell = True, stdout = subprocess.PIPE)
table = {}
max_addr = 0
for line in nm_command.stdout.readlines():
    word = line.split()
    addr = int(word[0], 16)
    func = word[2]
    table[addr] = func
    if addr > max_addr:
        max_addr = addr
    #print "%08x = %s" % (addr, func)

table_keys = sorted(table.keys())
#print table_keys

# Find a name of the function for the given address.
# Return the name and the offset.
def find_function (addr):
    if addr <= max_addr:
        i = bisect.bisect_right(table_keys, addr)
        if i > 0:
            last = table_keys[i-1]
            return (table[last], addr - last)
    return ("", 0)

# Print a function name for the given address.
last_func = ""
def process_instruction(addr, level):
    #print "--- process_instruction(%#x)" % addr
    global last_func

    (func, offset) = find_function (addr)
    if func != last_func:
        if offset == 0:
            print "%08x : %*s%s" % (addr, level*2, "", func)
        else:
            print "%08x : %*s%s + %u" % (addr, level*2, "", func, offset)
        last_func = func

# Check whether the string is a hex number
hex_digits = set(string.hexdigits)
def is_hex(s):
     return all(c in hex_digits for c in s)

# Read the trace file.
trace_file = open (sys.argv[1])
pc = 0
op = ""
last_op = ""
level = 0
for line in trace_file.readlines():
    word = line.split()
    if len(word) > 0 and word[0] == "---":
        if pc > max_addr and len(word) == 6 and word[1] == "I/O" and \
            word[2] == "Read" and word[5] == "U4STA":
            # Skip bootloader timeout
            continue

        # Print i/o events.
        print line.strip()
        continue

    if len(word) > 1 and word[0] == "Info" and word[1] == "(MIPS32_EXCEPT)":
        # Print exceptions.
        print "---", string.join(word[3:])
        continue

    if len(word) < 7:
        continue

    va = word[2]
    pa = word[3]
    cca = word[4]
    if not (word[1] == ":" and
            len(va) == 8 and len(pa) == 8 and
            is_hex(va) and is_hex(pa)):
        continue
    pc = int(va, 16)

    # Skip bootloader region.
    if pc > max_addr:
        continue

    if cca != "2:" and cca != "3:":
        print "Warning: unexpected CCA value!"

    if last_op == "JAL":
        level = level + 1
    elif last_op == "JR":
        level = level - 1

    #print pc, ":", string.join(word[6:])
    process_instruction(pc, level)

    # Keep the history of two last instructions
    last_op = op
    op = word[6]

    if word[6] == "JAL" or word[6] == "JALR":
        op = "JAL"
    elif (word[6] == "JR" or word[6] == "JR.HB") and word[7] == "$31":
        op = "JR"
    else:
        op = ""

# Print the last executed address.
if pc != 0:
    last_func = ""
    print "=== Stopped at: ==="
    process_instruction(pc, 0)
