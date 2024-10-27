#include "io.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

// Implement the read_dicom_data function for float
Custom3DArray<float> read_dicom_data(const std::string& filename, size_t depth, size_t rows, size_t cols) {
    Custom3DArray<float> data(depth, rows, cols);
    std::ifstream file(filename, std::ios::binary);
    
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return data; // Return an empty array
    }

    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            file.read(reinterpret_cast<char*>(&data(d, r, 0)), cols * sizeof(float));
            if (!file) {
                std::cerr << "Error reading row " << r << " of depth " << d << " from file: " << filename << std::endl;
                file.close();
                return data; // Return the partially filled array
            }
        }
    }

    file.close(); // Ensure the file is closed
    return data; // Return the filled 3D array
}

// Read shape implementation
jbutil::vector<size_t> read_shape(const std::string& shape_filename) {
    jbutil::vector<size_t> shape;
    std::ifstream file(shape_filename);
    
    if (!file) {
        std::cerr << "Error opening shape file: " << shape_filename << std::endl;
        return shape;
    }

    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        shape.push_back(std::stoul(item));
    }
    
    return shape;
}

// Function to export data to a binary file
bool export_data_to_binary(const Custom3DArray<float>& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return false;
    }

    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    std::cout << "Exporting data with dimensions: " 
              << depth << " x " << rows << " x " << cols << std::endl;

    file.write(reinterpret_cast<const char*>(&depth), sizeof(depth));
    file.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
    file.write(reinterpret_cast<const char*>(&cols), sizeof(cols));

    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            file.write(reinterpret_cast<const char*>(&data(d, r, 0)), cols * sizeof(float));
        }
    }

    file.close();
    return true;
}