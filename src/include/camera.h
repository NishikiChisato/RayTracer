#ifndef CAMERA_H
#define CAMERA_H

#include <fstream>

#include "color.h"
#include "hittable.h"
#include "material.h"
#include "timer.h"
#include "vec3.h"

namespace raytracer {

using utility::timer;

class camera;

class options {
 public:
  double aspect_ratio_{16.0 / 9.0};
  int image_width_{1600};
  int samples_per_pixel_{10};
  int max_depth_{10};
  double vfov_{90};                 // field of view(vertical view angle)
  point3 lookfrom_{vec3{0, 0, 0}};  // point camera is looking from
  point3 lookat_{vec3{0, 0, -1}};   // point camera is looking at
  vec3 vup_{vec3{0, 1, 0}};         // camera-relative up direction
  double defocus_angle_{0};         // Variation angle of rays through each pixel
  double focus_dis_{10};            // Distance from camera lookfrom point to plane of perfect focus

 private:
  friend camera;

  int image_height_{};

  double viewport_height_;
  double viewport_width_{};

  double focal_length_{};
  vec3 center_{};
  vec3 viewport_u_{};
  vec3 viewport_v_{};
  vec3 pixel_delta_u_{};
  vec3 pixel_delta_v_{};
  vec3 viewport_upper_left_{};
  vec3 pixel00_loc_{};

  double pixel_samples_scale_{};
  vec3 u_{};               // unit vector pointing to camera right
  vec3 v_{};               // unit vector pointing to camera up
  vec3 w_{};               // unit vector pointing opposite the view direction
  vec3 defocus_disk_u_{};  // horizontal
  vec3 defocus_disk_v_{};  // vertical
};

class camera {
 public:
  camera() = default;
  explicit camera(const options& opts) : opts_{opts} {}

  void render(const hittable& world) {
    initialize();
    timer_.report("[render]: calculating pixels...");
    auto pixels_buf = calculate_pixels(world);
    timer_.report("[render]: calculating pixels done.");
    timer_.report("[render]: writing to file...");
    write2file(pixels_buf);
    timer_.report("[render]: writing to file done.");
  }

 private:
  void initialize() {
    opts_.image_height_ = static_cast<int>(opts_.image_width_ / opts_.aspect_ratio_);
    const auto real_aspect_ratio{static_cast<double>(opts_.image_width_) / opts_.image_height_};
    opts_.center_ = opts_.lookfrom_;
    opts_.focal_length_ = (opts_.lookfrom_ - opts_.lookat_).length_squared();
    const auto theta = degrees2radians(opts_.vfov_);
    const auto h = std::tan(theta / 2);
    opts_.viewport_height_ = 2 * h * opts_.focus_dis_;
    opts_.viewport_width_ = opts_.viewport_height_ * real_aspect_ratio;

    opts_.w_ = unit_vec(opts_.lookfrom_ - opts_.lookat_);
    opts_.u_ = unit_vec(cross(opts_.vup_, opts_.w_));
    opts_.v_ = cross(opts_.w_, opts_.u_);

    opts_.viewport_u_ = opts_.viewport_width_ * opts_.u_;
    opts_.viewport_v_ = opts_.viewport_height_ * -opts_.v_;
    opts_.pixel_delta_u_ = opts_.viewport_u_ / opts_.image_width_;
    opts_.pixel_delta_v_ = opts_.viewport_v_ / opts_.image_height_;
    opts_.viewport_upper_left_ = opts_.center_ - (opts_.focus_dis_ * opts_.w_) -
                                 opts_.viewport_u_ / 2 - opts_.viewport_v_ / 2;
    opts_.pixel00_loc_ =
        opts_.viewport_upper_left_ + 0.5 * (opts_.pixel_delta_u_ + opts_.pixel_delta_v_);
    opts_.pixel_samples_scale_ = 1.0 / opts_.samples_per_pixel_;

    // calculating camera defocus disk basis vectors
    const auto defocus_radius =
        opts_.focus_dis_ * std::tan(degrees2radians(opts_.defocus_angle_ / 2));
    opts_.defocus_disk_u_ = opts_.u_ * defocus_radius;
    opts_.defocus_disk_v_ = opts_.v_ * defocus_radius;
  }
  std::vector<color> calculate_pixels(const hittable& world) {
    const auto total_pixels = opts_.image_width_ * opts_.image_height_;
    std::vector<color> pixels_buf{};
    pixels_buf.resize(total_pixels);

#ifdef HAVE_OPENMP
#pragma omp parallel for collapse(2) schedule(dynamic)
#endif
    for (int j = 0; j < opts_.image_height_; j++) {
      for (int i = 0; i < opts_.image_width_; i++) {
        color pixel_color = color{0, 0, 0};
        for (int sample = 0; sample < opts_.samples_per_pixel_; sample++) {
          ray r = get_ray(i, j);
          pixel_color += ray_color(r, opts_.max_depth_, world);
          pixels_buf[(j * opts_.image_width_) + i] = opts_.pixel_samples_scale_ * pixel_color;
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
    std::format_to(std::back_inserter(file_buf), "P3\n{} {}\n255\n", opts_.image_width_,
                   opts_.image_height_);
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
    const color pixel_sample = opts_.pixel00_loc_ + ((i + offset.x()) * opts_.pixel_delta_u_) +
                               ((j + offset.y()) * opts_.pixel_delta_v_);
    const auto ray_cen{opts_.defocus_angle_ <= 0 ? opts_.center_ : defocus_disk_sample()};
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
      ray scattered{};
      color attenuation;
      if (rec.material_->scatter(r, rec, attenuation, scattered)) {
        // reflection occur here!!!
        return attenuation * ray_color(scattered, depth - 1, world);
      }
      return color{0, 0, 0};
    }
    const vec3 unit_dir = unit_vec(r.direction());
    const auto a{0.5 * (unit_dir.y() + 1.0)};
    return (1.0 - a) * color{1.0, 1.0, 1.0} + a * color{0.5, 0.7, 1.0};
  }

  [[nodiscard]] point3 defocus_disk_sample() const {
    auto p = random_in_unit_disk();
    return opts_.center_ + (p.x() * opts_.defocus_disk_u_) + (p.y() * opts_.defocus_disk_v_);
  }

 private:
  options opts_{};
  timer timer_{};
};

}  // namespace raytracer

#endif
