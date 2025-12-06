#include "include/camera.h"
#include "include/color.h"
#include "include/hittable_list.h"
#include "include/rt.h"
#include "include/sphere.h"
#include "include/timer.h"

namespace rt = raytracer;
namespace rtut = raytracer::utility;

namespace {}  // namespace

int main() {
  constexpr auto aspect_ratio{16.0 / 9.0};
  constexpr auto image_width{3200};
  constexpr auto samples_per_pixel{100};
  constexpr auto max_depth{50};

  rt::camera camera{rt::options{
      image_width,
      aspect_ratio,
      samples_per_pixel,
      max_depth,
  }};

  rt::hittable_list world{};
  world.add(std::make_shared<rt::sphere>(rt::point3{0, 0, -1}, 0.5));
  world.add(std::make_shared<rt::sphere>(rt::point3{0, -100.5, -1}, 100));

  camera.render(world);

  return 0;
}
