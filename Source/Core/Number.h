#pragma once

#include <bit>
#include <charconv>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <limits>
#include <numbers>
#include <type_traits>
#include <utility>

#include "Core/Debug/Debug.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

template <typename T>
concept NumberValue = (std::integral<T> && !std::same_as<T, bool> && !std::same_as<T, char>) || std::floating_point<T>;

template <NumberValue T>
class Number
{
public:
    using ValueType = T;

    T Value{};

    static constexpr Number Zero{T{}};
    static constexpr Number One{T{1}};
    static constexpr Number MinValue{std::numeric_limits<T>::lowest()};
    static constexpr Number MaxValue{std::numeric_limits<T>::max()};

    constexpr Number() noexcept = default;

    constexpr Number(const T InValue) noexcept : Value(InValue) {}

    template <NumberValue U>
        requires(!std::same_as<U, T>)
    constexpr Number(const Number<U> Other) noexcept : Value(static_cast<T>(Other.Value))
    {
    }

    [[nodiscard]] constexpr operator T() const noexcept
    {
        return Value;
    }

    constexpr Number& operator++() noexcept
        requires std::integral<T>
    {
        ++Value;
        return *this;
    }

    constexpr Number operator++(int) noexcept
        requires std::integral<T>
    {
        const Number Previous{*this};
        ++Value;
        return Previous;
    }

    constexpr Number& operator--() noexcept
        requires std::integral<T>
    {
        --Value;
        return *this;
    }

    constexpr Number operator--(int) noexcept
        requires std::integral<T>
    {
        const Number Previous{*this};
        --Value;
        return Previous;
    }

    constexpr Number& operator+=(const Number Other) noexcept
    {
        Value = static_cast<T>(Value + Other.Value);
        return *this;
    }

    constexpr Number& operator-=(const Number Other) noexcept
    {
        Value = static_cast<T>(Value - Other.Value);
        return *this;
    }

    constexpr Number& operator*=(const Number Other) noexcept
    {
        Value = static_cast<T>(Value * Other.Value);
        return *this;
    }

    constexpr Number& operator/=(const Number Other) noexcept
    {
        Value = static_cast<T>(Value / Other.Value);
        return *this;
    }

    constexpr Number& operator%=(const Number Other) noexcept
        requires std::integral<T>
    {
        Value = static_cast<T>(Value % Other.Value);
        return *this;
    }

    constexpr Number& operator&=(const Number Other) noexcept
        requires std::integral<T>
    {
        Value = static_cast<T>(Value & Other.Value);
        return *this;
    }

    constexpr Number& operator|=(const Number Other) noexcept
        requires std::integral<T>
    {
        Value = static_cast<T>(Value | Other.Value);
        return *this;
    }

    constexpr Number& operator^=(const Number Other) noexcept
        requires std::integral<T>
    {
        Value = static_cast<T>(Value ^ Other.Value);
        return *this;
    }

    constexpr Number& operator<<=(const int Count) noexcept
        requires std::integral<T>
    {
        Value = static_cast<T>(Value << Count);
        return *this;
    }

    constexpr Number& operator>>=(const int Count) noexcept
        requires std::integral<T>
    {
        Value = static_cast<T>(Value >> Count);
        return *this;
    }

    [[nodiscard]] constexpr Number operator+() const noexcept
    {
        return *this;
    }

    [[nodiscard]] constexpr Number operator-() const noexcept
    {
        return Number{static_cast<T>(-Value)};
    }

    [[nodiscard]] constexpr Number operator~() const noexcept
        requires std::integral<T>
    {
        return Number{static_cast<T>(~Value)};
    }

    [[nodiscard]] String ToString() const
    {
        if constexpr (std::integral<T>)
        {
            char Buffer[32];
            const auto [End, ErrorCode] = std::to_chars(Buffer, Buffer + sizeof(Buffer), Value);
            (void)ErrorCode;
            return String{Buffer, static_cast<String::SizeType>(End - Buffer)};
        }
        else
        {
            char Buffer[64];
            const auto [End, ErrorCode] = std::to_chars(Buffer, Buffer + sizeof(Buffer), Value);
            (void)ErrorCode;
            return String{Buffer, static_cast<String::SizeType>(End - Buffer)};
        }
    }

    [[nodiscard]] static bool TryParse(StringView Text, Number& Out) noexcept
    {
        const StringView Trimmed = Text.Trim();
        if (Trimmed.Empty())
        {
            return false;
        }

        const char* Begin = Trimmed.Data();
        const char* End = Begin + Trimmed.Size();
        if (*Begin == '+')
        {
            ++Begin;
            if (Begin == End)
            {
                return false;
            }
        }

        T Parsed{};
        const auto [Pointer, ErrorCode] = std::from_chars(Begin, End, Parsed);
        if (ErrorCode == std::errc{} && Pointer == End)
        {
            Out.Value = Parsed;
            return true;
        }

        if constexpr (std::floating_point<T>)
        {
            return TryParseFloatKeyword(Trimmed, Out);
        }

        return false;
    }

