#!/bin/bash

# Script to display extended ASCII characters from octal 0240 to 0377
# on a UTF-8 Linux console, in 6 columns with 16 characters per column

echo "Latin-1 Supplement characters (octal 0240-0377):"

# Function to print a single character with its octal code
print_char() {
    local decimal=$1
    local octal=$(printf "%o" $decimal)

    # Convert the character to proper UTF-8 encoding
    if [ $decimal -lt 192 ]; then
        # Characters 128-191: UTF-8 encoding is 0xC2 followed by the original byte
        char=$(printf "\xC2\x$(printf "%x" $decimal)")
    else
        # Characters 192-255: UTF-8 encoding is 0xC3 followed by (byte-64)
        offset=$(($decimal - 64))
        char=$(printf "\xC3\x$(printf "%x" $offset)")
    fi

    # Print octal code and character
    printf " %04o %s  " $decimal "$char"
}

# Print header row
echo

# Print the table
for row in {0..15}; do
    for col in {2..7}; do
        char_index=$((128 + col*16 + row))
        if [ $char_index -le 255 ]; then
            print_char $char_index
        fi
    done
    echo  # New line after each row
done
