#include "include/hittable_list.h"
#include "include/rt.h"
#include "include/sphere.h"

namespace rt = raytracer;
namespace rtut = raytracer::utility;

struct Options {
  int image_width{};
  int image_height{};
  double aspect_ratio{};
  double viewport_height{};
  double viewport_width{};
  double focal_length{};
  rt::vec3 camera_center{};
  rt::vec3 viewport_u{};
  rt::vec3 viewport_v{};
  rt::vec3 pixel_delta_u{};
  rt::vec3 pixel_delta_v{};
  rt::vec3 viewport_upper_left{};
  rt::vec3 pixel00_loc{};
};

namespace {

constexpr std::size_t kMaxBufSize{1024Z * 1024Z};

rtut::Timer timer{};

rt::color ray_color(const rt::ray& r, const rt::hittable& world) {
  rt::hit_record rec{};
  if (world.hit(r, 0, rt::infinite, rec)) {
    return 0.5 * (rec.normal + rt::color{1, 1, 1});
  }
  const rt::vec3 unit_dir = rt::unit_vec(r.direction());
  const auto a{0.5 * (unit_dir.y() + 1.0)};
  return (1.0 - a) * rt::color{1.0, 1.0, 1.0} + a * rt::color{0.5, 0.7, 1.0};
}

std::vector<rt::color> Render(const Options& opts, const rt::hittable& world) {
  const auto image_width = opts.image_width;
  const auto image_height = opts.image_height;
  const auto camera_center = opts.camera_center;
  const auto pixel00_loc = opts.pixel00_loc;
  const auto pixel_delta_u = opts.pixel_delta_u;
  const auto pixel_delta_v = opts.pixel_delta_v;

  std::vector<rt::color> pixels_buf{};
  pixels_buf.resize(image_width * image_height);  // NOLINT

  // #ifdef HAVE_OPENMP
  // #pragma omp parallel for collapse(2) schedule(dynamic)
  // #endif
  for (int j = 0; j < image_height; j++) {
    for (int i = 0; i < image_width; i++) {
      const rt::color pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
      const auto ray_dir{pixel_center - camera_center};
      const rt::ray r{camera_center, ray_dir};
      const auto pixel_color = ray_color(r, world);
      pixels_buf[(j * image_width) + i] = pixel_color;
    }
  }

  return pixels_buf;
}

template <typename Pixels>
void Write2File(const std::vector<Pixels>& pixels_buf, const Options& opts) {
  const auto& [image_width, image_height, aspect_ratio, viewport_height, viewport_width,
               focal_length, camera_center, viewport_u, viewport_v, pixel_delta_u, pixel_delta_v,
               viewport_upper_left, pixel00_loc] = opts;

  std::ofstream ofs("output.ppm", std::ios::trunc | std::ios::binary);
  std::vector<char> file_buf{};
  file_buf.reserve(kMaxBufSize);
  std::format_to(std::back_inserter(file_buf), "P6\n{} {}\n255\n", image_width, image_height);
  for (const auto& pixel : pixels_buf) {
    std::format_to(std::back_inserter(file_buf), "{}", rt::as_color(pixel));
  }
  ofs.write(file_buf.data(), std::ssize(file_buf));
}

}  // namespace

int main() {
  constexpr auto aspect_ratio{16.0 / 9.0};
  constexpr auto image_width{3200};
  constexpr auto image_height{static_cast<int>(image_width / aspect_ratio)};
  constexpr auto real_aspect_ratio{static_cast<double>(image_width) / image_height};
  constexpr auto viewport_height{2.0};
  constexpr auto viewport_width{viewport_height * real_aspect_ratio};
  constexpr auto focal_length{1.0};
  constexpr auto camera_center{rt::vec3{0, 0, 0}};
  constexpr auto viewport_u{rt::vec3{viewport_width, 0, 0}};
  constexpr auto viewport_v{rt::vec3{0, -viewport_height, 0}};
  constexpr auto pixel_delta_u{viewport_u / image_width};
  constexpr auto pixel_delta_v{viewport_v / image_height};
  constexpr auto viewport_upper_left{camera_center - rt::vec3{0, 0, focal_length} - viewport_u / 2 -
                                     viewport_v / 2};
  constexpr auto pixel00_loc{viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v)};

  Options opts{.image_width = image_width,
               .image_height = image_height,
               .aspect_ratio = aspect_ratio,
               .viewport_height = viewport_height,
               .viewport_width = viewport_width,
               .focal_length = focal_length,
               .camera_center = camera_center,
               .viewport_u = viewport_u,
               .viewport_v = viewport_v,
               .pixel_delta_u = pixel_delta_u,
               .pixel_delta_v = pixel_delta_v,
               .viewport_upper_left = viewport_upper_left,
               .pixel00_loc = pixel00_loc};

  timer.report("World setuping...");
  rt::hittable_list world{};
  world.add(std::make_shared<rt::sphere>(rt::point3{0, 0, -1}, 0.5));
  world.add(std::make_shared<rt::sphere>(rt::point3{0, -100.5, -1}, 100));

  timer.report("World setup done. Start to rendering");
  auto pixels_buf = Render(opts, world);

  timer.report("Rendering done. Writing to file...");
  Write2File(pixels_buf, opts);
  timer.report("Writing file done.");

  return 0;
}
