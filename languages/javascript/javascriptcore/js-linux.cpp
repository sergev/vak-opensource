//
// Compute factorial in JavaScript.
// Compile on Linux with:
// c++ js-linux.cpp -o js-linux `pkg-config --cflags --libs javascriptcoregtk-6.0`
//
#include <stdio.h>
#include <webkitgtk-6.0/jsc/jsc.h>

const char code[] = "\
function factorial(n) {\
    if (n === 0) {\
        return 1;\
    } else {\
        return n * factorial(n - 1);\
    }\
}\
\
factorial(10)";

int main()
{
    // Create a new JavaScript virtual machine.
    JSCContext *vm = jsc_context_new();

    // Evaluate arbitrary JavaScript code.
    JSCValue *result = jsc_context_evaluate(vm, code, -1);

    // Print the result.
    if (jsc_value_is_number(result)) {
        double value = jsc_value_to_double(result);
        printf("The result is: %f\n", value);
    } else {
        printf("The result is not a number.\n");
    }

    // Deallocate resources.
    g_object_unref(result);
    g_object_unref(vm);
}
