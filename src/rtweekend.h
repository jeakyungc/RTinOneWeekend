#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
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

// Common headers
#include "vec3.h"
#include "interval.h"
#include "color.h"
#include "ray.h"

#endif