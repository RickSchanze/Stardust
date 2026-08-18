#pragma once

#include <concepts>

#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/vector_relational.hpp>

#include "Core/String/String.h"

template <typename T>
class Vector2;

template <typename T>
class Vector3;

template <typename T>
class Vector4;

template <typename Derived, glm::length_t Dimension, typename T>
class VectorOps
{
public:
    using ValueType = T;
    using GlmType = glm::vec<Dimension, T>;
    static constexpr glm::length_t ComponentCount = Dimension;

    [[nodiscard]] GlmType& AsGlm() noexcept
    {
        return *reinterpret_cast<GlmType*>(static_cast<Derived*>(this));
    }

    [[nodiscard]] const GlmType& AsGlm() const noexcept
    {
        return *reinterpret_cast<const GlmType*>(static_cast<const Derived*>(this));
    }

    [[nodiscard]] T* Data() noexcept
    {
        return &AsGlm()[0];
    }

    [[nodiscard]] const T* Data() const noexcept
    {
        return &AsGlm()[0];
    }

    [[nodiscard]] T& operator[](glm::length_t Index) noexcept
    {
        return AsGlm()[Index];
    }

    [[nodiscard]] const T& operator[](glm::length_t Index) const noexcept
    {
        return AsGlm()[Index];
    }

    [[nodiscard]] static Derived Zero() noexcept
    {
        return Derived{};
    }

    [[nodiscard]] static Derived One() noexcept
    {
        return Derived{T{1}};
    }

    Derived& operator+=(const Derived& Other) noexcept
    {
        AsGlm() += Other.AsGlm();
        return Self();
    }

    Derived& operator-=(const Derived& Other) noexcept
    {
        AsGlm() -= Other.AsGlm();
        return Self();
    }

    Derived& operator*=(const Derived& Other) noexcept
    {
        AsGlm() *= Other.AsGlm();
        return Self();
    }

    Derived& operator/=(const Derived& Other) noexcept
    {
        AsGlm() /= Other.AsGlm();
        return Self();
    }

    Derived& operator+=(T Scalar) noexcept
    {
        AsGlm() += Scalar;
        return Self();
    }

    Derived& operator-=(T Scalar) noexcept
    {
        AsGlm() -= Scalar;
        return Self();
    }

    Derived& operator*=(T Scalar) noexcept
    {
        AsGlm() *= Scalar;
        return Self();
    }

    Derived& operator/=(T Scalar) noexcept
    {
        AsGlm() /= Scalar;
        return Self();
    }

    [[nodiscard]] Derived operator+() const noexcept
    {
        return Self();
    }

    [[nodiscard]] Derived operator-() const noexcept
    {
        return Derived{-AsGlm()};
    }

    [[nodiscard]] Derived operator+(const Derived& Other) const noexcept
    {
        return Derived{AsGlm() + Other.AsGlm()};
    }

    [[nodiscard]] Derived operator-(const Derived& Other) const noexcept
    {
        return Derived{AsGlm() - Other.AsGlm()};
    }

    [[nodiscard]] Derived operator*(const Derived& Other) const noexcept
    {
        return Derived{AsGlm() * Other.AsGlm()};
    }

    [[nodiscard]] Derived operator/(const Derived& Other) const noexcept
    {
        return Derived{AsGlm() / Other.AsGlm()};
    }

    [[nodiscard]] Derived operator+(T Scalar) const noexcept
    {
        return Derived{AsGlm() + Scalar};
    }

    [[nodiscard]] Derived operator-(T Scalar) const noexcept
    {
        return Derived{AsGlm() - Scalar};
    }

    [[nodiscard]] Derived operator*(T Scalar) const noexcept
    {
        return Derived{AsGlm() * Scalar};
    }

    [[nodiscard]] Derived operator/(T Scalar) const noexcept
    {
        return Derived{AsGlm() / Scalar};
    }

    [[nodiscard]] bool operator==(const Derived& Other) const noexcept
    {
        return AsGlm() == Other.AsGlm();
    }

