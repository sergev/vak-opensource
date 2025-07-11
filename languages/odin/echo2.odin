package main

import "core:flags"
import "core:fmt"
import "core:os"
import "core:strings"

Options :: struct {
    n:        bool              `args:"name=n" usage:"do not output the trailing newline"`,
    version:  bool              `args:"name=version" usage:"output version information and exit"`,
    overflow: [dynamic]string   `args:"hidden"`,
}

main :: proc() {
    opt: Options
    defer delete(opt.overflow)

    flags.parse_or_exit(&opt, os.args, .Unix)

    if opt.version {
        fmt.println("echo (Odin implementation) 1.0")
        return
    }

    sb: strings.Builder
    defer strings.builder_destroy(&sb)

    for s, i in opt.overflow {
        if i > 0 {
            strings.write_byte(&sb, ' ')
        }
        strings.write_string(&sb, s)
    }

    fmt.print(strings.to_string(sb))
    if !opt.n {
        fmt.print("\n")
    }
}
