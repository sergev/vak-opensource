import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

fun main(args: Array<String>) {

    val current = LocalDateTime.now()

    val formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss.SSS")
    val formatted = current.format(formatter)

    println("Current Date and Time is: $formatted")
}
