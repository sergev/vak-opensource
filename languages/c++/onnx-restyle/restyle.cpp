//
// Copyright (C) 2024 Serge Vakulenko
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include <onnxruntime/onnxruntime_cxx_api.h>
#include <png.h>

#include <iostream>

#include "../argparse.h"

//
// Remove specified extension from a filename, if it's present.
//
std::string remove_extension(const std::string &filename, const std::string &extension)
{
    size_t lastdot = filename.find_last_of('.');
    if (lastdot == std::string::npos) {
        // No extension.
        return filename;
    }
    if (filename.substr(lastdot) != extension) {
        // Different extension.
        return filename;
    }
    // Remove extension.
    return filename.substr(0, lastdot);
}

//
// Convert image from HWC format to CHW format.
//
std::vector<float> hwc_to_chw(const uint8_t input[], unsigned height, unsigned width,
                              unsigned nchan)
{
    auto const stride = height * width;
    std::vector<float> output;

    output.resize(stride * nchan);
    for (unsigned i = 0; i < stride; ++i) {
        for (unsigned c = 0; c < nchan; ++c) {
            output[c * stride + i] = input[i * nchan + c];
        }
    }
    return output;
}

//
// Convert image from CHW format to HWC format.
//
std::vector<uint8_t> chw_to_hwc(const float input[], unsigned height, unsigned width,
                                unsigned nchan)
{
    auto const stride = height * width;
    std::vector<uint8_t> output;

    output.resize(stride * nchan);
    for (unsigned c = 0; c < nchan; ++c) {
        for (unsigned i = 0; i < stride; ++i) {
            float value = input[c * stride + i];

            // Restrict to uint8 data range.
            output[i * nchan + c] = (uint8_t)std::clamp(value, 0.0f, 255.0f);
        }
    }
    return output;
}

//
// Read PNG image of required size.
// Return as CHW layout.
//
void read_image(std::vector<float> &output, const std::string &input_file, unsigned height,
                unsigned width, unsigned nchan)
{
    // Open input image for read.
    png_image image{ .version = PNG_IMAGE_VERSION };
    int ret = png_image_begin_read_from_file(&image, input_file.c_str());
    if (ret != 1)
        throw std::runtime_error(input_file + ": " + image.message);
    std::cout << "Read " << input_file << '\n';

    // Check size of the image.
    image.format               = PNG_FORMAT_BGR;
    unsigned input_data_length = PNG_IMAGE_SIZE(image);
    if (input_data_length != height * width * nchan)
        throw std::runtime_error(input_file + ": Wrong size");

    // Read image data.
    uint8_t bytes_hwc[input_data_length];
    memset(bytes_hwc, 0, input_data_length);
    ret = png_image_finish_read(&image, nullptr, bytes_hwc, 0, nullptr);
    if (ret != 1)
        throw std::runtime_error(input_file + ": " + image.message);
    if (image.width != width)
        throw std::runtime_error(input_file + ": Wrong width");
    if (image.height != height)
        throw std::runtime_error(input_file + ": Wrong height");

    // Convert image from height-width-channel layout to channel-height-width layout.
    output = hwc_to_chw(bytes_hwc, height, width, nchan);
}

//
// Write CHW image to PNG file.
// Convert to HWC layout.
//
void write_image(const float pixels_chw[], const std::string &output_file, unsigned height,
                 unsigned width, unsigned nchan)
{
    // Convert image from channel-height-width layout to height-width-channel layout.
    std::vector<uint8_t> bytes_hwc = chw_to_hwc(pixels_chw, height, width, nchan);

    // Write image to file.
    png_image image{
        .version = PNG_IMAGE_VERSION,
        .format  = PNG_FORMAT_BGR,
        .height  = height,
        .width   = width,
    };
    int ret = png_image_write_to_file(&image, output_file.c_str(), 0, bytes_hwc.data(), 0, nullptr);
    if (ret != 1)
        throw std::runtime_error(output_file + ": " + image.message);
    std::cout << "Write " << output_file << '\n';
}

