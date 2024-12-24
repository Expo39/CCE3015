#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include "io.h"
#include "DWT.h"

using namespace std;

int main(int argc, char* argv[]) {
    try {
        // Check if the number of arguments is valid
        if (argc != 5 && argc != 6 && argc != 7) {
            throw invalid_argument("Usage: " + string(argv[0]) + " <file number> <dataset type (CT/MR)> <filter type> <levels> [MR type (T1DUAL/T2SPIR)] [Phase type (InPhase/OutPhase)]");
        }

        // Parse command line arguments
        string file_number = argv[1];
        string dataset_type = argv[2];
        string filter_type = argv[3];
        int levels = stoi(argv[4]);

        // Optional arguments for MR dataset type
        string mr_type = argc >= 6 ? argv[5] : "";
        string phase_type = argc == 7 ? argv[6] : "";

        // Construct filenames based on input parameters
        auto [binary_filename, shape_filename, output_filename] = IO::construct_filenames(file_number, dataset_type, mr_type, phase_type, filter_type, levels);

        // Create the outputs directory if it does not exist
        filesystem::create_directories("data/outputs");

        // Perform the 3D wavelet transform
        perform_transform(binary_filename, output_filename, filter_type, levels);
        
    } catch (const invalid_argument& e) {
        // Handle invalid argument exceptions
        cerr << "Invalid argument: " << e.what() << endl;
        return 1;
    } catch (const exception& e) {
        // Handle other exceptions
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}