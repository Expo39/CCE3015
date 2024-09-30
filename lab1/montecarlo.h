#ifndef MONTECARLO_H
#define MONTECARLO_H

#include "threadargs.h"
#include "../shared/jbutil.h"
#include <pthread.h>

const double a = -2.0;
const double b = 2.0;
const double A = 0.0;
const double B = 0.5;

// Estimation function for integration
double f(double x);

// Thread-safe Monte Carlo Function with loop unrolling
void* ThreadMC(void* args);

// Monte Carlo Method
void MonteCarlo(const int N);

#endif 