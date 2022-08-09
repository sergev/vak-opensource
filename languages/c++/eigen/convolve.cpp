//
// Method convolve(const Kernel& kernel, const Dimensions& dims)
//
// Returns a tensor that is the output of the convolution of the input tensor with
// the kernel, along the specified dimensions of the input tensor. The dimension
// size for dimensions of the output tensor which were part of the convolution will
// be reduced by the formula:
//
//     output_dim_size = input_dim_size - kernel_dim_size + 1
//
// (requires: input_dim_size >= kernel_dim_size). The dimension sizes for
// dimensions that were not part of the convolution will remain the same.
// Performance of the convolution can depend on the length of the stride(s) of the
// input tensor dimension(s) along which the convolution is computed (the first
// dimension has the shortest stride for ColMajor, whereas RowMajor's shortest
// stride is for the last dimension).
//
// Compile as:
//      c++ -std=c++14 -O convolve.cpp -o convolve -I/usr/local/Cellar/eigen/3.4.0_1/include/eigen3
//
// For details, see:
//      https://github.com/PX4/eigen/blob/master/unsupported/Eigen/CXX11/src/Tensor/README.md#convolutions
//
#include <iostream>
#include <unsupported/Eigen/CXX11/Tensor>

bool almost_equal(float x, float y)
{
    float maxval = std::max({
        1.0f,
        std::fabsf(x),
        std::fabsf(y),
    });
    float epsilon = maxval * std::numeric_limits<float>::epsilon();

    return std::fabsf(x - y) <= epsilon;
}

int main()
{
    //
    // Compute convolution along the second and third dimension.
    //
    constexpr auto DataLayout = Eigen::RowMajor;
    Eigen::Tensor<float, 4, DataLayout> input(3, 3, 7, 11);
    Eigen::Tensor<float, 2, DataLayout> kernel(2, 2);
    Eigen::Tensor<float, 4, DataLayout> output(3, 2, 6, 11);
    input.setRandom();
    kernel.setRandom();

    // Specify second and third dimension for convolution.
    Eigen::array<ptrdiff_t, 2> dims({1, 2});
    output = input.convolve(kernel, dims);

    // Check result.
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 2; ++j) {
            for (int k = 0; k < 6; ++k) {
                for (int l = 0; l < 11; ++l) {
                    const float result = output(i, j, k, l);
                    const float expected = input(i, j+0, k+0, l) * kernel(0, 0) +
                                           input(i, j+1, k+0, l) * kernel(1, 0) +
                                           input(i, j+0, k+1, l) * kernel(0, 1) +
                                           input(i, j+1, k+1, l) * kernel(1, 1);

                    if (!almost_equal(result, expected)) {
                        std::cout << "Test failed at i,j,k,l = "
                                  << i << ' ' << j << ' ' << k << ' ' << l
                                  << std::endl;
                        return -1;
                    }
                }
            }
        }
    }
    std::cout << "Test passed!" << std::endl;
}
