#include "convolve.h"
#include <cassert>
#include <iostream>

// Constructor for the Convolve class
Convolve::Convolve(const float* lpf, const float* hpf, int filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

// Convolution along the first dimension (rows)
void Convolve::dim0(const Array3D<float>& input, Array3D<float>& output, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t c = 0; c < col_limit; ++c) {
            vector<float> L_row(row_limit / 2); 
            vector<float> H_row(row_limit / 2); 

            for (size_t i = 0; i < row_limit / 2; ++i) {
                float sum_low = 0.0f;
                float sum_high = 0.0f;

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = (2 * i + j) % row_limit;
                    float input_val = input(d, index, c);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;
                }

                L_row[i] = sum_low;
                H_row[i] = sum_high;
            }

            for (size_t r = 0; r < row_limit / 2; ++r) {
                output(d, r, c) = L_row[r];
                output(d, r + row_limit / 2, c) = H_row[r];
            }
        }
    }
}

// Convolution along the second dimension (columns)
void Convolve::dim1(const Array3D<float>& input, Array3D<float>& output, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t r = 0; r < row_limit; ++r) {
            vector<float> L_col(col_limit / 2);
            vector<float> H_col(col_limit / 2); 

            for (size_t i = 0; i < col_limit / 2; ++i) {
                float sum_low = 0.0f;
                float sum_high = 0.0f;

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = (2 * i + j) % col_limit;
                    float input_val = input(d, r, index);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;
                }

                L_col[i] = sum_low;
                H_col[i] = sum_high;
            }

            for (size_t c = 0; c < col_limit / 2; ++c) {
                output(d, r, c) = L_col[c];
                output(d, r, c + col_limit / 2) = H_col[c];
            }
        }
    }
}

// Convolution along the third dimension (depths)
void Convolve::dim2(const Array3D<float>& input, Array3D<float>& output, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    for (size_t r = 0; r < row_limit; ++r) {
        for (size_t c = 0; c < col_limit; ++c) {
            vector<float> L_depth(depth_limit / 2); 
            vector<float> H_depth(depth_limit / 2); 

            for (size_t i = 0; i < depth_limit / 2; ++i) {
                float sum_low = 0.0f;
                float sum_high = 0.0f;

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = (2 * i + j) % depth_limit;
                    float input_val = input(index, r, c);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;
                }

                L_depth[i] = sum_low;
                H_depth[i] = sum_high;
            }

            for (size_t d = 0; d < depth_limit / 2; ++d) {
                output(d, r, c) = L_depth[d];
                output(d + depth_limit / 2, r, c) = H_depth[d];
            }
        }
    }
}