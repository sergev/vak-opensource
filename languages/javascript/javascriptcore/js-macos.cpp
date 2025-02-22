//
// Compute factorial in JavaScript.
// Compile on MacOS with:
// c++ js-macos.cpp -o js-macos -framework JavaScriptCore
//
#include <stdio.h>
#include <JavaScriptCore/JavaScriptCore.h>

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
    JSGlobalContextRef vm = JSGlobalContextCreate(NULL);

    // Evaluate arbitrary JavaScript code.
    JSStringRef script = JSStringCreateWithUTF8CString(code);
    JSValueRef result = JSEvaluateScript(vm, script, NULL, NULL, 0, NULL);

    // Print the result.
    if (JSValueIsNumber(vm, result)) {
        double value = JSValueToNumber(vm, result, NULL);
        printf("The result is: %f\n", value);
    } else {
        printf("The result is not a number.\n");
    }

    // Deallocate resources.
    JSStringRelease(script);
    JSGlobalContextRelease(vm);
}
