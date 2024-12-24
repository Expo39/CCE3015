#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "utilities/utils.h"
#include "filters.h"

class Convolve {
public:
    Convolve(const float* lpf, const float* hpf, size_t filter_size);

    void dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;


private:
    void convolve(Array3D<float>& data, size_t limit1, size_t limit2, size_t limit3, 
                        size_t stride1, size_t stride2, size_t stride3, int dimension) const;

    const float* lpf;
    const float* hpf;
    size_t filter_size;
};

#endif // CONVOLVE_H