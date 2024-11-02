#include "DWT.h"
#include "convolve.h"

DWT::DWT(const float* lpf, const float* hpf, size_t filter_size)
    : convolve(lpf, hpf, filter_size) {}

Array3D<float> DWT::dwt_3d(const Array3D<float>& data, int levels) const {
    if (levels < 1) {
        throw std::invalid_argument("Levels must be greater than or equal to 1");
    }

    Array3D<float> result = data;

    for (int level = 0; level < levels; ++level) {
        // Apply 1D convolution and subsampling along each dimension
        convolve.dim0(result, result);
        convolve.dim1(result, result);
        convolve.dim2(result, result);

        // If it's the last level, return the result directly
        if (level == levels - 1) {
            return result;
        }

        // Calculate new dimensions, rounding up to handle uneven dimensions
        size_t new_depth = (result.get_depth() + 1) / 2;
        size_t new_rows = (result.get_rows() + 1) / 2;
        size_t new_cols = (result.get_cols() + 1) / 2;

        // Ensure new dimensions are even by rounding up if necessary
        new_depth = (new_depth % 2 == 0) ? new_depth : new_depth + 1;
        new_rows = (new_rows % 2 == 0) ? new_rows : new_rows + 1;
        new_cols = (new_cols % 2 == 0) ? new_cols : new_cols + 1;

        // Extract the LLL subband for the next level, halving dimensions
        Array3D<float> LLL_subband(new_depth, new_rows, new_cols);
        for (size_t d = 0; d < new_depth; ++d) {
            for (size_t r = 0; r < new_rows; ++r) {
                for (size_t c = 0; c < new_cols; ++c) {
                    LLL_subband(d, r, c) = result(d, r, c);
                }
            }
        }

        // Update result with the LLL subband for the next iteration
        result = LLL_subband;
    }

    return result;
}