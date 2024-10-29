#ifndef FILTERS_H
#define FILTERS_H

#include "../shared/jbutil.h" // Include the necessary jbutil header

// Struct to hold the filters
struct Filters {
    jbutil::vector<float> low_pass_filter;
    jbutil::vector<float> high_pass_filter;
};

// Function to initialize filters based on the filter type
void initialize_filters(Filters& filters, const std::string& filter_type);

#endif // FILTERS_H