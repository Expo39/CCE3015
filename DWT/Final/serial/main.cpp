#include "DWT.h"
#include "convolve.h"
#include "inverse.h"
#include "../io/io.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

using namespace std;

int main() {
    string binary_filename = "../data/3/3.bin";
    string shape_filename = "../data/3/3_shape.txt";
    string output_filename = "outputs/3out.bin";
    string inverse_output_filename = "outputs/3inverse.bin";

    // Read the DICOM data into an array
    Array3D<float> dicom_data = IO::read(binary_filename, shape_filename);

    // Set the number of levels for the multi-level transform
    int levels = 1; 

    // Choose the wavelet filters
    const float* lpf = DB2_L;
    const float* hpf = DB2_H;

    // Choose the inverse wavelet filters
    const float* Ilpf = DB2_I_L;
    const float* Ihpf = DB2_I_H;

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
    
    // Create an Inverse object to store filter information
    Inverse inverse(Ilpf, Ihpf, filter_size);

    // Perform the inverse 3D wavelet transform
    Array3D<float> reconstructed_data = inverse.inverse_dwt_3d(wavelet_3d, levels);

    // Export the reconstructed data to a binary file
    IO::export_inverse(reconstructed_data, inverse_output_filename);

    cout << "Inverse 3D Wavelet Transform completed successfully." << endl;
    cout << "Data exported to " << inverse_output_filename << " successfully." << endl;

    return 0;
}