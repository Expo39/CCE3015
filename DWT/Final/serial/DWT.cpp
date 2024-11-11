#include "DWT.h"
#include "convolve.h"

// Constructor for the DWT class to be used for convolving the filters
DWT::DWT(const float* lpf, const float* hpf, size_t filter_size)
    : convolve(lpf, hpf, filter_size) {}

// Perform the 3D Discrete Wavelet Transform
Array3D<float> DWT::dwt_3d(const Array3D<float>& data, int levels) const {
    if (levels < 1) {
        throw std::invalid_argument("Levels must be greater than or equal to 1");
    }

    Array3D<float> result = data;

    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    for (int level = 0; level < levels; ++level) {
        // Convolve and subsample ONLY within the bounds of the current level
        convolve.dim0(result, depth, rows, cols);
        convolve.dim1(result, depth, rows, cols);
        convolve.dim2(result, depth, rows, cols);

        // Calculate new bounds for the next level's LLL subband
        depth = (depth + 1) / 2;
        rows = (rows + 1) / 2;
        cols = (cols + 1) / 2;
    }

    return result;
}