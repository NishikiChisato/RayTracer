#ifndef INTERVAL_H
#define INTERVAL_H

#include "rt.h"

namespace raytracer {

class interval {
 public:
  constexpr interval() : min_{+infinite}, max_{-infinite} {}
  constexpr interval(const double min, const double max) : min_{min}, max_{max} {}

  [[nodiscard]] constexpr double size() const {
    return max_ - min_;
  }
  [[nodiscard]] constexpr bool contains(const double x) const {
    return x >= min_ && x <= max_;
  }
  [[nodiscard]] constexpr bool surround(const double x) const {
    return x > min_ && x < max_;
  }
  [[nodiscard]] constexpr double min() const {
    return min_;
  }
  [[nodiscard]] constexpr double max() const {
    return max_;
  }
  [[nodiscard]] constexpr double clamp(const double x) const {
    if (x < min_) {
      return min_;
    }
    if (x > max_) {
      return max_;
    }
    return x;
  }

 private:
  double min_{};
  double max_{};
};

constexpr interval empty{+infinite, -infinite}, universe{-infinite, +infinite};

}  // namespace raytracer

#endif
