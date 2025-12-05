#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"

namespace raytracer {

using color = vec3;

struct as_color {
  explicit as_color(const color& color) : ref_{color} {}

  [[maybe_unused]] const color& ref_;  // NOLINT
};

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
    const auto raspect = static_cast<char>(255.999 * ref.x());
    const auto gaspect = static_cast<char>(255.999 * ref.y());
    const auto baspect = static_cast<char>(255.999 * ref.z());
    return std::format_to(ctx.out(), "{}{}{}", raspect, gaspect, baspect);
  }
};

#endif