    [[nodiscard]] static Number Parse(StringView Text)
    {
        Number Result{};
        Assert(TryParse(Text, Result));
        return Result;
    }

    [[nodiscard]] static constexpr Number Abs(const Number Value) noexcept
    {
        if constexpr (std::is_unsigned_v<T>)
        {
            return Value;
        }
        else
        {
            return Number{Value.Value < T{} ? static_cast<T>(-Value.Value) : Value.Value};
        }
    }

    [[nodiscard]] static constexpr Number Min(const Number Left, const Number Right) noexcept
    {
        return Left.Value < Right.Value ? Left : Right;
    }

    [[nodiscard]] static constexpr Number Max(const Number Left, const Number Right) noexcept
    {
        return Left.Value < Right.Value ? Right : Left;
    }

    [[nodiscard]] static constexpr Number
    Clamp(const Number Value, const Number MinValue, const Number MaxValue) noexcept
    {
        return Max(MinValue, Min(MaxValue, Value));
    }

    [[nodiscard]] static constexpr int Sign(const Number Value) noexcept
    {
        if (Value.Value > T{})
        {
            return 1;
        }
        if constexpr (std::is_signed_v<T> || std::floating_point<T>)
        {
            if (Value.Value < T{})
            {
                return -1;
            }
        }
        return 0;
    }

    [[nodiscard]] static constexpr Number CopySign(const Number Value, const Number SignValue) noexcept
        requires(std::is_signed_v<T> || std::floating_point<T>)
    {
        if constexpr (std::floating_point<T>)
        {
            return Number{std::copysign(Value.Value, SignValue.Value)};
        }
        else
        {
            const Number Magnitude = Abs(Value);
            return SignValue.Value < T{} ? -Magnitude : Magnitude;
        }
    }

    [[nodiscard]] static constexpr Number DivRem(const Number Left, const Number Right, Number& Remainder) noexcept
        requires std::integral<T>
    {
        Remainder.Value = static_cast<T>(Left.Value % Right.Value);
        return Number{static_cast<T>(Left.Value / Right.Value)};
    }

    [[nodiscard]] static constexpr bool IsEven(const Number Value) noexcept
        requires std::integral<T>
    {
        return (Value.Value & T{1}) == T{};
    }

    [[nodiscard]] static constexpr bool IsOdd(const Number Value) noexcept
        requires std::integral<T>
    {
        return (Value.Value & T{1}) != T{};
    }

    [[nodiscard]] static constexpr bool IsPow2(const Number Value) noexcept
        requires std::integral<T>
    {
        if constexpr (std::is_signed_v<T>)
        {
            if (Value.Value <= T{})
            {
                return false;
            }
        }
        else if (Value.Value == T{})
        {
            return false;
        }
        return std::has_single_bit(static_cast<std::make_unsigned_t<T>>(Value.Value));
    }

    [[nodiscard]] static constexpr int Log2(const Number Value) noexcept
        requires std::integral<T>
    {
        return std::bit_width(static_cast<std::make_unsigned_t<T>>(Value.Value)) - 1;
    }

    [[nodiscard]] static constexpr int LeadingZeroCount(const Number Value) noexcept
        requires std::integral<T>
    {
        return std::countl_zero(static_cast<std::make_unsigned_t<T>>(Value.Value));
    }

    [[nodiscard]] static constexpr int TrailingZeroCount(const Number Value) noexcept
        requires std::integral<T>
    {
        return std::countr_zero(static_cast<std::make_unsigned_t<T>>(Value.Value));
    }

    [[nodiscard]] static constexpr int PopCount(const Number Value) noexcept
        requires std::integral<T>
    {
        return std::popcount(static_cast<std::make_unsigned_t<T>>(Value.Value));
    }

    [[nodiscard]] static constexpr Number RotateLeft(const Number Value, const int Count) noexcept
        requires std::integral<T>
    {
        using UnsignedType = std::make_unsigned_t<T>;
        return Number{static_cast<T>(std::rotl(static_cast<UnsignedType>(Value.Value), Count))};
    }

    [[nodiscard]] static constexpr Number RotateRight(const Number Value, const int Count) noexcept
        requires std::integral<T>
    {
        using UnsignedType = std::make_unsigned_t<T>;
        return Number{static_cast<T>(std::rotr(static_cast<UnsignedType>(Value.Value), Count))};
    }