    [[nodiscard]] T Dot(const Derived& Other) const noexcept
    {
        return glm::dot(AsGlm(), Other.AsGlm());
    }

    [[nodiscard]] T LengthSquared() const noexcept
    {
        return glm::dot(AsGlm(), AsGlm());
    }

    [[nodiscard]] T Length() const noexcept
        requires std::floating_point<T>
    {
        return glm::length(AsGlm());
    }

    [[nodiscard]] T Distance(const Derived& Other) const noexcept
        requires std::floating_point<T>
    {
        return glm::distance(AsGlm(), Other.AsGlm());
    }

    [[nodiscard]] T DistanceSquared(const Derived& Other) const noexcept
    {
        const GlmType Delta = AsGlm() - Other.AsGlm();
        return glm::dot(Delta, Delta);
    }

    Derived& Normalize() noexcept
        requires std::floating_point<T>
    {
        AsGlm() = glm::normalize(AsGlm());
        return Self();
    }

    [[nodiscard]] Derived Normalized() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::normalize(AsGlm())};
    }

    [[nodiscard]] Derived SafeNormalized(const Derived& Fallback = Derived{}) const noexcept
        requires std::floating_point<T>
    {
        const T Squared = LengthSquared();
        if (Squared <= glm::epsilon<T>() * glm::epsilon<T>())
        {
            return Fallback;
        }
        return Derived{AsGlm() * (T{1} / glm::sqrt(Squared))};
    }

    [[nodiscard]] bool IsNormalized(T Epsilon = T(0.0001)) const noexcept
        requires std::floating_point<T>
    {
        return glm::abs(LengthSquared() - T{1}) <= Epsilon;
    }

    [[nodiscard]] bool Equals(const Derived& Other, T Epsilon) const noexcept
        requires std::floating_point<T>
    {
        return glm::all(glm::lessThanEqual(glm::abs(AsGlm() - Other.AsGlm()), GlmType{Epsilon}));
    }

    [[nodiscard]] Derived Abs() const noexcept
    {
        return Derived{glm::abs(AsGlm())};
    }

    [[nodiscard]] Derived Sign() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::sign(AsGlm())};
    }

    [[nodiscard]] Derived Floor() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::floor(AsGlm())};
    }

    [[nodiscard]] Derived Ceil() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::ceil(AsGlm())};
    }

    [[nodiscard]] Derived Round() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::round(AsGlm())};
    }

    [[nodiscard]] Derived Fract() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::fract(AsGlm())};
    }

    [[nodiscard]] Derived Min(const Derived& Other) const noexcept
    {
        return Derived{glm::min(AsGlm(), Other.AsGlm())};
    }

    [[nodiscard]] Derived Max(const Derived& Other) const noexcept
    {
        return Derived{glm::max(AsGlm(), Other.AsGlm())};
    }

    [[nodiscard]] Derived Clamp(T MinValue, T MaxValue) const noexcept
    {
        return Derived{glm::clamp(AsGlm(), MinValue, MaxValue)};
    }

    [[nodiscard]] Derived Clamp(const Derived& MinValue, const Derived& MaxValue) const noexcept
    {
        return Derived{glm::clamp(AsGlm(), MinValue.AsGlm(), MaxValue.AsGlm())};
    }

    [[nodiscard]] Derived Lerp(const Derived& Other, T Alpha) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::mix(AsGlm(), Other.AsGlm(), Alpha)};
    }

    [[nodiscard]] Derived Lerp(const Derived& Other, const Derived& Alpha) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::mix(AsGlm(), Other.AsGlm(), Alpha.AsGlm())};
    }

    [[nodiscard]] Derived Step(T Edge) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::step(Edge, AsGlm())};
    }

    [[nodiscard]] Derived SmoothStep(T Edge0, T Edge1) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::smoothstep(Edge0, Edge1, AsGlm())};
    }

    [[nodiscard]] Derived Pow(T Exponent) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::pow(AsGlm(), GlmType{Exponent})};
    }

    [[nodiscard]] Derived Sqrt() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::sqrt(AsGlm())};
    }

    [[nodiscard]] Derived InverseSqrt() const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::inversesqrt(AsGlm())};
    }

    [[nodiscard]] Derived Reflect(const Derived& Normal) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::reflect(AsGlm(), Normal.AsGlm())};
    }

    [[nodiscard]] Derived Refract(const Derived& Normal, T Eta) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::refract(AsGlm(), Normal.AsGlm(), Eta)};
    }

    [[nodiscard]] Derived FaceForward(const Derived& Incident, const Derived& Reference) const noexcept
        requires std::floating_point<T>
    {
        return Derived{glm::faceforward(AsGlm(), Incident.AsGlm(), Reference.AsGlm())};
    }

    [[nodiscard]] Derived Project(const Derived& Onto) const noexcept
        requires std::floating_point<T>
    {
        const T Denominator = Onto.LengthSquared();
        if (Denominator <= glm::epsilon<T>() * glm::epsilon<T>())
        {
            return Derived{};
        }
        return Onto * (Dot(Onto) / Denominator);
    }

    [[nodiscard]] T Angle(const Derived& Other) const noexcept
        requires std::floating_point<T>
    {
        return glm::acos(glm::clamp(Normalized().Dot(Other.Normalized()), T{-1}, T{1}));
    }

    [[nodiscard]] T MinComponent() const noexcept
    {
        T Result = AsGlm()[0];
        for (glm::length_t Index = 1; Index < Dimension; ++Index)
        {
            Result = glm::min(Result, AsGlm()[Index]);
        }
        return Result;
    }

    [[nodiscard]] T MaxComponent() const noexcept
    {
        T Result = AsGlm()[0];
        for (glm::length_t Index = 1; Index < Dimension; ++Index)
        {
            Result = glm::max(Result, AsGlm()[Index]);
        }
        return Result;
    }

    [[nodiscard]] bool AnyNaN() const noexcept
        requires std::floating_point<T>
    {
        return glm::any(glm::isnan(AsGlm()));
    }

    [[nodiscard]] bool AllFinite() const noexcept
        requires std::floating_point<T>
    {
        return glm::all(glm::not_(glm::isnan(AsGlm()))) && glm::all(glm::not_(glm::isinf(AsGlm())));
    }

    [[nodiscard]] String ToString() const
    {
        if constexpr (Dimension == 2)
        {
            return String::Format("({}, {})", AsGlm()[0], AsGlm()[1]);
        }
        else if constexpr (Dimension == 3)
        {
            return String::Format("({}, {}, {})", AsGlm()[0], AsGlm()[1], AsGlm()[2]);
        }
        else
        {
            return String::Format("({}, {}, {}, {})", AsGlm()[0], AsGlm()[1], AsGlm()[2], AsGlm()[3]);
        }
    }

