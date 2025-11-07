package main

import (
    "fmt"
    "time"
)

//
// Handshake with data channel and data ack
//
type Handshake[Treq any, Tack any] struct {
    Req chan Treq // carries data + request
    Ack chan Tack // carries data + acknowledgment
}

//
// AsyncAdder consumes two input handshakes and produces sum on output handshake
//
func AsyncAdder(a, b, sum *Handshake[int, struct{}]) {
    for {
        // Wait for inputs to arrive
        var x, y int
        haveA, haveB := false, false

        for !(haveA && haveB) {
            select {
            case x = <-a.Req:
                haveA = true
            case y = <-b.Req:
                haveB = true
            }
        }

        // Compute sum
        s := x + y

        // Send result downstream (Req↑)
        sum.Req <- s

        // Wait for output Ack↑
        <-sum.Ack

        // Only now acknowledge inputs (completing 4-phase handshake)
        a.Ack <- struct{}{}
        b.Ack <- struct{}{}
    }
}

func main() {
    // Create handshakes
    A := &Handshake[int, struct{}]{make(chan int), make(chan struct{})}
    B := &Handshake[int, struct{}]{make(chan int), make(chan struct{})}
    S := &Handshake[int, struct{}]{make(chan int), make(chan struct{})}

    // Launch asynchronous adder
    go AsyncAdder(A, B, S)

    // Producer for A
    go func() {
        for _, v := range []int{1, 2, 3} {
            fmt.Println("[SRC-A] sent:", v)
            A.Req <- v
            <-A.Ack // wait for ack before sending next
            time.Sleep(100 * time.Millisecond)
        }
    }()

    // Producer for B
    go func() {
        for _, v := range []int{10, 20, 30} {
            fmt.Println("[SRC-B] sent:", v)
            B.Req <- v
            <-B.Ack
            time.Sleep(150 * time.Millisecond)
        }
    }()

    // Consumer for Sum
    for i := 0; i < 3; i++ {
        v := <-S.Req
        fmt.Println("[SUM] received:", v)
        S.Ack <- struct{}{}
    }
}
