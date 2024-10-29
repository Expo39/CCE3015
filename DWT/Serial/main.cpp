#include <iostream>
#include "io.h"
#include "DWT.h"
#include "../shared/jbutil.h"
#include <filesystem>

int main() {
    std::string binary_filename = "../Binary/3/3.bin";
    std::string shape_filename = "../Binary/3/3_shape.txt";
    std::string output_filename = "outputs/3out.bin";

    // Directly check if the files exist
    if (!std::filesystem::exists(binary_filename)) {
        std::cerr << "Binary file does not exist: " << binary_filename << std::endl;
        return 1;
    }
    if (!std::filesystem::exists(shape_filename)) {
        std::cerr << "Shape file does not exist: " << shape_filename << std::endl;
        return 1;
    }

    // Read the shape information
    jbutil::vector<size_t> shape = read_shape(shape_filename);
    if (shape.size() != 3) {
        std::cerr << "Invalid shape information" << std::endl;
        return 1;
    }

    size_t depth = shape[0];
    size_t rows = shape[1];
    size_t cols = shape[2];
    size_t num_elements = depth * rows * cols;

    std::cout << "Number of elements: " << num_elements << std::endl;
    std::cout << "Shape dimensions: depth=" << shape[0] << ", rows=" << shape[1] << ", cols=" << shape[2] << std::endl;

    // Read the DICOM data into an array
    Custom3DArray<float> dicom_data = read_dicom_data(binary_filename, depth, rows, cols);

    // Check if the data was read correctly
    if (dicom_data.get_depth() == 0 || dicom_data.get_rows() == 0 || dicom_data.get_cols() == 0) {
        std::cerr << "Failed to read DICOM data" << std::endl;
        return 1;
    }

    std::cout << "DICOM data read successfully." << std::endl;

    // Measure the time taken for the 3D wavelet transform
    double start_time = jbutil::gettime();

    // Perform the 2D wavelet transform
    //Wavelet2DResult wavelet_2d = dwt_2d(dicom_data);

    // Perform the 3D wavelet transform
    Wavelet3DResult wavelet_3d = dwt_3d(dicom_data);

    double end_time = jbutil::gettime();
    double elapsed_time = end_time - start_time;

    std::cout << "3D Wavelet Transform completed successfully." << std::endl;
    std::cout << "Time taken for 3D Wavelet Transform: " << elapsed_time << " seconds" << std::endl;

    // Export the transformed data to a binary file
    if (!export_data_to_binary(wavelet_3d, output_filename)) {
        std::cerr << "Failed to export data to binary file" << std::endl;
        return 1;
    }

    std::cout << "Data exported to " << output_filename << " successfully." << std::endl;

    return 0;
}