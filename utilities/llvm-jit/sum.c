/*
 * LLVM equivalent of:
 *
 * int sum(int a, int b) {
 *     return a + b;
 * }
 *
 * Full article:
 * https://pauladamsmith.com/blog/2015/01/how-to-get-started-with-llvm-c-api.html
 */
#include <llvm-c/Core.h>
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

typedef int32_t (*funcPtr_t) (int32_t, int32_t);

int main(int argc, char const *argv[])
{
    // The first step is to create a module. A module is a collection of
    // the global variables, functions, external references, and other
    // data in LLVM. Modules are the top-level containers for all things
    // built in LLVM, so we start by creating one.
    // The string "my_module" passed to the module factory function is
    // an identifier of your choosing.

    LLVMModuleRef mod = LLVMModuleCreateWithName("my_module");

    // Next, I create the sum function and add it to the module.
    // A function consists of:
    //  * its type (return type),
    //  * a vector of its parameter types, and
    //  * a set of basic blocks.
    // First, we’ll handle the type and parameter types of the function -
    // its prototype, in C terms - and add it to the module.

    LLVMTypeRef param_types[] = { LLVMInt32Type(), LLVMInt32Type() };
    LLVMTypeRef ret_type = LLVMFunctionType(
        LLVMInt32Type(),    // the function’s return type
        param_types,        // array of parameter types
        2,                  // parameter count
        0);                 // a boolean whether the function accepts a variable number of arguments

    // Add the function type to the module, and give it the name sum.
    // We get a value reference in return, which can be thought of as
    // a concrete location in the code upon which to add the function’s body, which we do below.

    LLVMValueRef sum = LLVMAddFunction(mod, "sum", ret_type);

    // The next step is to add a basic block to the function.
    // Basic blocks are parts of code that only have one entry and exit
    // point - in other words, there is no other way execution can go
    // than by single stepping through a list of instructions.

    LLVMBasicBlockRef entry = LLVMAppendBasicBlock(sum, "entry");

    // Instruction builder is how we add instructions to our function’s one and only basic block.
    // We’re positioning the builder to start writing instructions where we left off
    // with the entry to the basic block.

    LLVMBuilderRef builder = LLVMCreateBuilder();
    LLVMPositionBuilderAtEnd(builder, entry);

    // Now comes the crux of our function, the actual instructions to add
    // the two integers passed in as arguments and return them to the caller.
    // LLVMBuildAdd() takes a reference to the builder, the two integers to add,
    // and a name to give the result.

    LLVMValueRef tmp = LLVMBuildAdd(builder, LLVMGetParam(sum, 0), LLVMGetParam(sum, 1), "tmp");

    // We call LLVMBuildRet() to generate the return statement and arrange
    // for the temporary result of the add instruction to be the value returned.

    LLVMBuildRet(builder, tmp);

    // Let’s verify the module. This will ensure that our module was correctly
    // built and will abort if we missed or mixed up any steps.

    char *error = NULL;
    LLVMVerifyModule(mod, LLVMAbortProcessAction, &error);
    LLVMDisposeMessage(error);

    // Now create an execution engine for our module.

    LLVMExecutionEngineRef engine;
    error = NULL;
    LLVMLinkInMCJIT();
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();
    if (LLVMCreateExecutionEngineForModule(&engine, mod, &error) != 0) {
        fprintf(stderr, "failed to create execution engine\n");
        abort();
    }
    if (error) {
        fprintf(stderr, "error: %s\n", error);
        LLVMDisposeMessage(error);
        exit(EXIT_FAILURE);
    }

    // Get input data from the command line.

    if (argc < 3) {
        fprintf(stderr, "usage: %s x y\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int32_t x = strtoll(argv[1], NULL, 10);
    int32_t y = strtoll(argv[2], NULL, 10);

    // Now for the moment of truth: we can call our (JIT’d) function!

    funcPtr_t funcPtr = (funcPtr_t) LLVMGetPointerToGlobal(engine, sum);
    printf("%d\n", funcPtr(x, y));

    // Write out bitcode to file

    if (LLVMWriteBitcodeToFile(mod, "sum.bc") != 0) {
        fprintf(stderr, "error writing bitcode to file, skipping\n");
    }

    // Deallocate builder and engine.

    LLVMDisposeBuilder(builder);
    LLVMDisposeExecutionEngine(engine);
    return 0;
}