    template <NumberValue U>
    [[nodiscard]] static constexpr Number CreateSaturating(const U Other) noexcept
        requires std::integral<T> && std::integral<U>
    {
        if (std::cmp_less(Other, std::numeric_limits<T>::lowest()))
        {
            return MinValue;
        }
        if (std::cmp_greater(Other, std::numeric_limits<T>::max()))
        {
            return MaxValue;
        }
        return Number{static_cast<T>(Other)};
    }

    [[nodiscard]] static constexpr bool IsNaN(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return std::isnan(Value.Value);
    }

    [[nodiscard]] static constexpr bool IsInfinity(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return std::isinf(Value.Value);
    }

    [[nodiscard]] static constexpr bool IsPositiveInfinity(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return std::isinf(Value.Value) && Value.Value > T{};
    }

    [[nodiscard]] static constexpr bool IsNegativeInfinity(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return std::isinf(Value.Value) && Value.Value < T{};
    }

    [[nodiscard]] static constexpr bool IsFinite(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return std::isfinite(Value.Value);
    }

    [[nodiscard]] static constexpr bool IsNormal(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return std::isnormal(Value.Value);
    }

    [[nodiscard]] static constexpr bool IsSubnormal(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return std::fpclassify(Value.Value) == FP_SUBNORMAL;
    }

