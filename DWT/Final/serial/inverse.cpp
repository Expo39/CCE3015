#include "inverse.h"
#include <cassert>
#include <iostream>

// Constructor for the Inverse class
Inverse::Inverse(const float* lpf, const float* hpf, size_t filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

// Inverse Convolution along the first dimension (rows)
void Inverse::dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data); // Temporary copy to avoid overwriting

    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t c = 0; c < col_limit; ++c) {
            // Iterate over the first half of the rows
            for (size_t i = 0; i < row_limit / 2; ++i) {
                // Retrieve the low and high frequency components
                float low_val = temp(d, i, c);
                float high_val = temp(d, i + row_limit / 2, c);

                // Apply the filter coefficients dynamically based on filter size
                data(d, 2 * i, c) = 0.0f; // Reset the value
                data(d, 2 * i + 1, c) = 0.0f;

                // Loop over filter coefficients for dynamic sizes
                for (size_t j = 0; j < filter_size; j += 2) {
                    data(d, 2 * i, c) += lpf[j] * low_val + hpf[j] * high_val;  // Apply low-pass filter
                    data(d, 2 * i + 1, c) += lpf[j + 1] * low_val + hpf[j + 1] * high_val;  // Apply high-pass filter
                }
            }
        }
    }
}

// Inverse Convolution along the second dimension (columns)
void Inverse::dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data); // Temporary copy to avoid overwriting

    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t r = 0; r < row_limit; ++r) {
            // Iterate over the first half of the columns
            for (size_t i = 0; i < col_limit / 2; ++i) {
                // Retrieve the low and high frequency components
                float low_val = temp(d, r, i);
                float high_val = temp(d, r, i + col_limit / 2);

                data(d, r, 2 * i) = 0.0f; // Reset the value
                data(d, r, 2 * i + 1) = 0.0f;

                // Loop over filter coefficients for dynamic sizes
                for (size_t j = 0; j < filter_size; j += 2) {
                    data(d, r, 2 * i) += lpf[j] * low_val + hpf[j] * high_val;  // Apply low-pass filter
                    data(d, r, 2 * i + 1) += lpf[j + 1] * low_val + hpf[j + 1] * high_val;  // Apply high-pass filter
                }
            }
        }
    }
}

// Inverse Convolution along the third dimension (depths)
void Inverse::dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data); // Temporary copy to avoid overwriting

    for (size_t r = 0; r < row_limit; ++r) {
        for (size_t c = 0; c < col_limit; ++c) {
            // Iterate over the first half of the depth slices
            for (size_t i = 0; i < depth_limit / 2; ++i) {
                // Retrieve the low and high frequency components
                float low_val = temp(i, r, c);
                float high_val = temp(i + depth_limit / 2, r, c);

                // Apply the filter coefficients dynamically based on filter size
                data(2 * i, r, c) = 0.0f; // Reset the value
                data(2 * i + 1, r, c) = 0.0f;

                // Loop over filter coefficients for dynamic sizes
                for (size_t j = 0; j < filter_size; j += 2) {
                    data(2 * i, r, c) += lpf[filter_size - j - 1] * low_val + hpf[filter_size - j - 1] * high_val;  // Apply low-pass filter
                    data(2 * i + 1, r, c) += lpf[filter_size - j] * low_val + hpf[filter_size - j] * high_val;  // Apply high-pass filter
                }
            }
        }
    }
}

// Perform the inverse 3D wavelet transform
Array3D<float> Inverse::inverse_dwt_3d(const Array3D<float>& data, size_t levels) const {
    Array3D<float> result(data);

    // Start from the smallest subband size and work backwards to full size
    for (size_t level = levels; level > 0; --level) {
        // Calculate the limits for the current level
        size_t depth_limit = result.get_depth() >> (level - 1);
        size_t row_limit = result.get_rows() >> (level - 1);
        size_t col_limit = result.get_cols() >> (level - 1);

        // Apply inverse convolutions to recombine the subbands
        dim2(result, depth_limit, row_limit, col_limit);
        dim1(result, depth_limit, row_limit, col_limit);
        dim0(result, depth_limit, row_limit, col_limit);
    }

    return result;
}
