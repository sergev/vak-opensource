package main

import (
    "crypto/tls"
    "crypto/x509"
    "log"
    "net/rpc"
)

func main() {
    cert, err := tls.LoadX509KeyPair("certs/client.crt", "certs/client.key")
    if err != nil {
        log.Fatalf("client: loadkeys: %s", err)
    }
    if len(cert.Certificate) != 2 {
        log.Fatal("client.crt should have 2 concatenated certificates: client + CA")
    }
    ca, err := x509.ParseCertificate(cert.Certificate[1])
    if err != nil {
        log.Fatal(err)
    }
    certPool := x509.NewCertPool()
    certPool.AddCert(ca)
    config := tls.Config{
        Certificates: []tls.Certificate{cert},
        RootCAs:      certPool,
    }
    conn, err := tls.Dial("tcp", "127.0.0.1:8000", &config)
    if err != nil {
        log.Fatalf("client: dial: %s", err)
    }
    defer conn.Close()
    log.Println("client: connected to: ", conn.RemoteAddr())
    rpcClient := rpc.NewClient(conn)
    res := new(Result)
    if err := rpcClient.Call("Foo.Bar", "twenty-three", &res); err != nil {
        log.Fatal("Failed to call RPC", err)
    }
    log.Printf("Returned result is %d", res.Data)
}

type Result struct {
    Data int
}
