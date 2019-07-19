fun printAllWithPrefix(vararg messages: String, prefix: String) {
    for (m in messages)
        println(prefix + m)
}

fun main(args: Array<String>) {
    printAllWithPrefix(
        "Hello", "Hallo", "Salut", "Hola", "你好",
        prefix = "Greeting: "
    )
}
