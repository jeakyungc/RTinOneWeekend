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
            // must devied in 255 to make [0,255] ranged value into [0,1].
            auto r = static_cast<double>(j) / (image_width - 1);  
            auto g = static_cast<double>(i) / (image_height - 1);
            auto b = 0.0f;

            // remove denominator and multiply 255.999 to behave as rounding off
            int ir = static_cast<int>(255.999 * r);  
            int ig = static_cast<int>(255.999 * g);
            int ib = static_cast<int>(255.999 * b);

            std::cout << ir << ' ' << ig << ' ' << ib << '\n';
        }
    }
    std::clog << "\rDone.                   \n";
}