#include "inverse.h"
#include <algorithm> // For std::min and std::max

Inverse::Inverse(const float* lpf, const float* hpf, size_t filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

void Inverse::dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data);

    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t c = 0; c < col_limit; ++c) {
            for (size_t i = 0; i < row_limit; ++i) {
                data(d, i, c) = 0.0f; // Initialize the data array to zero
            }
            for (size_t i = 0; i < row_limit / 2; ++i) {
                float low_val = temp(d, i, c);
                float high_val = temp(d, i + row_limit / 2, c);

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = 2 * i + j;
                    if (index < row_limit) {
                        data(d, index, c) += (lpf[j] * low_val) + (hpf[j] * high_val);
                    } else {
                        data(d, index, c) += 0.0f; // Zero padding
                    }
                }
            }
        }
    }
}

void Inverse::dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data);

    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t r = 0; r < row_limit; ++r) {
            for (size_t i = 0; i < col_limit; ++i) {
                data(d, r, i) = 0.0f; // Initialize the data array to zero
            }
            for (size_t i = 0; i < col_limit / 2; ++i) {
                float low_val = temp(d, r, i);
                float high_val = temp(d, r, i + col_limit / 2);

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = 2 * i + j;
                    if (index < col_limit) {
                        data(d, r, index) += (lpf[j] * low_val) + (hpf[j] * high_val);
                    } else {
                        data(d, r, index) += 0.0f; // Zero padding
                    }
                }
            }
        }
    }
}

void Inverse::dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data);

    for (size_t r = 0; r < row_limit; ++r) {
        for (size_t c = 0; c < col_limit; ++c) {
            for (size_t i = 0; i < depth_limit; ++i) {
                data(i, r, c) = 0.0f; // Initialize the data array to zero
            }
            for (size_t i = 0; i < depth_limit / 2; ++i) {
                float low_val = temp(i, r, c);
                float high_val = temp(i + depth_limit / 2, r, c);

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = 2 * i + j;
                    if (index < depth_limit) {
                        data(index, r, c) += (lpf[j] * low_val) + (hpf[j] * high_val);
                    } else {
                        data(index, r, c) += 0.0f; // Zero padding
                    }
                }
            }
        }
    }
}

Array3D<float> Inverse::inverse_dwt_3d(const Array3D<float>& data, int levels) const {
    // Get the initial dimensions of the data
    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    // Create a copy of the input data to store the result
    Array3D<float> result = data;

    // Adjust the dimensions for the number of levels
    vector<size_t> depth_levels(levels);
    vector<size_t> row_levels(levels);
    std::vector<size_t> col_levels(levels);

    depth_levels[0] = depth;
    row_levels[0] = rows;
    col_levels[0] = cols;

    for (int i = 1; i < levels; ++i) {
        depth_levels[i] = (depth_levels[i-1] + 1) / 2;
        row_levels[i] = (row_levels[i-1] + 1) / 2;
        col_levels[i] = (col_levels[i-1] + 1) / 2;
    }

    for (int level = levels - 1; level >= 0; --level) {

        // Perform inverse convolution along each dimension
        dim2(result, depth_levels[level], row_levels[level], col_levels[level]);
        dim1(result, depth_levels[level], row_levels[level], col_levels[level]);
        dim0(result, depth_levels[level], row_levels[level], col_levels[level]);
    }

    // Return the reconstructed data
    return result;
}