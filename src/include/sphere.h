#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "vec3.h"

namespace raytracer {

class sphere : public hittable {
 public:
  sphere(const point3& center, const double radius, const std::shared_ptr<material>& material)
      : center_{center}, radius_{std::fmax(0, radius)}, material_{material} {}

  bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override {
    vec3 oc = center_ - r.origin();
    const auto a = r.direction().length_squared();
    const auto h = dot(r.direction(), oc);
    const auto c = oc.length_squared() - radius_ * radius_;
    const auto discriminant = h * h - a * c;
    if (discriminant < 0.0) {
      return false;
    }
    const auto sqrtd = std::sqrt(discriminant);
    auto root = (h - sqrtd) / a;
    if (!ray_t.surround(root)) {
      root = (h + sqrtd) / a;
      if (!ray_t.surround(root)) {
        return false;
      }
    }
    rec.t_ = root;
    rec.p_ = r.at(rec.t_);
    vec3 outward_normal = (rec.p_ - center_) / radius_;
    rec.set_face_normal(r, outward_normal);
    rec.material_ = material_;
    return true;
  }

 private:
  point3 center_{};
  double radius_{};
  std::shared_ptr<material> material_;
};

}  // namespace raytracer

#endif
