//
// Печать аргументов вызова программы.
//

package main

import "os"

func main() {
	print ("Total ", len(os.Args), " arguments:\n")
	for i, arg := range os.Args {
		print ("  ", i, ": ", arg, "\n")
	}
}
