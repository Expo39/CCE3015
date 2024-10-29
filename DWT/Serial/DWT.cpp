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
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            jbutil::vector<float> col(depth);
            for (size_t d = 0; d < depth; ++d) {
                col[d] = data(d, r, c);
            }
            jbutil::vector<float> L_col = convolve(col, low_pass_filter);
            jbutil::vector<float> H_col = convolve(col, high_pass_filter);
            for (size_t d = 0; d < static_cast<size_t>(L_col.size()); ++d) {
                L(d, r, c) = L_col[d];
                H(d, r, c) = H_col[d];
            }
        }
    }

    // Apply 1D convolution and subsampling along the second dimension
    Custom3DArray<float> LL(depth / 2, rows / 2, cols);
    Custom3DArray<float> LH(depth / 2, rows / 2, cols);
    Custom3DArray<float> HL(depth / 2, rows / 2, cols);
    Custom3DArray<float> HH(depth / 2, rows / 2, cols);
    for (size_t d = 0; d < depth / 2; ++d) {
        for (size_t c = 0; c < cols; ++c) {
            jbutil::vector<float> row(rows);
            for (size_t r = 0; r < rows; ++r) {
                row[r] = L(d, r, c);
            }
            jbutil::vector<float> LL_row = convolve(row, low_pass_filter);
            jbutil::vector<float> LH_row = convolve(row, high_pass_filter);
            for (size_t r = 0; r < static_cast<size_t>(LL_row.size()); ++r) {
                LL(d, r, c) = LL_row[r];
                LH(d, r, c) = LH_row[r];
            }

            for (size_t r = 0; r < rows; ++r) {
                row[r] = H(d, r, c);
            }
            jbutil::vector<float> HL_row = convolve(row, low_pass_filter);
            jbutil::vector<float> HH_row = convolve(row, high_pass_filter);
            for (size_t r = 0; r < static_cast<size_t>(HL_row.size()); ++r) {
                HL(d, r, c) = HL_row[r];
                HH(d, r, c) = HH_row[r];
            }
        }
    }

    // Apply 1D convolution and subsampling along the third dimension
    Custom3DArray<float> LLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHH(depth / 2, rows / 2, cols / 2);
    for (size_t d = 0; d < depth / 2; ++d) {
        for (size_t r = 0; r < rows / 2; ++r) {
            jbutil::vector<float> col(cols);
            for (size_t c = 0; c < cols; ++c) {
                col[c] = LL(d, r, c);
            }
            jbutil::vector<float> LLL_col = convolve(col, low_pass_filter);
            jbutil::vector<float> LLH_col = convolve(col, high_pass_filter);
            for (size_t c = 0; c < static_cast<size_t>(LLL_col.size()); ++c) {
                LLL(d, r, c) = LLL_col[c];
                LLH(d, r, c) = LLH_col[c];
            }

            for (size_t c = 0; c < cols; ++c) {
                col[c] = LH(d, r, c);
            }
            jbutil::vector<float> LHL_col = convolve(col, low_pass_filter);
            jbutil::vector<float> LHH_col = convolve(col, high_pass_filter);
            for (size_t c = 0; c < static_cast<size_t>(LHL_col.size()); ++c) {
                LHL(d, r, c) = LHL_col[c];
                LHH(d, r, c) = LHH_col[c];
            }

            for (size_t c = 0; c < cols; ++c) {
                col[c] = HL(d, r, c);
            }
            jbutil::vector<float> HLL_col = convolve(col, low_pass_filter);
            jbutil::vector<float> HLH_col = convolve(col, high_pass_filter);
            for (size_t c = 0; c < static_cast<size_t>(HLL_col.size()); ++c) {
                HLL(d, r, c) = HLL_col[c];
                HLH(d, r, c) = HLH_col[c];
            }

            for (size_t c = 0; c < cols; ++c) {
                col[c] = HH(d, r, c);
            }
            jbutil::vector<float> HHL_col = convolve(col, low_pass_filter);
            jbutil::vector<float> HHH_col = convolve(col, high_pass_filter);
            for (size_t c = 0; c < static_cast<size_t>(HHL_col.size()); ++c) {
                HHL(d, r, c) = HHL_col[c];
                HHH(d, r, c) = HHH_col[c];
            }
        }
    }

    return {LLL, LLH, LHL, LHH, HLL, HLH, HHL, HHH};
}