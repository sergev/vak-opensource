package main

import (
    "io"
    "log"
    "net"
    "net/http"
    "net/rpc"
    "net/rpc/jsonrpc"
    "os"
)

type HttpConn struct {
    in  io.Reader
    out io.Writer
}

func (c *HttpConn) Read(p []byte) (n int, err error)  { return c.in.Read(p) }
func (c *HttpConn) Write(d []byte) (n int, err error) { return c.out.Write(d) }
func (c *HttpConn) Close() error                      { return nil }

// RPC Api structure
type Test struct{}

// Greet method arguments
type GreetArgs struct {
    Name string
}

// Grret message accept object with single param Name
func (test *Test) Greet(args *GreetArgs, result *string) error {
    *result = "Hello " + args.Name
    return nil
}

// Start server with Test instance as a service
func startServer(port string) {
    test := new(Test)

    server := rpc.NewServer()
    server.Register(test)

    listener, err := net.Listen("tcp", ":"+port)

    if err != nil {
        log.Fatal("listen error:", err)
    }

    defer listener.Close()
    http.Serve(listener, http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {

        if r.URL.Path == "/test" {
            serverCodec := jsonrpc.NewServerCodec(&HttpConn{in: r.Body, out: w})
            w.Header().Set("Content-type", "application/json")
            w.WriteHeader(200)
            err := server.ServeRequest(serverCodec)
            if err != nil {
                log.Printf("Error while serving JSON request: %v", err)
                http.Error(w, "Error while serving JSON request, details have been logged.", 500)
                return
            }
        }

    }))
}

func main() {
    if len(os.Args) < 2 {
        log.Fatal("port not specified")
    }

    port := os.Args[1]

    startServer(port)
}
