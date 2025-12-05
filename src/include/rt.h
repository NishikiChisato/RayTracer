#ifndef RT_H
#define RT_H

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <numbers>
#include <print>
#include <vector>

#include "color.h"
#include "ray.h"
#include "timer.h"

namespace raytracer {

constexpr double infinite{std::numeric_limits<double>::infinity()};
constexpr double pi{std::numbers::pi};

inline double degrees2radians(const double degrees) {
  return degrees * pi / 180.0;
}

}  // namespace raytracer

#endif
