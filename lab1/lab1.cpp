#include "montecarlo.h"

using namespace std;

// Estimation function for integration
double f(double x) {
   const double mu = 0.0;
   const double sigma = 1.0;

   return (1 / sqrt(2 * pi * sigma * sigma)) * exp(-pow(x - mu, 2) / (2 * sigma * sigma));
}

// Thread-safe Monte Carlo Function with loop unrolling
void* ThreadMC(void* args) {
    ThreadArgs* threadArgs = static_cast<ThreadArgs*>(args);
    int samples = threadArgs->samples;
    int* M = threadArgs->M;
    jbutil::randgen* rng = threadArgs->rng;

    int localM = 0;

    // Unroll the loop by a factor of 4
    for (int i = 0; i < samples; i += 4) { 
        double x1 = a + (b - a) * rng->fval();
        double y1 = A + (B - A) * rng->fval();
        if (y1 < f(x1)) localM++;

        double x2 = a + (b - a) * rng->fval();
        double y2 = A + (B - A) * rng->fval();
        if (y2 < f(x2)) localM++;

        double x3 = a + (b - a) * rng->fval();
        double y3 = A + (B - A) * rng->fval();
        if (y3 < f(x3)) localM++;

        double x4 = a + (b - a) * rng->fval();
        double y4 = A + (B - A) * rng->fval();
        if (y4 < f(x4)) localM++;
    }

    *M += localM;
    return nullptr;
}

// Monte Carlo Method
void MonteCarlo(const int N) {
    int M = 0;

    cerr << "\nImplementation (" << N << " samples)" << "\n";

    // start timer
    double t = jbutil::gettime();

    // Number of threads available by my personal computer
    const int nthreads = 8;

    // Arrays for threads, results and random numbers
    pthread_t threads[nthreads];
    int results[nthreads] = {0};
    jbutil::randgen rng[nthreads];

    // Initialize random number generators for each thread
    for (int i = 0; i < nthreads; ++i) {
        rng[i] = jbutil::randgen(static_cast<unsigned long>(jbutil::gettime() * 1E6) + i);
    }

    // Divide the work among threads
    const int samples = N / nthreads;
    ThreadArgs threadArgs[nthreads];
    for (int i = 0; i < nthreads; ++i) {
        threadArgs[i] = {samples, &results[i], &rng[i]};
        pthread_create(&threads[i], nullptr, ThreadMC, &threadArgs[i]);
    }

        // Wait for all threads to complete and combine results
    for (int i = 0; i < nthreads; ++i) {
        pthread_join(threads[i], nullptr);
        M += results[i];
    }

    double estimate = (static_cast<double>(M) / N) * (B - A) * (b - a) + A * (b - a);
    cerr << "Estimated integral: " << estimate << "\n";

    t = jbutil::gettime() - t;
    cerr << "Time taken: " << t << "s\n";

    // Numerical result using error function
    double numericalResult = erf(sqrt(2));
    cerr << "\nNumerical result using error function: " << numericalResult << "\n";
}

// Main program
int main() {
    cerr << "Lab 1: Monte Carlo integral\n";
    const int N = int(1E8);
    MonteCarlo(N);
}