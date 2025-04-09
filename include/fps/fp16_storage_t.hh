#pragma once

#include <cinttypes>
#include <type_traits>

#include "half-private/fp_convert.hh"

namespace fps {

template <typename E_, typename = std::enable_if_t<std::is_enum_v<E_>>> 
  //requires std::is_enum_v<E_>
[[nodiscard]]
constexpr inline auto to_underlying(E_ e) noexcept -> std::underlying_type_t<E_> {
  using type = std::underlying_type_t<E_>;
  return static_cast<type>(e);
}

template <typename E_, typename = std::enable_if_t<std::is_enum_v<E_>>>
  //requires std::is_enum_v<E_>
[[nodiscard]]
constexpr inline auto from_underlying(std::underlying_type_t<E_> v) noexcept -> E_ {
  return static_cast<E_>(v);
}

enum struct fp16_storage_t : std::uint16_t {};
enum struct fp32_storage_t : std::uint32_t {};

[[nodiscard]]
constexpr inline auto convert_h2f(fp16_storage_t v) -> fp32_storage_t {
  auto uv = to_underlying(v);
  return from_underlying<fp32_storage_t>(half::half_to_float(uv));
}

[[nodiscard]]
constexpr inline auto convert_f2h(fp32_storage_t v) -> fp16_storage_t
{
  auto uv = to_underlying(v);
  return from_underlying<fp16_storage_t>(half::float_to_half(uv));
}

} // namespace fps

namespace fps::literals {

inline auto operator "" _fs16(long double l) noexcept -> fp16_storage_t
{
   const auto v = std::bit_cast<std::uint32_t>(static_cast<float>(l));
   return convert_f2h(from_underlying<fp32_storage_t>(v));
}

inline auto operator "" _fs32(long double l) noexcept -> fp32_storage_t
{
   const auto v = std::bit_cast<std::uint32_t>(static_cast<float>(l));
   return from_underlying<fp32_storage_t>(v);
}

} // namespace fps::literals
