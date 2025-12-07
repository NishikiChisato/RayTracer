#ifndef VEC3_H
#define VEC3_H

#include <array>
#include <cmath>
#include <format>

#include "rt.h"

namespace raytracer {

class vec3 {
 private:
  double x_;
  double y_;
  double z_;

 public:
  constexpr vec3() = default;
  constexpr vec3(const double x, const double y, const double z) : x_{x}, y_{y}, z_{z} {}

  [[nodiscard]] constexpr double x() const {
    return x_;
  }
  [[nodiscard]] constexpr double y() const {
    return y_;
  }
  [[nodiscard]] constexpr double z() const {
    return z_;
  }

  [[nodiscard]] constexpr double length_squared() const {
    return (x_ * x_) + (y_ * y_) + (z_ * z_);
  }

  [[nodiscard]] constexpr double length() const {
    return std::sqrt(length_squared());
  }

  [[nodiscard]] bool near_zero() const {
    const auto eps{1e-8};
    return (std::fabs(x_) < eps) && (std::fabs(y_) < eps) && (std::fabs(z_) < eps);
  }

  static vec3 random() {
    return vec3{random_double(), random_double(), random_double()};
  }

  static vec3 random(const double min, const double max) {
    return vec3{random_double(min, max), random_double(min, max), random_double(min, max)};
  }

  constexpr vec3 operator-() const {
    return vec3{-x_, -y_, -z_};
  }

  constexpr vec3& operator+=(const vec3& rhs) {
    return (x_ += rhs.x(), y_ += rhs.y(), z_ += rhs.z(), *this);
  }
  constexpr vec3& operator*=(const double rhs) {
    return (x_ *= rhs, y_ *= rhs, z_ *= rhs, *this);
  }
  constexpr vec3& operator/=(const double rhs) {
    return *this *= (1 / rhs);
  }
};

constexpr vec3 operator+(const vec3& lhs, const vec3& rhs) {
  return vec3{lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z()};
}

constexpr vec3 operator-(const vec3& lhs, const vec3& rhs) {
  return vec3{lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z()};
}

constexpr vec3 operator*(const vec3& lhs, const vec3& rhs) {
  return vec3{lhs.x() * rhs.x(), lhs.y() * rhs.y(), lhs.z() * rhs.z()};
}

constexpr vec3 operator*(const double lhs, const vec3& rhs) {
  return vec3{lhs * rhs.x(), lhs * rhs.y(), lhs * rhs.z()};
}

constexpr vec3 operator*(const vec3& lhs, const double rhs) {
  return rhs * lhs;
}

constexpr vec3 operator/(const vec3& lhs, const double rhs) {
  return 1 / rhs * lhs;
}

constexpr double dot(const vec3& lhs, const vec3& rhs) {
  return (lhs.x() * rhs.x()) + (lhs.y() * rhs.y()) + (lhs.z() * rhs.z());
}

constexpr vec3 cross(const vec3& lhs, const vec3& rhs) {
  return vec3{(lhs.y() * rhs.z()) - (lhs.z() * rhs.y()), (lhs.z() * rhs.x()) - (lhs.x() * rhs.z()),
              (lhs.x() * rhs.y()) - (lhs.y() * rhs.x())};
}

constexpr vec3 unit_vec(const vec3& vec) {
  return vec / vec.length();
}

inline vec3 random_in_unit_disk() {
  while (true) {
    auto p = vec3{random_double(-1, 1), random_double(-1, 1), 0};
    if (p.length_squared() < 1) {
      return p;
    }
  }
}

inline vec3 random_unit_vector() {
  while (true) {
    auto p = vec3::random(-1, 1);
    auto lensq = p.length_squared();
    if (lensq > 1e-160 && lensq <= 1) {
      return p / std::sqrt(lensq);
    }
  }
}

inline vec3 random_on_hemisphere(const vec3& normal) {
  auto on_sphere_vec = random_unit_vector();
  if (dot(on_sphere_vec, normal) > 0) {
    return on_sphere_vec;
  }
  return -on_sphere_vec;
}

// computing reflection ray of incident ray
inline vec3 reflect(const vec3& incident, const vec3& normal) {
  return incident - 2 * dot(incident, normal) * normal;
}

inline vec3 refract(const vec3& incident, const vec3& normal, double etai_over_etat) {
  const double cos_theta = std::fmin(dot(-incident, normal), 1.0);
  const vec3 out_perpendicular = etai_over_etat * (incident + cos_theta * normal);
  const vec3 out_parallel =
      -std::sqrt(std::fabs(1.0 - out_perpendicular.length_squared())) * normal;
  return out_perpendicular + out_parallel;
}

using point3 = vec3;

struct as_point3 {
  explicit as_point3(const point3& ref) : ref_{ref} {}

  [[maybe_unused]] const point3& ref_{};  // NOLINT
};

}  // namespace raytracer

template <>
struct std::formatter<raytracer::vec3> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) {
    auto iter = ctx.begin();
    while (iter != ctx.begin() && *iter != '}') {
      iter++;
    }
    return iter;
  }

  template <typename FormatContext>
  constexpr FormatContext::iterator format(const raytracer::vec3& vec, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{} {} {}", vec.x(), vec.y(), vec.z());
  }
};

template <>
struct std::formatter<raytracer::as_point3> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) {
    auto iter = ctx.begin();
    while (iter != ctx.begin() && *iter != '}') {
      iter++;
    }
    return iter;
  }

  template <typename FormatContext>
  constexpr FormatContext::iterator format(const raytracer::as_point3& point,
                                           FormatContext& ctx) const {
    return std::format_to(ctx.out(), "({}, {}, {})", point.ref_.x(), point.ref_.y(),
                          point.ref_.z());
  }
};

#endif
