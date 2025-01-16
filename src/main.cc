#include "color.h"
#include "vec3.h"

#include <iostream>

int main()
{
    // Image
    int image_width = 256;  // [0,255]
    int image_height =256;

    // Render

    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for(int i = 0; i < image_height; i++)  // [0,255]
    {
        // log. \r is for CR
        std::clog << "\rScanlines remaining: " << (image_height-i) << ' ' << std::flush;
        for(int j = 0; j < image_width; j++)
        {
            // conventionally, color has [0,1] range
            auto pixel_color = color(static_cast<double>(j)/(image_width-1), static_cast<double>(i)/(image_height-1), 0.0f);
            wirte_color(std::cout, pixel_color);
        }
    }
    std::clog << "\rDone.                   \n";
}