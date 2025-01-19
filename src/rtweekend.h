#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <random>
#include <iostream>
#include <limits>
#include <memory>

// C++ STD usings
using std::make_shared;
using std::shared_ptr;

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Untility Functions
inline double degrees_to_radians(double degress)
{
    return degress * pi / 180.0;
}

inline double random_double()
{
    // Returns a random real in [0,1)
    // "static" keyword makes only one "distribution" object
    // thus, only one init in first call and reused after calles.
    // This makes generated random numbers' consistancy 
    // and have lower overhead.
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max)
{
    // Returns a random real in [min,max)
    static std::uniform_real_distribution<double> distribution(min, max);
    static std::mt19937 generator;
    return distribution(generator);
}

// Common headers
#include "vec3.h"
#include "interval.h"
#include "color.h"
#include "ray.h"

#include "hittable.h"
#include "hittable_list.h"

#endif