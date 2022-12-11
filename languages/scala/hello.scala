//
// Run as:
// $ scala hello.scala abra cadabra
//
object Hello {
    def main(args: Array[String]) = {
        // Print string.
        println("Hello!")

        // Pring args.
        for (arg <- args)
            println(arg)
    }
}
