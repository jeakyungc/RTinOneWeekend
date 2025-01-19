#ifndef COLOR_H
#define COLOR_H

using color = vec3;

void write_color(std::ostream& out, const color& pixel_color)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // This ensures samples averaged value to clamped in range [0,1)
    static const interval intensity(0.000, 0.999);
    // Translate [0,1] to [0,255]
    // remove denominator and multiply 255.999 to behave as rounding off
    int rbyte = static_cast<int>(255.999 * intensity.clamp(r));
    int gbyte = static_cast<int>(255.999 * intensity.clamp(g));
    int bbyte = static_cast<int>(255.999 * intensity.clamp(b));

    // Write to ostream
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}
#endif