// WASI bindings for command-line arguments and output
@external("wasi_snapshot_preview1", "args_sizes_get")
export declare function args_sizes_get(argc: i32, argv_buf_size: i32): i32;

@external("wasi_snapshot_preview1", "args_get")
export declare function args_get(argv: i32, argv_buf: i32): i32;

@external("wasi_snapshot_preview1", "fd_write")
export declare function fd_write(fd: i32, iovs: i32, iovs_len: i32, nwritten: i32): i32;

// Function to convert a null-terminated C-string (UTF-8) to AssemblyScript string
function cStringToString(ptr: i32): string
{
    let result = "";
    let offset = 0;
    // Read bytes until null terminator (0)
    while (true) {
        let byte = load<u8>(ptr + offset);
        if (byte == 0) break;
        // Assume single-byte UTF-8 (ASCII) for simplicity
        result += String.fromCharCode(byte);
        offset++;
    }
    return result;
}

// Function to convert an AssemblyScript string (UTF-16) to UTF-8 bytes
function stringToUTF8(str: string): ArrayBuffer
{
    // Estimate buffer size (worst case: 1 byte per UTF-16 char for ASCII)
    let buffer = new ArrayBuffer(str.length);
    let ptr = changetype<i32>(buffer);
    let offset = 0;

    // Convert each UTF-16 char to UTF-8 (ASCII only for simplicity)
    for (let i = 0; i < str.length; i++) {
        let char = str.charCodeAt(i);

        // For ASCII (char <= 127), use single byte
        if (char <= 0x7F) {
            store<u8>(ptr + offset, char);
            offset++;
        }
        // Add handling for multi-byte UTF-8 if needed
    }
    // Return only the used portion of the buffer
    return buffer.slice(0, offset);
}

// Function to write a string to stdout
function writeString(str: string): void
{
    // Convert string to UTF-8 bytes
    let utf8Buffer = stringToUTF8(str);
    let utf8Ptr = changetype<i32>(utf8Buffer);
    let utf8Len = utf8Buffer.byteLength;

    // Allocate memory for iovec structure (ptr, len)
    let iovec_ptr = changetype<i32>(new ArrayBuffer(8));
    store<i32>(iovec_ptr, utf8Ptr);
    store<i32>(iovec_ptr + 4, utf8Len);

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
