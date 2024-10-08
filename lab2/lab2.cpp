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

// Lanczos function
template <class real>
real lanczos(real x, int a) {
    if (x == 0) return 1;
    if (x < -a || x > a) return 0;
    return (sin(pi * x) / (pi * x)) * (sin(pi * x / a) / (pi * x / a));
}

// Convolution function
template <class real>
real convolve(const jbutil::image<int>& image, real m, real n, int a, real R) {
    int m_start = std::max(0, static_cast<int>(std::ceil(m / R - a)));
    int m_end = std::min(image.get_cols() - 1, static_cast<int>(std::floor(m / R + a)));
    int n_start = std::max(0, static_cast<int>(std::ceil(n / R - a)));
    int n_end = std::min(image.get_rows() - 1, static_cast<int>(std::floor(n / R + a)));

    real sum = 0.0;
    real weight = 0.0;

    // TODO: Make more efficient
    for (int i = m_start; i <= m_end; ++i) {
        for (int j = n_start; j <= n_end; ++j) {
            real w = lanczos((m / R) - i, a) * lanczos((n / R) - j, a);
            sum += image(0, j, i) * w; // Note: image(0, j, i) accesses the pixel at (i, j) in the first channel
            weight += w;
        }
    }

    return sum / weight;
}

// Compute the convolution for each destination image pixel
template <class real>
void resample_image(const jbutil::image<int>& image_in, jbutil::image<int>& image_out, real R, int a) {
    int new_width = image_out.get_cols();
    int new_height = image_out.get_rows();

    for (int i = 0; i < new_width; ++i) {
        for (int j = 0; j < new_height; ++j) {
            image_out(0, j, i) = std::min(255, std::max(0, round(convolve(image_in, static_cast<real>(i), static_cast<real>(j), a, R))));
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

    // Apply Lanczos resampling
    resample_image(image_in, image_out, R, a);

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
