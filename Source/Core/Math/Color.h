#pragma once

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <type_traits>

#include "Core/Math/Vector.h"
#include "Core/String/String.h"

template <typename T>
concept ColorChannel = std::same_as<T, float> || std::same_as<T, std::uint8_t>;

template <typename To, typename From>
[[nodiscard]] constexpr To ConvertColorChannel(From Value) noexcept
{
    if constexpr (std::floating_point<To> && std::floating_point<From>)
    {
        return static_cast<To>(Value);
    }
    else if constexpr (std::integral<To> && std::integral<From>)
    {
        return static_cast<To>(std::clamp(static_cast<int>(Value), 0, 255));
    }
    else if constexpr (std::floating_point<To>)
    {
        return static_cast<To>(Value) / To{255};
    }
    else
    {
        const float Scaled = std::clamp(static_cast<float>(Value), 0.0f, 1.0f) * 255.0f;
        return static_cast<To>(std::lround(Scaled));
    }
}

template <typename T>
concept ColorFloatArg = std::floating_point<T>;

template <typename T>
concept ColorByteArg = std::integral<T> && !std::same_as<T, bool>;

template <ColorChannel T>
class Color
{
public:
    using ValueType = T;
    using VectorType = Vector4<T>;
    using RgbVectorType = Vector3<T>;

    T R{};
    T G{};
    T B{};
    T A{Opaque()};

