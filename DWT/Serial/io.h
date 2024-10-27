#ifndef IO_H
#define IO_H

#include <string>
#include "../shared/jbutil.h" // Include the jbutil header file
#include "../utils.h" // Include the custom 3D array header file

// Function to read DICOM data from a binary file into a 3D array
Custom3DArray<float> read_dicom_data(const std::string& filename, size_t depth, size_t rows, size_t cols);

// Function to read the shape information from a file
jbutil::vector<size_t> read_shape(const std::string& shape_filename);

void check_subband_sizes(const Custom3DArray<float>& data, size_t expected_depth, size_t expected_rows, size_t expected_cols);

// Function to check if the binary and shape files exist
bool check_files_exist(const std::string& binary_filename, const std::string& shape_filename);

// Function to export data to a binary file
bool export_data_to_binary(const Custom3DArray<float>& data, const std::string& filename);

bool export_2d_slice_to_binary(const Custom3DArray<float>& data, size_t depth_index, const std::string& filename);

#endif // IO_H