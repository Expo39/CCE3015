#include "DWT.h"
#include "convolve.h"

DWT::DWT(const float* lpf, const float* hpf, size_t filter_size)
    : convolve(lpf, hpf, filter_size) {}

LevelData DWT::dwt_3d(const Array3D<float>& data, int levels) const {
    if (levels < 1) {
        throw std::invalid_argument("Levels must be greater than or equal to 1");
    }

    Array3D<float> result = data;
    LevelData level_data;

        // ------------------------------------ STEP 1
        convolve.convolve_x(result, convolve.get_lpf(), level_data.L);
        convolve.convolve_x(result, convolve.get_hpf(), level_data.H);

        // ------------------------------------ STEP 2
        convolve.convolve_y(level_data.L, convolve.get_lpf(), level_data.LL);     // LL
        convolve.convolve_y(level_data.L, convolve.get_hpf(), level_data.LH);     // LH
        convolve.convolve_y(level_data.H, convolve.get_lpf(), level_data.HL);     // HL
        convolve.convolve_y(level_data.H, convolve.get_hpf(), level_data.HH);     // HH

        // ------------------------------------ STEP 3
        convolve.convolve_z(level_data.LL, convolve.get_lpf(), level_data.LLL);   // LLL
        convolve.convolve_z(level_data.LL, convolve.get_hpf(), level_data.LLH);   // LLH
        convolve.convolve_z(level_data.LH, convolve.get_lpf(), level_data.LHL);   // LHL
        convolve.convolve_z(level_data.LH, convolve.get_hpf(), level_data.LHH);   // LHH

        convolve.convolve_z(level_data.HL, convolve.get_lpf(), level_data.HLL);   // HLL
        convolve.convolve_z(level_data.HL, convolve.get_hpf(), level_data.HLH);   // HLH
        convolve.convolve_z(level_data.HH, convolve.get_lpf(), level_data.HHL);   // HHL
        convolve.convolve_z(level_data.HH, convolve.get_hpf(), level_data.HHH);   // HHH

    return level_data;
}