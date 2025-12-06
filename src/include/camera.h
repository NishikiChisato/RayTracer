#ifndef CAMERA_H
#define CAMERA_H

#include <fstream>

#include "color.h"
#include "hittable.h"
#include "timer.h"

namespace raytracer {

using utility::timer;

class camera;

class options {
 public:
  options() = default;
  options(const int image_width, const double aspect_ratio, const int samples_per_pixel,
          const int max_depth)
      : image_width{image_width},
        aspect_ratio{aspect_ratio},
        samples_per_pixel{samples_per_pixel},
        max_depth{max_depth} {}

 private:
  friend camera;

  int image_width{1600};
  double aspect_ratio{16.0 / 9.0};
  int image_height{static_cast<int>(image_width / aspect_ratio)};

  double viewport_height{2.0};
  double viewport_width{};

  double focal_length{1.0};
  vec3 camera_center{vec3{0, 0, 0}};
  vec3 viewport_u{};
  vec3 viewport_v{};
  vec3 pixel_delta_u{};
  vec3 pixel_delta_v{};
  vec3 viewport_upper_left{};
  vec3 pixel00_loc{};

  int samples_per_pixel{10};
  double pixel_samples_scale{1.0 / samples_per_pixel};
  int max_depth{10};
};

class camera {
 public:
  camera() = default;
  explicit camera(const options& opts) : opts_{opts} {}

  void render(const hittable& world) {
    initialize();
    timer_.report("[render]: calculaeing pixels...");
    auto pixels_buf = calculate_pixels(world);
    timer_.report("[render]: calculaeing pixels done.");
    timer_.report("[render]: writing to file...");
    write2file(pixels_buf);
    timer_.report("[render]: writing to file done.");
  }

 private:
  void initialize() {
    const auto real_aspect_ratio{static_cast<double>(opts_.image_width) / opts_.image_height};
    opts_.viewport_width = opts_.viewport_height * real_aspect_ratio;
    opts_.viewport_u = vec3{opts_.viewport_width, 0, 0};
    opts_.viewport_v = vec3{0, -opts_.viewport_height, 0};
    opts_.pixel_delta_u = opts_.viewport_u / opts_.image_width;
    opts_.pixel_delta_v = opts_.viewport_v / opts_.image_height;
    opts_.viewport_upper_left = opts_.camera_center - vec3{0, 0, opts_.focal_length} -
                                opts_.viewport_u / 2 - opts_.viewport_v / 2;
    opts_.pixel00_loc =
        opts_.viewport_upper_left + 0.5 * (opts_.pixel_delta_u + opts_.pixel_delta_v);
  }
  std::vector<color> calculate_pixels(const hittable& world) {
    const auto total_pixels = opts_.image_width * opts_.image_height;
    std::vector<color> pixels_buf{};
    pixels_buf.resize(total_pixels);

#ifdef HAVE_OPENMP
#pragma omp parallel for collapse(2) schedule(dynamic)
#endif
    for (int j = 0; j < opts_.image_height; j++) {
      for (int i = 0; i < opts_.image_width; i++) {
        color pixel_color = color{0, 0, 0};
        for (int sample = 0; sample < opts_.samples_per_pixel; sample++) {
          ray r = get_ray(i, j);
          pixel_color += ray_color(r, opts_.max_depth, world);
          pixels_buf[(j * opts_.image_width) + i] = opts_.pixel_samples_scale * pixel_color;
        }
      }
    }

    return pixels_buf;
  }

  template <typename Pixels>
  void write2file(const std::vector<Pixels>& pixels_buf) {
    std::ofstream ofs("output.ppm", std::ios::trunc | std::ios::binary);
    std::vector<char> file_buf{};
    file_buf.reserve(kMaxBufSize);
    std::format_to(std::back_inserter(file_buf), "P3\n{} {}\n255\n", opts_.image_width,
                   opts_.image_height);
    for (const auto& pixel : pixels_buf) {
      std::format_to(std::back_inserter(file_buf), "{}\n", as_color(pixel));
    }
    ofs.write(file_buf.data(), std::ssize(file_buf));
  }

  [[nodiscard]] vec3 sample_square() const {
    // random vector point in [-.5,-.5,0] to [+.5,+.5,0]
    return vec3{random_double() - 0.5, random_double() - 0.5, 0};
  }

  [[nodiscard]] ray get_ray(const int i, const int j) const {
    const auto offset = sample_square();
    const color pixel_sample = opts_.pixel00_loc + ((i + offset.x()) * opts_.pixel_delta_u) +
                               ((j + offset.y()) * opts_.pixel_delta_v);
    const auto ray_cen{opts_.camera_center};
    const auto ray_dir{pixel_sample - ray_cen};
    return {ray_cen, ray_dir};
  }

  [[nodiscard]] color ray_color(const ray& r, int depth, const hittable& world) const {
    if (depth <= 0) {
      return color{0, 0, 0};
    }
    hit_record rec{};
    // the intersection point may result in round to zeor if t is too small
    // we should ignore such root
    if (world.hit(r, interval{0.00001, +infinite}, rec)) {
      vec3 dir = rec.normal + random_unit_vector();
      // reflection occur here!!!
      return 0.5 * ray_color(ray{rec.p, dir}, depth - 1, world);
    }
    const vec3 unit_dir = unit_vec(r.direction());
    const auto a{0.5 * (unit_dir.y() + 1.0)};
    return (1.0 - a) * color{1.0, 1.0, 1.0} + a * color{0.5, 0.7, 1.0};
  }

 private:
  options opts_{};
  timer timer_{};
};

}  // namespace raytracer

#endif
