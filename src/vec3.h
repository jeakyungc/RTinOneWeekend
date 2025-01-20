#ifndef VEC3_H
#define VEC3_H

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

    // Returns [0,1) ranged vector
    static vec3 random()
    {
        return vec3(random_double(), random_double(), random_double());
    }

    // Returns [min,max) ranged vector
    static vec3 random(double min, double max)
    {
        return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
    }
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

inline bool operator==(const vec3& u, const vec3& v)
{
    if(u.e[0] == v.e[0] && u.e[1] == v.e[1] && u.e[2] == v.e[2]) return true;
    return false;
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

inline vec3 random_unit_vector()
{
    while(1)
    {
        auto p = vec3::random(-1,1);
        auto lensq = p.length_squared();
    
        if(1e-160 < lensq && lensq <= 1)
        {
            return p / sqrt(lensq);
            // don't use p.length() for calling overhead
        } 
        // With very small valued element vector, it can underflow to 0
        // and normalizing this vector results in [+-inf, +-inf, +-inf]
        // Thus, we have to cut off small values and double precision type
        // guarantees that the values greater than 10^-160 (=1e-160)
    }
    // Using rejection sampling method.
    // Might worry about while loop overhead. 
    // Total sample area is 2*2*2 cube, and sample accepting area is 4/3*pi*1. 
    // So rejection rate is 1-[(4/3*pi) / 8] = 0.476 = 47%, and 47%+ due to
    // rejecting small vectors.
    // Expected value E can be calculated in "E = 1/p", where p is probability 
    // to be accepcted. (Look for "Geometric Distribution")
    // so, let p = 47%+ = approximatly 50%, than, E = 1/(50%) = 2. Thus, 
    // random vector can be expected to be accepted in 2 loops.
}

inline vec3 random_on_hemisphere(const vec3& normal)
{
    vec3 on_unit_sphere = random_unit_vector();
    // In the same hemisphere as the normal (pointing out from surface)
    if(dot(on_unit_sphere, normal) > 0.0f) return on_unit_sphere;
    else return -on_unit_sphere;
}
#endif