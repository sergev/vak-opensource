//
// Print prime numbers less than 100, very inefficiently.
//
// Compile and run:
//      $ scalac primes.scala
//      $ scala primes
//
object primes extends App {
    def isPrime(n: Int) = (2 until n) forall (n % _ != 0)

    for (i <- 1 to 100 if isPrime(i))
        println(i)
}
