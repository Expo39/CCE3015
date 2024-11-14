#ifndef IO_H
#define IO_H

#include "../utilities/utils.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

class IO {
public:
    // Read the data from a binary file and return it as a 3D array
    static Array3D<float> read(const string& filename, const string& shape_filename);

    // Export the data to a binary file
    static bool export_data(const Array3D<float>& data, const string& filename);

private:
    // Read the shape information from a shape file
    static vector<size_t> read_shape(const string& shape_filename);
};

#endif // IO_H