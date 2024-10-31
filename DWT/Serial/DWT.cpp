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
void apply_convolution(const Custom3DArray<float>& input, Custom3DArray<float>& output_L, Custom3DArray<float>& output_H, size_t dim) {
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
                    output_L(d, r, c) = L_col[d];
                    output_H(d, r, c) = H_col[d];
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
                    output_L(d, r, c) = L_row[r];
                    output_H(d, r, c) = H_row[r];
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
                    output_L(d, r, c) = L_col[c];
                    output_H(d, r, c) = H_col[c];
                }
            }
        }
    }
}

// Function to perform multi-level 3D wavelet transform
Wavelet3DResult dwt_3d(const Custom3DArray<float>& data, int levels) {
    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    // Apply 1D convolution and subsampling along the first dimension
    Custom3DArray<float> L(depth / 2, rows, cols);
    Custom3DArray<float> H(depth / 2, rows, cols);
    apply_convolution(data, L, H, 0);

    // Apply 1D convolution and subsampling along the second dimension
    Custom3DArray<float> LL(depth / 2, rows / 2, cols);
    Custom3DArray<float> LH(depth / 2, rows / 2, cols);
    Custom3DArray<float> HL(depth / 2, rows / 2, cols);
    Custom3DArray<float> HH(depth / 2, rows / 2, cols);
    apply_convolution(L, LL, LH, 1);
    apply_convolution(H, HL, HH, 1);

    // Apply 1D convolution and subsampling along the third dimension
    Custom3DArray<float> LLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHH(depth / 2, rows / 2, cols / 2);
    apply_convolution(LL, LLL, LLH, 2);
    apply_convolution(LH, LHL, LHH, 2);
    apply_convolution(HL, HLL, HLH, 2);
    apply_convolution(HH, HHL, HHH, 2);

    // If more levels are required, recursively apply dwt_3d to the LLL sub-band
    if (levels > 1) {
        Wavelet3DResult next_level_result = dwt_3d(LLL, levels - 1);
        // Combine the results from the current level and the next level
        return {next_level_result.LLL, next_level_result.LLH, next_level_result.LHL, next_level_result.LHH,
                next_level_result.HLL, next_level_result.HLH, next_level_result.HHL, next_level_result.HHH};
    }

    return {LLL, LLH, LHL, LHH, HLL, HLH, HHL, HHH};
}