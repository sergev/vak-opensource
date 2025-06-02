package main

import (
    "flag"
    "fmt"
    "strings"
)

func main() {
    // Define command-line flags
    var noNewline bool
    var enableEscapes bool
    flag.BoolVar(&noNewline, "n", false, "do not output the trailing newline")
    flag.BoolVar(&enableEscapes, "e", false, "enable interpretation of backslash escapes")
    flag.Parse()

    // Get the arguments after flags
    args := flag.Args()

    // Process the input string
    output := strings.Join(args, " ")

    if enableEscapes {
        output = interpretEscapes(output)
    }

    // Print the output
    fmt.Print(output)

    // Add newline unless -n is specified
    if !noNewline {
        fmt.Println()
    }
}

// interpretEscapes processes backslash escape sequences
func interpretEscapes(input string) string {
    var result strings.Builder
    i := 0
    for i < len(input) {
        if input[i] == '\\' && i+1 < len(input) {
            i++
            switch input[i] {
            case 'a':
                result.WriteByte('\a') // Alert (bell)
            case 'b':
                result.WriteByte('\b') // Backspace
            case 'f':
                result.WriteByte('\f') // Form feed
            case 'n':
                result.WriteByte('\n') // Newline
            case 'r':
                result.WriteByte('\r') // Carriage return
            case 't':
                result.WriteByte('\t') // Tab
            case 'v':
                result.WriteByte('\v') // Vertical tab
            case '\\':
                result.WriteByte('\\') // Backslash
            default:
                // If not a recognized escape, keep the backslash and character
                result.WriteByte('\\')
                result.WriteByte(input[i])
            }
        } else {
            result.WriteByte(input[i])
        }
        i++
    }
    return result.String()
}
