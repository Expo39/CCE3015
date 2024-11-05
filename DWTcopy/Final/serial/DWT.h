#ifndef DWT_H
#define DWT_H

#include "../utilities/utils.h"
#include "convolve.h"

// LevelData struct
struct LevelData {
    Array3D<float> L, H, LL, LH, HL, HH, LLL, LLH, LHL, LHH, HLL, HLH, HHL, HHH;
};

class DWT {
public:
    DWT(const float* lpf, const float* hpf, size_t filter_size);

    LevelData dwt_3d(const Array3D<float>& data, int levels) const;

private:
    Convolve convolve;
};

#endif // DWT_H