/*!
 * \file
 * \brief   Lab 2 - SIMD Programming.
 * \author  Johann A. Briffa
 *
 * Template for the solution to Lab 2 practical exercise on image resampling
 * using the Lanczos filter.
 */

#include "../shared/jbutil.h"

// Resample the image using Lanczos filter

// Precompute Lanczos values
template <class real>
std::vector<real> precompute_lanczos(int a, int R) {
    std::vector<real> lanczos_values(a * R + 1);
    for (int i = 0; i <= a * R; ++i) {
        real x = static_cast<real>(i) / R;
        lanczos_values[i] = (x == 0) ? 1 : (sin(pi * x) / (pi * x)) * (sin(pi * x / a) / (pi * x / a));
    }
    return lanczos_values;
}

// Lanczos function using precomputed values
template <class real>
real lanczos(const std::vector<real>& lanczos_values, real x, int a, int R) {
    int index = std::min(static_cast<int>(std::abs(x) * R), a * R);
    return lanczos_values[index];
}

// Convolution function with loop unrolling
template <class real>
real convolve(const jbutil::image<int>& image, real m, real n, int a, real R, const std::vector<real>& lanczos_values) {
    int m_start = std::max(0, static_cast<int>(std::ceil(m / R - a)));
    int m_end = std::min(image.get_cols() - 1, static_cast<int>(std::floor(m / R + a)));
    int n_start = std::max(0, static_cast<int>(std::ceil(n / R - a)));
    int n_end = std::min(image.get_rows() - 1, static_cast<int>(std::floor(n / R + a)));

    real sum = 0.0;
    real weight = 0.0;

    for (int i = m_start; i <= m_end; ++i) {
        int j = n_start;
        for (; j <= n_end - 3; j += 4) { // Unroll by a factor of 4
            real w0 = lanczos(lanczos_values, std::abs((m / R) - i), a, R) * lanczos(lanczos_values, std::abs((n / R) - j), a, R);
            real w1 = lanczos(lanczos_values, std::abs((m / R) - i), a, R) * lanczos(lanczos_values, std::abs((n / R) - (j + 1)), a, R);
            real w2 = lanczos(lanczos_values, std::abs((m / R) - i), a, R) * lanczos(lanczos_values, std::abs((n / R) - (j + 2)), a, R);
            real w3 = lanczos(lanczos_values, std::abs((m / R) - i), a, R) * lanczos(lanczos_values, std::abs((n / R) - (j + 3)), a, R);

            sum += image(0, j, i) * w0;
            sum += image(0, j + 1, i) * w1;
            sum += image(0, j + 2, i) * w2;
            sum += image(0, j + 3, i) * w3;

            weight += w0;
            weight += w1;
            weight += w2;
            weight += w3;
        }

        // Handle remaining iterations
        for (; j <= n_end; ++j) {
            real w = lanczos(lanczos_values, std::abs((m / R) - i), a, R) * lanczos(lanczos_values, std::abs((n / R) - j), a, R);
            sum += image(0, j, i) * w;
            weight += w;
        }
    }

    return sum / weight;
}

// Compute the convolution for each destination image pixel
template <class real>
void resample_image(const jbutil::image<int>& image_in, jbutil::image<int>& image_out, real R, int a, const std::vector<real>& lanczos_values) {
    int new_width = image_out.get_cols();
    int new_height = image_out.get_rows();

    for (int i = 0; i < new_width; ++i) {
    int j = 0;
    for (; j <= new_height - 4; j += 4) { // Unroll by a factor of 4
        image_out(0, j, i) = std::min(255, std::max(0, round(convolve(image_in, static_cast<real>(i), static_cast<real>(j), a, R, lanczos_values))));
        image_out(0, j + 1, i) = std::min(255, std::max(0, round(convolve(image_in, static_cast<real>(i), static_cast<real>(j + 1), a, R, lanczos_values))));
        image_out(0, j + 2, i) = std::min(255, std::max(0, round(convolve(image_in, static_cast<real>(i), static_cast<real>(j + 2), a, R, lanczos_values))));
        image_out(0, j + 3, i) = std::min(255, std::max(0, round(convolve(image_in, static_cast<real>(i), static_cast<real>(j + 3), a, R, lanczos_values))));
    }

        // Handle remaining iterations
        for (; j < new_height; ++j) {
            image_out(0, j, i) = std::min(255, std::max(0, round(convolve(image_in, static_cast<real>(i), static_cast<real>(j), a, R, lanczos_values))));
        }
    }
}

// Resample the image using Lanczos filter
template <class real>
void process(const std::string infile, const std::string outfile, const real R, const int a) {
    // load image
    jbutil::image<int> image_in;
    std::ifstream file_in(infile.c_str());
    image_in.load(file_in);
    // start timer
    double t = jbutil::gettime();

    // Initialize output image with new dimensions
    int new_width = int(image_in.get_cols() * R);
    int new_height = int(image_in.get_rows() * R);
    jbutil::image<int> image_out(new_height, new_width, 1);

    // Precompute Lanczos values
    std::vector<real> lanczos_values = precompute_lanczos<real>(a, R);

    // Apply Lanczos resampling
    resample_image(image_in, image_out, R, a, lanczos_values);

    // stop timer
    t = jbutil::gettime() - t;
    
    // save image
    std::ofstream file_out(outfile.c_str());
    image_out.save(file_out);
    // show time taken
    std::cerr << "Time taken: " << t << "s" << std::endl;
}

// Main program entry point

int main(int argc, char *argv[])
   {
   std::cerr << "Lab 2: Image resampling with Lanczos filter" << std::endl;
   if (argc != 5)
      {
      std::cerr << "Usage: " << argv[0]
            << " <infile> <outfile> <scale-factor> <limit>" << std::endl;
      exit(1);
      }
   process<float> (argv[1], argv[2], atof(argv[3]), atoi(argv[4]));
   }
