#ifndef CONVOLVE_H
#define CONVOLVE_H

#include <vector>
#include "../utils.h"

// Function to perform 1D convolution
jbutil::vector<float> convolve(const jbutil::vector<float>& data, bool is_low_pass);

// Function to apply 1D convolution along the first dimension (rows)
void convolution_dim0(const Custom3DArray<float>& input, Custom3DArray<float>& output);

// Function to apply 1D convolution along the second dimension (columns)
void convolution_dim1(const Custom3DArray<float>& input, Custom3DArray<float>& output);

// Function to apply 1D convolution along the third dimension (depth)
void convolution_dim2(const Custom3DArray<float>& input, Custom3DArray<float>& output);

#endif // CONVOLVE_H