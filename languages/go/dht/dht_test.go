package dht

import (
    "crypto/rand"
    "crypto/tls"
    "errors"
    "fmt"
    "log"
    "math"
    "net/http"
    "net/rpc"
    "testing"
)

type Args struct {
    A, B int
}

type Quotient struct {
    Quo, Rem int
}

type Arith int

// example rpc function that multiplies A and B
func (t *Arith) Multiply(args *Args, reply *int) error {
    *reply = args.A * args.B
    return nil
}

// example rpc function that divides A by B
func (t *Arith) Divide(args *Args, quo *Quotient) error {
    if args.B == 0 {
        return errors.New("divide by zero")
    }
    quo.Quo = args.A / args.B
    quo.Rem = args.A % args.B
    return nil
}

// tests the storing and lookup of keys
func TestLookup(t *testing.T) {
    first := SetupTestingCluster()
    for i := uint32(0); i < uint32(math.Pow(2, k)-10000010); i += 10000003 {
        first.Store(Key(i), fmt.Sprintf("TEST:%d", i))
    }

    for i := uint32(0); i < uint32(math.Pow(2, k)-10000010); i += 10000003 {
        val := first.Lookup(Key(i))
        expected := fmt.Sprintf("TEST:%d", i)
        if val != expected {
            t.Errorf("dht[%v] = %v, want %v", i, val, expected)
        }
    }
}

// sets up a test cluster to run tests on
func SetupTestingCluster() *Node {
    first := &Node{id: Key(math.Pow(2, k) - 1), data: map[Key]string{}}
    prev := first
    for i := Key(math.Pow(2, k) - 1); i > 100000000; i -= 100000000 {
        new_prev := NewNode(i)
        new_prev.next = prev
        prev = new_prev
    }
    first.next = prev
    return first
}

// example rpc client
func RPCTestClient(t *testing.T) {
    cert, err := tls.LoadX509KeyPair("certs/client.pem", "certs/client.key")
    if err != nil {
        t.Errorf("RPCTestClient: loadkeys: %v", err)
    }
    config := tls.Config{Certificates: []tls.Certificate{cert}, InsecureSkipVerify: true}

    client, err := DialHTTPS("tcp", "localhost:1234", &config)

    if err != nil {
        t.Errorf("RPCTestClient: DialHTTPS: %v", err)
    }

    args := &Args{7, 8}
    var reply int
    err = client.Call("Arith.Multiply", args, &reply)
    if err != nil {
        t.Errorf("RPCTestClient: Arith.Multiply: %v", err)
    }
    expected := args.A * args.B
    if reply != expected {
        t.Errorf("RPCTestClient: Arith.Multiply: %v*%v = %v, want %v", args.A, args.B, reply, expected)
    }
}

func TestRPCFunctions(t *testing.T) {
    // start server startup code
    arith := new(Arith)
    rpc.Register(arith)
    rpc.HandleHTTP()

    // load TLS certificate
    cert, err := tls.LoadX509KeyPair("certs/server.pem", "certs/server.key")
    if err != nil {
        log.Fatal("server: loadkeys: ", err)
    }

    config := tls.Config{Certificates: []tls.Certificate{cert}, InsecureSkipVerify: true}
    config.Rand = rand.Reader

    l, e := tls.Listen("tcp", ":1234", &config)
    if e != nil {
        log.Fatal("listen error:", e)
    }
    go http.Serve(l, nil)

    RPCTestClient(t)
}
