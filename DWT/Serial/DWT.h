#ifndef DWT_H
#define DWT_H

#include "../utils.h"
#include "../shared/jbutil.h"
#include "filters.h"

struct Wavelet2DResult {
    Custom3DArray<float> LL;
    Custom3DArray<float> LH;
    Custom3DArray<float> HL;
    Custom3DArray<float> HH;
};

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

// Function to perform 1D convolution and subsampling
jbutil::vector<float> convolve(const jbutil::vector<float>& data, const jbutil::vector<float>& filter);

void apply_convolution(const Custom3DArray<float>& input, Custom3DArray<float>& output_low, Custom3DArray<float>& output_high, const jbutil::vector<float>& low_pass_filter, const jbutil::vector<float>& high_pass_filter, size_t dim);

// Function to perform 3D wavelet transform
Wavelet3DResult dwt_3d(const Custom3DArray<float>& data, Filters filter);

#endif // DWT_H
