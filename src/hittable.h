#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"

class material; // forward declaration

class hit_record
{
public:
    point3 p;       // ray hit point
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    double u;
    double v;
    bool front_face;

    void set_face_normal(const ray& r, const vec3& outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable
{
public:
    // This default virtual destructor forces derived class's 
    // destructor to be called when deleted.
    // Otherwise, derived class's destructor cannot be called 
    // and only base class's destructor is called
    // even overrided(implemented) in derived class.
    virtual ~hittable() = default;
    
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;

    virtual aabb bounding_box() const = 0;
};
#endif