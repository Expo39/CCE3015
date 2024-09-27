#include "../shared/jbutil.h"
#include <pthread.h>

// Function to be integrated
double f(double x) {
   const double mu = 0.0;
   const double sigma = 1.0;

   return (1 / sqrt(2 * pi * sigma * sigma)) * exp(-pow(x - mu, 2) / (2 * sigma * sigma));
}

// Structure to pass arguments to the thread function
struct ThreadArgs {
    int N;
    int* M;
    double a;
    double b;
    double A;
    double B;
    jbutil::randgen* rng;
};

// Thread-safe Monte Carlo Function with loop unrolling
void* ThreadMC(void* args) {
    ThreadArgs* threadArgs = static_cast<ThreadArgs*>(args);
    int N = threadArgs->N;
    int* M = threadArgs->M;
    double a = threadArgs->a;
    double b = threadArgs->b;
    double A = threadArgs->A;
    double B = threadArgs->B;
    jbutil::randgen* rng = threadArgs->rng;

    int localM = 0;

    // Unroll the loop by a factor of 4
    for (int i = 0; i < N; i += 4) { 
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

    std::cerr << "\nImplementation (" << N << " samples)" << std::endl;

    // start timer
    double t = jbutil::gettime();

    int M = 0;
    const double a = -2.0;
    const double b = 2.0;
    const double A = 0.0;
    const double B = 0.5;

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
    const int samplesPerThread = N / nthreads;
    ThreadArgs threadArgs[nthreads];
    for (int i = 0; i < nthreads; ++i) {
        threadArgs[i] = {samplesPerThread, &results[i], a, b, A, B, &rng[i]};
        pthread_create(&threads[i], nullptr, ThreadMC, &threadArgs[i]);
    }

    // Wait for all threads to complete and combine results
    for (int i = 0; i < nthreads; ++i) {
        pthread_join(threads[i], nullptr);
        M += results[i];
    }

    double estimate = (static_cast<double>(M) / N) * (B - A) * (b - a) + A * (b - a);
    std::cerr << "Estimated integral: " << estimate << std::endl;

    t = jbutil::gettime() - t;
    std::cerr << "Time taken: " << t << "s" << std::endl;

    // Numerical result using error function
    double numericalResult = std::erf(sqrt(2));
    std::cerr << "\nNumerical result using error function: " << numericalResult << std::endl;
}

// Main program
int main() {

    std::cerr << "Lab 1: Monte Carlo integral" << std::endl;
    const int N = int(1E8);
    MonteCarlo(N);
}