#include "../shared/jbutil.h"
#include <thread>

// Function to be integrated
double f(double x) {
   double  mu = 0.0;
   double sigma = 1.0;

   return (1 / sqrt(2 * pi * sigma  * sigma)) * exp (-pow(x - mu, 2) / (2 * sigma * sigma));
}

// Thread-safe Monte Carlo Function with loop unrolling
void ThreadMC(const int N, int& M, const double a, const double b, const double A, const double B, jbutil::randgen& rng) {
    int localM = 0;
    for (int i = 0; i < N; i += 4) { // Unroll the loop by a factor of 4
        double x1 = a + (b - a) * rng.fval();
        double y1 = A + (B - A) * rng.fval();
        if (y1 < f(x1)) localM++;

        double x2 = a + (b - a) * rng.fval();
        double y2 = A + (B - A) * rng.fval();
        if (y2 < f(x2)) localM++;

        double x3 = a + (b - a) * rng.fval();
        double y3 = A + (B - A) * rng.fval();
        if (y3 < f(x3)) localM++;

        double x4 = a + (b - a) * rng.fval();
        double y4 = A + (B - A) * rng.fval();
        if (y4 < f(x4)) localM++;
    }

    M += localM;
}

// Monte Carlo Method
void MonteCarlo(const int N) {

    std::cerr << "\nImplementation (" << N << " samples)" << std::endl;

    // start timer
    double t = jbutil::gettime();

    int M = 0;
    const double a = -2.0;
    const double b = 2.0;
    const double A = 0.0;
    const double B = 0.5;

    // Number of threads
    const int nthreads = 4;

    // Dynamic arrays for threads, results and random numbers
    jbutil::vector<std::thread> threads;
    jbutil::vector<int> results(nthreads, 0);
    jbutil::vector<jbutil::randgen> rng;

    // Initialize random number generators for each thread
    for (int i = 0; i < nthreads; ++i) {
        rng.emplace_back(static_cast<unsigned long>(jbutil::gettime() * 1E6) + i);
    }

    // Divide the work among threads
    const int samplesPerThread = N / nthreads;
    for (int i = 0; i < nthreads; ++i) {
        threads.emplace_back(ThreadMC, samplesPerThread, std::ref(results[i]), a, b, A, B, std::ref(rng[i]));
    }

    // Wait for all threads to complete and combine results
    for (int i = 0; i < nthreads; ++i) {
        threads[i].join();
        M += results[i];
    }

    double estimate = (static_cast<double>(M) / N) * (B-A) * (b-a) + A * (b-a);
    std::cerr << "Estimated integral: " << estimate << std::endl;

    t = jbutil::gettime() - t;
    std::cerr << "Time taken: " << t << "s" << std::endl;

    // Numerical result using error function
    double numericalResult = std::erf(sqrt(2));
    std::cerr << "\nNumerical result using error function: " << numericalResult << std::endl;

}

//Main program
int main() {

    std::cerr << "Lab 1: Monte Carlo integral" << std::endl;
    const int N = int(1E8);
    MonteCarlo(N);
}



// #include <iostream>
// #include <cmath>
// #include <ctime>
// #include "../shared/jbutil.h"


// // Function to be integrated
// double f(double x) {
//    double  mu = 0.0;
//    double sigma = 1.0;


//    return (1 / sqrt(2 * pi * sigma  * sigma)) * exp (-pow(x - mu, 2) / (2 * sigma * sigma));
// }


// double randomInterval() {
//    return (rand() + 1.0) / (RAND_MAX + 2.0);
// }


// // Monte Carlo Method
// void MonteCarlo(const int N) {


//    std::cerr << "\nImplementation (" << N << " samples)" << std::endl;


//    // start timer
//   double t = jbutil::gettime();


//    int M = 0;
//    double a = -2.0;
//    double b = 2.0;
//    double A = 0.0;
//    double B = 0.5;


//    for (int i = 0; i < N; ++i) {
//        double xi = randomInterval() * (b-a) + a;
//        double yi = randomInterval() * (B-A) + A;


//        if (yi < f(xi)) {
//            ++M;
//        }
//     }


//     double estimate = (static_cast<double>(M) / N) * (B-A) * (b-a) + A * (b-a);
//     std::cerr << "Estimated integral: " << estimate << std::endl;


//   t = jbutil::gettime() - t;
//   std::cerr << "Time taken: " << t << "s" << std::endl;


// }


// //Main program
// int main() {


//    std::cerr << "Lab 1: Monte Carlo integral" << std::endl;
//    const int N = int(1E8);
//    MonteCarlo(N);
// }

