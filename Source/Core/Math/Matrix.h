#pragma once

#include <concepts>

#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/matrix.hpp>

#include "Core/Math/Vector.h"
#include "Core/String/String.h"

template <typename T = float>
class Matrix4x4
{
public:
    using ValueType = T;
    using GlmType = glm::mat<4, 4, T>;

    Vector4<T> Columns[4]{
        Vector4<T>{T{1}, T{0}, T{0}, T{0}},
        Vector4<T>{T{0}, T{1}, T{0}, T{0}},
        Vector4<T>{T{0}, T{0}, T{1}, T{0}},
        Vector4<T>{T{0}, T{0}, T{0}, T{1}},
    };

    constexpr Matrix4x4() noexcept = default;

    constexpr Matrix4x4(const Vector4<T>& Column0,
                        const Vector4<T>& Column1,
                        const Vector4<T>& Column2,
                        const Vector4<T>& Column3) noexcept : Columns{Column0, Column1, Column2, Column3}
    {
    }

    explicit Matrix4x4(T Diagonal) noexcept
    {
        AsGlm() = GlmType{Diagonal};
    }

    Matrix4x4(const GlmType& Value) noexcept
    {
        AsGlm() = Value;
    }

    [[nodiscard]] GlmType& AsGlm() noexcept
    {
        return *reinterpret_cast<GlmType*>(this);
    }

    [[nodiscard]] const GlmType& AsGlm() const noexcept
    {
        return *reinterpret_cast<const GlmType*>(this);
    }

    [[nodiscard]] T* Data() noexcept
    {
        return Columns[0].Data();
    }

    [[nodiscard]] const T* Data() const noexcept
    {
        return Columns[0].Data();
    }

    [[nodiscard]] Vector4<T>& operator[](glm::length_t Index) noexcept
    {
        return Columns[Index];
    }

    [[nodiscard]] const Vector4<T>& operator[](glm::length_t Index) const noexcept
    {
        return Columns[Index];
    }

    [[nodiscard]] T& operator()(glm::length_t Row, glm::length_t Column) noexcept
    {
        return Columns[Column][Row];
    }

    [[nodiscard]] const T& operator()(glm::length_t Row, glm::length_t Column) const noexcept
    {
        return Columns[Column][Row];
    }

    [[nodiscard]] static Matrix4x4 Identity() noexcept
    {
        return Matrix4x4{};
    }

    [[nodiscard]] static Matrix4x4 Zero() noexcept
    {
        Matrix4x4 Result;
        Result.AsGlm() = GlmType{T{0}};
        return Result;
    }

