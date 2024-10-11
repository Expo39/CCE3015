/*!
 * \file
 * \brief   Lab 2 - SIMD Programming.
 * \author  Johann A. Briffa
 *
 * Template for the solution to Lab 2 practical exercise on image resampling
 * using the Lanczos filter.
 */

#include "../shared/jbutil.h"
#include <thread>

using namespace std;

// Define vector types
typedef float v4sf __attribute__ ((vector_size (16)));

// Struct to hold parameters for resample_image_chunk
template <class real>
struct ResampleParams {
    const jbutil::image<int>& image_in;
    jbutil::image<int>& image_out;
    real R;
    int a;
    jbutil::vector<real>& lanczos_values;
    int start_col;
    int end_col;
};

// Precompute Lanczos values using SIMD and alignment
template <class real>
void precompute_lanczos(jbutil::vector<real>& lanczos_values, int a, int R) {
    int size = a * R + 1;
    lanczos_values.resize(size);
    int i = 0;
    for (; i <= size - 4; i += 4) { // Unroll by a factor of 4
        v4sf x = { static_cast<real>(i) / R,
                   static_cast<real>(i + 1) / R,
                   static_cast<real>(i + 2) / R,
                   static_cast<real>(i + 3) / R };

        v4sf pi_x = __builtin_ia32_mulps(x, (v4sf){static_cast<real>(pi), static_cast<real>(pi), static_cast<real>(pi), static_cast<real>(pi)});
        v4sf pi_x_a = __builtin_ia32_divps(pi_x, (v4sf){static_cast<real>(a), static_cast<real>(a), static_cast<real>(a), static_cast<real>(a)});

        v4sf sin_pi_x = { static_cast<real>(sin(pi_x[0])), static_cast<real>(sin(pi_x[1])), static_cast<real>(sin(pi_x[2])), static_cast<real>(sin(pi_x[3])) };
        v4sf sin_pi_x_a = { static_cast<real>(sin(pi_x_a[0])), static_cast<real>(sin(pi_x_a[1])), static_cast<real>(sin(pi_x_a[2])), static_cast<real>(sin(pi_x_a[3])) };

        v4sf lanczos_val = __builtin_ia32_divps(__builtin_ia32_mulps(sin_pi_x, sin_pi_x_a), __builtin_ia32_mulps(pi_x, pi_x_a));

        lanczos_values[i] = lanczos_val[0];
        lanczos_values[i + 1] = lanczos_val[1];
        lanczos_values[i + 2] = lanczos_val[2];
        lanczos_values[i + 3] = lanczos_val[3];
    }

    // Handle remaining iterations
    for (; i < size; ++i) {
        real x = static_cast<real>(i) / R;
        lanczos_values[i] = (x == 0) ? 1 : (static_cast<real>(sin(pi * x)) / (pi * x)) * (static_cast<real>(sin(pi * x / a)) / (pi * x / a));
    }
}

// Lanczos function using precomputed values
template <class real>
real lanczos(const jbutil::vector<real>& lanczos_values, real x, int a, int R) {
    int index = min(static_cast<int>(abs(x) * R), a * R);
    return lanczos_values[index];
}

// Convolution function with vectorization
template <class real>
real convolve(const jbutil::image<int>& image, real m, real n, int a, real R, const jbutil::vector<real>& lanczos_values) {
    int m_start = max(0, static_cast<int>(ceil(m / R - a)));
    int m_end = min(image.get_cols() - 1, static_cast<int>(floor(m / R + a)));
    int n_start = max(0, static_cast<int>(ceil(n / R - a)));
    int n_end = min(image.get_rows() - 1, static_cast<int>(floor(n / R + a)));

    real sum = 0.0;
    real weight = 0.0;

    for (int i = m_start; i <= m_end; ++i) {
        int j = n_start;
        for (; j <= n_end - 3; j += 4) { // Unroll by a factor of 4
            v4sf m_vec = { (m / R) - i, (m / R) - i, (m / R) - i, (m / R) - i };
            v4sf n_vec = { (n / R) - j, (n / R) - (j + 1), (n / R) - (j + 2), (n / R) - (j + 3) };

            v4sf lanczos_m = { lanczos(lanczos_values, abs((m / R) - i), a, R),
                               lanczos(lanczos_values, abs((m / R) - i), a, R),
                               lanczos(lanczos_values, abs((m / R) - i), a, R),
                               lanczos(lanczos_values, abs((m / R) - i), a, R) };

            v4sf lanczos_n = { lanczos(lanczos_values, abs((n / R) - j), a, R),
                               lanczos(lanczos_values, abs((n / R) - (j + 1)), a, R),
                               lanczos(lanczos_values, abs((n / R) - (j + 2)), a, R),
                               lanczos(lanczos_values, abs((n / R) - (j + 3)), a, R) };

            v4sf w_vec = __builtin_ia32_mulps(lanczos_m, lanczos_n);

            v4sf image_vec = { static_cast<real>(image(0, j, i)),
                               static_cast<real>(image(0, j + 1, i)),
                               static_cast<real>(image(0, j + 2, i)),
                               static_cast<real>(image(0, j + 3, i)) };

            v4sf sum_vec = __builtin_ia32_mulps(image_vec, w_vec);
            v4sf weight_vec = w_vec;

            // Use built-in addition
            sum += sum_vec[0] + sum_vec[1] + sum_vec[2] + sum_vec[3];
            weight += weight_vec[0] + weight_vec[1] + weight_vec[2] + weight_vec[3];
        }

        // Handle remaining iterations
        for (; j <= n_end; ++j) {
            real w = lanczos(lanczos_values, abs((m / R) - i), a, R) * lanczos(lanczos_values, abs((n / R) - j), a, R);
            sum += image(0, j, i) * w;
            weight += w;
        }
    }

    return sum / weight;
}

