#ifndef CONVOLVE_H
#define CONVOLVE_H

#include "../utilities/utils.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

jbutil::vector<float> convolve(const jbutil::vector<float>& data, const float* lpf, const float* hpf, size_t filter_size, bool is_low_pass);

void convolution_dim0(const Custom3DArray<float>& input, Custom3DArray<float>& output, const float* lpf, const float* hpf, size_t filter_size);
void convolution_dim1(const Custom3DArray<float>& input, Custom3DArray<float>& output, const float* lpf, const float* hpf, size_t filter_size);
void convolution_dim2(const Custom3DArray<float>& input, Custom3DArray<float>& output, const float* lpf, const float* hpf, size_t filter_size);

#endif // CONVOLVE_H