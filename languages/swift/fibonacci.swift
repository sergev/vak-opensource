//
// Compile with: swiftc -O fibonacci.swift
//
func fibonacci(n: Int) -> Int {
    if n < 2 {
        return n
    } else {
        return fibonacci(n: n - 1) + fibonacci(n: n - 2)
    }
}

func main() {
    if (CommandLine.arguments.count < 2) {
        print("Usage: fibonacci n")
        return;
    }
    let n = Int(CommandLine.arguments[1])!
    print(fibonacci(n: n))
}

main()
