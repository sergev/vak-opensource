package main

import (
    "crypto/rand"
    "crypto/tls"
    "crypto/x509"
    //"fmt"

    "log"
    "net"
    "net/rpc"
)

func main() {
    if err := rpc.Register(new(Foo)); err != nil {
        log.Fatal("Failed to register RPC method")
    }
    cert, err := tls.LoadX509KeyPair("certs/server.crt", "certs/server.key")
    if err != nil {
        log.Fatalf("server: loadkeys: %s", err)
    }
    if len(cert.Certificate) != 2 {
        log.Fatal("server.crt should have 2 concatenated certificates: server + CA")
    }
    ca, err := x509.ParseCertificate(cert.Certificate[1])
    if err != nil {
        log.Fatal(err)
    }
    certPool := x509.NewCertPool()
    certPool.AddCert(ca)
    config := tls.Config{
        Certificates: []tls.Certificate{cert},
        ClientAuth:   tls.RequireAndVerifyClientCert,
        ClientCAs:    certPool,
    }
    config.Rand = rand.Reader
    service := "127.0.0.1:8000"
    listener, err := tls.Listen("tcp", service, &config)
    if err != nil {
        log.Fatalf("server: listen: %s", err)
    }
    log.Print("server: listening")
    for {
        conn, err := listener.Accept()
        if err != nil {
            log.Printf("server: accept: %s", err)
            break
        }
        log.Printf("server: accepted from %s", conn.RemoteAddr())
        go handleClient(conn)
    }
}

func handleClient(conn net.Conn) {
    defer conn.Close()
    rpc.ServeConn(conn)
    log.Println("server: conn: closed")
}

type Foo bool

type Result struct {
    Data int
}

func (f *Foo) Bar(args *string, res *Result) error {
    res.Data = len(*args)
    log.Printf("Received %q, its length is %d", *args, res.Data)
    //return fmt.Errorf("Whoops, error happened")
    return nil
}
