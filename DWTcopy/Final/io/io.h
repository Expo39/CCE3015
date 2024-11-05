#ifndef IO_H
#define IO_H

#include "../utilities/utils.h"
#include "../serial/DWT.h" 
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

// Forward declaration of LevelData struct
struct LevelData;

// Function to write a 3D array to a binary file
void write_arr(const Array3D<float>& array, std::ofstream& file);

// Function to read a 3D array from a binary file
Array3D<float> read_arr(std::ifstream& file);

// Function to save the DWT sub-bands into a single binary file
void save_levels(const LevelData& level_data, const std::string& filename);

#endif // IO_H