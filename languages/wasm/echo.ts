// WASI bindings for command-line arguments and output
@external("wasi_snapshot_preview1", "args_sizes_get")
export declare function args_sizes_get(argc: i32, argv_buf_size: i32): i32;

@external("wasi_snapshot_preview1", "args_get")
export declare function args_get(argv: i32, argv_buf: i32): i32;

@external("wasi_snapshot_preview1", "fd_write")
export declare function fd_write(fd: i32, iovs: i32, iovs_len: i32, nwritten: i32): i32;

// Function to write an ASCII string to stdout
function writeAscii(str: string): void
{
    let buffer = new ArrayBuffer(str.length);
    let ptr = changetype<i32>(buffer);
    let offset = 0;

    for (let i = 0; i < str.length; i++) {
        let ch = str.charCodeAt(i);

        // Copy only ASCII characters.
        if (ch <= 0x7F) {
            store<u8>(ptr + offset, ch);
            offset++;
        }
    }

    // Allocate iovec structure (ptr, len)
    let iovec_ptr = changetype<i32>(new ArrayBuffer(8));
    store<i32>(iovec_ptr, changetype<i32>(buffer));
    store<i32>(iovec_ptr + 4, offset);

    // Write to stdout (fd=1)
    let nwritten_ptr = changetype<i32>(new ArrayBuffer(4));
    fd_write(1, iovec_ptr, 1, nwritten_ptr);
}

// Function to write UTF-8 bytes directly to stdout
function writeUtf8(arg_ptr: i32): void
{
    // Calculate length of null-terminated UTF-8 string
    let length = 0;
    while (load<u8>(arg_ptr + length) != 0) {
        length++;
    }

    // Allocate iovec structure (ptr, len)
    let iovec_ptr = changetype<i32>(new ArrayBuffer(8));
    store<i32>(iovec_ptr, arg_ptr);
    store<i32>(iovec_ptr + 4, length);

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
        writeAscii("\n");
        return;
    }

    // Iterate through arguments
    for (let i = firstArg; i < argc; i++) {
        // Get pointer to argument string
        let arg_ptr = load<i32>(argv + i * 4);

        // Write UTF-8 bytes directly
        writeUtf8(arg_ptr);

        // Add space between arguments, but not after the last one
        if (i < argc - 1) {
            writeAscii(" ");
        }
    }

    // Print final newline
    writeAscii("\n");
}
