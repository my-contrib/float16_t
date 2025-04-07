#pragma once

#include <cinttypes>
#include <bit>

namespace half::half_private {

[[maybe_unused, nodiscard]] constexpr inline std::uint32_t
_uint32_sels(std::uint32_t test, std::uint32_t a, std::uint32_t b) noexcept {
  const std::uint32_t mask =
    (((std::int32_t)test) >> 31); // Until c++20 implementation defined
  const std::uint32_t sel_a = (a & mask);
  const std::uint32_t sel_b = (b & ~mask);
  const std::uint32_t result = (sel_a | sel_b);
  return result;
}

[[maybe_unused, nodiscard]] constexpr inline std::uint32_t
_uint32_selb(std::uint32_t mask, std::uint32_t a, std::uint32_t b) noexcept {
  const std::uint32_t sel_a = (a & mask);
  const std::uint32_t sel_b = (b & ~mask);
  const std::uint32_t result = (sel_a | sel_b);
  return result;
}

[[maybe_unused, nodiscard]] constexpr inline std::uint16_t
_uint16_sels(std::uint16_t test, std::uint16_t a, std::uint16_t b) noexcept {
  const std::uint16_t mask =
    (((int16_t)test) >> 15); // Until c++20 implementation defined
  const std::uint16_t sel_a = (a & mask);
  const std::uint16_t sel_b = (b & ~mask);
  const std::uint16_t result = (sel_a | sel_b);
  return result;
}

[[maybe_unused, nodiscard]] constexpr inline std::uint32_t
_uint32_cntlz(std::uint32_t x) noexcept {
#if FLOAT16_T_HAS_STD_BITOPS
  {
    return std::countl_zero(x);
  }
#elif defined(__GNUC__)
  {
    std::uint32_t is_x_nez_msb = (-x);
    std::uint32_t nlz = __builtin_clz(x);
    std::uint32_t result = _uint32_sels(is_x_nez_msb, nlz, 0x00000020);
    return result;
  }
#else
  {
    const std::uint32_t x0 = (x >> 1);
    const std::uint32_t x1 = (x | x0);
    const std::uint32_t x2 = (x1 >> 2);
    const std::uint32_t x3 = (x1 | x2);
    const std::uint32_t x4 = (x3 >> 4);
    const std::uint32_t x5 = (x3 | x4);
    const std::uint32_t x6 = (x5 >> 8);
    const std::uint32_t x7 = (x5 | x6);
    const std::uint32_t x8 = (x7 >> 16);
    const std::uint32_t x9 = (x7 | x8);
    const std::uint32_t xA = (~x9);
    const std::uint32_t xB = (xA >> 1);
    const std::uint32_t xC = (xB & 0x55555555);
    const std::uint32_t xD = (xA - xC);
    const std::uint32_t xE = (xD & 0x33333333);
    const std::uint32_t xF = (xD >> 2);
    const std::uint32_t x10 = (xF & 0x33333333);
    const std::uint32_t x11 = (xE + x10);
    const std::uint32_t x12 = (x11 >> 4);
    const std::uint32_t x13 = (x11 + x12);
    const std::uint32_t x14 = (x13 & 0x0f0f0f0f);
    const std::uint32_t x15 = (x14 >> 8);
    const std::uint32_t x16 = (x14 + x15);
    const std::uint32_t x17 = (x16 >> 16);
    const std::uint32_t x18 = (x16 + x17);
    const std::uint32_t x19 = (x18 & 0x0000003f);
    return x19;
  }
#endif // NOT __GNUC__
}

[[maybe_unused, nodiscard]] constexpr inline std::uint16_t
_uint16_cntlz(std::uint16_t x) noexcept {
#if FLOAT16_T_HAS_STD_BITOPS
  {
    return std::countl_zero(x);
  }
#elif defined(__GNUC__)
  // if constexpr( using_gnu_c )
  {
    std::uint16_t nlz32 = (std::uint16_t)_uint32_cntlz((std::uint32_t)x);
    std::uint32_t nlz = (nlz32 - 16);
    return nlz;
  }
#else
  // else
  {
    const std::uint16_t x0 = (x >> 1);
    const std::uint16_t x1 = (x | x0);
    const std::uint16_t x2 = (x1 >> 2);
    const std::uint16_t x3 = (x1 | x2);
    const std::uint16_t x4 = (x3 >> 4);
    const std::uint16_t x5 = (x3 | x4);
    const std::uint16_t x6 = (x5 >> 8);
    const std::uint16_t x7 = (x5 | x6);
    const std::uint16_t x8 = (~x7);
    const std::uint16_t x9 = ((x8 >> 1) & 0x5555);
    const std::uint16_t xA = (x8 - x9);
    const std::uint16_t xB = (xA & 0x3333);
    const std::uint16_t xC = ((xA >> 2) & 0x3333);
    const std::uint16_t xD = (xB + xC);
    const std::uint16_t xE = (xD >> 4);
    const std::uint16_t xF = ((xD + xE) & 0x0f0f);
    const std::uint16_t x10 = (xF >> 8);
    const std::uint16_t x11 = ((xF + x10) & 0x001f);
    return x11;
  }
#endif // NOT __GNUC__
}

} // namespace half::half_private