private:
    [[nodiscard]] Derived& Self() noexcept
    {
        return *static_cast<Derived*>(this);
    }

    [[nodiscard]] const Derived& Self() const noexcept
    {
        return *static_cast<const Derived*>(this);
    }
};

template <typename T = float>
class Vector2 : public VectorOps<Vector2<T>, 2, T>
{
public:
    using GlmType = glm::vec<2, T>;

    T X{};
    T Y{};

    constexpr Vector2() noexcept = default;

    constexpr Vector2(T InX, T InY) noexcept : X(InX), Y(InY) {}

    constexpr explicit Vector2(T Scalar) noexcept : X(Scalar), Y(Scalar) {}

    constexpr Vector2(const GlmType& Value) noexcept : X(Value.x), Y(Value.y) {}

    [[nodiscard]] static constexpr Vector2 UnitX() noexcept
    {
        return {T{1}, T{0}};
    }

    [[nodiscard]] static constexpr Vector2 UnitY() noexcept
    {
        return {T{0}, T{1}};
    }

    [[nodiscard]] constexpr T Cross(const Vector2& Other) const noexcept
    {
        return X * Other.Y - Y * Other.X;
    }

    [[nodiscard]] constexpr Vector2 Perpendicular() const noexcept
    {
        return {-Y, X};
    }

