#include "DWT.h"
#include "convolve.h"
#include "../io/io.h" // Ensure this includes the declaration of save_levels and read_binary_file

// Add the declaration of read_binary_file if not already present
Array3D<float> read_binary_file(const std::string& binary_filename, const std::string& shape_filename);
#include "../utilities/jbutil.h"
#include "../filters/filters.h"
#include <fstream>
#include <sstream>
#include <iostream>

int main() {
    std::string binary_filename = "../data/3/3.bin";
    std::string shape_filename = "../data/3/3_shape.txt";
    std::string output_filename = "outputs/3out.bin";

    // Read the DICOM data into an array using the read_binary_file function
    Array3D<float> dicom_data = read_binary_file(binary_filename, shape_filename);
    if (dicom_data.get_depth() == 0 || dicom_data.get_rows() == 0 || dicom_data.get_cols() == 0) {
        std::cerr << "Error: Failed to read DICOM data." << std::endl;
        return 1;
    }

    std::cout << "Read DICOM data with dimensions: depth=" << dicom_data.get_depth() << ", rows=" << dicom_data.get_rows() << ", cols=" << dicom_data.get_cols() << std::endl;

    // Set the number of levels for the multi-level transform
    int levels = 1; 

    // Choose the wavelet filters
    const float* lpf = DB1_L;
    const float* hpf = DB1_H;
    size_t filter_size = 2;

    // Create a DWT object
    DWT dwt(lpf, hpf, filter_size);

    // Measure the time taken for the 3D wavelet transform
    double start_time = jbutil::gettime();

    // Perform the 3D wavelet transform with the desired number of levels
    LevelData wavelet_3d = dwt.dwt_3d(dicom_data, levels);

    double end_time = jbutil::gettime();
    double elapsed_time = end_time - start_time;

    std::cout << "3D Wavelet Transform completed successfully." << std::endl;
    std::cout << "Time taken for 3D Wavelet Transform: " << elapsed_time << " seconds" << std::endl;

    // Save the DWT sub-bands into a single binary file
    save_levels(wavelet_3d, output_filename);

    std::cout << "Data exported to " << output_filename << " successfully." << std::endl;

    return 0;
}