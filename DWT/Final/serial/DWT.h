#ifndef DWT_H
#define DWT_H

#include "../utilities/utils.h"
#include "convolve.h"

class DWT {
public:
    DWT(const float* lpf, const float* hpf, size_t filter_size);

    Array3D<float> dwt_3d(const Array3D<float>& data, int levels) const;

private:
    Convolve convolve;
};

#endif // DWT_H