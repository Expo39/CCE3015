#include "io.h"

/* ============================================================================
 * ---------------------------- Function: write_arr
 * Writes a 3D array to a binary file.
 *
 * ------ Parameters ------
 * array:       The 3D array to write to the file.
 * file:        The binary file stream to write to.
 * ============================================================================
 */
void write_arr(
    const Array3D<float>& array,
    std::ofstream& file
) {
    // Get dimensions
    size_t depth = array.get_depth();
    size_t rows = array.get_rows();
    size_t cols = array.get_cols();

    file.write(reinterpret_cast<const char*>(&depth), sizeof(depth));
    file.write(reinterpret_cast<const char*>(&rows), sizeof(rows));
    file.write(reinterpret_cast<const char*>(&cols), sizeof(cols));
    
    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            file.write(
                reinterpret_cast<const char*>(
                    &array(d, r, 0)), cols * sizeof(float)
            );
        }
    }
}

/* ============================================================================
 * ---------------------------- Function: read_arr
 * Reads a 3D array from a binary file.
 *
 * ------ Parameters ------
 * file:        The binary file stream to read from.
 *
 * ------ Returns ------
 * A 3D array read from the file.
 * ============================================================================
 */
Array3D<float> read_binary_file(const std::string& binary_file_path, const std::string& shape_file_path) {
    // Read the shape from the shape file
    size_t depth, rows, cols;
    std::ifstream shape_file(shape_file_path);
    if (!shape_file.is_open()) {
        std::cerr << "Error: Could not open shape file." << std::endl;
        return Array3D<float>();
    }
    char comma;
    shape_file >> depth >> comma >> rows >> comma >> cols;
    shape_file.close();

    // Check for valid dimensions
    if (depth == 0 || rows == 0 || cols == 0) {
        std::cerr << "Error: Invalid dimensions read from shape file." << std::endl;
        return Array3D<float>();
    }

    // Create the 3D array with the read dimensions
    Array3D<float> array_3d(depth, rows, cols);

    // Read the binary data into the 3D array
    std::ifstream binary_file(binary_file_path, std::ios::binary);
    if (!binary_file.is_open()) {
        std::cerr << "Error: Could not open binary file." << std::endl;
        return Array3D<float>();
    }
    for (size_t d = 0; d < depth; ++d) {
        for (size_t r = 0; r < rows; ++r) {
            binary_file.read(reinterpret_cast<char*>(&array_3d(d, r, 0)), cols * sizeof(float));
        }
    }
    binary_file.close();

    return array_3d;
}

/* ============================================================================
 * ---------------------------- Function: save_levels
 * Saves the DWT sub-bands into a single binary file.
 *
 * ------ Parameters ------
 * level_data: A struct containing the DWT results.
 * filename:   The name of the file to write to.
 * ============================================================================
 */
void save_levels(const LevelData& level_data, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        std::exit(EXIT_FAILURE);
    }

    write_arr(level_data.LLL, file);
    write_arr(level_data.LLH, file);
    write_arr(level_data.LHL, file);
    write_arr(level_data.LHH, file);
    write_arr(level_data.HLL, file);
    write_arr(level_data.HLH, file);
    write_arr(level_data.HHL, file);
    write_arr(level_data.HHH, file);

    file.close();
}