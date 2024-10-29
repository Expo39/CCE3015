#include "filters.h"
#include <iostream>  // Include for error messages
#include "../shared/jbutil.h"  // Include your custom vector header

// Function to initialize filters based on the filter type
void initialize_filters(Filters& filters, const std::string& filter_type) {
    // Temporary jbutil::vector to hold filter coefficients
    jbutil::vector<float> low_pass_coeffs;
    jbutil::vector<float> high_pass_coeffs;

    if (filter_type == "haar") {
        // Haar filter coefficients
        low_pass_coeffs.assign({1.0f / std::sqrt(2), 1.0f / std::sqrt(2)});
        high_pass_coeffs.assign({-1.0f / std::sqrt(2),1.0f / std::sqrt(2)});
    } 
    else if (filter_type == "db2") {
        // Daubechies (db2) coefficients
        low_pass_coeffs.assign({
            -0.12940952255126037f, 0.2241438680420134f, 
            0.8365163037378079f, 0.48296291314453416f});
        high_pass_coeffs.assign({
            -0.48296291314453416f, 0.8365163037378079f, 
            -0.2241438680420134f, -0.12940952255126037f});
    } 
    else if (filter_type == "db3") {
        // Daubechies (db3) coefficients
        low_pass_coeffs.assign({
            0.03522629188570953f, -0.08544127388202666f, 
            -0.13501102001025458f, 0.45987750211849154f, 
            0.8068915093110925f, 0.33267055295008263f});
        high_pass_coeffs.assign({
            -0.33267055295008263f, 0.8068915093110925f, 
            -0.45987750211849154f, -0.13501102001025458f, 
            0.08544127388202666f, 0.03522629188570953f});
    } 
    else {
        // Default to Haar filter if filter type is unknown
        std::cerr << "Warning: Unknown filter type '" << filter_type << "'. Defaulting to Haar filter." << std::endl;
        initialize_filters(filters, "haar"); // Recursively call with default filter type
        return;  // Exit to avoid further assignments
    }

    // Assign populated jbutil::vector to the filters struct
    filters.low_pass_filter.assign(low_pass_coeffs.begin(), low_pass_coeffs.end());
    filters.high_pass_filter.assign(high_pass_coeffs.begin(), high_pass_coeffs.end());
}