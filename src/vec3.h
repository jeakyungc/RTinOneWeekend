#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

// most 3D graphics systems uses 4d homogenous coors, but
// our project, 3d coor. system might suffice.

/* Class declariation */ 
class vec3 {
public:
    // Here, double precision is used, 
    // but in some scenarios, single precision 'float' type 
    // might be better option for limited memory conditions.
    double e[3]; 

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }  // return value
    double& operator[](int i) { return e[i]; }  // return ref

    vec3& operator+=(const vec3& v)
    {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;  
        // Must return reference type.
        // When return value type, 
        // it becomes temporary and cannot be reaccessed.
    }

    vec3& operator*=(double t)
    {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t)
    {
        return *this *= 1/t;
    }

    double length_squared() const { return e[0]*e[0]+e[1]*e[1]+e[2]*e[2]; }
    double length() const { return std::sqrt(length_squared()); }
};

/* Point3 is just an alias */
// for vec3, but useful for gemetric clarity in the code, 
// but not having safty check features to prevent 
// misbehaviours like subtracting point on color
using point3 = vec3;

/* Vector Utility Functions */
// inline keyword 
// requests to complier to subtitute 
// corresponding function to code at compile time

// passing variable in reference is just to avoid overhead.
inline std::ostream& operator<<(std::ostream& out, const vec3& v) 
{
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

// element-wise vec3 operation (Hadamard product)
inline vec3 operator+(const vec3& u, const vec3& v)
{
    return vec3(u.e[0]+v.e[0], u.e[1]+v.e[1], u.e[2]+v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v)
{
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v)
{
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3& v)
{
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t)
{
    // use previous overloaded operator
    return t * v;
}

inline vec3 operator/(const vec3& v, double t)
{
    return (1/t) * v;
}

// dot product (scalar)
inline double dot(const vec3& u, const vec3& v)
{
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

// cross product (vector)
inline vec3 cross(const vec3& u, const vec3& v)
{
    return vec3(u.e[1]*v.e[2] - u.e[2]*v.e[1],
                u.e[2]*v.e[0] - u.e[0]*v.e[2],
                u.e[0]*v.e[1] - u.e[1]*v.e[0]);
}

inline vec3 unit_vector(const vec3& v)
{
    // use overloaded operator '/'
    return v / v.length();
}
#endif