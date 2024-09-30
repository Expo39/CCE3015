#ifndef THREADARGS_H
#define THREADARGS_H

#include <atomic>
#include "../shared/jbutil.h"

struct ThreadArgs {
    int samples;
    std::atomic<int>* M;
    jbutil::randgen* rng;

    // Default constructor
    ThreadArgs() : samples(0), M(nullptr), rng(nullptr) {}

    // Constructor to initialize all members
    ThreadArgs(int samples, std::atomic<int>* M, jbutil::randgen* rng)
        : samples(samples), M(M), rng(rng) {}
};

#endif