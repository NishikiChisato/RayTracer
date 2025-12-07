#ifndef HITTABLE_H
#define HITTABLE_H

#include "interval.h"
#include "ray.h"

namespace raytracer {

class material;

struct hit_record {
  point3 p_{};                            // NOLINT
  vec3 normal_{};                         // NOLINT
  std::shared_ptr<material> material_{};  // NOLINT
  double t_{};                            // NOLINT
  bool front_face_{};                     // NOLINT

  void set_face_normal(const ray& r, const vec3& outward_normal) {
    front_face_ = dot(r.direction(), outward_normal) < 0;
    normal_ = front_face_ ? outward_normal : -outward_normal;
  }
};

class hittable {
 public:
  virtual ~hittable() = default;

  virtual bool hit(const ray& r, const interval& ray_t, hit_record& rec) const = 0;
};

}  // namespace raytracer

#endif
