#include "convolve.h"

Convolve::Convolve(const float* lpf, const float* hpf, size_t filter_size)
    : lpf(lpf), hpf(hpf), filter_size(filter_size) {}

jbutil::vector<float> Convolve::convolve(const jbutil::vector<float>& data, bool is_low_pass) const {
    size_t data_size = data.size();
    size_t N = data_size / 2;
    jbutil::vector<float> s_odd(N);
    jbutil::vector<float> s_even(N);

    // Separate odd and even samples
    for (size_t i = 0; i < N; ++i) {
        s_odd[i] = data[2 * i];
        s_even[i] = data[2 * i + 1];
    }

    jbutil::vector<float> result(N);

    for (size_t i = 0; i < N; ++i) {
        float sum = 0.0f;

        const float* filter = is_low_pass ? lpf : hpf;

        for (size_t j = 0; j < filter_size; ++j) {
            size_t index = i + j / 2;
            if (index >= N) {
                index -= N;
            }
            sum += filter[j] * ((j % 2 == 0) ? s_odd[index] : s_even[index]);
        }

        result[i] = sum;
    }

    return result;
}

// Function to apply 1D convolution along the first dimension (rows)
void Convolve::dim0(const Array3D<float>& input, Array3D<float>& output) const {
    size_t depth = input.get_depth();
    size_t rows = input.get_rows();
    size_t cols = input.get_cols();

    for (size_t d = 0; d < depth; ++d) {
        for (size_t c = 0; c < cols; ++c) {
            jbutil::vector<float> row(rows);
            for (size_t r = 0; r < rows; ++r) {
                row[r] = input(d, r, c);
            }
            jbutil::vector<float> L_row = convolve(row, true);  
            jbutil::vector<float> H_row = convolve(row, false);
            for (size_t r = 0; r < static_cast<size_t>(L_row.size()); ++r) {
                output(d, r, c) = L_row[r];
                output(d, r + rows / 2, c) = H_row[r];
            }
        }
    }
}

// Function to apply 1D convolution along the second dimension (columns)
void Convolve::dim1(const Array3D<float>& input, Array3D<float>& output) const {
    size_t depth = input.get_depth();
    size_t rows = input.get_rows();
    size_t cols = input.get_cols();

    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            jbutil::vector<float> col(cols);
            for (size_t c = 0; c < cols; ++c) {
                col[c] = input(d, r, c);
            }
            jbutil::vector<float> L_col = convolve(col, true);  
            jbutil::vector<float> H_col = convolve(col, false); 
            for (size_t c = 0; c < static_cast<size_t>(L_col.size()); ++c) {
                output(d, r, c) = L_col[c];
                output(d, r, c + cols / 2) = H_col[c];
            }
        }
    }
}

// Function to apply 1D convolution along the third dimension (depth)
void Convolve::dim2(const Array3D<float>& input, Array3D<float>& output) const {
    size_t depth = input.get_depth();
    size_t rows = input.get_rows();
    size_t cols = input.get_cols();

    for (size_t r = 0; r < rows; ++r) {
        for (size_t c = 0; c < cols; ++c) {
            jbutil::vector<float> depth_col(depth);
            for (size_t d = 0; d < depth; ++d) {
                depth_col[d] = input(d, r, c);
            }
            jbutil::vector<float> L_depth = convolve(depth_col, true);  
            jbutil::vector<float> H_depth = convolve(depth_col, false);
            for (size_t d = 0; d < static_cast<size_t>(L_depth.size()); ++d) {
                output(d, r, c) = L_depth[d];
                output(d + depth / 2, r, c) = H_depth[d];
            }
        }
    }
}