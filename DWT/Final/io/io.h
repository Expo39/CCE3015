#ifndef IO_H
#define IO_H

#include "utilities/jbutil.h" 
#include "utilities/utils.h"

using namespace std;

class IO {
public:

    // Function to read the data from a binary file and return it as a 3D array
    static Array3D<float> read(const std::string& filename, const std::string& shape_filename);

    // Function to read the shape information from a shape file
    static jbutil::vector<size_t> read_shape(const std::string& shape_filename);

    // Function to export the 3D array data to a binary file
    static bool export_data(const Array3D<float>& data, const std::string& filename);

    static bool export_inverse(const Array3D<float>& data, const std::string& filename);
    
};

#endif // IO_H