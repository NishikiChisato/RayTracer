#include "include/camera.h"
#include "include/color.h"
#include "include/hittable_list.h"
#include "include/material.h"
#include "include/rt.h"
#include "include/sphere.h"
#include "include/timer.h"

namespace rt = raytracer;
namespace rtut = raytracer::utility;

namespace {
bool check_distance(const rt::point3& center, const rt::point3& other, double dis) {
  return (center - other).length() > dis;
}
}  // namespace

int main() {
  rt::options opts{};
  opts.aspect_ratio_ = 16.0 / 9.0;
  opts.image_width_ = 6400;
  opts.samples_per_pixel_ = 1000;
  opts.max_depth_ = 50;
  opts.vfov_ = 25;
  opts.lookfrom_ = rt::vec3{13, 2, 6};
  opts.lookat_ = rt::vec3{0, 0, 0};
  opts.vup_ = rt::vec3{0, 1, 0};
  opts.defocus_angle_ = 0.3;
  opts.focus_dis_ = 13;

  rt::camera camera{opts};

  rt::hittable_list world{};

  // ground
  const auto ground_material = std::make_shared<rt::lambertian>(rt::color{0.5, 0.5, 0.5});
  world.add(std::make_shared<rt::sphere>(rt::point3{0, -1000, 0}, 1000, ground_material));

  // special sphere
  constexpr int sphere_height{1};
  constexpr int sphere_radius{1};
  constexpr rt::point3 special_point1{4, sphere_height, 0};   // mental
  constexpr rt::point3 special_point2{0, sphere_height, 0};   // dielectric
  constexpr rt::point3 special_point3{-4, sphere_height, 0};  // lambertian
  constexpr rt::point3 special_point4{0, sphere_height, 4};   // hollow glass
  constexpr rt::point3 special_point5{0, sphere_height, 4};   // hollow glass

  constexpr int grid_x{11};
  constexpr int grid_z{11};
  constexpr double obj_r{0.2};

  for (int a = -grid_x; a < grid_x; a++) {
    for (int b = -grid_z; b < grid_z; b++) {
      const auto random_material = rt::random_double();
      rt::point3 center{a + 0.9 * rt::random_double(), obj_r, b * 0.9 * rt::random_double()};
      if (check_distance(special_point1, center, 1) && check_distance(special_point2, center, 1) &&
          check_distance(special_point3, center, 1) && check_distance(special_point4, center, 1)) {
        std::shared_ptr<rt::material> sphere_material{};
        if (random_material < 0.7) {
          // diffuse(even reflection)
          const auto albedo = rt::color::random();
          sphere_material = std::make_shared<rt::lambertian>(albedo);
          world.add(std::make_shared<rt::sphere>(center, obj_r, sphere_material));
        } else if (random_material < 0.9) {
          // metal(mirror reflection)
          const auto albedo = rt::color::random(0.5, 1);
          const auto fuzz = rt::random_double(0, 0.5);
          sphere_material = std::make_shared<rt::metal>(albedo, fuzz);
          world.add(std::make_shared<rt::sphere>(center, obj_r, sphere_material));
        } else {
          // glass(refraction or internal mirror reflection)
          sphere_material = std::make_shared<rt::dielectric>(1.5);
          world.add(std::make_shared<rt::sphere>(center, obj_r, sphere_material));
        }
      }
    }
  }

  const auto material1 = std::make_shared<rt::metal>(rt::color{0.7, 0.6, 0.5}, 0.0);
  world.add(std::make_shared<rt::sphere>(special_point1, sphere_radius, material1));
  const auto material2 = std::make_shared<rt::dielectric>(1.5);
  world.add(std::make_shared<rt::sphere>(special_point2, sphere_radius, material2));
  const auto material3 = std::make_shared<rt::lambertian>(rt::color{0.4, 0.2, 0.1});
  world.add(std::make_shared<rt::sphere>(special_point3, sphere_radius, material3));
  const auto material4 = std::make_shared<rt::dielectric>(1.5);
  world.add(std::make_shared<rt::sphere>(special_point4, sphere_radius, material4));
  world.add(std::make_shared<rt::sphere>(special_point5, sphere_radius - 0.2, material4));

  camera.render(world);

  return 0;
}
