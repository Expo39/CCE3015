#include "DWT.h"
#include "../io/io.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"
#include "convolve.h"

int main() {
    std::string binary_filename = "../data/11/11.bin";
    std::string shape_filename = "../data/11/11_shape.txt";
    std::string output_filename = "outputs/11out.bin";

    // Read the shape information
    jbutil::vector<size_t> shape = read_shape(shape_filename);
    if (shape.size() != 3) {
        std::cerr << "Invalid shape information" << std::endl;
        return 1;
    }

    size_t depth = shape[0];
    size_t rows = shape[1];
    size_t cols = shape[2];

    // Read the DICOM data into an array
    Custom3DArray<float> dicom_data = read(binary_filename, depth, rows, cols);

    // Set the number of levels for the multi-level transform
    int levels = 1; 

    // Choose the wavelet filters
    const float* lpf = DB1_L;
    const float* hpf = DB1_H;
    size_t filter_size = 2;

    // Measure the time taken for the 3D wavelet transform
    double start_time = jbutil::gettime();

    // Perform the 3D wavelet transform with the desired number of levels
    Custom3DArray<float> wavelet_3d = dwt_3d(dicom_data, levels, lpf, hpf, filter_size);

    double end_time = jbutil::gettime();
    double elapsed_time = end_time - start_time;

    std::cout << "3D Wavelet Transform completed successfully." << std::endl;
    std::cout << "Time taken for 3D Wavelet Transform: " << elapsed_time << " seconds" << std::endl;

    // Export the transformed data to a binary file
    if (!export_data(wavelet_3d, output_filename)) {
        std::cerr << "Failed to export data to binary file" << std::endl;
        return 1;
    }

    std::cout << "Data exported to " << output_filename << " successfully." << std::endl;

    return 0;
}