    [[nodiscard]] static constexpr T Opaque() noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return T{1};
        }
        else
        {
            return T{255};
        }
    }

    constexpr Color() noexcept = default;

    template <ColorFloatArg InR, ColorFloatArg InG, ColorFloatArg InB, ColorFloatArg InA = float>
    constexpr Color(InR InRed, InG InGreen, InB InBlue, InA InAlpha = 1.0f) noexcept :
        R(ConvertColorChannel<T>(InRed)), G(ConvertColorChannel<T>(InGreen)), B(ConvertColorChannel<T>(InBlue)),
        A(ConvertColorChannel<T>(InAlpha))
    {
    }

    template <ColorByteArg InR, ColorByteArg InG, ColorByteArg InB, ColorByteArg InA = int>
    constexpr Color(InR InRed, InG InGreen, InB InBlue, InA InAlpha = 255) noexcept :
        R(ConvertColorChannel<T>(InRed)), G(ConvertColorChannel<T>(InGreen)), B(ConvertColorChannel<T>(InBlue)),
        A(ConvertColorChannel<T>(InAlpha))
    {
    }

    constexpr explicit Color(float Gray, float InAlpha = 1.0f) noexcept : Color(Gray, Gray, Gray, InAlpha) {}

    constexpr explicit Color(int Gray, int InAlpha = 255) noexcept : Color(Gray, Gray, Gray, InAlpha) {}

    constexpr Color(const Vector3<float>& Rgb, float InAlpha = 1.0f) noexcept : Color(Rgb.X, Rgb.Y, Rgb.Z, InAlpha) {}

    constexpr Color(const Vector4<float>& Rgba) noexcept : Color(Rgba.X, Rgba.Y, Rgba.Z, Rgba.W) {}

    constexpr Color(const Vector3<std::uint8_t>& Rgb, std::uint8_t InAlpha = 255) noexcept :
        Color(Rgb.X, Rgb.Y, Rgb.Z, InAlpha)
    {
    }

    constexpr Color(const Vector4<std::uint8_t>& Rgba) noexcept : Color(Rgba.X, Rgba.Y, Rgba.Z, Rgba.W) {}

    template <ColorChannel U>
        requires(!std::same_as<U, T>)
    constexpr Color(const Color<U>& Other) noexcept :
        R(ConvertColorChannel<T>(Other.R)), G(ConvertColorChannel<T>(Other.G)), B(ConvertColorChannel<T>(Other.B)),
        A(ConvertColorChannel<T>(Other.A))
    {
    }

    [[nodiscard]] static constexpr Color White() noexcept
    {
        return {1.0f, 1.0f, 1.0f};
    }

    [[nodiscard]] static constexpr Color Black() noexcept
    {
        return {0.0f, 0.0f, 0.0f};
    }

    [[nodiscard]] static constexpr Color Red() noexcept
    {
        return {1.0f, 0.0f, 0.0f};
    }

    [[nodiscard]] static constexpr Color Green() noexcept
    {
        return {0.0f, 1.0f, 0.0f};
    }

    [[nodiscard]] static constexpr Color Blue() noexcept
    {
        return {0.0f, 0.0f, 1.0f};
    }

    [[nodiscard]] static constexpr Color Yellow() noexcept
    {
        return {1.0f, 1.0f, 0.0f};
    }

    [[nodiscard]] static constexpr Color Cyan() noexcept
    {
        return {0.0f, 1.0f, 1.0f};
    }

    [[nodiscard]] static constexpr Color Magenta() noexcept
    {
        return {1.0f, 0.0f, 1.0f};
    }

    [[nodiscard]] static constexpr Color Transparent() noexcept
    {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    [[nodiscard]] static Color FromRgba32(std::uint32_t Rgba) noexcept
    {
        const auto InR = static_cast<std::uint8_t>((Rgba >> 24) & 0xFFu);
        const auto InG = static_cast<std::uint8_t>((Rgba >> 16) & 0xFFu);
        const auto InB = static_cast<std::uint8_t>((Rgba >> 8) & 0xFFu);
        const auto InA = static_cast<std::uint8_t>(Rgba & 0xFFu);
        return {InR, InG, InB, InA};
    }

    [[nodiscard]] std::uint32_t ToRgba32() const noexcept
    {
        const Color<std::uint8_t> Bytes{*this};
        return (static_cast<std::uint32_t>(Bytes.R) << 24) | (static_cast<std::uint32_t>(Bytes.G) << 16) |
               (static_cast<std::uint32_t>(Bytes.B) << 8) | static_cast<std::uint32_t>(Bytes.A);
    }

    [[nodiscard]] VectorType& AsVector() noexcept
    {
        return *reinterpret_cast<VectorType*>(this);
    }

    [[nodiscard]] const VectorType& AsVector() const noexcept
    {
        return *reinterpret_cast<const VectorType*>(this);
    }

    [[nodiscard]] constexpr RgbVectorType Rgb() const noexcept
    {
        return {R, G, B};
    }

    [[nodiscard]] constexpr VectorType Rgba() const noexcept
    {
        return {R, G, B, A};
    }

    [[nodiscard]] T* Data() noexcept
    {
        return &R;
    }

    [[nodiscard]] const T* Data() const noexcept
    {
        return &R;
    }

    [[nodiscard]] T& operator[](int Index) noexcept
    {
        return AsVector()[static_cast<glm::length_t>(Index)];
    }

    [[nodiscard]] const T& operator[](int Index) const noexcept
    {
        return AsVector()[static_cast<glm::length_t>(Index)];
    }

    [[nodiscard]] Color WithAlpha(T InAlpha) const noexcept
    {
        return Color{Rgb(), InAlpha};
    }

    [[nodiscard]] Color Premultiplied() const noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return {R * A, G * A, B * A, A};
        }
        else
        {
            return Color<float>(*this).Premultiplied();
        }
    }

    [[nodiscard]] Color Lerp(const Color& Other, float Alpha) const noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return Color{AsVector().Lerp(Other.AsVector(), Alpha)};
        }
        else
        {
            return Color<float>(*this).Lerp(Color<float>(Other), Alpha);
        }
    }

    [[nodiscard]] Color Clamp() const noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return Color{AsVector().Clamp(T{0}, Opaque())};
        }
        else
        {
            return *this;
        }
    }

    Color& operator*=(const Color& Other) noexcept
    {
        *this = *this * Other;
        return *this;
    }

    Color& operator*=(float Scale) noexcept
    {
        *this = *this * Scale;
        return *this;
    }

    [[nodiscard]] Color operator*(const Color& Other) const noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return Color{AsVector() * Other.AsVector()};
        }
        else
        {
            return Color<float>(*this) * Color<float>(Other);
        }
    }

    [[nodiscard]] Color operator*(float Scale) const noexcept
    {
        if constexpr (std::floating_point<T>)
        {
            return Color{AsVector() * Scale};
        }
        else
        {
            return Color<float>(*this) * Scale;
        }
    }

    [[nodiscard]] constexpr bool operator==(const Color& Other) const noexcept
    {
        return R == Other.R && G == Other.G && B == Other.B && A == Other.A;
    }

    [[nodiscard]] String ToString() const
    {
        if constexpr (std::floating_point<T>)
        {
            return String::Format("Color({}, {}, {}, {})", R, G, B, A);
        }
        else
        {
            return String::Format("Color8({}, {}, {}, {})", R, G, B, A);
        }
    }
};

template <ColorChannel T>
[[nodiscard]] Color<T> operator*(float Scale, const Color<T>& Value) noexcept
{
    return Value * Scale;
}

static_assert(sizeof(Color<float>) == sizeof(Vector4<float>));
static_assert(alignof(Color<float>) == alignof(Vector4<float>));
static_assert(sizeof(Color<std::uint8_t>) == sizeof(Vector4<std::uint8_t>));
static_assert(alignof(Color<std::uint8_t>) == alignof(Vector4<std::uint8_t>));

using Colorf = Color<float>;
using Color8 = Color<std::uint8_t>;
