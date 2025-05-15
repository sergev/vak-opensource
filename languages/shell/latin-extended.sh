#!/bin/bash

# Function to convert Unicode code point to UTF-8 bytes
unicode_to_utf8() {
    local code=$1
    local byte1 byte2 byte3
    if ((code >= 0x0100 && code <= 0x07FF)); then
        # 2-byte UTF-8: 110xxxxx 10xxxxxx
        byte1=$((0xC0 | (code >> 6)))
        byte2=$((0x80 | (code & 0x3F)))
        printf "\x$(printf "%x" $byte1)\x$(printf "%x" $byte2)"
    elif ((code >= 0x0800 && code <= 0x02FF)); then
        # 3-byte UTF-8: 1110xxxx 10xxxxxx 10xxxxxx
        byte1=$((0xE0 | (code >> 12)))
        byte2=$((0x80 | ((code >> 6) & 0x3F)))
        byte3=$((0x80 | (code & 0x3F)))
        printf "\x$(printf "%x" $byte1)\x$(printf "%x" $byte2)\x$(printf "%x" $byte3)"
    else
        echo -n ""
    fi
}

# Function to print a block of 128 characters in 8 columns, 16 rows
print_block() {
    local start=$1
    local end=$(($2 - 1))
    local row col code char
    for ((row=0; row<16; row++)); do
        for ((col=0; col<8; col++)); do
            code=$((start + row + col*16))
            if ((code <= end && code <= 0x02FF)); then
                # Get UTF-8 bytes and print character
                char=$(unicode_to_utf8 $code)
                # Evaluate the \xNN sequence to print the actual character
                printf " %04X %s  " "$code" "$char"
            else
                printf "          "
            fi
        done
        echo
    done
    echo
}

# Print Latin Extended-A and part of Latin Extended-B
echo "Latin Extended-A and part of Latin Extended-B (U+0100 to U+01FF)"
echo
print_block 0x0100 0x200

# Print rest of Latin Extended-B
echo "Latin Extended-B (U+0200 to U+024F)"
echo
print_block 0x0200 0x250
