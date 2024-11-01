#ifndef IO_H
#define IO_H

#include "utilities/jbutil.h" 
#include "utilities/utils.h"

// Function to read DICOM data from a binary file into a 3D array
Custom3DArray<float> read(const std::string& filename, size_t depth, size_t rows, size_t cols);

// Read the dimensions of the collection of images
jbutil::vector<size_t> read_shape(const std::string& shape_filename);

// Function to export data to a binary file
bool export_data(const Custom3DArray<float>& data, const std::string& filename);

#endif // IO_H