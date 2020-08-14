#include "raytracer.h"

#include "hittablelist.hpp"
#include "sphere.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "movingsphere.hpp"
#include "texture.hpp"
#include "aarect.hpp"
#include "box.hpp"
#include "bvh.hpp"

#include <ctime>

Color ray_color(const Ray &r, const Color &background, const Hittable &world, int depth)
{
    hit_record rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth < 0)
        return Color(0, 0, 0);
    //use eps instead of 0. This gets rid of the shadow acne problem.
    if (!world.hit(r, eps, infinity, rec))
        return background;

    Ray scattered;
    Color attenutaion;
    Color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    double pdf;
    Color albedo;

    if (!rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf))
        return emitted;

    auto on_light = Point3(random_double(213, 343), 554, random_double(227, 332));
    auto to_light = on_light - rec.p;
    auto distance_squared = to_light.length_sqr();
    to_light = unit_vector(to_light);

    if (dot(to_light, rec.norm) < 0)
        return emitted;

    double light_area = (343 - 213) * (332 - 227);
    auto light_cosine = fabs(to_light.y());
    if (light_cosine < eps)
        return emitted;

    pdf = distance_squared / (light_cosine * light_area);
    scattered = Ray(rec.p, to_light, r.time());

    return emitted +
           albedo * rec.mat_ptr->scattering_pdf(r, rec, scattered) *
               ray_color(scattered, background, world, depth - 1) / pdf;
}

HittableList cornell_box()
{
    HittableList objects;

    auto red = make_shared<Lambertian>(make_shared<SolidColor>(.65, .05, .05));
    auto white = make_shared<Lambertian>(make_shared<SolidColor>(.73, .73, .73));
    auto green = make_shared<Lambertian>(make_shared<SolidColor>(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(make_shared<SolidColor>(15, 15, 15));

    objects.add(make_shared<FlipFace>(make_shared<YZRect>(0, 555, 0, 555, 555, green)));
    objects.add(make_shared<YZRect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<FlipFace>(make_shared<XZRect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<FlipFace>(make_shared<XZRect>(0, 555, 0, 555, 0, white)));
    objects.add(make_shared<XZRect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<FlipFace>(make_shared<XYRect>(0, 555, 0, 555, 555, white)));

    shared_ptr<Hittable> box1 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = make_shared<RotateY>(box1, 15);
    box1 = make_shared<Translate>(box1, Vec3(265, 0, 295));
    objects.add(box1);

    shared_ptr<Hittable> box2 = make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = make_shared<RotateY>(box2, -18);
    box2 = make_shared<Translate>(box2, Vec3(130, 0, 65));
    objects.add(box2);

    HittableList world;
    world.add(make_shared<BVHnode>(objects, 0, 1));

    return world;
}

int main()
{
    double t = std::clock();

    // Image
    const auto aspect_ratio = 1.0 / 1.0;
    const int image_width = 500;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 100;
    const int max_depth = 50;

    // World
    auto lights = make_shared<HittableList>();
    lights->add(make_shared<XZRect>(213, 343, 227, 332, 554, shared_ptr<Material>()));
    lights->add(make_shared<Sphere>(Point3(190, 90, 190), 90, shared_ptr<Material>()));

    HittableList world = cornell_box();

    const Color background(0, 0, 0);

    // Camera
    Point3 lookfrom(278, 278, -800);
    Point3 lookat(278, 278, 0);
    Vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;
    auto t0 = 0.0;
    auto t1 = 1.0;

    Camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, t0, t1);

    // Render
    std::cout << "P3\n"
              << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j)
    {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i)
        {
            Color pixel_color(0, 0, 0);
            for (int k = 0; k < samples_per_pixel; ++k)
            {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                Ray r = cam.get_ray(u, v);

                pixel_color += ray_color(r, background, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
        }
    }

    std::cerr << "\nDone.\n";
    std::cerr << (std::clock() - t) / CLOCKS_PER_SEC;
}