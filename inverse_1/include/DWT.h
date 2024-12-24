#ifndef DWT_H
#define DWT_H

#include "utilities/utils.h"
#include "utilities/jbutil.h"
#include "convolve.h"
#include "io.h"
#include "filters.h"
#include "inverse.h"

#include <string>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

// Class for performing Discrete Wavelet Transform (DWT)
class DWT {
public:
    // Constructor to initialize the DWT with low-pass and high-pass filters
    DWT(const float* lpf, const float* hpf, size_t filter_size);
   
    // Function to perform 3D Discrete Wavelet Transform on the input data
    Array3D<float> dwt_3d(const Array3D<float>& data, int levels) const;

private:
    // Convolution object used for performing convolutions across dimensions
    Convolve convolve;
};

// Function to perform the transform
void perform_transform(const string& binary_filename, const string& output_filename, const string& filter_type, int levels);

#endif // DWT_H