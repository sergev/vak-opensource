//
// Compute factorial in JavaScript.
// Compile on Linux or MacOS:
// c++ factorial.cpp -o factorial `pkg-config --cflags --libs mozjs-128`
//
#include <js/CompilationAndEvaluation.h>
#include <js/Conversions.h>
#include <js/Exception.h>
#include <js/Initialization.h>
#include <js/SourceText.h>
#include <jsapi.h>
#include <iostream>

static const char code[] = R"(
function factorial(n) {
    if (n === 0) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

factorial(10)
)";

int main()
{
    // Create a new JavaScript virtual machine.
    if (!JS_Init()) {
        throw std::runtime_error("JS_Init failed");
    }
    JSContext *vm = JS_NewContext(JS::DefaultHeapMaxBytes);
    if (!vm) {
        throw std::runtime_error("JS_NewContext failed");
    }
    if (!JS::InitSelfHostedCode(vm)) {
        throw std::runtime_error("JS::InitSelfHostedCode failed");
    }

    // Create a global object: the top-level 'this' value in a script.
    JS::RealmOptions realm_options;
    static JSClass BoilerplateGlobalClass = { "BoilerplateGlobal", JSCLASS_GLOBAL_FLAGS,
                                              &JS::DefaultGlobalClassOps };
    JS::RootedObject global(vm, JS_NewGlobalObject(vm, &BoilerplateGlobalClass, nullptr,
                                                   JS::FireOnNewGlobalHook, realm_options));
    if (!global) {
        throw std::runtime_error("JS_NewGlobalObject failed");
    }

    {
        // Enter a "realm": environment within one global object.
        JSAutoRealm realm(vm, global);

        // Create buffer with source code.
        JS::SourceText<mozilla::Utf8Unit> source;
        if (!source.init(vm, code, strlen(code), JS::SourceOwnership::Borrowed)) {
            throw std::runtime_error("Cannot initialize JS::SourceText");
        }

        // Evaluate arbitrary JavaScript code.
        JS::CompileOptions options(vm);
        JS::RootedValue rval(vm);
        if (!JS::Evaluate(vm, options, source, &rval)) {
            throw std::runtime_error("JS::Evaluate failed");
        }

        // Print the result.
        if (rval.isNumber()) {
            std::cout << "The result is: "
                      << JS_EncodeStringToASCII(vm, JS::ToString(vm, rval)).get()
                      << '\n';
        } else {
            std::cout << "The result is not a number.\n";
        }
    }

    JS_DestroyContext(vm);
    JS_ShutDown();
}
