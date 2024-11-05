#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "../utilities/utils.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

using namespace jbutil;

class Convolve {
public:
    Convolve(const float* lpf, const float* hpf, int filter_size);

    // convolve.h
    void dim0(const Array3D<float>& input, Array3D<float>& output, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim1(const Array3D<float>& input, Array3D<float>& output, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim2(const Array3D<float>& input, Array3D<float>& output, size_t depth_limit, size_t row_limit, size_t col_limit) const;


private:
    const float* lpf;
    const float* hpf;
    int filter_size;
};

#endif // CONVOLVE_H