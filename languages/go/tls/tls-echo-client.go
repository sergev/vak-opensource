/* TLSEchoClient
 */
package main

import (
    "fmt"
    "os"
    "crypto/tls"
)

func main() {
    if len(os.Args) != 2 {
        fmt.Println("Usage: ", os.Args[0], "host:port")
        os.Exit(1)
    }
    service := os.Args[1]

    conn, err := tls.Dial("tcp", service, nil)
    checkError(err)

    for n := 0; n < 10; n++ {
        fmt.Println("Writing...")
        conn.Write([]byte("Hello " + string(n+48)))

        var buf [512]byte
        n, err := conn.Read(buf[0:])
        checkError(err)

        fmt.Println(string(buf[0:n]))
    }
    os.Exit(0)
}

func checkError(err error) {
    if err != nil {
        fmt.Println("Fatal error ", err.Error())
        os.Exit(1)
    }
}
