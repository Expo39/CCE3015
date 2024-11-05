#include "convolve.h"
#include <cassert>
#include <iostream>

Convolve::Convolve(const float* lpf, const float* hpf, size_t filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

// Function to apply 1D convolution along the first dimension (rows)
void Convolve::convolve_x(const Array3D<float>& input, const float* filter, Array3D<float>& output) const {
    size_t depth = input.get_depth();
    size_t rows = input.get_rows();
    size_t cols = input.get_cols();

    size_t new_cols = cols / 2;

    output.resize(depth, rows, new_cols);

    for (size_t d = 0; d < depth; d++) {
        for (size_t r = 0; r < rows; r++) {
            for (size_t c = 0; c < new_cols; c++) {
                float sum = 0.0f;
                for (size_t j = 0; j < filter_size; j++) {
                    size_t index = 2 * c + j;
                    if (index < cols) {
                        sum += input(d, r, index) * filter[j];
                    }
                }
                output(d, r, c) = sum;
            }
        }
    }
}

// Function to apply 1D convolution along the second dimension (columns)
void Convolve::convolve_y(const Array3D<float>& input, const float* filter, Array3D<float>& output) const {
    size_t depth = input.get_depth();
    size_t rows = input.get_rows();
    size_t cols = input.get_cols();

    size_t new_rows = rows / 2;

    output.resize(depth, new_rows, cols);

    for (size_t d = 0; d < depth; d++) {
        for (size_t r = 0; r < new_rows; r++) {
            for (size_t c = 0; c < cols; c++) {
                float sum = 0.0f;
                for (size_t j = 0; j < filter_size; j++) {
                    size_t index = 2 * r + j;
                    if (index < rows) {
                        sum += input(d, index, c) * filter[j];
                    }
                }
                output(d, r, c) = sum;
            }
        }
    }
}

// Function to apply 1D convolution along the third dimension (depth)
void Convolve::convolve_z(const Array3D<float>& input, const float* filter, Array3D<float>& output) const {
    size_t depth = input.get_depth();
    size_t rows = input.get_rows();
    size_t cols = input.get_cols();

    size_t new_depth = depth / 2;

    output.resize(new_depth, rows, cols);

    for (size_t d = 0; d < new_depth; d++) {
        for (size_t r = 0; r < rows; r++) {
            for (size_t c = 0; c < cols; c++) {
                float sum = 0.0f;
                for (size_t j = 0; j < filter_size; j++) {
                    size_t index = 2 * d + j;
                    if (index < depth) {
                        sum += input(index, r, c) * filter[j];
                    }
                }
                output(d, r, c) = sum;
            }
        }
    }
}