//
// Re-style one image with given model.
//
void restyle(const std::string &suffix, const std::string &input_file,
             const std::string &model_file)
{
    // Size of input/output image in Fast Neural Style Transfer models.
    constexpr unsigned HEIGHT = 224;
    constexpr unsigned WIDTH  = 224;
    constexpr unsigned NCHAN  = 3;

    // Initialize ONNX runtime.
    Ort::Env env(ORT_LOGGING_LEVEL_ERROR, "onnx");
    Ort::SessionOptions session_options;

    // Open the model.
    Ort::Session session(env, model_file.c_str(), session_options);

    // Check model properties.
    if (session.GetInputCount() != 1)
        throw std::runtime_error(model_file + ": Bad input count");
    if (session.GetOutputCount() != 1)
        throw std::runtime_error(model_file + ": Bad output count");

    // Check input shape.
    auto const input_info  = session.GetInputTypeInfo(0);
    auto const input_type  = input_info.GetTensorTypeAndShapeInfo().GetElementType();
    auto const input_shape = input_info.GetTensorTypeAndShapeInfo().GetShape();
    if (input_type != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT)
        throw std::runtime_error(model_file + ": Bad input type");
    if (input_shape.size() != 4)
        throw std::runtime_error(model_file + ": Bad input shape");
    if (input_shape[0] != 1)
        throw std::runtime_error(model_file + ": Bad input batch dimension");
    if (input_shape[1] != NCHAN)
        throw std::runtime_error(model_file + ": Bad input channel dimension");
    if (input_shape[2] != HEIGHT)
        throw std::runtime_error(model_file + ": Bad input height dimension");
    if (input_shape[3] != WIDTH)
        throw std::runtime_error(model_file + ": Bad input width dimension");

    // Check output shape.
    auto const output_info  = session.GetOutputTypeInfo(0);
    auto const output_type  = output_info.GetTensorTypeAndShapeInfo().GetElementType();
    auto const output_shape = output_info.GetTensorTypeAndShapeInfo().GetShape();
    if (output_type != ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT)
        throw std::runtime_error(model_file + ": Bad output type");
    if (output_shape.size() != 4)
        throw std::runtime_error(model_file + ": Bad output shape");
    if (output_shape[0] != 1)
        throw std::runtime_error(model_file + ": Bad output batch dimension");
    if (output_shape[1] != NCHAN)
        throw std::runtime_error(model_file + ": Bad output channel dimension");
    if (output_shape[2] != HEIGHT)
        throw std::runtime_error(model_file + ": Bad output height dimension");
    if (output_shape[3] != WIDTH)
        throw std::runtime_error(model_file + ": Bad output width dimension");

    // Get names of input/output.
    Ort::AllocatorWithDefaultOptions allocator;
    std::string input_name      = session.GetInputNameAllocated(0, allocator).get();
    std::string output_name     = session.GetOutputNameAllocated(0, allocator).get();
    const char *input_names[1]  = { input_name.c_str() };
    const char *output_names[1] = { output_name.c_str() };

    // Read input image.
    std::vector<float> input_data;
    read_image(input_data, input_file, HEIGHT, WIDTH, NCHAN);

    // Create input tensor object from data values.
    auto memory_info  = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
    auto input_tensor = Ort::Value::CreateTensor<float>(
        memory_info, input_data.data(), input_data.size(), input_shape.data(), input_shape.size());
    if (!input_tensor.IsTensor())
        throw std::runtime_error(model_file + ": Cannot create input tensor");

    // Run inference, get output tensors.
    Ort::RunOptions run_options;
    auto output_tensors = session.Run(run_options, input_names, &input_tensor, 1, output_names, 1);
    if (output_tensors.size() != 1)
        throw std::runtime_error(model_file + ": Too many output tensors");
    if (!output_tensors[0].IsTensor())
        throw std::runtime_error(model_file + ": Bad output tensor");

    // Extract output.
    const float *output_data = output_tensors[0].GetTensorData<float>();
    std::string output_file  = remove_extension(input_file, ".png") + "-" + suffix + ".png";
    write_image(output_data, output_file, HEIGHT, WIDTH, NCHAN);
}

int main(int argc, const char **argv)
{
    // Parse command line arguments.
    argparse::ArgumentParser program("onnx");
    program.add_argument("input").help("Input picture").nargs(1);
    try {
        program.parse_args(argc, argv);
    } catch (const std::exception &err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }
    auto filename = program.get<std::string>("input");

    // Apply five networks to the input image.
    restyle("candy", filename, "candy-9.onnx");
    restyle("mosaic", filename, "mosaic-9.onnx");
    restyle("pointilism", filename, "pointilism-9.onnx");
    restyle("rain-princess", filename, "rain-princess-9.onnx");
    restyle("udnie", filename, "udnie-9.onnx");
}
