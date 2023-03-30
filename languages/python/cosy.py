#!/usr/bin/python3
#
# Decode COSY format as supported by *EDIT in MS Dubna on BESM-6.
#
koi7_to_unicode = [
    '\u0000', '\u0001', '\u0002', '\u0003', '\u0004', '\u0005', '\u0006', '\u0007',
    '\u0008', '\u0009', '\u000a', '\u000b', '\u000c', '\u000d', '\u000e', '\u000f',
    '\u0010', '\u0011', '\u0012', '\u0013', '\u0014', '\u0015', '\u0016', '\u0017',
    '\u0018', '\u0019', '\u001a', '\u001b', '\u001c', '\u001d', '\u001e', '\u001f',

    '\u0020', '\u0021', '\u0022', '\u0023', '\u0024', '\u0025', '\u0026', '\u0027',
    '\u0028', '\u0029', '\u002a', '\u002b', '\u002c', '\u002d', '\u002e', '\u002f',
    '\u0030', '\u0031', '\u0032', '\u0033', '\u0034', '\u0035', '\u0036', '\u0037',
    '\u0038', '\u0039', '\u003a', '\u003b', '\u003c', '\u003d', '\u003e', '\u003f',

    '\u0040', '\u0041', '\u0042', '\u0043', '\u0044', '\u0045', '\u0046', '\u0047',
    '\u0048', '\u0049', '\u004a', '\u004b', '\u004c', '\u004d', '\u004e', '\u004f',
    '\u0050', '\u0051', '\u0052', '\u0053', '\u0054', '\u0055', '\u0056', '\u0057',
    '\u0058', '\u0059', '\u005a', '\u005b', '\u005c', '\u005d', '\u005e', '\u005f',

    '\u042e', '\u0410', '\u0411', '\u0426', '\u0414', '\u0415', '\u0424', '\u0413',
    '\u0425', '\u0418', '\u0419', '\u041a', '\u041b', '\u041c', '\u041d', '\u041e',
    '\u041f', '\u042f', '\u0420', '\u0421', '\u0422', '\u0423', '\u0416', '\u0412',
    '\u042c', '\u042b', '\u0417', '\u0428', '\u042d', '\u0429', '\u0427', '\u007f',
]

input = [
    0o1244_7101_2324_2601,
    0o2124_6520_2505_4710,
    0o0242_0040_1002_0012,
    0o1244_2516_2110_0506,
    0o2224_6105_6240_5012,
    0o1245_1105_2024_2040,
    0o2364_6104_6240_5012,
    0o1244_2516_2102_0106,
    0o2224_6105_1014_4412,
]

print("input =", input)

word_ptr = 0
byte_index = 0

def getbyte():
    global input, word_ptr, byte_index

    # Extract byte
    if word_ptr >= len(input):
        return None
    w = input[word_ptr]
    ch = (w >> (40 - byte_index*8)) & 0xff

    # Increment the pointer.
    byte_index += 1
    if byte_index == 6:
        word_ptr += 1
        byte_index = 0

    return ch

print("Decode:")
while True:
    byte = getbyte()
    if not byte:
        break

    if byte == 0o12:
        print(f"  {byte:o} -> newline")

    elif byte <= 0o177:
        u = koi7_to_unicode[byte]
        print(f"  {byte:o} -> {u}")

    elif byte == 0o201:
        print(f"  {byte:o} -> ␣")

    elif byte > 0o201:
        n = byte - 0o200
        print(f"  {byte:o} -> {n} spaces")
    else:
        print(f"  {byte:o}")

    if byte_index == 0:
        print(f"------------")
