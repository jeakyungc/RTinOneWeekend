#include "rtweekend.h"

#include "bvh.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "sphere.h"

#include <ctime>

#pragma message("Including: " __FILE__)

void bouncing_spheres() 
{
    //World
    hittable_list world;

    // auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    auto checker = make_shared<checker_texture>(0.32, color(0.2, 0.3, 0.1), color(0.9, 0.9, 0.9));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, make_shared<lambertian>(checker)));

    auto material1 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material1));

    auto material2 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    int dcnt, mcnt, dicnt;
    dcnt = mcnt = dicnt = 0;

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    auto center2 = center + vec3(0, random_double(0,0.5), 0);
                    world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
                    dcnt++;
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                    mcnt++;
                }else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                    dicnt++;
                }
            }
        }
    }

    world = hittable_list(make_shared<bvh_node>(world));

    std::clog << dcnt << ' ' << mcnt << ' ' << dicnt << '\n';

    camera cam;
    cam.render_mode = Render_mode::MATERIAL;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 20;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

void checkered_spheres()
{
    hittable_list world;

    auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    world.add(make_shared<sphere>(point3(0,-10, 0), 10, make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));

    camera cam;

    cam.render_mode = Render_mode::MATERIAL;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void earth() {
    hittable_list world;
    
    auto earth_texture = make_shared<image_texture>("earthmap.jpg");
    auto earth_surface = make_shared<lambertian>(earth_texture);
    auto globe = make_shared<sphere>(point3(0,0,-1), 2, earth_surface);
    world.add(globe);
    
    /*
    auto glass_surface = make_shared<dielectric>(1.50);
    auto glass_ball = make_shared<sphere>(point3(2,0,0), 3, glass_surface);
    world.add(glass_ball);
    
    auto ground_checker = make_shared<checker_texture>(0.32, color(.0, .0, .0), color(.9, .9, .9));
    auto ground_surface = make_shared<lambertian>(ground_checker);
    auto ground_geometry= make_shared<sphere>(point3(0,-1004,0), 1000, ground_surface);
    world.add(ground_geometry);
    */

    camera cam;

    cam.render_mode = Render_mode::MATERIAL;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(0,0,15);
    cam.lookat   = point3(0,0,1);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    // cam.render(world);
    cam.render(world);
}

void scene_run(void (*scene_function)())
{
    if(scene_function == nullptr) return;

    clock_t start, finish;
    double duration;
    start = clock();

    scene_function();
    
    finish = clock();

    duration = (double)(finish - start) / CLOCKS_PER_SEC;
    std::clog << duration << "s\n";
}


void (*cmd_input(std::string argv_scene_name))()
{
    if(argv_scene_name == "bouncing_spheres")
        return bouncing_spheres;
    else if(argv_scene_name == "checkered_spheres")
        return checkered_spheres;
    else if(argv_scene_name == "earth")
        return earth;
    else
    {
        std::cerr << "Invalid scene name\n";
        return nullptr;
    }
}

int main(int argc, char *argv[])
{   
    std::clog << "Ray Tracing in One Weekend\n";
    std::clog << "Written by Peter Shirley\n";
    std::clog << "Current Available Scenes:\n \
        [bouncing_spheres], \n \
        [checkered_spheres]\n \
        [earth]\n";

    if(argc != 2)
    {
        std::cerr << "Usage: ./main [scene_name]\n";
        return 1;
    }

    void (*scene_function)() = cmd_input(argv[1]);
    scene_run(scene_function);
}