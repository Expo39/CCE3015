#ifndef DWT_H
#define DWT_H

#include "../utils.h"
#include "../shared/jbutil.h"

// Function to perform 1D convolution and subsampling
jbutil::vector<float> convolve_and_subsample(const jbutil::vector<float>& data, const jbutil::vector<float>& filter);

// Function to perform 2D wavelet transform
Custom3DArray<float> perform_2d_wavelet_transform(const Custom3DArray<float>& data);

// Function to perform 3D wavelet transform
Custom3DArray<float> perform_3d_wavelet_transform(const Custom3DArray<float>& slices);

#endif // DWT_H
