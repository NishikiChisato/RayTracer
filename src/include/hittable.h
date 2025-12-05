#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

namespace raytracer {

struct hit_record {
  point3 p{};         // NOLINT
  vec3 normal{};      // NOLINT
  double t{};         // NOLINT
  bool front_face{};  // NOLINT

  void set_face_normal(const ray& r, const vec3& outward_normal) {
    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable {
 public:
  virtual ~hittable() = default;

  virtual bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const = 0;
};

}  // namespace raytracer

#endif
