#include "DWT.h"
#include "convolve.h"

// Function to perform multi-level 3D wavelet transform
Custom3DArray<float> dwt_3d(const Custom3DArray<float>& data, int levels) {
    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    // Allocate a Custom3DArray to store all results
    Custom3DArray<float> result(depth, rows, cols);

    // Apply 1D convolution and subsampling along the first dimension (rows)
    convolution_dim0(data, result);

    // Apply 1D convolution and subsampling along the second dimension (columns)
    convolution_dim1(result, result);

    // Apply 1D convolution and subsampling along the third dimension (depth)
    convolution_dim2(result, result);

    // If more levels are required, recursively apply dwt_3d to the LLL sub-band
    if (levels > 1) {
        Custom3DArray<float> LLL_subband(depth / 2, rows / 2, cols / 2);
        for (size_t d = 0; d < depth / 2; ++d) {
            for (size_t r = 0; r < rows / 2; ++r) {
                for (size_t c = 0; c < cols / 2; ++c) {
                    LLL_subband(d, r, c) = result(d, r, c);
                }
            }
        }
        Custom3DArray<float> multi_level = dwt_3d(LLL_subband, levels - 1);
        // Copy the next level result back into the appropriate section of the result array
        for (size_t d = 0; d < depth / 2; ++d) {
            for (size_t r = 0; r < rows / 2; ++r) {
                for (size_t c = 0; c < cols / 2; ++c) {
                    result(d, r, c) = multi_level(d, r, c);
                }
            }
        }
    }

    return result;
}