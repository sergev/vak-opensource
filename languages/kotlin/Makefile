KOTLINC = /snap/kotlin/37/bin/kotlinc

all:    hello.jar varargs.jar datetime.jar sort.jar

clean:
	rm -f *.jar

hello.jar: hello.kt
	$(KOTLINC) $< -include-runtime -d $@

hello:  hello.jar
	java -jar hello.jar

varargs.jar: varargs.kt
	$(KOTLINC) $< -include-runtime -d $@

varargs: varargs.jar
	java -jar varargs.jar

datetime.jar: datetime.kt
	$(KOTLINC) $< -include-runtime -d $@

datetime: datetime.jar
	java -jar datetime.jar

sort.jar: sort.kt
	$(KOTLINC) $< -include-runtime -d $@

sort:   sort.jar
	java -jar sort.jar
