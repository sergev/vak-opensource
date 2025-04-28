// WASI bindings for command-line arguments and output
@external("wasi_snapshot_preview1", "args_sizes_get")
export declare function args_sizes_get(argc: i32, argv_buf_size: i32): i32;

@external("wasi_snapshot_preview1", "args_get")
export declare function args_get(argv: i32, argv_buf: i32): i32;

@external("wasi_snapshot_preview1", "fd_write")
export declare function fd_write(fd: i32, iovs: i32, iovs_len: i32, nwritten: i32): i32;

// Function to convert a null-terminated UTF-8 C-string to AssemblyScript string
function cStringToString(ptr: i32): string
{
    let result = "";
    let offset = 0;
    while (true) {
        let byte = load<u8>(ptr + offset);
        if (byte == 0)
            break;

        // Handle UTF-8 decoding
        if (byte < 0x80) {
            // 1-byte (ASCII)
            result += String.fromCharCode(byte);
            offset += 1;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte
            let byte2 = load<u8>(ptr + offset + 1);
            let code = ((byte & 0x1F) << 6) | (byte2 & 0x3F);
            result += String.fromCharCode(code);
            offset += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte
            let byte2 = load<u8>(ptr + offset + 1);
            let byte3 = load<u8>(ptr + offset + 2);
            let code = ((byte & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
            result += String.fromCharCode(code);
            offset += 3;
        }
        // Ignore invalid UTF-8 for simplicity
    }
    return result;
}

// Function to convert a UTF-16 code unit to UTF-8 bytes
function utf8PutChar(ch: i32, buffer: ArrayBuffer, offset: i32): i32
{
    let ptr = changetype<i32>(buffer);
    if (ch < 0x80) {
        // 1-byte UTF-8 (ASCII)
        store<u8>(ptr + offset, ch);
        return 1;
    }
    if (ch < 0x800) {
        // 2-byte UTF-8
        store<u8>(ptr + offset, (ch >> 6) | 0xC0);
        store<u8>(ptr + offset + 1, (ch & 0x3F) | 0x80);
        return 2;
    }
    // 3-byte UTF-8
    store<u8>(ptr + offset, (ch >> 12) | 0xE0);
    store<u8>(ptr + offset + 1, ((ch >> 6) & 0x3F) | 0x80);
    store<u8>(ptr + offset + 2, (ch & 0x3F) | 0x80);
    return 3;
}

// Function to write a string to stdout
function writeString(str: string): void
{
    // Estimate buffer size (up to 3 bytes per UTF-16 char)
    let maxBytes = str.length * 3;
    let buffer = new ArrayBuffer(maxBytes);
    let offset = 0;

    // Convert each UTF-16 char to UTF-8
    for (let i = 0; i < str.length; i++) {
        let char = str.charCodeAt(i);
        offset += utf8PutChar(char, buffer, offset);
    }

    // Allocate iovec structure (ptr, len)
    let iovec_ptr = changetype<i32>(new ArrayBuffer(8));
    store<i32>(iovec_ptr, changetype<i32>(buffer));
    store<i32>(iovec_ptr + 4, offset);

    // Write to stdout (fd=1)
    let nwritten_ptr = changetype<i32>(new ArrayBuffer(4));
    fd_write(1, iovec_ptr, 1, nwritten_ptr);
}

// Main function
export function _start(): void
{
    // Allocate memory for argument count and buffer size
    let argc_ptr = changetype<i32>(new ArrayBuffer(4));
    let argv_buf_size_ptr = changetype<i32>(new ArrayBuffer(4));

    // Get argument count and buffer size
    args_sizes_get(argc_ptr, argv_buf_size_ptr);
    let argc = load<i32>(argc_ptr);
    let argv_buf_size = load<i32>(argv_buf_size_ptr);

    // Allocate memory for argv pointers and buffer
    let argv = changetype<i32>(new ArrayBuffer(argc * 4));
    let argv_buf = changetype<i32>(new ArrayBuffer(argv_buf_size));

    // Get argument values
    args_get(argv, argv_buf);

    // Skip the first argument (program name)
    let firstArg = 1;

    // If no arguments (besides program name), print newline and exit
    if (argc <= 1) {
        writeString("\n");
        return;
    }

    // Iterate through arguments
    for (let i = firstArg; i < argc; i++) {
        // Get pointer to argument string
        let arg_ptr = load<i32>(argv + i * 4);

        // Convert C-string to AssemblyScript string
        let arg = cStringToString(arg_ptr);
        writeString(arg);

        // Add space between arguments, but not after the last one
        if (i < argc - 1) {
            writeString(" ");
        }
    }

    // Print final newline
    writeString("\n");
}
