#ifndef DWT_H
#define DWT_H

#include "../utils.h"
#include "convolve.h"

// Function to perform multi-level 3D wavelet transform
Custom3DArray<float> dwt_3d(const Custom3DArray<float>& data, int levels);

#endif // DWT_H