    [[nodiscard]] static constexpr Number Floor(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::floor(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Ceiling(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::ceil(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Round(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::round(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Truncate(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::trunc(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Sqrt(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::sqrt(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Cbrt(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::cbrt(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Pow(const Number Base, const Number Exponent) noexcept
        requires std::floating_point<T>
    {
        return Number{std::pow(Base.Value, Exponent.Value)};
    }

    [[nodiscard]] static constexpr Number Exp(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::exp(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Exp2(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::exp2(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Log(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::log(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Log2(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::log2(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Log10(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::log10(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Sin(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::sin(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Cos(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::cos(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Tan(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::tan(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Asin(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::asin(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Acos(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::acos(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Atan(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::atan(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Atan2(const Number Y, const Number X) noexcept
        requires std::floating_point<T>
    {
        return Number{std::atan2(Y.Value, X.Value)};
    }

    [[nodiscard]] static constexpr Number Sinh(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::sinh(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Cosh(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::cosh(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Tanh(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::tanh(Value.Value)};
    }

    [[nodiscard]] static constexpr Number Hypot(const Number X, const Number Y) noexcept
        requires std::floating_point<T>
    {
        return Number{std::hypot(X.Value, Y.Value)};
    }

    [[nodiscard]] static constexpr Number FusedMultiplyAdd(const Number A, const Number B, const Number C) noexcept
        requires std::floating_point<T>
    {
        return Number{std::fma(A.Value, B.Value, C.Value)};
    }

    [[nodiscard]] static constexpr Number Lerp(const Number A, const Number B, const Number Amount) noexcept
        requires std::floating_point<T>
    {
        return Number{std::lerp(A.Value, B.Value, Amount.Value)};
    }

    [[nodiscard]] static constexpr Number Epsilon() noexcept
        requires std::floating_point<T>
    {
        return Number{std::numeric_limits<T>::epsilon()};
    }

    [[nodiscard]] static constexpr Number NaN() noexcept
        requires std::floating_point<T>
    {
        return Number{std::numeric_limits<T>::quiet_NaN()};
    }

    [[nodiscard]] static constexpr Number PositiveInfinity() noexcept
        requires std::floating_point<T>
    {
        return Number{std::numeric_limits<T>::infinity()};
    }

    [[nodiscard]] static constexpr Number NegativeInfinity() noexcept
        requires std::floating_point<T>
    {
        return Number{-std::numeric_limits<T>::infinity()};
    }

    [[nodiscard]] static constexpr Number Reciprocal(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{T{1} / Value.Value};
    }

    [[nodiscard]] static constexpr Number BitDecrement(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::nextafter(Value.Value, -std::numeric_limits<T>::infinity())};
    }

    [[nodiscard]] static constexpr Number BitIncrement(const Number Value) noexcept
        requires std::floating_point<T>
    {
        return Number{std::nextafter(Value.Value, std::numeric_limits<T>::infinity())};
    }

    [[nodiscard]] static constexpr Number DegreesToRadians(const Number Degrees) noexcept
        requires std::floating_point<T>
    {
        return Number{Degrees.Value * (std::numbers::pi_v<T> / T{180})};
    }

    [[nodiscard]] static constexpr Number RadiansToDegrees(const Number Radians) noexcept
        requires std::floating_point<T>
    {
        return Number{Radians.Value * (T{180} / std::numbers::pi_v<T>)};
    }

    [[nodiscard]] static constexpr auto ToBits(const Number Value) noexcept
        requires std::floating_point<T>
    {
        if constexpr (std::same_as<T, float>)
        {
            return std::bit_cast<std::uint32_t>(Value.Value);
        }
        else
        {
            return std::bit_cast<std::uint64_t>(Value.Value);
        }
    }

    [[nodiscard]] static constexpr Number FromBits(const std::uint32_t Bits) noexcept
        requires std::same_as<T, float>
    {
        return Number{std::bit_cast<T>(Bits)};
    }

    [[nodiscard]] static constexpr Number FromBits(const std::uint64_t Bits) noexcept
        requires std::same_as<T, double>
    {
        return Number{std::bit_cast<T>(Bits)};
    }

    [[nodiscard]] static constexpr int Compare(const Number Left, const Number Right) noexcept
    {
        if (Left.Value < Right.Value)
        {
            return -1;
        }
        if (Right.Value < Left.Value)
        {
            return 1;
        }
        return 0;
    }

private:
    [[nodiscard]] static constexpr char ToLowerAscii(const char Character) noexcept
    {
        return Character >= 'A' && Character <= 'Z' ? static_cast<char>(Character - 'A' + 'a') : Character;
    }

    [[nodiscard]] static constexpr bool EqualsIgnoreCase(const StringView Text, const StringView Expected) noexcept
    {
        if (Text.Size() != Expected.Size())
        {
            return false;
        }
        for (StringView::SizeType Index = 0; Index < Text.Size(); ++Index)
        {
            if (ToLowerAscii(Text[Index]) != ToLowerAscii(Expected[Index]))
            {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] static bool TryParseFloatKeyword(const StringView Text, Number& Out) noexcept
        requires std::floating_point<T>
    {
        StringView Body = Text;
        T Sign{1};
        if (!Body.Empty() && (Body.Front() == '-' || Body.Front() == '+'))
        {
            if (Body.Front() == '-')
            {
                Sign = T{-1};
            }
            Body = Body.Substr(1);
        }

        if (EqualsIgnoreCase(Body, "nan"))
        {
            Out.Value = Sign * std::numeric_limits<T>::quiet_NaN();
            return true;
        }
        if (EqualsIgnoreCase(Body, "inf") || EqualsIgnoreCase(Body, "infinity"))
        {
            Out.Value = Sign * std::numeric_limits<T>::infinity();
            return true;
        }
        return false;
    }
};

using Int8 = Number<std::int8_t>;
using Int16 = Number<std::int16_t>;
using Int32 = Number<std::int32_t>;
using Int64 = Number<std::int64_t>;
using UInt8 = Number<std::uint8_t>;
using UInt16 = Number<std::uint16_t>;
using UInt32 = Number<std::uint32_t>;
using UInt64 = Number<std::uint64_t>;
using Float = Number<float>;
using Double = Number<double>;

namespace NumberDetail
{
    template <typename T>
    struct FormatUnderlying
    {
        using Type = T;
    };

    template <>
    struct FormatUnderlying<std::int8_t>
    {
        using Type = int;
    };

    template <>
    struct FormatUnderlying<std::uint8_t>
    {
        using Type = unsigned;
    };
} // namespace NumberDetail

template <NumberValue T>
struct fmt::formatter<Number<T>> : fmt::formatter<typename NumberDetail::FormatUnderlying<T>::Type>
{
    using Base = fmt::formatter<typename NumberDetail::FormatUnderlying<T>::Type>;

    auto format(const Number<T>& Value, fmt::format_context& Context) const
    {
        using Underlying = typename NumberDetail::FormatUnderlying<T>::Type;
        return Base::format(static_cast<Underlying>(Value.Value), Context);
    }
};

template <NumberValue T>
struct std::hash<Number<T>>
{
    [[nodiscard]] std::size_t operator()(const Number<T> Value) const noexcept
    {
        return std::hash<T>{}(Value.Value);
    }
};

static_assert(sizeof(Int8) == sizeof(std::int8_t));
static_assert(sizeof(Int16) == sizeof(std::int16_t));
static_assert(sizeof(Int32) == sizeof(std::int32_t));
static_assert(sizeof(Int64) == sizeof(std::int64_t));
static_assert(sizeof(UInt8) == sizeof(std::uint8_t));
static_assert(sizeof(UInt16) == sizeof(std::uint16_t));
static_assert(sizeof(UInt32) == sizeof(std::uint32_t));
static_assert(sizeof(UInt64) == sizeof(std::uint64_t));
static_assert(sizeof(Float) == sizeof(float));
static_assert(sizeof(Double) == sizeof(double));
static_assert(alignof(Int32) == alignof(std::int32_t));
static_assert(alignof(Float) == alignof(float));
