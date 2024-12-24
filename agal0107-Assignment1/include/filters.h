#ifndef FILTERS_H
#define FILTERS_H

#include <iostream>

using namespace std;

// Filter Definitions

extern float DB1_L[2];
extern float DB1_H[2];

extern float DB2_L[4];
extern float DB2_H[4];

extern float DB3_L[6];
extern float DB3_H[6];

extern float DB4_L[8];
extern float DB4_H[8];

extern float DB5_L[10];
extern float DB5_H[10];

extern float DB6_L[12];
extern float DB6_H[12];

extern float DB7_L[14];
extern float DB7_H[14];

extern float DB8_L[16];
extern float DB8_H[16];

// Function to get the filter coefficients based on the filter type
bool get_filters(const string& filter_type, const float*& lpf, const float*& hpf, size_t& filter_size);

#endif // FILTERS_H