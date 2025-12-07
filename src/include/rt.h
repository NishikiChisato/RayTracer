#ifndef RT_H
#define RT_H

#include <limits>
#include <numbers>
#include <random>

namespace raytracer {

constexpr double infinite{std::numeric_limits<double>::infinity()};
constexpr double pi{std::numbers::pi};
constexpr std::size_t kMaxBufSize{64Z * 1024Z * 1024Z};

inline double degrees2radians(const double degrees) {
  return degrees * pi / 180.0;
}

// random double in [0,1)
inline double random_double() {
  thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
  thread_local std::mt19937 gen(std::random_device{}());
  return dist(gen);
}

inline double random_double(const double min, const double max) {
  return min + (max - min) * random_double();
}

}  // namespace raytracer

#endif
