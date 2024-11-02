#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "../utilities/utils.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

class Convolve {
public:
    Convolve(const float* lpf, const float* hpf, size_t filter_size);

    jbutil::vector<float> convolve(const jbutil::vector<float>& data, bool is_low_pass) const;

    void dim0(const Array3D<float>& input, Array3D<float>& output) const;
    void dim1(const Array3D<float>& input, Array3D<float>& output) const;
    void dim2(const Array3D<float>& input, Array3D<float>& output) const;

private:
    const float* lpf;
    const float* hpf;
    size_t filter_size;
};

#endif // CONVOLVE_H