#ifndef DWT_H
#define DWT_H

#include "../utils.h"
#include "../shared/jbutil.h"

struct Wavelet3DResult {
    Custom3DArray<float> LLL;
    Custom3DArray<float> LLH;
    Custom3DArray<float> LHL;
    Custom3DArray<float> LHH;
    Custom3DArray<float> HLL;
    Custom3DArray<float> HLH;
    Custom3DArray<float> HHL;
    Custom3DArray<float> HHH;
};

// Function to perform 1D convolution
jbutil::vector<float> convolve(const jbutil::vector<float>& data, bool is_low_pass); 

// Function to apply convolution along a given dimension
void apply_convolution(const Custom3DArray<float>& input, Custom3DArray<float>& output_L, Custom3DArray<float>& output_H, size_t dim);

// Function to perform multi-level 3D wavelet transform
Wavelet3DResult dwt_3d(const Custom3DArray<float>& data, int levels);

#endif // DWT_H