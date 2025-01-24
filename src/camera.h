#ifndef CAMERA_H
#define CAMERA_H

#include "material.h"

enum class Render_mode
{
    NORMAL, MATERIAL
};

class camera
{
public:
    // Controlable Image porperties
    int     image_width         = 100;  // Rendered image width in pixel count   
    double  aspect_ratio        = 1.0;  // Ratio of image width over height
    int     samples_per_pixel   = 10;   // Count of random samples for each pixel
    int     max_depth           = 10;   // Maximum number of ray bounces into scene

    // NOTE : own version - local variable to public member
    double focal_length;
    double viewport_height;
    Render_mode render_mode = Render_mode::NORMAL;

    void render(const hittable& world)
    {
        // calls init first
        initialize();

        // Writes ppm file (Render) (by '>' redirection in terminal)
        // Render
        std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for(int j = 0; j < image_height; j++)
        {
            // \r is for CR
            std::clog << "\rScanlines remaining: " << (image_height-j) << ' ' << std::flush;
            for(int i = 0; i < image_width; i++)
            {
                color pixel_color(0,0,0);
                for(int sample = 0; sample < samples_per_pixel; sample++)
                {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                write_color(std::cout, pixel_samples_scale * pixel_color);
            }
        }
        std::clog << "\rDone.                   \n";
    }
    
private:
    int     image_height;           // Desired & Rendered image heigth
    double  pixel_samples_scale;    // Color scale factor for a sum of pixel samples
    point3  center;                 // Camera center
    point3  pixel00_loc;            // Location of pixel 0,0
    vec3    pixel_delta_u;          // Offset to right pixel
    vec3    pixel_delta_v;          // Offset to below pixel

    double viewport_width; // NOTE : own version - local to private member

    void initialize()
    {
        // Caculate the image height, and ensure that it's at least 1
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        // Camera
        center = point3(0,0,0);
        focal_length = 1.0;
        // Viewport width less than 1 are ok since they are real valued.
        viewport_height = 2.0;
        
        // Reason why we don't use aspect_ratio directly is aspect_ratio is ideal value, 
        // so when we direclty use that value, 
        // this might not match with actual integer image size values.
        // (Image height is clamped to have 1 at least and rounded integer value.)
        // static_cast<double>(image_width / image_height); is error, because
        // it divids in int type, rounding down the decimal points, and convert to double.
        viewport_width = viewport_height * static_cast<double>(image_width) / image_height;
        
        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        auto viewport_u = vec3(viewport_width,0,0);
        auto viewport_v = vec3(0,-viewport_height,0);  // inverted with camera coors.

        // Calculate the horizontal and vertical delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel (P_{0,0})
        auto viewport_upper_left = center 
                                - vec3(0,0,focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    color ray_color(const ray& r, int depth, const hittable& world) const
    {
        // If we've exceeded the ray bounce limit, no more light is gathered.
        if(depth <= 0) return color(0,0,0);
        
        hit_record rec;
        
        if(render_mode == Render_mode::NORMAL && world.hit(r, interval(0, infinity), rec))
        {
            // 0.5 is for normalizing ([-1,1] normal range to [0,1] color range)
            return 0.5 * (rec.normal + color(1,1,1));
        }
        // set ray_tmin=0.001 to solve shadow acne problem
        else if(render_mode == Render_mode::MATERIAL && world.hit(r, interval(0.001, infinity), rec))
        {
            ray scattered;
            color attenuation;

            if(rec.mat->scatter(r, rec, attenuation, scattered))
                return attenuation * ray_color(scattered, depth-1, world);
            return color(0,0,0);
        }

        // linear interpolation (lerp) of white to skyblue color along the y height
        vec3 unit_direction = unit_vector(r.direction());
        // y element is in (-1,1)
        // then +1 to (0,2) and *0.5 to (0,1)
        auto a = 0.5*(unit_direction.y()+1.0f);
        // blended value = (1-a) * start value + a * end value
        return (1.0-a)*color(1.0,1.0,1.0) + a*color(0.5,0.7,1.0);
    }

    ray get_ray(int i, int j) const
    {
        // Construct a camera ray originating from the origin and directed at randomly sampled
        // point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc
                          + ((i + offset.x()) * pixel_delta_u)
                          + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = center;
        auto ray_direction = pixel_sample - ray_origin;

        return ray(ray_origin, ray_direction);
    }

    vec3 sample_square() const
    {
        // Returns the vector to a random point in the [-0.5, +0.5] unit square.
        // random_double() is in range [0,1)
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }
    // "sample_disk()" can be used alternativly, and can be found in official github repo.
};
#endif