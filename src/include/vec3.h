#ifndef VEC3_H
#define VEC3_H

#include <array>
#include <cmath>
#include <format>

namespace raytracer {

class vec3 {
 private:
  std::array<double, 3> e_{};

 public:
  constexpr vec3() = default;
  constexpr vec3(const double xcord, const double ycord, const double zcord)
      : e_{xcord, ycord, zcord} {}

  [[nodiscard]] constexpr double x() const {
    return e_[0];
  }
  [[nodiscard]] constexpr double y() const {
    return e_[1];
  }
  [[nodiscard]] constexpr double z() const {
    return e_[2];
  }

  [[nodiscard]] constexpr double length_squared() const {
    return (e_[0] * e_[0]) + (e_[1] * e_[1]) + (e_[2] * e_[2]);
  }

  [[nodiscard]] constexpr double length() const {
    return std::sqrt(length_squared());
  }

  constexpr vec3 operator-() const {
    return vec3{-e_[0], -e_[1], -e_[2]};
  }
  constexpr double operator[](std::ptrdiff_t idx) const {
    return e_[idx];
  }
  constexpr double& operator[](std::ptrdiff_t idx) {
    return e_[idx];
  }

  constexpr vec3& operator+=(const vec3& rhs) {
    return (e_[0] += rhs[0], e_[1] += rhs[1], e_[2] += rhs[2], *this);
  }
  constexpr vec3& operator*=(const double rhs) {
    return (e_[0] *= rhs, e_[1] *= rhs, e_[2] *= rhs, *this);
  }
  constexpr vec3& operator/=(const double rhs) {
    return *this *= (1 / rhs);
  }
};

constexpr vec3 operator+(const vec3& lhs, const vec3& rhs) {
  return vec3{lhs[0] + rhs[0], lhs[1] + rhs[1], lhs[2] + rhs[2]};
}

constexpr vec3 operator-(const vec3& lhs, const vec3& rhs) {
  return vec3{lhs[0] - rhs[0], lhs[1] - rhs[1], lhs[2] - rhs[2]};
}

constexpr vec3 operator*(const vec3& lhs, const vec3& rhs) {
  return vec3{lhs[0] * rhs[0], lhs[1] * rhs[1], lhs[2] * rhs[2]};
}

constexpr vec3 operator*(const double lhs, const vec3& rhs) {
  return vec3{lhs * rhs[0], lhs * rhs[1], lhs * rhs[2]};
}

constexpr vec3 operator*(const vec3& lhs, const double rhs) {
  return rhs * lhs;
}

constexpr vec3 operator/(const vec3& lhs, const double rhs) {
  return 1 / rhs * lhs;
}

constexpr double dot(const vec3& lhs, const vec3& rhs) {
  return (lhs[0] * rhs[0]) + (lhs[1] * rhs[1]) + (lhs[2] * rhs[2]);
}

constexpr vec3 cross(const vec3& lhs, const vec3& rhs) {
  return vec3{(lhs[1] * rhs[2]) - (lhs[2] * rhs[1]), (lhs[2] * rhs[0]) - (lhs[0] * rhs[2]),
              (lhs[0] * rhs[1]) - (lhs[1] * rhs[0])};
}

constexpr vec3 unit_vec(const vec3& vec) {
  return vec / vec.length();
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
    return std::format_to(ctx.out(), "{} {} {}", vec[0], vec[1], vec[2]);
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
    return std::format_to(ctx.out(), "({}, {}, {})", point.ref_[0], point.ref_[1], point.ref_[2]);
  }
};

#endif
