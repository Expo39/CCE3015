#include "DWT.h"
#include <cmath>

// Function to perform 1D convolution and subsampling
jbutil::vector<float> convolve_and_subsample(const jbutil::vector<float>& data, const jbutil::vector<float>& filter) {
    jbutil::vector<float> result;
    size_t filter_size = filter.size();
    size_t step = 2;  // Subsampling factor

    for (size_t i = 0; i <= data.size() - filter_size; i += step) {
        float sum = 0.0f; // Use float directly
        for (size_t j = 0; j < filter_size; ++j) {
            sum += data[i + j] * filter[j];
        }
        result.push_back(sum);
    }

    return result;
}

// Function to perform 2D wavelet transform
Custom3DArray<float> perform_2d_wavelet_transform(const Custom3DArray<float>& data) {
    jbutil::vector<float> low_pass_filter;
    jbutil::vector<float> high_pass_filter;

    low_pass_filter.push_back(static_cast<float>(1 / std::sqrt(2))); // Normalized Haar filter
    low_pass_filter.push_back(static_cast<float>(1 / std::sqrt(2))); // Normalized Haar filter

    high_pass_filter.push_back(static_cast<float>(1 / std::sqrt(2))); // Normalized Haar filter
    high_pass_filter.push_back(static_cast<float>(-1 / std::sqrt(2))); // Normalized Haar filter

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
            jbutil::vector<float> L_row = convolve_and_subsample(row_data, low_pass_filter);
            jbutil::vector<float> H_row = convolve_and_subsample(row_data, high_pass_filter);
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
            jbutil::vector<float> LL_col = convolve_and_subsample(col_data, low_pass_filter);
            jbutil::vector<float> LH_col = convolve_and_subsample(col_data, high_pass_filter);
            for (size_t r = 0; r < static_cast<size_t>(LL_col.size()); ++r) {
                LL(d, r, c) = LL_col[r];
                LH(d, r, c) = LH_col[r];
            }

            for (size_t r = 0; r < rows; ++r) {
                col_data[r] = H_rows(d, r, c);
            }
            jbutil::vector<float> HL_col = convolve_and_subsample(col_data, low_pass_filter);
            jbutil::vector<float> HH_col = convolve_and_subsample(col_data, high_pass_filter);
            for (size_t r = 0; r < static_cast<size_t>(HL_col.size()); ++r) {
                HL(d, r, c) = HL_col[r];
                HH(d, r, c) = HH_col[r];
            }
        }
    }

    // Combine LL, LH, HL, and HH into one 3D array for return
    Custom3DArray<float> wavelet_coeffs(depth, rows, cols); // This will hold all coefficients

    // Assign coefficients to the combined array
    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows / 2; ++r) {
            for (size_t c = 0; c < cols / 2; ++c) {
                wavelet_coeffs(d, r, c) = LL(d, r, c);            // LL coefficients
                wavelet_coeffs(d, r, c + cols / 2) = LH(d, r, c); // LH coefficients
                wavelet_coeffs(d, r + rows / 2, c) = HL(d, r, c); // HL coefficients
                wavelet_coeffs(d, r + rows / 2, c + cols / 2) = HH(d, r, c); // HH coefficients
            }
        }
    }
    return wavelet_coeffs;
}

