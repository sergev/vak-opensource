package main

import (
	"./arith"
	"fmt"
	"log"
	"net"
	"net/rpc/jsonrpc"
)

func main() {
	// client connecting to RPC service
	// and calling methods

	conn, err := net.Dial("tcp", "localhost:8222")

	if err != nil {
		panic(err)
	}
	defer conn.Close()

	c := jsonrpc.NewClient(conn)

	var reply arith.Reply
	var args *arith.Args
	for i := 0; i < 11; i++ {
		// passing Args to RPC call
		args = &arith.Args{7, i}

		// calling "Arith.Mul" on RPC server
		err = c.Call("Arith.Mul", args, &reply)
		if err != nil {
			log.Fatal("arith error:", err)
		}
		fmt.Printf("Arith: %d * %d = %v\n", args.A, args.B, reply.C)

		// calling "Arith.Add" on RPC server
		err = c.Call("Arith.Add", args, &reply)
		if err != nil {
			log.Fatal("arith error:", err)
		}
		fmt.Printf("Arith: %d + %d = %v\n", args.A, args.B, reply.C)

		// NL
		fmt.Printf("\033[33m%s\033[m\n", "---------------")

	}
}
