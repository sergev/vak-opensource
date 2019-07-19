//
// Using Java varargs
//
// Compile and run:
//      $ scalac varargs.scala
//      $ scala varargs
//
object varargs extends App {
    val msg = java.text.MessageFormat.format(
        "At {1,time} on {1,date}, there was {2} on planet {0}.",
        "Hoth", new java.util.Date(), "a disturbance in the Force")
    println("Message=" + msg)
}
