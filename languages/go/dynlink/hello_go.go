package main

import "stream"
import "unsafe"

var x int = 12345

func Start(console stream.Stream_t) {
	console.Put_String ("Hello, World!\n")

	console.Put_String ("Global variable x at ")
	console.Put_Unsigned (uintptr(unsafe.Pointer(&x)), 8, 16)
	console.Put_String (" = ")
	console.Put_Integer (x, 0, 10)
	console.Put_String ("\n")
	x += x;
}
