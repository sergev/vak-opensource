#include <onnxruntime/onnxruntime_cxx_api.h>
#include <iostream>
#include <cstdint>
#include "../argparse.h"

//
// Print a shape dimension vector.
//
void print_shape(std::ostream &out, const std::vector<std::int64_t> &shape)
{
    out << shape[0];
    for (unsigned i = 1; i < shape.size(); i++) {
        out << " x " << shape[i];
    }
    out << '\n';
}

void print_model_properties(const Ort::Session &session)
{
    std::cout << "Input Count: " << session.GetInputCount() << '\n';
    std::cout << "Output Count: " << session.GetOutputCount() << '\n';
    std::cout << "Overridable Initializer Count: " << session.GetOverridableInitializerCount()
              << '\n';
    std::cout << "Profiling Start Time: " << session.GetProfilingStartTimeNs() << " nsec\n";
}

void print_model_inputs(const Ort::Session &session)
{
    Ort::AllocatorWithDefaultOptions allocator;

    for (unsigned i = 0; i < session.GetInputCount(); i++) {
        auto const name = session.GetInputNameAllocated(i, allocator);
        std::cout << "Input #" << i << " Name: " << name.get() << '\n';

        auto const shape = session.GetInputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
        std::cout << "Input #" << i << " Shape: ";
        print_shape(std::cout, shape);
    }
}

void print_model_outputs(const Ort::Session &session)
{
    Ort::AllocatorWithDefaultOptions allocator;

    for (unsigned i = 0; i < session.GetOutputCount(); i++) {
        auto const name = session.GetOutputNameAllocated(i, allocator);
        std::cout << "Output #" << i << " Name: " << name.get() << '\n';

        auto const shape = session.GetOutputTypeInfo(i).GetTensorTypeAndShapeInfo().GetShape();
        std::cout << "Output #" << i << " Shape: ";
        print_shape(std::cout, shape);
    }
}

void print_model_metadata(const Ort::ModelMetadata &meta)
{
    Ort::AllocatorWithDefaultOptions allocator;

    std::cout << "Producer Name: " << meta.GetProducerNameAllocated(allocator).get() << '\n';
    std::cout << "Graph Name: " << meta.GetGraphNameAllocated(allocator).get() << '\n';
    std::cout << "Domain: " << meta.GetDomainAllocated(allocator).get() << '\n';
    std::cout << "Description: " << meta.GetDescriptionAllocated(allocator).get() << '\n';
    std::cout << "Graph Description: " << meta.GetGraphDescriptionAllocated(allocator).get() << '\n';
    std::cout << "Version: " << meta.GetVersion() << '\n';
}

//
// Process candy.onnx model.
//
void inspect(const std::string &filename)
{
    // Initialize ONNX runtime.
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "onnx");
    Ort::SessionOptions session_options;

    std::cout << "Model: " << filename << '\n';

    // Open the model.
    Ort::Session session(env, filename.c_str(), session_options);

    // Print model properties.
    print_model_properties(session);
    print_model_inputs(session);
    print_model_outputs(session);
    print_model_metadata(session.GetModelMetadata());
    std::cout << '\n';
}

int main(int argc, const char **argv)
{
    argparse::ArgumentParser program("onnx");
    program.add_argument("files")
        .help("The list of input ONNX files")
        .nargs(argparse::nargs_pattern::at_least_one);

    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }
    auto files = program.get<std::vector<std::string>>("files");

    for (auto &filename : files) {
        inspect(filename);
    }
    std::cout << std::endl;
}
