#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "../utilities/utils.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

class Convolve {
public:
    Convolve(const float* lpf, const float* hpf, size_t filter_size);

    void convolve_x(const Array3D<float>& input, const float* filter, Array3D<float>& output) const;
    void convolve_y(const Array3D<float>& input, const float* filter, Array3D<float>& output) const;
    void convolve_z(const Array3D<float>& input, const float* filter, Array3D<float>& output) const;

    const float* get_lpf() const { return lpf; }
    const float* get_hpf() const { return hpf; }

private:
    const float* lpf;
    const float* hpf;
    size_t filter_size;
};

#endif // CONVOLVE_H