// Function to perform 3D wavelet transform
Custom3DArray<float> perform_3d_wavelet_transform(const Custom3DArray<float>& slices) {
    size_t depth = slices.get_depth();
    size_t rows = slices.get_rows();
    size_t cols = slices.get_cols();

    // Prepare filters for 1D wavelet transform
    jbutil::vector<float> low_pass_filter;
    jbutil::vector<float> high_pass_filter;

    low_pass_filter.push_back(static_cast<float>(1.0)); // Non-normalized Haar filter
    low_pass_filter.push_back(static_cast<float>(1.0)); // Non-normalized Haar filter

    high_pass_filter.push_back(static_cast<float>(1.0)); // Non-normalized Haar filter
    high_pass_filter.push_back(static_cast<float>(-1.0)); // Non-normalized Haar filter

    // Extract subbands from slices
    Custom3DArray<float> LL_slices(depth, rows / 2, cols / 2);
    Custom3DArray<float> LH_slices(depth, rows / 2, cols / 2);
    Custom3DArray<float> HL_slices(depth, rows / 2, cols / 2);
    Custom3DArray<float> HH_slices(depth, rows / 2, cols / 2);

    for (size_t d = 0; d < depth / 2; ++d) {
        for (size_t r = 0; r < rows / 2; ++r) {
            for (size_t c = 0; c < cols / 2; ++c) {
                LL_slices(d, r, c) = slices(d, r, c);
                LH_slices(d, r, c) = slices(d, r, c + cols / 2);
                HL_slices(d, r, c) = slices(d + depth / 2, r, c); // Corrected indexing
                HH_slices(d, r, c) = slices(d + depth / 2, r, c + cols / 2); // Corrected indexing
            }
        }
    }

    // Apply 1D convolution and subsampling along the third dimension for each subband
    Custom3DArray<float> LLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> LHH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HLH(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHL(depth / 2, rows / 2, cols / 2);
    Custom3DArray<float> HHH(depth / 2, rows / 2, cols / 2);

    for (size_t r = 0; r < rows / 2; ++r) {
        for (size_t c = 0; c < cols / 2; ++c) {
            jbutil::vector<float> LL_col(depth);
            jbutil::vector<float> LH_col(depth);
            jbutil::vector<float> HL_col(depth);
            jbutil::vector<float> HH_col(depth);

            for (size_t d = 0; d < depth; ++d) {
                LL_col[d] = LL_slices(d, r, c);
                LH_col[d] = LH_slices(d, r, c);
                HL_col[d] = HL_slices(d, r, c);
                HH_col[d] = HH_slices(d, r, c);
            }

            jbutil::vector<float> LLL_col = convolve_and_subsample(LL_col, low_pass_filter);
            jbutil::vector<float> LLH_col = convolve_and_subsample(LL_col, high_pass_filter);
            jbutil::vector<float> LHL_col = convolve_and_subsample(LH_col, low_pass_filter);
            jbutil::vector<float> LHH_col = convolve_and_subsample(LH_col, high_pass_filter);
            jbutil::vector<float> HLL_col = convolve_and_subsample(HL_col, low_pass_filter);
            jbutil::vector<float> HLH_col = convolve_and_subsample(HL_col, high_pass_filter);
            jbutil::vector<float> HHL_col = convolve_and_subsample(HH_col, low_pass_filter);
            jbutil::vector<float> HHH_col = convolve_and_subsample(HH_col, high_pass_filter);

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

    // Combine LLL, LLH, LHL, LHH, HLL, HLH, HHL, and HHH into one 3D array for return
    Custom3DArray<float> wavelet_coeffs(depth, rows, cols); // This will hold all coefficients

    // Assign coefficients to the combined array
    for (size_t d = 0; d < depth / 2; ++d) {
        for (size_t r = 0; r < rows / 2; ++r) {
            for (size_t c = 0; c < cols / 2; ++c) {
                wavelet_coeffs(d, r, c) = LLL(d, r, c);            // LLL coefficients
                wavelet_coeffs(d, r, c + cols / 2) = LLH(d, r, c); // LLH coefficients
                wavelet_coeffs(d, r + rows / 2, c) = LHL(d, r, c); // LHL coefficients
                wavelet_coeffs(d, r + rows / 2, c + cols / 2) = LHH(d, r, c); // LHH coefficients
                wavelet_coeffs(d + depth / 2, r, c) = HLL(d, r, c); // HLL coefficients
                wavelet_coeffs(d + depth / 2, r, c + cols / 2) = HLH(d, r, c); // HLH coefficients
                wavelet_coeffs(d + depth / 2, r + rows / 2, c) = HHL(d, r, c); // HHL coefficients
                wavelet_coeffs(d + depth / 2, r + rows / 2, c + cols / 2) = HHH(d, r, c); // HHH coefficients
            }
        }
    }

    return wavelet_coeffs;
}