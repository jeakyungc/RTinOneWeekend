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
    
    double  vfov = 90;                  // Vertical view angle (field of view)
    point3  lookfrom = point3(0,0,0);   // Point that camera is looking from
    point3  lookat   = point3(0,0,-1);  // Point that camera is looking at
    vec3    vup      = vec3(0,1,0);     // Camera-relative "up" direction

    double defocus_angle = 0;           // Variabtion angle of rays through each pixel
    double focus_dist = 10;             // Distance from camera lookfrom point to plane of perfect focus


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

    vec3    u, v, w;                // Camera frame basis vectors
    vec3    defocus_disk_u;         // Defocus disk horizontal radius.
    vec3    defocus_disk_v;         // Defocus disk vertical radius. (basis)


    void initialize()
    {
        // Caculate the image height, and ensure that it's at least 1
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determin viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = std::tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        
        // Viewport width less than 1 are ok since they are real valued.
        // Reason why we don't use aspect_ratio directly is aspect_ratio is ideal value, 
        // so when we direclty use that value, 
        // this might not match with actual integer image size values.
        // (Image height is clamped to have 1 at least and rounded integer value.)
        // static_cast<double>(image_width / image_height); is error, because
        // it divids in int type, rounding down the decimal points, and convert to double.
        auto viewport_width = viewport_height * static_cast<double>(image_width) / image_height;
        
        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge
        // inverted with camera coors.

        // Calculate the horizontal and vertical delta vectors from pixel to pixel
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel (P_{0,0})
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the camera defocus disk basis vectors.
        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle/ 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
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
            // out params
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

        // ray orignates randomly when defocus_angle is larger than 0.
        // defocus_angle = 0 can be thought as pinhole and no defocus blur
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
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


    point3 defocus_disk_sample() const
    {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};
#endif