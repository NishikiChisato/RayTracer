#ifndef MATERIAL_H
#define MATERIAL_H

#include "color.h"
#include "hittable.h"

namespace raytracer {

class material {
 public:
  virtual ~material() = default;

  virtual bool scatter(const ray& ray_in, const hit_record& rec, color& attenuation,
                       ray& scattered) const {
    return false;
  }
};

}  // namespace raytracer

#endif
