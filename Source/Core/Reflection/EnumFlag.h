#pragma once

#include <meta>
#include <type_traits>
#include <utility>

namespace Reflection
{

struct EnumFlag
{
};

template <typename E>
[[nodiscard]] consteval bool HasEnumFlagAttribute()
{
    return !std::meta::annotations_of_with_type(^^E, ^^EnumFlag).empty();
}

template <typename E>
concept IsEnumFlag = std::is_enum_v<E> && HasEnumFlagAttribute<E>();

template <typename E>
    requires IsEnumFlag<E>
[[nodiscard]] constexpr E operator|(E Left, E Right) noexcept
{
    using Underlying = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<Underlying>(std::to_underlying(Left) | std::to_underlying(Right)));
}

template <typename E>
    requires IsEnumFlag<E>
[[nodiscard]] constexpr E operator&(E Left, E Right) noexcept
{
    using Underlying = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<Underlying>(std::to_underlying(Left) & std::to_underlying(Right)));
}

template <typename E>
    requires IsEnumFlag<E>
[[nodiscard]] constexpr E operator~(E Value) noexcept
{
    using Underlying = std::underlying_type_t<E>;
    return static_cast<E>(static_cast<Underlying>(~std::to_underlying(Value)));
}

template <typename E>
    requires IsEnumFlag<E>
constexpr E& operator|=(E& Left, E Right) noexcept
{
    Left = Left | Right;
    return Left;
}

template <typename E>
    requires IsEnumFlag<E>
constexpr E& operator&=(E& Left, E Right) noexcept
{
    Left = Left & Right;
    return Left;
}

template <typename E>
    requires IsEnumFlag<E>
[[nodiscard]] constexpr bool HasFlag(E Value, E Flag) noexcept
{
    return (Value & Flag) == Flag;
}

template <typename E>
    requires IsEnumFlag<E>
[[nodiscard]] constexpr E SetFlag(E Value, E Flag) noexcept
{
    return Value | Flag;
}

template <typename E>
    requires IsEnumFlag<E>
[[nodiscard]] constexpr E ClearFlag(E Value, E Flag) noexcept
{
    return static_cast<E>(std::to_underlying(Value) & ~std::to_underlying(Flag));
}

} // namespace Reflection

using Reflection::ClearFlag;
using Reflection::EnumFlag;
using Reflection::HasFlag;
using Reflection::SetFlag;
using Reflection::operator|;
using Reflection::operator&;
using Reflection::operator~;
using Reflection::operator|=;
using Reflection::operator&=;
