#ifndef DWT_H
#define DWT_H

#include "../utilities/utils.h"
#include "convolve.h"

// Class for performing Discrete Wavelet Transform (DWT)
class DWT {
public:
    // Constructor to initialize the DWT with low-pass and high-pass filters
    DWT(const float* lpf, const float* hpf, int filter_size);

    // Function to perform 3D Discrete Wavelet Transform on the input data
    Array3D<float> dwt_3d(const Array3D<float>& data, int levels) const;

private:
    // Convolution object used for performing convolutions across dimensions
    Convolve convolve;
};

#endif // DWT_H