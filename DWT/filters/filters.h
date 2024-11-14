#ifndef FILTERS_H
#define FILTERS_H

#include <iostream>

using namespace std;

extern float DB1_L[2];
extern float DB1_H[2];

extern float DB2_L[4];
extern float DB2_H[4];

extern float DB3_L[6];
extern float DB3_H[6];

extern float DB4_L[8];
extern float DB4_H[8];

bool get_filters(const std::string& filter_type, const float*& lpf, const float*& hpf, size_t& filter_size);

#endif // FILTERS_H