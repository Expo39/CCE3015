#include "io.h"

/* 
 * Function to read the data from a binary file and return it as a 3D array
 * Parameters:
 * - filename: the name of the binary file to read
 * - shape_filename: the name of the shape file that contains the dimensions of the 3D array
 * Returns:
 * - A 3D array of float values read from the binary file
 */
Array3D<float> IO::read(const string& filename, const string& shape_filename) {
    // Check if the file exists
    if (!filesystem::exists(filename)) {
        throw runtime_error("File does not exist: " + filename);
    }
    // Read the shape information from the shape file
    vector<size_t> shape = read_shape(shape_filename);

    // Check if the shape information is valid
    if (shape.size() != 3) {
        throw runtime_error("Invalid shape information");
    }

    size_t depth = shape[0];
    size_t rows = shape[1];
    size_t cols = shape[2];

    // Create a 3D array with the dimensions
    Array3D<float> data(depth, rows, cols);
    ifstream file(filename, ios::binary);

    // Check if the file was opened successfully
    if (!file) {
        throw runtime_error("Error opening file: " + filename);
    }

    // Read the data from the file into the 3D array
    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            file.read(reinterpret_cast<char*>(&data(d, r, 0)), cols * sizeof(float));
            if (!file) {
                throw runtime_error("Error reading row " + to_string(r) + " of depth " + to_string(d) + " from file: " + filename);
            }
        }
    }

    file.close(); 
    return data; 
}

/*
 * Function to read the shape information from a shape file
 * Parameters:
 * - shape_filename: the name of the shape file that contains the dimensions of the 3D array
 * Returns:
 * - A vector of size_t values representing the dimensions of the 3D array
 */
vector<size_t> IO::read_shape(const string& shape_filename) {
    vector<size_t> shape;
    ifstream file(shape_filename);
    
    // Check if the shape file was opened successfully
    if (!file) {
        throw runtime_error("Error opening shape file: " + shape_filename);
    }

    // Read the shape information from the file
    string line;
    getline(file, line);
    stringstream ss(line);
    string item;
    
    while (getline(ss, item, ',')) {
        shape.push_back(stoul(item));
    }

    file.close();
    return shape;
}

/* Function to export the 3D array data to a binary file
 * Parameters:
 * - data: the 3D array of data to be exported
 * - filename: the name of the binary file to write to
 */
void IO::export_data(const Array3D<float>& data, const string& filename) {
    ofstream file(filename, ios::binary);
    
    // Check if the file was opened successfully
    if (!file) {
        throw runtime_error("Error opening file for writing: " + filename);
    }

    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    // Define the dimensions of each sub-band
    size_t sub_depth = depth / 2;
    size_t sub_rows = rows / 2;
    size_t sub_cols = cols / 2;

    // Lambda function to export a sub-band of the 3D array
    auto export_subband = [&](size_t offset_depth, size_t offset_rows, size_t offset_cols) {
        file.write(reinterpret_cast<const char*>(&sub_depth), sizeof(sub_depth));
        file.write(reinterpret_cast<const char*>(&sub_rows), sizeof(sub_rows));
        file.write(reinterpret_cast<const char*>(&sub_cols), sizeof(sub_cols));

        for (size_t d = 0; d < sub_depth; ++d) {
            for (size_t r = 0; r < sub_rows; ++r) {
                file.write(reinterpret_cast<const char*>(&data(offset_depth + d, offset_rows + r, offset_cols)), sub_cols * sizeof(float));
            }
        }
    };

    // Export each sub-band
    export_subband(0, 0, 0); // LLL
    export_subband(0, 0, sub_cols); // LLH
    export_subband(0, sub_rows, 0); // LHL
    export_subband(0, sub_rows, sub_cols); // LHH
    export_subband(sub_depth, 0, 0); // HLL
    export_subband(sub_depth, 0, sub_cols); // HLH
    export_subband(sub_depth, sub_rows, 0); // HHL
    export_subband(sub_depth, sub_rows, sub_cols); // HHH

    file.close();
}

/* Function to construct filenames based on input parameters
 * Parameters:
 * - file_number: the file number
 * - dataset_type: the dataset type (CT/MR)
 * - mr_type: the MR type (T1DUAL/T2SPIR)
 * - phase_type: the phase type (InPhase/OutPhase)
 * Returns:
 * - A tuple containing the binary filename, shape filename, and output filename
 */
tuple<string, string, string> IO::construct_filenames(const string& file_number, const string& dataset_type, const string& mr_type, const string& phase_type, const string& filter_type, int levels) {
    string binary_filename = "data/inputs/" + file_number + "_" + dataset_type + (dataset_type == "MR" ? "_" + mr_type + (mr_type == "T1DUAL" ? "_" + phase_type : "") : "") + ".bin";
    string shape_filename = "data/inputs/" + file_number + "_" + dataset_type + (dataset_type == "MR" ? "_" + mr_type + (mr_type == "T1DUAL" ? "_" + phase_type : "") : "") + "_shape.txt";
    string output_filename = "data/outputs/" + file_number + "_" + dataset_type + (dataset_type == "MR" ? "_" + mr_type + (mr_type == "T1DUAL" ? "_" + phase_type : "") : "") + "_" + filter_type + "_" + to_string(levels) + ".bin";

    return make_tuple(binary_filename, shape_filename, output_filename);
}