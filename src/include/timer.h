#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <format>
#include <print>
#include <string_view>

namespace raytracer::utility {

// console color
enum class color : std::uint8_t {
  kNone = 0,
  kBlck = 30,
  kRed,
  kGreen,
  kYellow,
  kBlue,
  kMagenta,
  kCyan,
  kWhite,
  kBrightBlack = 90,
  kBrightRed,
  kBrightGreen,
  kBrightYellow,
  kBrightBlue,
  kBrightMagenta,
  kBrightCyan,
  kBrightWhite,
};

// Usage: styled("Hello World", color::kBlue)
struct styled {
  std::string_view value_{};  // NOLINT
  color c_{color::kNone};     // NOLINT
};

}  // namespace raytracer::utility

template <>
struct std::formatter<raytracer::utility::color> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr FormatContext::iterator format(const raytracer::utility::color& color,
                                           FormatContext& ctx) const {
    return std::format_to(ctx.out(), "\033[{}m", static_cast<int>(color));
  }
};

template <>
struct std::formatter<raytracer::utility::styled> {
  template <typename ParseContext>
  constexpr ParseContext::iterator parse(ParseContext& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  constexpr FormatContext::iterator format(const raytracer::utility::styled& styled,
                                           FormatContext& ctx) const {
    auto out = ctx.out();
    out = std::format_to(out, "\033[{}m", static_cast<int>(styled.c_));
    out = std::format_to(out, "{}", styled.value_);
    return std::format_to(out, "\033[0m");
  }
};

namespace raytracer::utility {
// timer
class Timer {
 public:
  Timer() : start_{Clock::now()}, last_{Clock::now()} {}

  void reset() {
    last_ = Clock::now();
  }
  [[nodiscard]] double elapsedFromStart() const {
    return std::chrono::duration_cast<Second>(Clock::now() - start_).count();
  }
  [[nodiscard]] double elapsedFromLast() const {
    return std::chrono::duration_cast<Second>(Clock::now() - last_).count();
  }
  void report(std::string_view sv = "") {
    constexpr static std::string_view prefix{"[Timer]: [Elapsed]: {:.4f}, [Delta]: {:.4f}"};
    const auto substr = std::format(prefix, elapsedFromStart(), elapsedFromLast());
    std::println("{}{}{}", styled(substr, color::kCyan), ((sv == "") ? "" : "\n"), styled(sv));
    reset();
  }

 private:
  using Clock = std::chrono::high_resolution_clock;
  using Second = std::chrono::duration<double, std::ratio<1>>;
  std::chrono::time_point<Clock> start_{Clock::now()};
  std::chrono::time_point<Clock> last_{Clock::now()};
};

}  // namespace raytracer::utility

#endif
