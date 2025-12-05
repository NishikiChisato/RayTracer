#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <memory>
#include <vector>

#include "hittable.h"

namespace raytracer {

class hittable_list : public hittable {
 public:
  hittable_list() = default;
  explicit hittable_list(const std::shared_ptr<hittable>& obj) {
    add(obj);
  }

  void clear() {
    objects_.clear();
  }

  void add(const std::shared_ptr<hittable>& obj) {
    objects_.push_back(obj);
  }

  bool hit(const ray& r, double ray_tmin, double ray_tmax, hit_record& rec) const override {
    hit_record tmp_rec{};
    bool hit_anything{false};
    auto closest_t{ray_tmax};
    for (const auto& obj : objects_) {
      if (obj->hit(r, ray_tmin, closest_t, tmp_rec)) {
        hit_anything = true;
        closest_t = tmp_rec.t;
        rec = tmp_rec;
      }
    }
    return hit_anything;
  }

 private:
  std::vector<std::shared_ptr<hittable>> objects_{};
};

}  // namespace raytracer

#endif
