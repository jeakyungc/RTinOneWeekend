#include "color.h"
#include "vec3.h"
#include "ray.h"

#include <iostream>

bool hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = center - r.origin();  // C - Q
    auto a = dot(r.direction(), r.direction());
    auto b = -2.0f * dot(r.direction(), oc);
    auto c = dot(oc, oc) - radius * radius;

    auto discriminat = b*b - 4*a*c;
    
    return (discriminat >= 0); // if >=0, there is 1 or 2 real value solution.
}

color ray_color(const ray& r)
{
    // Place sphere centered at (0,0,-1) with radius 0.5f,
    // thus, intersection of viewport plane and sphere makes circle paralle to xy plane.
    // Return red color if ray intersects with ray from camera eye.
    if (hit_sphere(point3(0,0,-1), 0.5f, r)) return color(1,0,0); 

    // linear interpolation (lerp) of white to skyblue color along the y height
    vec3 unit_direction = unit_vector(r.direction());
    // y element is in (-1,1)
    // then +1 to (0,2) and *0.5 to (0,1)
    auto a = 0.5*(unit_direction.y()+1.0f);
    // blended value = (1-a) * start value + a * end value
    return (1.0f-a)*color(1.0f,1.0f,1.0f) + a*color(0.5f,0.7f,1.0f);
}


int main()
{
    // Image
    auto aspect_ratio = 16.0f / 9.0f;
    int image_width = 400;
    
    // Caculate the image height, and ensure that it's at least 1
    int image_height = static_cast<int>(image_width / aspect_ratio);
    image_height = (image_height < 1) ? 1 : image_height;


    // Camera
    auto focal_length = 1.0f;
    // Viewport width less than 1 are ok since they are real valued.
    auto viewport_height = 2.0f;
    // Reason why we don't use aspect_ratio directly is aspect_ratio is ideal value, 
    // so when we direclty use that value, 
    // this might not match with actual integer image size values.
    // (Image height is clamped to have 1 at least and rounded integer value.)
    auto viewport_width = viewport_height * static_cast<double>(image_width) / image_height;
    // static_cast<double>(image_width / image_height); is error, because
    // it divids in int type, rounding down the decimal points, and convert to double.
    auto camera_center = point3(0,0,0);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    auto viewport_u = vec3(viewport_width,0,0);
    auto viewport_v = vec3(0,-viewport_height,0);  // inverted with camera coors.

    // Calculate the horizontal and vertical delta vectors from pixel to pixel
    auto pixel_delta_u = viewport_u / image_width;
    auto pixel_delta_v = viewport_v / image_height;

    // Calculate the location of the upper left pixel (P_{0,0})
    auto viewport_upper_left = camera_center 
                             - vec3(0,0,focal_length) - viewport_u / 2 - viewport_v / 2;
    auto pixel00_loc = viewport_upper_left + 0.5f * (pixel_delta_u + pixel_delta_v);

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int j = 0; j < image_height; j++)
    {
        // log. \r is for CR
        std::clog << "\rScanlines remaining: " << (image_height-j) << ' ' << std::flush;
        for(int i = 0; i < image_width; i++)
        {
            // conventionally, color has [0,1] range
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            ray r(camera_center, ray_direction);

            auto pixel_color = ray_color(r);
            wirte_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                   \n";
}