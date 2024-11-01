#ifndef DWT_H
#define DWT_H

#include "../utilities/utils.h"
#include "convolve.h"

// Function to perform multi-level 3D wavelet transform
Custom3DArray<float> dwt_3d(const Custom3DArray<float>& data, int levels, const float* lpf, const float* hpf, size_t filter_size);

#endif // DWT_H