// Compute the convolution for each destination image pixel
template <class real>
void resample_image_chunk(const ResampleParams<real>& params) {
    int new_height = params.image_out.get_rows();

    for (int i = params.start_col; i < params.end_col; ++i) {
        int j = 0;
        for (; j <= new_height - 4; j += 4) { // Unroll by a factor of 4
            v4sf j_vec = { static_cast<real>(j), static_cast<real>(j + 1), static_cast<real>(j + 2), static_cast<real>(j + 3) };
            v4sf convolve_vec = { convolve(params.image_in, static_cast<real>(i), j_vec[0], params.a, params.R, params.lanczos_values),
                                  convolve(params.image_in, static_cast<real>(i), j_vec[1], params.a, params.R, params.lanczos_values),
                                  convolve(params.image_in, static_cast<real>(i), j_vec[2], params.a, params.R, params.lanczos_values),
                                  convolve(params.image_in, static_cast<real>(i), j_vec[3], params.a, params.R, params.lanczos_values) };

            v4sf rounded_vec = __builtin_ia32_roundps(convolve_vec, 0);
            v4sf min_vec = __builtin_ia32_minps(rounded_vec, (v4sf){255.0f, 255.0f, 255.0f, 255.0f});
            v4sf max_vec = __builtin_ia32_maxps(min_vec, (v4sf){0.0f, 0.0f, 0.0f, 0.0f});

            params.image_out(0, j, i) = static_cast<int>(max_vec[0]);
            params.image_out(0, j + 1, i) = static_cast<int>(max_vec[1]);
            params.image_out(0, j + 2, i) = static_cast<int>(max_vec[2]);
            params.image_out(0, j + 3, i) = static_cast<int>(max_vec[3]);
        }

        // Handle remaining iterations
        for (; j < new_height; ++j) {
            params.image_out(0, j, i) = min(255, max(0, static_cast<int>(round(convolve(params.image_in, 
            static_cast<real>(i), static_cast<real>(j), params.a, params.R, params.lanczos_values)))));
        }
    }
}

template <class real>
void process(const string infile, const string outfile, const real R, const int a) {
    // Load image
    jbutil::image<int> image_in;
    ifstream file_in(infile.c_str());
    image_in.load(file_in);
    // Start timer
    double t = jbutil::gettime();

    // Initialize output image with new dimensions
    int new_width = int(image_in.get_cols() * R);
    int new_height = int(image_in.get_rows() * R);
    jbutil::image<int> image_out(new_height, new_width, 1);

    // Allocate memory for lanczos_values
    jbutil::vector<real> lanczos_values;

    // Precompute Lanczos values
    precompute_lanczos(lanczos_values, a, R);

    // Determine the number of threads and chunk size
    const int num_threads = 8;
    int chunk_size = new_width / num_threads; // Base chunk size
    int remainder = new_width % num_threads; // Remainder columns to distribute

    // Create threads
    vector<thread> threads;
    for (int t = 0; t < num_threads; ++t) {
        int start_col = t * chunk_size + min(t, remainder); // Distribute remainder
        int end_col = start_col + chunk_size + (t < remainder ? 1 : 0); // Add 1 to first `remainder` threads
        ResampleParams<real> params = {image_in, image_out, R, a, lanczos_values, start_col, end_col};
        threads.emplace_back(resample_image_chunk<real>, params);
    }

    // Join threads
    for (auto& thread : threads) {
        thread.join();
    }

    // Stop timer
    t = jbutil::gettime() - t;

    // Save image
    ofstream file_out(outfile.c_str());
    image_out.save(file_out);
    // Show time taken
    cerr << "Time taken: " << t << "s" << endl;
}

// Main program entry point
int main(int argc, char *argv[]) {
    cerr << "Lab 2: Image resampling with Lanczos filter" << endl;
    if (argc != 5) {
        cerr << "Usage: " << argv[0]
             << " <infile> <outfile> <scale-factor> <limit>" << endl;
        exit(1);
    }
    process<float>(argv[1], argv[2], atof(argv[3]), atoi(argv[4]));
    return 0;
}