    [[nodiscard]] static Matrix4x4 Translation(const Vector3<T>& Offset) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::translate(GlmType{T{1}}, Offset.AsGlm())};
    }

    [[nodiscard]] static Matrix4x4 Rotation(T AngleRadians, const Vector3<T>& Axis) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::rotate(GlmType{T{1}}, AngleRadians, Axis.AsGlm())};
    }

    [[nodiscard]] static Matrix4x4 RotationX(T AngleRadians) noexcept
        requires std::floating_point<T>
    {
        return Rotation(AngleRadians, Vector3<T>::UnitX());
    }

    [[nodiscard]] static Matrix4x4 RotationY(T AngleRadians) noexcept
        requires std::floating_point<T>
    {
        return Rotation(AngleRadians, Vector3<T>::UnitY());
    }

    [[nodiscard]] static Matrix4x4 RotationZ(T AngleRadians) noexcept
        requires std::floating_point<T>
    {
        return Rotation(AngleRadians, Vector3<T>::UnitZ());
    }

    [[nodiscard]] static Matrix4x4 Scaling(const Vector3<T>& Scale) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::scale(GlmType{T{1}}, Scale.AsGlm())};
    }

    [[nodiscard]] static Matrix4x4 Scaling(T Scale) noexcept
        requires std::floating_point<T>
    {
        return Scaling(Vector3<T>{Scale});
    }

    [[nodiscard]] static Matrix4x4
    LookAt(const Vector3<T>& Eye, const Vector3<T>& Center, const Vector3<T>& Up) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::lookAt(Eye.AsGlm(), Center.AsGlm(), Up.AsGlm())};
    }

    [[nodiscard]] static Matrix4x4
    LookAtRH(const Vector3<T>& Eye, const Vector3<T>& Center, const Vector3<T>& Up) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::lookAtRH(Eye.AsGlm(), Center.AsGlm(), Up.AsGlm())};
    }

    [[nodiscard]] static Matrix4x4
    LookAtLH(const Vector3<T>& Eye, const Vector3<T>& Center, const Vector3<T>& Up) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::lookAtLH(Eye.AsGlm(), Center.AsGlm(), Up.AsGlm())};
    }

    [[nodiscard]] static Matrix4x4 Perspective(T FovYRadians, T Aspect, T Near, T Far) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::perspective(FovYRadians, Aspect, Near, Far)};
    }

    [[nodiscard]] static Matrix4x4 PerspectiveRH(T FovYRadians, T Aspect, T Near, T Far) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::perspectiveRH(FovYRadians, Aspect, Near, Far)};
    }

    [[nodiscard]] static Matrix4x4 PerspectiveLH(T FovYRadians, T Aspect, T Near, T Far) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::perspectiveLH(FovYRadians, Aspect, Near, Far)};
    }

    [[nodiscard]] static Matrix4x4 Ortho(T Left, T Right, T Bottom, T Top) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::ortho(Left, Right, Bottom, Top)};
    }

    [[nodiscard]] static Matrix4x4 Ortho(T Left, T Right, T Bottom, T Top, T Near, T Far) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::ortho(Left, Right, Bottom, Top, Near, Far)};
    }

    [[nodiscard]] static Matrix4x4 OrthoRH(T Left, T Right, T Bottom, T Top, T Near, T Far) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::orthoRH(Left, Right, Bottom, Top, Near, Far)};
    }

    [[nodiscard]] static Matrix4x4 OrthoLH(T Left, T Right, T Bottom, T Top, T Near, T Far) noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::orthoLH(Left, Right, Bottom, Top, Near, Far)};
    }

    Matrix4x4& operator+=(const Matrix4x4& Other) noexcept
    {
        AsGlm() += Other.AsGlm();
        return *this;
    }

    Matrix4x4& operator-=(const Matrix4x4& Other) noexcept
    {
        AsGlm() -= Other.AsGlm();
        return *this;
    }

    Matrix4x4& operator*=(const Matrix4x4& Other) noexcept
    {
        AsGlm() *= Other.AsGlm();
        return *this;
    }

    Matrix4x4& operator*=(T Scalar) noexcept
    {
        AsGlm() *= Scalar;
        return *this;
    }

    Matrix4x4& operator/=(T Scalar) noexcept
    {
        AsGlm() /= Scalar;
        return *this;
    }

    [[nodiscard]] Matrix4x4 operator+() const noexcept
    {
        return *this;
    }

    [[nodiscard]] Matrix4x4 operator-() const noexcept
    {
        return Matrix4x4{-AsGlm()};
    }

    [[nodiscard]] Matrix4x4 operator+(const Matrix4x4& Other) const noexcept
    {
        return Matrix4x4{AsGlm() + Other.AsGlm()};
    }

    [[nodiscard]] Matrix4x4 operator-(const Matrix4x4& Other) const noexcept
    {
        return Matrix4x4{AsGlm() - Other.AsGlm()};
    }

    [[nodiscard]] Matrix4x4 operator*(const Matrix4x4& Other) const noexcept
    {
        return Matrix4x4{AsGlm() * Other.AsGlm()};
    }

    [[nodiscard]] Vector4<T> operator*(const Vector4<T>& Value) const noexcept
    {
        return Vector4<T>{AsGlm() * Value.AsGlm()};
    }

    [[nodiscard]] Matrix4x4 operator*(T Scalar) const noexcept
    {
        return Matrix4x4{AsGlm() * Scalar};
    }

    [[nodiscard]] Matrix4x4 operator/(T Scalar) const noexcept
    {
        return Matrix4x4{AsGlm() / Scalar};
    }

    [[nodiscard]] bool operator==(const Matrix4x4& Other) const noexcept
    {
        return AsGlm() == Other.AsGlm();
    }

    [[nodiscard]] Matrix4x4 CompMul(const Matrix4x4& Other) const noexcept
    {
        return Matrix4x4{glm::matrixCompMult(AsGlm(), Other.AsGlm())};
    }

    [[nodiscard]] T Determinant() const noexcept
        requires std::floating_point<T>
    {
        return glm::determinant(AsGlm());
    }

    [[nodiscard]] Matrix4x4 Transposed() const noexcept
    {
        return Matrix4x4{glm::transpose(AsGlm())};
    }

    Matrix4x4& Transpose() noexcept
    {
        AsGlm() = glm::transpose(AsGlm());
        return *this;
    }

    [[nodiscard]] Matrix4x4 Inversed() const noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::inverse(AsGlm())};
    }

    Matrix4x4& Inverse() noexcept
        requires std::floating_point<T>
    {
        AsGlm() = glm::inverse(AsGlm());
        return *this;
    }

    [[nodiscard]] Matrix4x4 AffineInversed() const noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::affineInverse(AsGlm())};
    }

    [[nodiscard]] Matrix4x4 Translated(const Vector3<T>& Offset) const noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::translate(AsGlm(), Offset.AsGlm())};
    }

    [[nodiscard]] Matrix4x4 Rotated(T AngleRadians, const Vector3<T>& Axis) const noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::rotate(AsGlm(), AngleRadians, Axis.AsGlm())};
    }

    [[nodiscard]] Matrix4x4 Scaled(const Vector3<T>& Scale) const noexcept
        requires std::floating_point<T>
    {
        return Matrix4x4{glm::scale(AsGlm(), Scale.AsGlm())};
    }

    [[nodiscard]] Vector3<T> TransformPoint(const Vector3<T>& Point) const noexcept
        requires std::floating_point<T>
    {
        const Vector4<T> Transformed = *this * Vector4<T>{Point, T{1}};
        if (Transformed.W != T{0})
        {
            return Transformed.XYZ() / Transformed.W;
        }
        return Transformed.XYZ();
    }

    [[nodiscard]] Vector3<T> TransformVector(const Vector3<T>& Direction) const noexcept
        requires std::floating_point<T>
    {
        return (*this * Vector4<T>{Direction, T{0}}).XYZ();
    }

    [[nodiscard]] Vector3<T> GetTranslation() const noexcept
    {
        return Columns[3].XYZ();
    }

    Matrix4x4& SetTranslation(const Vector3<T>& Offset) noexcept
    {
        Columns[3].X = Offset.X;
        Columns[3].Y = Offset.Y;
        Columns[3].Z = Offset.Z;
        return *this;
    }

    [[nodiscard]] Vector3<T> GetScale() const noexcept
        requires std::floating_point<T>
    {
        return {
            Columns[0].XYZ().Length(),
            Columns[1].XYZ().Length(),
            Columns[2].XYZ().Length(),
        };
    }

    [[nodiscard]] Vector4<T> GetColumn(glm::length_t Index) const noexcept
    {
        return Columns[Index];
    }

    [[nodiscard]] Vector4<T> GetRow(glm::length_t Index) const noexcept
    {
        return {Columns[0][Index], Columns[1][Index], Columns[2][Index], Columns[3][Index]};
    }

    Matrix4x4& SetColumn(glm::length_t Index, const Vector4<T>& Value) noexcept
    {
        Columns[Index] = Value;
        return *this;
    }

    Matrix4x4& SetRow(glm::length_t Index, const Vector4<T>& Value) noexcept
    {
        Columns[0][Index] = Value.X;
        Columns[1][Index] = Value.Y;
        Columns[2][Index] = Value.Z;
        Columns[3][Index] = Value.W;
        return *this;
    }

    [[nodiscard]] String ToString() const
    {
        const Vector4<T> Row0 = GetRow(0);
        const Vector4<T> Row1 = GetRow(1);
        const Vector4<T> Row2 = GetRow(2);
        const Vector4<T> Row3 = GetRow(3);
        return String::Format("[{}, {}, {}, {}]", Row0, Row1, Row2, Row3);
    }
};

template <typename T>
[[nodiscard]] Matrix4x4<T> operator*(T Scalar, const Matrix4x4<T>& Value) noexcept
{
    return Value * Scalar;
}

static_assert(sizeof(Matrix4x4<float>) == sizeof(glm::mat4));
static_assert(alignof(Matrix4x4<float>) == alignof(glm::mat4));

using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4d = Matrix4x4<double>;
