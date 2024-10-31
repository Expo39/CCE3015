#include "DWT.h"

// Function to perform 1D convolution
jbutil::vector<float> convolve(const jbutil::vector<float>& data, bool is_low_pass) {
    size_t data_size = data.size();
    size_t N = data_size / 2;
    jbutil::vector<float> s_odd(N);
    jbutil::vector<float> s_even(N);

    // Separate odd and even samples
    for (size_t i = 0; i < N; ++i) {
        s_odd[i] = data[2 * i];
        s_even[i] = data[2 * i + 1];
    }

    jbutil::vector<float> result(N);

    // Apply Daubechies-2 coefficients directly
    float a0 = (1 + std::sqrt(3));
    float a1 = (3 + std::sqrt(3));
    float a2 = (3 - std::sqrt(3));
    float a3 = (1 - std::sqrt(3));

    for (size_t i = 0; i < N; ++i) {
        float sum = 0.0f;
        if (is_low_pass) {
            sum = a0 * s_odd[i] + a1 * s_even[i] 
                + a2 * s_odd[(i + 1) % N] + a3 * s_even[(i + 1) % N];
        } else {
            sum = a3 * s_odd[(i + N - 1) % N] - a2 * s_even[(i + N - 1) % N]
                + a1 * s_odd[i] - a0 * s_even[i];
        }
        result[i] = sum / (4 * std::sqrt(2));
    }

    return result;
}

// Function to apply 1D convolution with odd/even separation across a specified dimension
void apply_convolution(const Custom3DArray<float>& input, Custom3DArray<float>& output, size_t dim, size_t offset_depth, size_t offset_rows, size_t offset_cols) {
    size_t depth = input.get_depth();
    size_t rows = input.get_rows();
    size_t cols = input.get_cols();

    if (dim == 0) { // First dimension
        for (size_t r = 0; r < rows; ++r) {
            for (size_t c = 0; c < cols; ++c) {
                jbutil::vector<float> col(depth);
                for (size_t d = 0; d < depth; ++d) {
                    col[d] = input(d, r, c);
                }
                jbutil::vector<float> L_col = convolve(col, true);  
                jbutil::vector<float> H_col = convolve(col, false);
                for (size_t d = 0; d < static_cast<size_t>(L_col.size()); ++d) {
                    output(offset_depth + d, offset_rows + r, offset_cols + c) = L_col[d];
                    output(offset_depth + d + depth / 2, offset_rows + r, offset_cols + c) = H_col[d];
                }
            }
        }
    } else if (dim == 1) { // Second dimension
        for (size_t d = 0; d < depth; ++d) {
            for (size_t c = 0; c < cols; ++c) {
                jbutil::vector<float> row(rows);
                for (size_t r = 0; r < rows; ++r) {
                    row[r] = input(d, r, c);
                }
                jbutil::vector<float> L_row = convolve(row, true);  
                jbutil::vector<float> H_row = convolve(row, false);
                for (size_t r = 0; r < static_cast<size_t>(L_row.size()); ++r) {
                    output(offset_depth + d, offset_rows + r, offset_cols + c) = L_row[r];
                    output(offset_depth + d, offset_rows + r + rows / 2, offset_cols + c) = H_row[r];
                }
            }
        }
    } else if (dim == 2) { // Third dimension
        for (size_t d = 0; d < depth; ++d) {
            for (size_t r = 0; r < rows; ++r) {
                jbutil::vector<float> col(cols);
                for (size_t c = 0; c < cols; ++c) {
                    col[c] = input(d, r, c);
                }
                jbutil::vector<float> L_col = convolve(col, true);  
                jbutil::vector<float> H_col = convolve(col, false); 
                for (size_t c = 0; c < static_cast<size_t>(L_col.size()); ++c) {
                    output(offset_depth + d, offset_rows + r, offset_cols + c) = L_col[c];
                    output(offset_depth + d, offset_rows + r, offset_cols + c + cols / 2) = H_col[c];
                }
            }
        }
    }
}

// Function to perform multi-level 3D wavelet transform
Custom3DArray<float> dwt_3d(const Custom3DArray<float>& data, int levels) {
    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    // Allocate a single Custom3DArray to store all results
    Custom3DArray<float> result(depth, rows, cols);

    // Apply 1D convolution and subsampling along the first dimension
    apply_convolution(data, result, 0, 0, 0, 0);

    // Apply 1D convolution and subsampling along the second dimension
    apply_convolution(result, result, 1, 0, 0, 0);

    // Apply 1D convolution and subsampling along the third dimension
    apply_convolution(result, result, 2, 0, 0, 0);

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
        Custom3DArray<float> next_level_result = dwt_3d(LLL_subband, levels - 1);
        // Copy the next level result back into the appropriate section of the result array
        for (size_t d = 0; d < depth / 2; ++d) {
            for (size_t r = 0; r < rows / 2; ++r) {
                for (size_t c = 0; c < cols / 2; ++c) {
                    result(d, r, c) = next_level_result(d, r, c);
                }
            }
        }
    }

    return result;
}