/*
   Package dht implements a simple Distributed
   Hash Table.

   Check out the tests for usage examples.

   RPC functions:
   PING
   STORE
   FIND_NODE
   FIND_VALUE
*/
package dht

import (
    "bufio"
    "crypto/tls"
    "errors"
    "io"
    "net"
    "net/http"
    "net/rpc"
)

// k defines key length in bits
const k = 32

// type of the keys used
type Key uint64

// the RPC server
type DHT int

type Pong struct{}
type Ping struct{}

type KeyVal struct {
    key Key
    val string
}

type Result struct {
    status bool
}

// RPC Ping function
func (t *DHT) Ping(ping *Ping, pong *Pong) error {
    return nil
}

// RPC Store function
func (t *DHT) Store(kv *KeyVal, res *Result) error {
    return nil
}

func (t *DHT) LookupNode() error {
    return nil
}

func (t *DHT) LookupValue() error {
    return nil
}

type Node struct {
    id     Key
    next   *Node
    data   map[Key]string
    finger map[Key]*Node
}

// constructor for a new node
func NewNode(id Key) *Node {
    return &Node{data: map[Key]string{}}
}

// defines the distance between two keys
// using the kademlia xor metric
func Distance(a, b Key) Key {
    return a ^ b
}

// locate the node responsible for a key
func (start *Node) Find(key Key) *Node {
    current := start
    for Distance(current.id, key) > Distance(current.next.id, key) {
        current = current.next
    }
    return current
}

// Find the node responsible for the given
// key and return the value stored there
func (start *Node) Lookup(key Key) string {
    node := start.Find(key)
    return node.data[key]
}

// Find the node responsible for the given
// key and store the value there
func (start *Node) Store(key Key, value string) {
    node := start.Find(key)
    node.data[key] = value
}

// DialHTTPS connects to an HTTPS RPC server at the specified network address
// listening on the default HTTP RPC path using tls.Config config
func DialHTTPS(network, address string, config *tls.Config) (*rpc.Client, error) {
    return DialHTTPSPath(network, address, rpc.DefaultRPCPath, config)
}

// DialHTTPSPath connects to an HTTPS RPC server
// at the specified network address and path using tls.Config config
func DialHTTPSPath(network, address, path string, config *tls.Config) (*rpc.Client, error) {
    var err error
    conn, err := tls.Dial(network, address, config)
    if err != nil {
        return nil, err
    }
    io.WriteString(conn, "CONNECT "+path+" HTTP/1.0\n\n")

    // Require successful HTTP response
    // before switch to RPC protocol.
    resp, err := http.ReadResponse(bufio.NewReader(conn), &http.Request{Method: "CONNECT"})
    if err == nil && resp.StatusCode == http.StatusOK {
        return rpc.NewClient(conn), nil
    }
    if err == nil {
        err = errors.New("unexpected HTTP response: " + resp.Status)
    }
    conn.Close()
    return nil, &net.OpError{
        Op:   "dial-http",
        Net:  network + " " + address,
        Addr: nil,
        Err:  err,
    }
}
