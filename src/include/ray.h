#ifndef RAY_H
#define RAY_H

#include "vec3.h"

namespace raytracer {

class ray {
 public:
  ray() = default;
  ray(const point3& origin, const vec3& direction) : ori_{origin}, dir_{direction} {}
  [[nodiscard]] const point3& origin() const {
    return ori_;
  }
  [[nodiscard]] const vec3& direction() const {
    return dir_;
  }
  [[nodiscard]] point3 at(const double t) const {
    return ori_ + t * dir_;
  }

 private:
  point3 ori_;
  vec3 dir_;
};

}  // namespace raytracer

#endif