    [[nodiscard]] constexpr Vector2 YX() const noexcept
    {
        return {Y, X};
    }

    [[nodiscard]] constexpr Vector3<T> XY0() const noexcept;
    [[nodiscard]] constexpr Vector3<T> XY1() const noexcept;
    [[nodiscard]] constexpr Vector4<T> XY00() const noexcept;
    [[nodiscard]] constexpr Vector4<T> XY01() const noexcept;
};

template <typename T = float>
class Vector3 : public VectorOps<Vector3<T>, 3, T>
{
public:
    using GlmType = glm::vec<3, T>;

    T X{};
    T Y{};
    T Z{};

    constexpr Vector3() noexcept = default;

    constexpr Vector3(T InX, T InY, T InZ) noexcept : X(InX), Y(InY), Z(InZ) {}

    constexpr explicit Vector3(T Scalar) noexcept : X(Scalar), Y(Scalar), Z(Scalar) {}

    constexpr Vector3(const Vector2<T>& XY, T InZ) noexcept : X(XY.X), Y(XY.Y), Z(InZ) {}

    constexpr Vector3(T InX, const Vector2<T>& YZ) noexcept : X(InX), Y(YZ.X), Z(YZ.Y) {}

    constexpr Vector3(const GlmType& Value) noexcept : X(Value.x), Y(Value.y), Z(Value.z) {}

    [[nodiscard]] static constexpr Vector3 UnitX() noexcept
    {
        return {T{1}, T{0}, T{0}};
    }

    [[nodiscard]] static constexpr Vector3 UnitY() noexcept
    {
        return {T{0}, T{1}, T{0}};
    }

    [[nodiscard]] static constexpr Vector3 UnitZ() noexcept
    {
        return {T{0}, T{0}, T{1}};
    }

    [[nodiscard]] Vector3 Cross(const Vector3& Other) const noexcept
    {
        return Vector3{glm::cross(this->AsGlm(), Other.AsGlm())};
    }

    [[nodiscard]] constexpr Vector2<T> XY() const noexcept
    {
        return {X, Y};
    }

    [[nodiscard]] constexpr Vector2<T> XZ() const noexcept
    {
        return {X, Z};
    }

    [[nodiscard]] constexpr Vector2<T> YZ() const noexcept
    {
        return {Y, Z};
    }

    [[nodiscard]] constexpr Vector2<T> YX() const noexcept
    {
        return {Y, X};
    }

    [[nodiscard]] constexpr Vector4<T> XYZ0() const noexcept;
    [[nodiscard]] constexpr Vector4<T> XYZ1() const noexcept;
};

template <typename T = float>
class Vector4 : public VectorOps<Vector4<T>, 4, T>
{
public:
    using GlmType = glm::vec<4, T>;

    T X{};
    T Y{};
    T Z{};
    T W{};

    constexpr Vector4() noexcept = default;

    constexpr Vector4(T InX, T InY, T InZ, T InW) noexcept : X(InX), Y(InY), Z(InZ), W(InW) {}

    constexpr explicit Vector4(T Scalar) noexcept : X(Scalar), Y(Scalar), Z(Scalar), W(Scalar) {}

    constexpr Vector4(const Vector3<T>& XYZ, T InW) noexcept : X(XYZ.X), Y(XYZ.Y), Z(XYZ.Z), W(InW) {}

    constexpr Vector4(const Vector2<T>& XY, T InZ, T InW) noexcept : X(XY.X), Y(XY.Y), Z(InZ), W(InW) {}

    constexpr Vector4(const Vector2<T>& XY, const Vector2<T>& ZW) noexcept : X(XY.X), Y(XY.Y), Z(ZW.X), W(ZW.Y) {}

    constexpr Vector4(const GlmType& Value) noexcept : X(Value.x), Y(Value.y), Z(Value.z), W(Value.w) {}

    [[nodiscard]] static constexpr Vector4 UnitX() noexcept
    {
        return {T{1}, T{0}, T{0}, T{0}};
    }

