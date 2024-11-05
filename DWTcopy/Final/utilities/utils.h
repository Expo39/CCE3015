#ifndef UTILS_H
#define UTILS_H

#include "jbutil.h"

// Template class for a custom 3D array
template <class T>
class Array3D {
private:
    jbutil::vector<T> data; 
    size_t depth, rows, cols; 

public:
    // Default constructor
    Array3D() : depth(0), rows(0), cols(0) {}

    // Constructor to initialize the 3D array with given dimensions
    Array3D(size_t d, size_t r, size_t c) : data(d * r * c), depth(d), rows(r), cols(c) {}

    // Non-const element access operator
    T& operator()(size_t d, size_t r, size_t c) {
        assert(d < depth && r < rows && c < cols); 
        return data[d * rows * cols + r * cols + c]; // Calculate the 1D index and return the element
    }

    // Const element access operator
    const T& operator()(size_t d, size_t r, size_t c) const {
        assert(d < depth && r < rows && c < cols); 
        return data[d * rows * cols + r * cols + c]; // Calculate the 1D index and return the element
    }

    // Get the depth of the 3D array
    size_t get_depth() const { return depth; }

    // Get the number of rows in the 3D array
    size_t get_rows() const { return rows; }

    // Get the number of columns in the 3D array
    size_t get_cols() const { return cols; }

    // Get the total number of elements in the 3D array
    size_t size() const { return data.size(); }

    // Resize the 3D array with new dimensions
    void resize(size_t new_depth, size_t new_rows, size_t new_cols) {
        data.resize(new_depth * new_rows * new_cols);
        depth = new_depth;
        rows = new_rows;
        cols = new_cols;
    }
};

#endif // UTILS_H