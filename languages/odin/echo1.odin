package main

import "core:fmt"
import "core:os"
import "core:strings"

main :: proc() {
    args := os.args[1:]
    no_newline := false
    i := 0
    for ; i < len(args); i += 1 {
        arg := args[i]
        if !strings.has_prefix(arg, "-") {
            break
        }
        if arg == "--" {
            i += 1
            break
        }
        if arg == "--help" {
            fmt.println("Usage: echo [OPTION]... [STRING]...")
            fmt.println("Echo the STRING(s) to standard output.\n")
            fmt.println("  -n             do not output the trailing newline")
            fmt.println("      --help     display this help and exit")
            fmt.println("      --version  output version information and exit")
            os.exit(0)
        }
        if arg == "--version" {
            fmt.println("echo (Odin implementation) 1.0")
            os.exit(0)
        }
        // Treat as short options
        valid := true
        for r, _ in arg[1:] {
            switch r {
            case 'n':
                no_newline = true
            case:
                valid = false
                break
            }
        }
        if valid {
            // Continue to next arg
        } else {
            break
        }
    }
    // Now print args[i:]
    for j := i; j < len(args); j += 1 {
        if j > i {
            fmt.print(" ")
        }
        fmt.print(args[j])
    }
    if !no_newline {
        fmt.print("\n")
    }
}
