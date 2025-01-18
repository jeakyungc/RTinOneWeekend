#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable
{
public:
    sphere(const point3& center, double radius) 
    : center(center), radius(std::fmax(0,radius)) {}
    // clamps radius range to [0,radius]

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        vec3 oc = center - r.origin();
        auto a = dot(r.direction(), r.direction());
        auto h = dot(r.direction(), oc);
        auto c = dot(oc, oc) - radius * radius;

        double discriminant = h*h - a*c;
        if(discriminant < 0) return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range [tmin, tmax]
        auto root = (h - sqrtd) / a;
        if(!ray_t.contains(root))
        {
            // try test with (+) root
            root = (h + sqrtd) / a;
            if(!ray_t.contains(root)) return false;
        }

        rec.t = root;
        rec.p = r.at(root);
        // normal is unit vector
        // simply divide with radius value instead using unit_vector function.
        vec3 outward_normal = (rec.p - center) / radius;
        rec.set_face_normal(r, outward_normal);

        return true;
    }

private:
    point3 center;
    double radius;
};

#endif