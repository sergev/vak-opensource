// WASI bindings for command-line arguments and output
@external("wasi_snapshot_preview1", "args_sizes_get")
export declare function args_sizes_get(argc: i32, argv_buf_size: i32): i32;

@external("wasi_snapshot_preview1", "args_get")
export declare function args_get(argv: i32, argv_buf: i32): i32;

@external("wasi_snapshot_preview1", "fd_write")
export declare function fd_write(fd: i32, iovs: i32, iovs_len: i32, nwritten: i32): i32;

// Function to convert a null-terminated C-string (ASCII) to AssemblyScript string
function cStringToString(ptr: i32): string
{
    let result = "";
    let offset = 0;
    while (true) {
        let byte = load<u8>(ptr + offset);
        if (byte == 0)
            break;
        result += String.fromCharCode(byte);
        offset++;
    }
    return result;
}

// Function to write an ASCII string to stdout
function writeString(str: string): void
{
    // Create ASCII byte array from UTF-16 string
    let asciiBuffer = new ArrayBuffer(str.length);
    let asciiPtr = changetype<i32>(asciiBuffer);
    for (let i = 0; i < str.length; i++) {
        // Extract ASCII byte (lower 8 bits of UTF-16 char)
        store<u8>(asciiPtr + i, str.charCodeAt(i) & 0xFF);
    }

    // Allocate iovec structure (ptr, len)
    let iovec_ptr = changetype<i32>(new ArrayBuffer(8));
    store<i32>(iovec_ptr, asciiPtr);
    store<i32>(iovec_ptr + 4, str.length);

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
