#include "io.h"

Array3D<float> IO::read(const std::string& filename, size_t depth, size_t rows, size_t cols) { 
    // Calculate new dimensions, rounding up to handle uneven dimensions 
    size_t new_depth = (depth % 2 == 0) ? depth : depth + 1; 
    size_t new_rows = (rows % 2 == 0) ? rows : rows + 1; 
    size_t new_cols = (cols % 2 == 0) ? cols : cols + 1;

    Array3D<float> data(new_depth, new_rows, new_cols);
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return data; 
    }

    // Check if the file contains enough data
    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    file.seekg(0, std::ios::beg);
    size_t expected_size = depth * rows * cols * sizeof(float);
    if (file_size < expected_size) {
        std::cerr << "File size is smaller than expected: " << filename << std::endl;
        file.close();
        return data;
    }

    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            file.read(reinterpret_cast<char*>(&data(d, r, 0)), cols * sizeof(float));
            if (!file) {
                std::cerr << "Error reading row " << r << " of depth " << d << " from file: " << filename << std::endl;
                file.close();
                return data;
            }
        }
    }

    file.close(); 
    return data; 
}

jbutil::vector<size_t> IO::read_shape(const std::string& shape_filename) {
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

    file.close();
    return shape;
}

bool IO::export_data(const Array3D<float>& data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return false;
    }

    size_t depth = data.get_depth();
    size_t rows = data.get_rows();
    size_t cols = data.get_cols();

    // Define the dimensions of each sub-band
    size_t sub_depth = depth / 2;
    size_t sub_rows = rows / 2;
    size_t sub_cols = cols / 2;

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

    // Export each subband
    export_subband(0, 0, 0); // LLL
    export_subband(0, 0, sub_cols); // LLH
    export_subband(0, sub_rows, 0); // LHL
    export_subband(0, sub_rows, sub_cols); // LHH
    export_subband(sub_depth, 0, 0); // HLL
    export_subband(sub_depth, 0, sub_cols); // HLH
    export_subband(sub_depth, sub_rows, 0); // HHL
    export_subband(sub_depth, sub_rows, sub_cols); // HHH

    file.close();
    return true;
}