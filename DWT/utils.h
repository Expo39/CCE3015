#ifndef UTILS_H
#define UTILS_H

#include "../shared/jbutil.h"
#include <cassert>

template <class T>
class Custom3DArray {
private:
    jbutil::vector<T> data;
    size_t depth, rows, cols;

public:
    Custom3DArray(size_t d, size_t r, size_t c) : data(d * r * c), depth(d), rows(r), cols(c) {}

    T& operator()(size_t d, size_t r, size_t c) {
        assert(d < depth && r < rows && c < cols);
        return data[d * rows * cols + r * cols + c];
    }

    const T& operator()(size_t d, size_t r, size_t c) const {
        assert(d < depth && r < rows && c < cols);
        return data[d * rows * cols + r * cols + c];
    }

    size_t get_depth() const { return depth; }
    size_t get_rows() const { return rows; }
    size_t get_cols() const { return cols; }
    size_t size() const { return data.size(); }
};

#endif // UTILS_H