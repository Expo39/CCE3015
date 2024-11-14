#include "DWT.h"
#include "convolve.h"
#include "../io/io.h"
#include "../utilities/jbutil.h"
#include "../filters/filters.h"

void perform_transform(const string& binary_filename, const string& output_filename, const string& filter_type, int levels) {
    
    // Choose the wavelet filters based on user input
    const float* lpf;
    const float* hpf;
    size_t filter_size;
    
    // Determine the shape file based on the binary file name
    string shape_filename = binary_filename.substr(0, binary_filename.find_last_of('.')) + "_shape.txt";

    if (!get_filters(filter_type, lpf, hpf, filter_size)) {
        cerr << "Failed to get filters for type: " << filter_type << endl;
        return;
    }

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
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <file name> <filter type> <levels>" << endl;
        return 1;
    }

    // Get the file name, filter type, and number of levels from the command line arguments
    string file_name = argv[1];
    string filter_type = argv[2];
    int levels = stoi(argv[3]);

    string binary_filename = "../data/" + file_name + ".bin";
    string output_filename = "outputs/" + file_name + "out.bin";

    perform_transform(binary_filename, output_filename, filter_type, levels);

}