#ifndef MATERIAL_H
#define MATERIAL_H

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
    lambertian(const color& albedo) : albedo(albedo) {}

    bool scatter(
        const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered
    ) const override
    {
        auto scatter_direction = rec.normal + random_unit_vector(); // Creates lambertian dist.

        // Catch degenerate scatter direction
        if(scatter_direction.near_zero()) scatter_direction = rec.normal;

        scattered = ray(rec.p, scatter_direction);
        attenuation = albedo; // attenuation is fractured reflectance form.
        return true;
    }

private:
    color albedo;
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
        scattered = ray(rec.p, reflected);
        attenuation = albedo;
        // if fuzzed ray is under the surface, return false (absorbed)
        return (dot(scattered.direction(), rec.normal) > 0);
    }

private:
    color albedo;
    double fuzz; // radius of fuzz sphere, 
};

#endif
