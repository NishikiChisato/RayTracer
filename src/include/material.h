#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"

namespace raytracer {

class material {
 public:
  virtual ~material() = default;

  virtual bool scatter([[maybe_unused]] const ray& ray_in, [[maybe_unused]] const hit_record& rec,
                       [[maybe_unused]] color& attenuation, [[maybe_unused]] ray& scattered) const {
    return false;
  }
};

// lambertian(diffuse) reflection material
class lambertian : public material {
 public:
  explicit constexpr lambertian(const color& albedo) : albedo_{albedo} {}

  bool scatter([[maybe_unused]] const ray& ray_in, const hit_record& rec, color& attenuation,
               ray& scattered) const override {
    auto scatter_dir = rec.normal_ + random_unit_vector();
    if (scatter_dir.near_zero()) {
      // if random unit vector is exactly opposite the normal vector
      scatter_dir = rec.normal_;
    }
    // here, we choose always scatter and fixed albedo
    // besides, we can also choose scatter with some fixed probability p
    // and have attenuation be albedo / p
    // the latter strategy requires one random number generation
    // therefore its efficiency is lower
    scattered = ray(rec.p_, scatter_dir);
    attenuation = albedo_;
    return true;
  }

 private:
  color albedo_{};
};

// mirrored reflection
class metal : public material {
 public:
  explicit constexpr metal(const color& albedo, const double fuzz)
      : albedo_{albedo}, fuzz_{fuzz < 1 ? fuzz : 1} {}
  bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation,
               ray& scattered) const override {
    // mirrored reflection
    vec3 reflected = reflect(ray_in.direction(), rec.normal_);
    // fuzz reflection
    reflected = unit_vec(reflected) + (fuzz_ * random_unit_vector());
    scattered = ray(rec.p_, reflected);
    attenuation = albedo_;
    // check if the scattered direction is in the same direction of normal direction
    return (dot(scattered.direction(), rec.normal_) > 0);
  }

 private:
  color albedo_{};
  double fuzz_{};
};

// refraction
class dielectric : public material {
 public:
  explicit constexpr dielectric(const double refraction_index)
      : relative_refractive_index_{refraction_index} {}
  bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation,
               ray& scattered) const override {
    attenuation = color{1.0, 1.0, 1.0};
    // if rec.front_face_ is true, means that incident ray has opposite direction of normal.
    // since our refraction_index_ is current material's refractive index over
    // enclosing material's refractive index, so we need to reverse it
    const double ri =
        rec.front_face_ ? (1.0 / relative_refractive_index_) : relative_refractive_index_;
    const auto unit_incident = unit_vec(ray_in.direction());

    const double cos_theta = std::fmin(dot(-unit_incident, rec.normal_), 1.0);
    const double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
    const bool cannot_refract = ri * sin_theta > 1.0;

    vec3 dir{};
    if (cannot_refract || reflectance(cos_theta, ri) > random_double()) {
      // cann't refract, which is total internal reflection
      dir = reflect(unit_incident, rec.normal_);
    } else {
      dir = refract(unit_incident, rec.normal_, ri);
    }

    scattered = ray{rec.p_, dir};
    return true;
  }

 private:
  // relative refraction index, which is the ratio of material's refractive index over the
  // refractive index of the enclosing material
  double relative_refractive_index_{};

  // Schlick Approximation
  static double reflectance(const double cosine, const double relative_refractive_index) {
    auto r0 = (1 - relative_refractive_index) / (1 + relative_refractive_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }
};

}  // namespace raytracer

#endif
