#include "DWT.h"

// Constructor for the DWT class to be used for convolving the filters
DWT::DWT(const float* lpf, const float* hpf, size_t filter_size)
    : convolve(lpf, hpf, filter_size) {}

/* 
 * Perform the 3D Discrete Wavelet Transform on the input data and export the result
 * Parameters:
 * - binary_filename: the name of the binary file containing the input data
 * - output_filename: the name of the binary file to write the transformed data to
 * - filter_type: the type of wavelet filter to use (e.g., "haar", "db1")
 * - levels: the number of levels of decomposition
 */
void perform_transform(const string& binary_filename, const string& output_filename, const string& filter_type, int levels) {
    // Choose the wavelet filters based on user input
    const float* lpf;
    const float* hpf;
    const float* Ilpf;
    const float* Ihpf;
    size_t filter_size;
    
    // Determine the shape file based on the binary file name
    string shape_filename = binary_filename.substr(0, binary_filename.find_last_of('.')) + "_shape.txt";

    if (!get_filters(filter_type, lpf, hpf, Ilpf, Ihpf, filter_size)) {
        cerr << "Failed to get filters for type: " << filter_type << endl;
        return;
    }

    try {
        // Read the DICOM data into an array
        Array3D<float> dicom_data = IO::read(binary_filename, shape_filename);

        cout << "\nData read from " << binary_filename << " successfully.\n" << endl;

        // Print the characteristics of the input data
        cout << "Filter type: " << filter_type << endl;
        cout << "Filter size: " << filter_size << endl;
        cout << "Levels: " << levels << endl;

        // Create a DWT object to store filter information
        DWT dwt(lpf, hpf, filter_size);

        // Measure the time taken for the 3D wavelet transform
        double start_time = jbutil::gettime();

        // Perform the 3D wavelet transform with the desired number of levels
        Array3D<float> wavelet_3d = dwt.dwt_3d(dicom_data, levels);

        double end_time = jbutil::gettime();
        double elapsed_time = end_time - start_time;

        cout << "Time taken for 3D Wavelet Transform: " << elapsed_time << " seconds\n" << endl;

        // Export the transformed data to a binary file
        IO::export_data(wavelet_3d, output_filename);

        cout << "Data exported to " << output_filename << " successfully.\n" << endl;

        // Create an Inverse object to store filter information
        Inverse inverse(Ilpf, Ihpf, filter_size);

        // Perform the inverse 3D wavelet transform
        Array3D<float> reconstructed_data = inverse.inverse_dwt_3d(wavelet_3d, levels);

        // Determine the inverse output filename
        std::string inverse_output_filename = "data/outputs/inverse_" + output_filename.substr(output_filename.find_last_of('/') + 1);
    
        // Export the reconstructed data to a binary file
        IO::export_inverse(reconstructed_data, inverse_output_filename);

        cout << "Inverse 3D Wavelet Transform completed successfully." << endl;
        cout << "Data exported to " << inverse_output_filename << " successfully." << endl;

    } catch (const runtime_error& e) {
        cerr << "Runtime error: " << e.what() << endl;
        return;
    }
}


/* 
 * Perform the convolutions of the Multi-Level 3D Discrete Wavelet Transform
 * Parameters:
 * - data: 3D array of data to be transformed
 * - levels: number of levels of decomposition
 * Returns:
 * - 3D array of transformed data
 */
Array3D<float> DWT::dwt_3d(const Array3D<float>& data, int levels) const {
    // Create a copy of the input data to store the result
    Array3D<float> result = data;

    // Get the initial dimensions of the data
    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    for (int level = 0; level < levels; ++level) {
        // Convolve and subsample ONLY within the bounds of the current level
        convolve.dim0(result, depth, rows, cols); // Convolve along the first dimension (rows)
        convolve.dim1(result, depth, rows, cols); // Convolve along the second dimension (columns)
        convolve.dim2(result, depth, rows, cols); // Convolve along the third dimension (depths)

        // Calculate new bounds for the next level's LLL subband
        depth = (depth+1) / 2;
        rows = (rows+1) / 2;
        cols = (cols+1) / 2;
    }

    // Return the transformed data
    return result;
}