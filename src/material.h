#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"

class material
{
public:
    virtual ~material() = default;

    virtual bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const
    {
        return false;
    }
};

class lambertian : public material
{
public:
    // lambertian(const color& albedo) : albedo(albedo) {}
    // albedo is now embedded in texture
    lambertian(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    lambertian(shared_ptr<texture> tex) : tex(tex) {}

    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector(); // Creates lambertian dist.

        // Catch degenerate scatter direction
        if(scatter_direction.near_zero()) scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction, r_in.time());
        attenuation = tex->value(rec.u, rec.v, rec.p); // attenuation is fractured reflectance form.
        return true;
    }

private:
    // color albedo;
    shared_ptr<texture> tex;
};

class metal : public material
{
public:
    metal(const color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}
    // if fuzz is 0, just specular reflection occurs (note that this is not TIR). 
    // if fuzz is (>1), clamp to 1.
    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override
    {
        vec3 reflected = reflect(r_in.direction(), rec.normal);
        reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
        scattered = ray(rec.p, reflected, r_in.time());
        attenuation = albedo;

        // if fuzzed ray is under the surface, return false (absorbed)
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    double fuzz; // radius of fuzz sphere, 
};

class dielectric : public material
{
public:
    dielectric(double refraction_index) : refraction_index(refraction_index) {}

    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override
    {
        // glass surface absorbs nothing, so attenuation is always 1. (transparent)
        attenuation = color(1.0,1.0,1.0);

        double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

        vec3 unit_direction = unit_vector(r_in.direction());
        auto cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
        auto sin_theta = std::sqrt(1.0-cos_theta*cos_theta);

        bool cannot_refract = ri * sin_theta > 1.0;
        vec3 direction;

        if(cannot_refract || reflectance(cos_theta, ri) > random_double()) // bigger than critical angle
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, ri); 
        
        scattered = ray(rec.p, direction, r_in.time());
        return true;
    }

private:
    // Refractive index in vacuum(1.0) or air(1.000293 = 1.0), or the ratio of the material's refractive index over
    // the refractive index of the enclosing media
    double refraction_index;
    //color albedo

    static double reflectance(double cosine, double refraction_index)
    {
        // Use Schlick's approximation for reflectance
        auto r0 = (1-refraction_index) / (1+refraction_index);
        r0 *= r0;
        return r0 + (1-r0)*std::pow((1-cosine), 5);
    }
};

#endif
