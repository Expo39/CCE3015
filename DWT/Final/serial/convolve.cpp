#include "convolve.h"
#include <cassert>
#include <iostream>

// Constructor for the Convolve class
Convolve::Convolve(const float* lpf, const float* hpf, size_t filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

// Function to apply mirror padding
size_t Convolve::mirror_index(size_t index, size_t limit) const{
    while (index >= limit) {
        if (index < 2 * limit - 2) {
            index = 2 * limit - index - 2;
        } else {
            index -= 2 * limit - 2;
        }
    }
    return index;
}

// Convolution along the first dimension (rows)
void Convolve::dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data); // Create a temporary copy to avoid overriding the original data

    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t c = 0; c < col_limit; ++c) {
            for (size_t i = 0; i < row_limit / 2; ++i) {
                float sum_low = 0.0f;
                float sum_high = 0.0f;

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = 2 * i + j;  
                    index = mirror_index(index, row_limit);
                    
                    float input_val = temp(d, index, c);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;

                    // // Debug statements
                    // std::cout << "dim0 - d: " << d << ", c: " << c << ", i: " << i << ", j: " << j
                    //           << ", index: " << index << ", input_val: " << input_val
                    //           << ", sum_low: " << sum_low << ", sum_high: " << sum_high << std::endl;
                }

                data(d, i, c) = sum_low;
                data(d, i + row_limit / 2, c) = sum_high;
            }
        }
    }
}

// Convolution along the second dimension (columns)
void Convolve::dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data); // Create a temporary copy to avoid overriding the original data

    for (size_t d = 0; d < depth_limit; ++d) {
        for (size_t r = 0; r < row_limit; ++r) {
            for (size_t i = 0; i < col_limit / 2; ++i) {
                float sum_low = 0.0f;
                float sum_high = 0.0f;

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = 2 * i + j;  
                    index = mirror_index(index, col_limit);  

                    float input_val = temp(d, r, index);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;

                    // // Debug statements
                    // std::cout << "dim1 - d: " << d << ", r: " << r << ", i: " << i << ", j: " << j
                    //           << ", index: " << index << ", input_val: " << input_val
                    //           << ", sum_low: " << sum_low << ", sum_high: " << sum_high << std::endl;
                }

                data(d, r, i) = sum_low;
                data(d, r, i + col_limit / 2) = sum_high;
            }
        }
    }
}

// Convolution along the third dimension (depths)
void Convolve::dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const {
    Array3D<float> temp(data); // Create a temporary copy to avoid overriding the original data

    for (size_t r = 0; r < row_limit; ++r) {
        for (size_t c = 0; c < col_limit; ++c) {
            for (size_t i = 0; i < depth_limit / 2; ++i) {
                float sum_low = 0.0f;
                float sum_high = 0.0f;

                for (size_t j = 0; j < filter_size; ++j) {
                    size_t index = 2 * i + j;  
                    index = mirror_index(index, depth_limit); 

                    float input_val = temp(index, r, c);
                    sum_low += lpf[j] * input_val;
                    sum_high += hpf[j] * input_val;

                    // // Debug statements
                    // std::cout << "dim2 - r: " << r << ", c: " << c << ", i: " << i << ", j: " << j
                    //           << ", index: " << index << ", input_val: " << input_val
                    //           << ", sum_low: " << sum_low << ", sum_high: " << sum_high << std::endl;
                }

                data(i, r, c) = sum_low;
                data(i + depth_limit / 2, r, c) = sum_high;
            }
        }
    }
}