#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class sphere : public hittable
{
public:
    // Stationary Sphere
    sphere(const point3& static_center, double radius, shared_ptr<material> mat)
    : center(static_center, vec3(0,0,0)), radius(std::fmax(0,radius)), mat(mat) 
    {
        auto rvec = vec3(radius, radius, radius);
        bbox = aabb(static_center - rvec, static_center + rvec);
    }
    // clamps radius range to [0,radius]

    // Moving Sphere
    sphere(const point3& center1, const point3& center2, double radius, shared_ptr<material> mat)
      : center(center1, center2 - center1), radius(std::fmax(0,radius)), mat(mat) 
    {
        auto rvec = vec3(radius, radius, radius);
        aabb box1(center.at(0) - rvec, center.at(0) + rvec);
        aabb box2(center.at(1) - rvec, center.at(1) + rvec);
        bbox = aabb(box1, box2);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override
    {
        point3 current_center = center.at(r.time());
        vec3 oc = current_center - r.origin();
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
        vec3 outward_normal = (rec.p - current_center) / radius;
        rec.set_face_normal(r, outward_normal);
        get_sphere_uv(outward_normal, rec.u, rec.v);
        rec.mat = mat;

        return true;
    }

private:
    ray center;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;

    static void get_sphere_uv(const point3& p, double& u, double& v)
    {
        // p : a given point on the sphere of radius=1, centere=(0,0,0) (origin).
        // u : returned value [0,1] of angle around the Y axis from X=-1 
        //     --> Z=+1 --> X=+1 --> Z=-1.
        // v : returned value [0,1] of angle from Y=-1 to Y=+1.
        // <1 0 0> : <0.5 0.5>  <-1  0  0> : <0.00 0.50>
        // <0 1 0> : <0.5 1.0>  < 0 -1  0> : <0.50 0.00>
        // <0 0 1> : <0.5 0.5>  < 0  0 -1> : <0.50 0.50>

        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(), p.x()) + pi;

        u = phi / (2*pi);
        v = theta / pi;
    }
};

#endif