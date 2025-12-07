#include "include/camera.h"
#include "include/color.h"
#include "include/hittable_list.h"
#include "include/material.h"
#include "include/rt.h"
#include "include/sphere.h"

namespace rt = raytracer;

namespace {
bool check_distance(const rt::point3& center, const rt::point3& other, double dis) {
  return (center - other).length() > dis;
}
}  // namespace

int main() {
  rt::options opts{};
  opts.aspect_ratio_ = 16.0 / 9.0;
  opts.image_width_ = 1600;
  opts.samples_per_pixel_ = 250;
  opts.max_depth_ = 50;
  opts.vfov_ = 25;
  opts.lookfrom_ = rt::vec3{11, 3, 8};
  opts.lookat_ = rt::vec3{0, 0, 0};
  opts.vup_ = rt::vec3{0, 1, 0};
  opts.defocus_angle_ = 0.1;
  opts.focus_dis_ = 12;

  rt::camera camera{opts};

  rt::hittable_list world{};

  // ground
  const auto ground_material = std::make_shared<rt::lambertian>(rt::color{0.5, 0.5, 0.5});
  world.add(std::make_shared<rt::sphere>(rt::point3{0, -1000, 0}, 1000, ground_material));

  // special sphere
  constexpr int sphere_height{1};
  constexpr int sphere_radius{1};
  constexpr rt::point3 special_point1{3.5, sphere_height, 0};  // mental
  constexpr rt::point3 special_point2{0, sphere_height, 0};    // dielectric
  constexpr rt::point3 special_point3{-4, sphere_height, 0};   // lambertian
  constexpr rt::point3 special_point4{0, sphere_height, 4};    // hollow glass
  constexpr rt::point3 special_point5{0, sphere_height, 4};    // hollow glass

  constexpr int grid_x{16};
  constexpr int grid_z{16};
  constexpr double obj_r{0.2};

  for (int a = -grid_x; a < grid_x; a++) {
    for (int b = -grid_z; b < grid_z; b++) {
      const auto random_material = rt::random_double();
      rt::point3 center{a + 0.8 * rt::random_double(), obj_r, b + 0.8 * rt::random_double()};
      if (check_distance(special_point1, center, sphere_radius + obj_r) &&
          check_distance(special_point2, center, sphere_radius + obj_r) &&
          check_distance(special_point3, center, sphere_radius + obj_r) &&
          check_distance(special_point4, center, sphere_radius + obj_r)) {
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
  const auto material5 = std::make_shared<rt::dielectric>(1 / 1.5);
  world.add(std::make_shared<rt::sphere>(special_point5, sphere_radius - 0.2, material5));

  camera.render(world);

  return 0;
}
