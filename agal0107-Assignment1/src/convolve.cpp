#include "convolve.h"

// Constructor for the Convolve class
Convolve::Convolve(const float* lpf, const float* hpf, size_t filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

/* 
 * Convolution along the first dimension (rows)
 * Parameters:
 * - data: 3D array of data to be convolved
 * - depth_limit: number of slices in the depth dimension
 * - row_limit: number of rows in each slice
 * - col_limit: number of columns in each slice
 */

void Convolve::dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    // Create a temporary copy to avoid overwriting the original data
    Array3D<float> temp(data);

    // Iterate over each slice in the depth dimension
    for (size_t d = 0; d < depth_limit; ++d) {
        // Iterate over each column in the current slice
        for (size_t c = 0; c < col_limit; ++c) {
            // Perform convolution for each row
            for (size_t i = 0; i < row_limit / 2; ++i) {
                float sum_low = 0.0f;  // Sum for low-pass filter
                float sum_high = 0.0f; // Sum for high-pass filter

                // Apply the filters
                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = (2 * i + j) % row_limit;
                    float input_val = temp(d, index, c);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;
                }

                // Store the results in the original data array
                data(d, i, c) = sum_low;
                data(d, i + row_limit / 2, c) = sum_high;
            }
        }
    }
}

/* 
 * Convolution along the second dimension (columns)
 * Parameters:
 * - data: 3D array of data to be convolved
 * - depth_limit: number of slices in the depth dimension
 * - row_limit: number of rows in each slice
 * - col_limit: number of columns in each slice
 */

void Convolve::dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    // Create a temporary copy to avoid overwriting the original data
    Array3D<float> temp(data);

    // Iterate over each slice in the depth dimension
    for (size_t d = 0; d < depth_limit; ++d) {
        // Iterate over each row in the current slice
        for (size_t r = 0; r < row_limit; ++r) {
            // Perform convolution for each column
            for (size_t i = 0; i < col_limit / 2; ++i) {
                float sum_low = 0.0f;  // Sum for low-pass filter
                float sum_high = 0.0f; // Sum for high-pass filter

                // Apply the filters
                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = (2 * i + j) % col_limit;
                    float input_val = temp(d, r, index);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;
                }

                // Store the results in the original data array
                data(d, r, i) = sum_low;
                data(d, r, i + col_limit / 2) = sum_high;
            }
        }
    }
}

/* 
 * Convolution along the third dimension (depths)
 * Parameters:
 * - data: 3D array of data to be convolved
 * - depth_limit: number of slices in the depth dimension
 * - row_limit: number of rows in each slice
 * - col_limit: number of columns in each slice
 */

void Convolve::dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    // Create a temporary copy to avoid overwriting the original data
    Array3D<float> temp(data);

    // Iterate over each row in the current slice
    for (size_t r = 0; r < row_limit; ++r) {
        // Iterate over each column in the current row
        for (size_t c = 0; c < col_limit; ++c) {
            // Perform convolution for each depth
            for (size_t i = 0; i < depth_limit / 2; ++i) {
                float sum_low = 0.0f;  // Sum for low-pass filter
                float sum_high = 0.0f; // Sum for high-pass filter

                // Apply the filters
                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = (2 * i + j) % depth_limit;
                    float input_val = temp(index, r, c);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;
                }

                // Store the results in the original data array
                data(i, r, c) = sum_low;
                data(i + depth_limit / 2, r, c) = sum_high;
            }
        }
    }
}