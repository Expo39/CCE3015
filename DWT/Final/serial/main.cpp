#include "DWT.h"
#include "convolve.h"
#include "../io/io.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

using namespace std;

int main() {
    string binary_filename = "../data/3/3.bin";
    string shape_filename = "../data/3/3_shape.txt";
    string output_filename = "outputs/3out.bin";

    // Read the DICOM data into an array
    Array3D<float> dicom_data = IO::read(binary_filename, shape_filename);

    // Set the number of levels for the multi-level transform
    int levels = 1; 

    // Choose the wavelet filters
    const float* lpf = DB2_L;
    const float* hpf = DB2_H;
    size_t filter_size = 4;

    // Create a DWT object to store filter information
    DWT dwt(lpf, hpf, filter_size);

    // Measure the time taken for the 3D wavelet transform
    double start_time = jbutil::gettime();

    // Perform the 3D wavelet transform with the desired number of levels
    Array3D<float> wavelet_3d = dwt.dwt_3d(dicom_data, levels);

    double end_time = jbutil::gettime();
    double elapsed_time = end_time - start_time;

    cout << "3D Wavelet Transform completed successfully." << endl;
    cout << "Time taken for 3D Wavelet Transform: " << elapsed_time << " seconds" << endl;

    // Export the transformed data to a binary file
    IO::export_data(wavelet_3d, output_filename);

    cout << "Data exported to " << output_filename << " successfully." << endl;

    return 0;
}