    [[nodiscard]] static constexpr Vector4 UnitY() noexcept
    {
        return {T{0}, T{1}, T{0}, T{0}};
    }

    [[nodiscard]] static constexpr Vector4 UnitZ() noexcept
    {
        return {T{0}, T{0}, T{1}, T{0}};
    }

    [[nodiscard]] static constexpr Vector4 UnitW() noexcept
    {
        return {T{0}, T{0}, T{0}, T{1}};
    }

    [[nodiscard]] constexpr Vector2<T> XY() const noexcept
    {
        return {X, Y};
    }

    [[nodiscard]] constexpr Vector2<T> XZ() const noexcept
    {
        return {X, Z};
    }

    [[nodiscard]] constexpr Vector2<T> YZ() const noexcept
    {
        return {Y, Z};
    }

    [[nodiscard]] constexpr Vector3<T> XYZ() const noexcept
    {
        return {X, Y, Z};
    }

    [[nodiscard]] constexpr Vector3<T> XYW() const noexcept
    {
        return {X, Y, W};
    }

    [[nodiscard]] constexpr Vector2<T> ZW() const noexcept
    {
        return {Z, W};
    }
};

template <typename T>
[[nodiscard]] constexpr Vector3<T> Vector2<T>::XY0() const noexcept
{
    return {X, Y, T{0}};
}

template <typename T>
[[nodiscard]] constexpr Vector3<T> Vector2<T>::XY1() const noexcept
{
    return {X, Y, T{1}};
}

template <typename T>
[[nodiscard]] constexpr Vector4<T> Vector2<T>::XY00() const noexcept
{
    return {X, Y, T{0}, T{0}};
}

template <typename T>
[[nodiscard]] constexpr Vector4<T> Vector2<T>::XY01() const noexcept
{
    return {X, Y, T{0}, T{1}};
}

template <typename T>
[[nodiscard]] constexpr Vector4<T> Vector3<T>::XYZ0() const noexcept
{
    return {X, Y, Z, T{0}};
}

template <typename T>
[[nodiscard]] constexpr Vector4<T> Vector3<T>::XYZ1() const noexcept
{
    return {X, Y, Z, T{1}};
}

template <typename Derived, glm::length_t Dimension, typename T>
[[nodiscard]] Derived operator*(T Scalar, const VectorOps<Derived, Dimension, T>& Value) noexcept
{
    return Derived{Value.AsGlm() * Scalar};
}

template <typename Derived, glm::length_t Dimension, typename T>
[[nodiscard]] Derived operator+(T Scalar, const VectorOps<Derived, Dimension, T>& Value) noexcept
{
    return Derived{Scalar + Value.AsGlm()};
}

template <typename Derived, glm::length_t Dimension, typename T>
[[nodiscard]] Derived operator-(T Scalar, const VectorOps<Derived, Dimension, T>& Value) noexcept
{
    return Derived{Scalar - Value.AsGlm()};
}

template <typename Derived, glm::length_t Dimension, typename T>
[[nodiscard]] Derived operator/(T Scalar, const VectorOps<Derived, Dimension, T>& Value) noexcept
{
    return Derived{Scalar / Value.AsGlm()};
}

static_assert(sizeof(Vector2<float>) == sizeof(glm::vec2));
static_assert(alignof(Vector2<float>) == alignof(glm::vec2));
static_assert(sizeof(Vector3<float>) == sizeof(glm::vec3));
static_assert(alignof(Vector3<float>) == alignof(glm::vec3));
static_assert(sizeof(Vector4<float>) == sizeof(glm::vec4));
static_assert(alignof(Vector4<float>) == alignof(glm::vec4));

using Vector2f = Vector2<float>;
using Vector3f = Vector3<float>;
using Vector4f = Vector4<float>;
using Vector2d = Vector2<double>;
using Vector3d = Vector3<double>;
using Vector4d = Vector4<double>;
using Vector2i = Vector2<int>;
using Vector3i = Vector3<int>;
using Vector4i = Vector4<int>;
