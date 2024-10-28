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

// Function to perform 2D wavelet transform
Wavelet2DResult dwt_2d(const Custom3DArray<float>& data) {
    jbutil::vector<float> low_pass_filter;
    jbutil::vector<float> high_pass_filter;

    // Normalized Low-Pass Haar filter
    low_pass_filter.push_back(1.0f / std::sqrt(2)); 
    low_pass_filter.push_back(1.0f / std::sqrt(2)); 

    // Normalized High-Pass Haar filter
    high_pass_filter.push_back(1.0f / std::sqrt(2)); 
    high_pass_filter.push_back(-1.0f / std::sqrt(2)); 

    size_t rows = data.get_rows();
    size_t cols = data.get_cols();
    size_t depth = data.get_depth();

    // Prepare output arrays for wavelet coefficients
    Custom3DArray<float> LL(depth, rows / 2, cols / 2);
    Custom3DArray<float> LH(depth, rows / 2, cols / 2);
    Custom3DArray<float> HL(depth, rows / 2, cols / 2);
    Custom3DArray<float> HH(depth, rows / 2, cols / 2);

    Custom3DArray<float> L_rows(depth, rows, cols / 2);
    Custom3DArray<float> H_rows(depth, rows, cols / 2);

    // Apply 1D convolution and subsampling along rows
    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            jbutil::vector<float> row_data(cols);
            for (size_t c = 0; c < cols; ++c) {
                row_data[c] = data(d, r, c);
            }
            jbutil::vector<float> L_row = convolve(row_data, low_pass_filter);
            jbutil::vector<float> H_row = convolve(row_data, high_pass_filter);
            for (size_t c = 0; c < static_cast<size_t>(L_row.size()); ++c) {
                L_rows(d, r, c) = L_row[c];
                H_rows(d, r, c) = H_row[c];
            }
        }
    }

    // Apply 1D convolution and subsampling along columns
    for (size_t d = 0; d < depth; ++d) {
        for (size_t c = 0; c < cols / 2; ++c) {
            jbutil::vector<float> col_data(rows);
            for (size_t r = 0; r < rows; ++r) {
                col_data[r] = L_rows(d, r, c);
            }
            jbutil::vector<float> LL_col = convolve(col_data, low_pass_filter);
            jbutil::vector<float> LH_col = convolve(col_data, high_pass_filter);
            for (size_t r = 0; r < static_cast<size_t>(LL_col.size()); ++r) {
                LL(d, r, c) = LL_col[r];
                LH(d, r, c) = LH_col[r];
            }

            for (size_t r = 0; r < rows; ++r) {
                col_data[r] = H_rows(d, r, c);
            }
            jbutil::vector<float> HL_col = convolve(col_data, low_pass_filter);
            jbutil::vector<float> HH_col = convolve(col_data, high_pass_filter);
            for (size_t r = 0; r < static_cast<size_t>(HL_col.size()); ++r) {
                HL(d, r, c) = HL_col[r];
                HH(d, r, c) = HH_col[r];
            }
        }
    }

    // Return the sub-bands directly
    return {LL, LH, HL, HH};
}

// Function to perform 3D wavelet transform
Wavelet3DResult dwt_3d(const Wavelet2DResult& slices) {
    size_t depth = slices.LL.get_depth();
    size_t rows = slices.LL.get_rows();
    size_t cols = slices.LL.get_cols();

    jbutil::vector<float> low_pass_filter;
    jbutil::vector<float> high_pass_filter;

    // Normalized Low-Pass Haar filter
    low_pass_filter.push_back(1.0f / std::sqrt(2)); 
    low_pass_filter.push_back(1.0f / std::sqrt(2)); 

    // Normalized High-Pass Haar filter
    high_pass_filter.push_back(1.0f / std::sqrt(2)); 
    high_pass_filter.push_back(-1.0f / std::sqrt(2)); 

    // Prepare output arrays for wavelet coefficients
    Custom3DArray<float> LLL(depth / 2, rows, cols);
    Custom3DArray<float> LLH(depth / 2, rows, cols);
    Custom3DArray<float> LHL(depth / 2, rows, cols);
    Custom3DArray<float> LHH(depth / 2, rows, cols);
    Custom3DArray<float> HLL(depth / 2, rows, cols);
    Custom3DArray<float> HLH(depth / 2, rows, cols);
    Custom3DArray<float> HHL(depth / 2, rows, cols);
    Custom3DArray<float> HHH(depth / 2, rows, cols);

    // Apply 1D convolution and subsampling along the third dimension for each subband
    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            jbutil::vector<float> LL_col(depth);
            jbutil::vector<float> LH_col(depth);
            jbutil::vector<float> HL_col(depth);
            jbutil::vector<float> HH_col(depth);

            for (size_t d = 0; d < depth; ++d) {
                LL_col[d] = slices.LL(d, r, c);
                LH_col[d] = slices.LH(d, r, c);
                HL_col[d] = slices.HL(d, r, c);
                HH_col[d] = slices.HH(d, r, c);
            }

            jbutil::vector<float> LLL_col = convolve(LL_col, low_pass_filter);
            jbutil::vector<float> LLH_col = convolve(LL_col, high_pass_filter);
            jbutil::vector<float> LHL_col = convolve(LH_col, low_pass_filter);
            jbutil::vector<float> LHH_col = convolve(LH_col, high_pass_filter);
            jbutil::vector<float> HLL_col = convolve(HL_col, low_pass_filter);
            jbutil::vector<float> HLH_col = convolve(HL_col, high_pass_filter);
            jbutil::vector<float> HHL_col = convolve(HH_col, low_pass_filter);
            jbutil::vector<float> HHH_col = convolve(HH_col, high_pass_filter);

            for (size_t d = 0; d < static_cast<size_t>(LLL_col.size()); ++d) {
                LLL(d, r, c) = LLL_col[d];
                LLH(d, r, c) = LLH_col[d];
                LHL(d, r, c) = LHL_col[d];
                LHH(d, r, c) = LHH_col[d];
                HLL(d, r, c) = HLL_col[d];
                HLH(d, r, c) = HLH_col[d];
                HHL(d, r, c) = HHL_col[d];
                HHH(d, r, c) = HHH_col[d];
            }
        }
    }

    return {LLL, LLH, LHL, LHH, HLL, HLH, HHL, HHH};
}