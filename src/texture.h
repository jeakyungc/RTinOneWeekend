#ifndef TEXTURE_H
#define TEXTURE_H

class texture
{   
public:
    virtual ~texture() = default;

    virtual color value(double u, double v, const point3& p) const = 0;
    // u,v are texture coordinates
    // p is the point in 3D space
    // returns a color value
};

class solid_color : public texture
// texture with a single color - albedo (or rgb value that calls delgating constructor)
{
public:
    solid_color(const color& albedo) : albedo(albedo) {}

    solid_color(double red, double green, double blue) : solid_color(color(red,green,blue)) {}

    color value(double u, double v, const point3& p) const override
    {
        return albedo;
    }

private:
    color albedo;
};

class checker_texture : public texture
{
public:
    // Takes two textures as input and scale
    checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
        : inv_scale(1.0/scale), even(even), odd(odd) {}
    
    // Takes two colors as input and scale (calles delgating constructor by making color to solid_color texture)
    checker_texture(double scale, const color& c1, const color& c2)
        : checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

    color value(double u, double v, const point3& p) const override
    {
        // Convert and scale 3d point to integer values
        auto xInteger = static_cast<int>(std::floor(inv_scale * p.x()));
        auto yInteger = static_cast<int>(std::floor(inv_scale * p.y()));
        auto zInteger = static_cast<int>(std::floor(inv_scale * p.z()));

        // Check Integer sum is even/odd
        bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

        // 
        return isEven ? even->value(u, v, p) : odd->value(u, v, p);
    }

private:
    double inv_scale;
    shared_ptr<texture> even;
    shared_ptr<texture> odd;
};

#endif
