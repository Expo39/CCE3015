#include "DWT.h"
#include <cmath>

// Function to perform 1D convolution and subsampling
jbutil::vector<float> convolve(const jbutil::vector<float>& data, const jbutil::vector<float>& filter) {
    jbutil::vector<float> result;
    size_t filter_size = filter.size();
    size_t step = 2;  // Subsampling factor

    for (size_t i = 0; i <= data.size() - filter_size; i += step) {
        float sum = 0.0f; 
        for (size_t j = 0; j < filter_size; ++j) {
            sum += data[i + j] * filter[j];
        }
        result.push_back(sum);
    }

    return result;
}

// Function to apply 1D convolution and subsampling along a dimension
void apply_convolution(const Custom3DArray<float>& input, Custom3DArray<float>& output_L, Custom3DArray<float>& output_H, const jbutil::vector<float>& low_pass_filter, const jbutil::vector<float>& high_pass_filter, size_t dim) {
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
                jbutil::vector<float> L_col = convolve(col, low_pass_filter);
                jbutil::vector<float> H_col = convolve(col, high_pass_filter);
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
                jbutil::vector<float> L_row = convolve(row, low_pass_filter);
                jbutil::vector<float> H_row = convolve(row, high_pass_filter);
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
                jbutil::vector<float> L_col = convolve(col, low_pass_filter);
                jbutil::vector<float> H_col = convolve(col, high_pass_filter);
                for (size_t c = 0; c < static_cast<size_t>(L_col.size()); ++c) {
                    output_L(d, r, c) = L_col[c];
                    output_H(d, r, c) = H_col[c];
                }
            }
        }
    }
}

// Function to perform 3D wavelet transform
Wavelet3DResult dwt_3d(const Custom3DArray<float>& data) {
    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    jbutil::vector<float> low_pass_filter;
    jbutil::vector<float> high_pass_filter;

    // Normalized Low-Pass Haar filter
    low_pass_filter.push_back(1.0f / std::sqrt(2)); 
    low_pass_filter.push_back(1.0f / std::sqrt(2)); 

    // Normalized High-Pass Haar filter
    high_pass_filter.push_back(1.0f / std::sqrt(2)); 
    high_pass_filter.push_back(-1.0f / std::sqrt(2)); 


    // Apply 1D convolution and subsampling along the first dimension
    Custom3DArray<float> L(depth / 2, rows, cols);
    Custom3DArray<float> H(depth / 2, rows, cols);
    apply_convolution(data, L, H, low_pass_filter, high_pass_filter, 0);

    // Apply 1D convolution and subsampling along the second dimension
    Custom3DArray<float> LL(depth / 2, rows / 2, cols);
    Custom3DArray<float> LH(depth / 2, rows / 2, cols);
    Custom3DArray<float> HL(depth / 2, rows / 2, cols);
    Custom3DArray<float> HH(depth / 2, rows / 2, cols);
    apply_convolution(L, LL, LH, low_pass_filter, high_pass_filter, 1);
    apply_convolution(H, HL, HH, low_pass_filter, high_pass_filter, 1);

    // Apply 1D convolution and subsampling along the third dimension
    Custom3DArray<float> LLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHH(depth / 2, rows / 2, cols / 2);
    apply_convolution(LL, LLL, LLH, low_pass_filter, high_pass_filter, 2);
    apply_convolution(LH, LHL, LHH, low_pass_filter, high_pass_filter, 2);
    apply_convolution(HL, HLL, HLH, low_pass_filter, high_pass_filter, 2);
    apply_convolution(HH, HHL, HHH, low_pass_filter, high_pass_filter, 2);

    return {LLL, LLH, LHL, LHH, HLL, HLH, HHL, HHH};
}