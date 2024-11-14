#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "../utilities/utils.h"
#include "../filters/filters.h"

class Convolve {
public:
    Convolve(const float* lpf, const float* hpf, size_t filter_size);

    void dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;

private:
    const float* lpf;
    const float* hpf;
    size_t filter_size;
};

#endif // CONVOLVE_H