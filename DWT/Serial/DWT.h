#ifndef DWT_H
#define DWT_H

#include "../utils.h"
#include "../shared/jbutil.h"

struct Wavelet2DResult {
    Custom3DArray<float> LL;
    Custom3DArray<float> LH;
    Custom3DArray<float> HL;
    Custom3DArray<float> HH;
};

// Function to perform 1D convolution and subsampling
jbutil::vector<float> convolve(const jbutil::vector<float>& data, const jbutil::vector<float>& filter);

// Function to perform 2D wavelet transform
Wavelet2DResult dwt_2d(const Custom3DArray<float>& data);

// Function to perform 3D wavelet transform
Custom3DArray<float> dwt_3d(const Wavelet2DResult& slices);

#endif // DWT_H
