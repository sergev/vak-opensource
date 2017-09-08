/*
 * Skeleton of a generic Go program, that reads and processes a file.
 *
 * Usage:
 *      skeleton input.txt
 *
 * Copyright (C) 2017 Serge Vakulenko, <serge.vakulenko@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * This file is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 */
package main

import (
    "flag"
    "fmt"
    "log"
    "os"
    "path"
    "bufio"
)

// Help notify the user of warnings and errors.
var notify *log.Logger

func main() {
    //
    // Setup logging routine.
    //
    progname := path.Base(os.Args[0])
    notify = log.New(os.Stderr, progname+": ", 0)

    //
    // Parse the command line.
    //
    flag.Usage = func() {
        fmt.Fprintf(os.Stderr, "Usage:\n")
        fmt.Fprintf(os.Stderr, "  %s [<option>]... <input.txt>\n", progname)
        fmt.Fprintf(os.Stderr, "Options:\n")
        flag.PrintDefaults()
    }
    count := flag.Uint("r", 1, "repeat count")
    flag.Parse()
    switch flag.NArg() {
    case 0:
        flag.Usage()
        return
    case 1:
        break
    default:
        notify.Fatal("Too many input files")
    }

    //
    // Open the input file.
    //
    file, err := os.Open(flag.Arg(0))
    if err != nil {
        notify.Fatal(err)
    }
    defer file.Close()

    //
    // Count lines in file.
    //
    for ; *count>0; *count-- {
        file.Seek(0, 0)
        fileScanner := bufio.NewScanner(file)
        lines := 0
        for fileScanner.Scan() {
            lines++
        }
        fmt.Println("number of lines:", lines)
    }
}
