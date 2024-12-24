#include "inverse.h"

// Constructor for the Inverse class
Inverse::Inverse(const float* lpf, const float* hpf, size_t filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

/* 
 * Inverse Convolution along the first dimension (rows)
 * Parameters:
 * - data: 3D array of data to be convolved
 * - depth_limit: number of slices in the depth dimension
 * - row_limit: number of rows in each slice
 * - col_limit: number of columns in each slice
 */

void Inverse::dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    // Create a temporary copy to avoid overwriting the original data
    Array3D<float> temp(data);

    // Iterate over each slice in the depth dimension
    for (size_t d = 0; d < depth_limit; ++d) {
        // Iterate over each column in the current slice
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

/* 
 * Inverse Convolution along the second dimension (columns)
 * Parameters:
 * - data: 3D array of data to be convolved
 * - depth_limit: number of slices in the depth dimension
 * - row_limit: number of rows in each slice
 * - col_limit: number of columns in each slice
 */

void Inverse::dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    // Create a temporary copy to avoid overwriting the original data
    Array3D<float> temp(data);

    // Iterate over each slice in the depth dimension
    for (size_t d = 0; d < depth_limit; ++d) {
        // Iterate over each row in the current slice
        for (size_t r = 0; r < row_limit; ++r) {
            // Iterate over the first half of the columns
            for (size_t i = 0; i < col_limit / 2; ++i) {
                // Retrieve the low and high frequency components
                float low_val = temp(d, r, i);
                float high_val = temp(d, r, i + col_limit / 2);

                // Apply the filter coefficients dynamically based on filter size
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

/* 
 * Inverse Convolution along the third dimension (depths)
 * Parameters:
 * - data: 3D array of data to be convolved
 * - depth_limit: number of slices in the depth dimension
 * - row_limit: number of rows in each slice
 * - col_limit: number of columns in each slice
 */

void Inverse::dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    // Create a temporary copy to avoid overwriting the original data
    Array3D<float> temp(data);

    // Iterate over each row in the current slice
    for (size_t r = 0; r < row_limit; ++r) {
        // Iterate over each column in the current row
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

/* 
 * Perform the inverse 3D wavelet transform
 * Parameters:
 * - data: 3D array of data to be transformed
 * - levels: number of levels of decomposition
 * Returns:
 * - 3D array of transformed data
 */

Array3D<float> Inverse::inverse_dwt_3d(const Array3D<float>& data, size_t levels) const {
    // Create a copy of the input data to store the result
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

    // Return the transformed data
    return result;
}