#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec3.h"

namespace raytracer {

using color = vec3;

struct as_color {
  explicit as_color(const color& color) : ref_{color} {}

  [[maybe_unused]] const color& ref_;  // NOLINT
};

inline double linear2gamma(double linear) {
  if (linear > 0) {
    return std::sqrt(linear);
  }
  return linear;
}

}  // namespace raytracer

template <>
struct std::formatter<raytracer::as_color> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) {
    auto iter = ctx.begin();
    while (iter != ctx.begin() && *iter != '}') {
      iter++;
    }
    return iter;
  }

  template <typename FormatContext>
  constexpr FormatContext::iterator format(const raytracer::as_color& color,
                                           FormatContext& ctx) const {
    const auto& ref = color.ref_;

    const auto x = raytracer::linear2gamma(ref.x());
    const auto y = raytracer::linear2gamma(ref.y());
    const auto z = raytracer::linear2gamma(ref.z());

    static const raytracer::interval intensity{0.000, 0.999};
    const auto raspect = static_cast<int>(256 * intensity.clamp(x));
    const auto gaspect = static_cast<int>(256 * intensity.clamp(y));
    const auto baspect = static_cast<int>(256 * intensity.clamp(z));
    return std::format_to(ctx.out(), "{} {} {}", raspect, gaspect, baspect);
  }
};

#endif
