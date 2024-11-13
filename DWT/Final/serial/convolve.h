#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "../utilities/utils.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

using namespace jbutil;

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

    size_t mirror_index(size_t index, size_t limit) const;
};

#endif // CONVOLVE_H