#ifndef INVERSE_H
#define INVERSE_H

#include "utilities/utils.h"
#include "utilities/jbutil.h"
#include "filters.h"

class Inverse {
public:
    Inverse(const float* lpf, const float* hpf, size_t filter_size);

    void dim0(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim1(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;
    void dim2(Array3D<float>& data, size_t depth_limit, size_t row_limit, size_t col_limit) const;

    Array3D<float> inverse_dwt_3d(const Array3D<float>& data, size_t levels) const;

private:
    const float* lpf;
    const float* hpf;
    size_t filter_size;

    size_t mirror_index(size_t index, size_t limit) const;

};

#endif // INVERSE_H