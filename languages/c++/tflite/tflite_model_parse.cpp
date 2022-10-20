// Copyright [2018] <Copyright SF-Zhou>
// Author: SF-Zhou
#include <flatbuffers/flatbuffers.h>

#include <fstream>  // C++ header file for file access
#include <iostream> // C++ header file for printing

#include "schema_tflite.h"

template <typename T>
void PrintVector(const flatbuffers::Vector<T> &values)
{
    printf("[");
    for (int i = 0; i < values.Length(); ++i) {
        if (i != 0) {
            printf(", ");
        }
        std::cout << values[i];
    }
    printf("]\n");
}

int main()
{
    std::ifstream infile;
    infile.open("mnist.tflite", std::ios::binary | std::ios::in);
    infile.seekg(0, std::ios::end);
    int length = infile.tellg();
    infile.seekg(0, std::ios::beg);
    std::vector<char> data(length);
    infile.read(data.data(), length);
    infile.close();

    auto &model = *tflite::GetMutableModel(data.data());
    auto &graph_list = *model.mutable_subgraphs();
    auto &graph = *graph_list.GetMutableObject(0);

    auto &inputs = *graph.inputs();
    printf("Inputs Count: %d\n", inputs.Length());
    auto input_index = inputs[0];
    printf("First Input Tensor Index: %d\n", input_index);

    auto &outputs = *graph.outputs();
    printf("Outputs Count: %d\n", outputs.Length());
    auto output_index = outputs[0];
    printf("First Output Tensor Index: %d\n", output_index);

    auto &tensors = *graph.mutable_tensors();
    printf("Tensors Count: %d\n", tensors.Length());

    auto &input_tensor = *tensors.GetMutableObject(input_index);
    printf("Input Tensor Name: %s\n", input_tensor.name()->c_str());
    auto &input_shape = *input_tensor.shape();
    printf("Input Shape: ");
    PrintVector(input_shape);

    auto &output_tensor = *tensors.GetMutableObject(output_index);
    printf("Output Tensor Name: %s\n", output_tensor.name()->c_str());
    auto &output_shape = *output_tensor.shape();
    printf("Output Shape: ");
    PrintVector(output_shape);

    auto &input_quantization = *input_tensor.mutable_quantization();
    auto &scales = *input_quantization.mutable_scale();
    printf("Input Quanzation Scales: ");
    PrintVector(scales);
    scales.Mutate(0, 2.3333);
    printf("Input Quanzation Scales (Mutated): ");
    PrintVector(scales);
    printf("Input Quanzation Zero-Points: ");
    PrintVector(*input_quantization.zero_point());

    auto &output_quantization = *output_tensor.mutable_quantization();
    printf("Output Quanzation Scales: ");
    PrintVector(*output_quantization.scale());
    printf("Output Quanzation Zero-Points: ");
    PrintVector(*output_quantization.zero_point());

    std::ofstream outfile;
    outfile.open("mnist_mutated.tflite", std::ios::binary | std::ios::out);
    outfile.write(data.data(), length);
    outfile.close();
}
