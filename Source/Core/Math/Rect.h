#pragma once

#include "Core/Math/Vector.h"
#include "Core/String/String.h"

template <typename T = float>
class Rect
{
public:
    using ValueType = T;

    Vector2<T> Min{};
    Vector2<T> Max{};

    constexpr Rect() noexcept = default;

    constexpr Rect(const Vector2<T>& InMin, const Vector2<T>& InMax) noexcept
        : Min(InMin),
          Max(InMax)
    {
    }

    constexpr Rect(T MinX, T MinY, T MaxX, T MaxY) noexcept
        : Min(MinX, MinY),
          Max(MaxX, MaxY)
    {
    }

    [[nodiscard]] static constexpr Rect FromPositionSize(const Vector2<T>& Position, const Vector2<T>& Size) noexcept
    {
        return {Position, Position + Size};
    }

    [[nodiscard]] static constexpr Rect FromCenterSize(const Vector2<T>& Center, const Vector2<T>& Size) noexcept
    {
        const Vector2<T> Half = Size * T{0.5};
        return {Center - Half, Center + Half};
    }

    [[nodiscard]] constexpr Vector2<T> GetSize() const noexcept
    {
        return Max - Min;
    }

    [[nodiscard]] constexpr T Width() const noexcept
    {
        return Max.X - Min.X;
    }

    [[nodiscard]] constexpr T Height() const noexcept
    {
        return Max.Y - Min.Y;
    }

    [[nodiscard]] constexpr Vector2<T> GetCenter() const noexcept
    {
        return (Min + Max) * T{0.5};
    }

    [[nodiscard]] constexpr Vector2<T> GetPosition() const noexcept
    {
        return Min;
    }

    [[nodiscard]] constexpr T Area() const noexcept
    {
        const Vector2<T> Size = GetSize();
        return Size.X * Size.Y;
    }

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return Min.X <= Max.X && Min.Y <= Max.Y;
    }

    [[nodiscard]] constexpr bool IsEmpty() const noexcept
    {
        return Min.X >= Max.X || Min.Y >= Max.Y;
    }

    Rect& MakeValid() noexcept
    {
        const Vector2<T> NewMin = Min.Min(Max);
        const Vector2<T> NewMax = Min.Max(Max);
        Min = NewMin;
        Max = NewMax;
        return *this;
    }

    [[nodiscard]] constexpr bool Contains(const Vector2<T>& Point) const noexcept
    {
        return Point.X >= Min.X && Point.X <= Max.X && Point.Y >= Min.Y && Point.Y <= Max.Y;
    }

    [[nodiscard]] constexpr bool Contains(const Rect& Other) const noexcept
    {
        return Other.Min.X >= Min.X && Other.Max.X <= Max.X && Other.Min.Y >= Min.Y && Other.Max.Y <= Max.Y;
    }

    [[nodiscard]] constexpr bool Overlaps(const Rect& Other) const noexcept
    {
        return Min.X <= Other.Max.X && Max.X >= Other.Min.X && Min.Y <= Other.Max.Y && Max.Y >= Other.Min.Y;
    }

    [[nodiscard]] Rect Intersect(const Rect& Other) const noexcept
    {
        Rect Result{Min.Max(Other.Min), Max.Min(Other.Max)};
        if (!Result.IsValid())
        {
            return Rect{};
        }
        return Result;
    }

    [[nodiscard]] Rect Combined(const Rect& Other) const noexcept
    {
        if (IsEmpty())
        {
            return Other;
        }
        if (Other.IsEmpty())
        {
            return *this;
        }
        return {Min.Min(Other.Min), Max.Max(Other.Max)};
    }

    Rect& Combine(const Rect& Other) noexcept
    {
        *this = Combined(Other);
        return *this;
    }

    [[nodiscard]] Rect Expanded(T Amount) const noexcept
    {
        const Vector2<T> Delta{Amount, Amount};
        return {Min - Delta, Max + Delta};
    }

    [[nodiscard]] Rect Expanded(const Vector2<T>& Amount) const noexcept
    {
        return {Min - Amount, Max + Amount};
    }

    Rect& Expand(T Amount) noexcept
    {
        *this = Expanded(Amount);
        return *this;
    }

    Rect& Expand(const Vector2<T>& Amount) noexcept
    {
        *this = Expanded(Amount);
        return *this;
    }

    [[nodiscard]] Rect Offset(const Vector2<T>& Delta) const noexcept
    {
        return {Min + Delta, Max + Delta};
    }

    Rect& operator+=(const Vector2<T>& Delta) noexcept
    {
        Min += Delta;
        Max += Delta;
        return *this;
    }

    Rect& operator-=(const Vector2<T>& Delta) noexcept
    {
        Min -= Delta;
        Max -= Delta;
        return *this;
    }

    [[nodiscard]] Rect operator+(const Vector2<T>& Delta) const noexcept
    {
        return Offset(Delta);
    }

    [[nodiscard]] Rect operator-(const Vector2<T>& Delta) const noexcept
    {
        return Offset(-Delta);
    }

    [[nodiscard]] constexpr bool operator==(const Rect& Other) const noexcept
    {
        return Min == Other.Min && Max == Other.Max;
    }

    [[nodiscard]] Vector2<T> GetClosestPoint(const Vector2<T>& Point) const noexcept
    {
        return Point.Clamp(Min, Max);
    }

    [[nodiscard]] String ToString() const
    {
        return String::Format("Rect(Min={}, Max={})", Min, Max);
    }
};

template <typename T = float>
class Rect3D
{
public:
    using ValueType = T;

    Vector3<T> Min{};
    Vector3<T> Max{};

    constexpr Rect3D() noexcept = default;

    constexpr Rect3D(const Vector3<T>& InMin, const Vector3<T>& InMax) noexcept
        : Min(InMin),
          Max(InMax)
    {
    }

    constexpr Rect3D(T MinX, T MinY, T MinZ, T MaxX, T MaxY, T MaxZ) noexcept
        : Min(MinX, MinY, MinZ),
          Max(MaxX, MaxY, MaxZ)
    {
    }

    [[nodiscard]] static constexpr Rect3D FromPositionSize(const Vector3<T>& Position, const Vector3<T>& Size) noexcept
    {
        return {Position, Position + Size};
    }

    [[nodiscard]] static constexpr Rect3D FromCenterSize(const Vector3<T>& Center, const Vector3<T>& Size) noexcept
    {
        const Vector3<T> Half = Size * T{0.5};
        return {Center - Half, Center + Half};
    }

    [[nodiscard]] constexpr Vector3<T> GetSize() const noexcept
    {
        return Max - Min;
    }

    [[nodiscard]] constexpr T Width() const noexcept
    {
        return Max.X - Min.X;
    }

    [[nodiscard]] constexpr T Height() const noexcept
    {
        return Max.Y - Min.Y;
    }

    [[nodiscard]] constexpr T Depth() const noexcept
    {
        return Max.Z - Min.Z;
    }

    [[nodiscard]] constexpr Vector3<T> GetCenter() const noexcept
    {
        return (Min + Max) * T{0.5};
    }

    [[nodiscard]] constexpr Vector3<T> GetPosition() const noexcept
    {
        return Min;
    }

    [[nodiscard]] constexpr T Volume() const noexcept
    {
        const Vector3<T> Size = GetSize();
        return Size.X * Size.Y * Size.Z;
    }

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return Min.X <= Max.X && Min.Y <= Max.Y && Min.Z <= Max.Z;
    }

    [[nodiscard]] constexpr bool IsEmpty() const noexcept
    {
        return Min.X >= Max.X || Min.Y >= Max.Y || Min.Z >= Max.Z;
    }

    Rect3D& MakeValid() noexcept
    {
        const Vector3<T> NewMin = Min.Min(Max);
        const Vector3<T> NewMax = Min.Max(Max);
        Min = NewMin;
        Max = NewMax;
        return *this;
    }

    [[nodiscard]] constexpr bool Contains(const Vector3<T>& Point) const noexcept
    {
        return Point.X >= Min.X && Point.X <= Max.X && Point.Y >= Min.Y && Point.Y <= Max.Y && Point.Z >= Min.Z &&
               Point.Z <= Max.Z;
    }

    [[nodiscard]] constexpr bool Contains(const Rect3D& Other) const noexcept
    {
        return Other.Min.X >= Min.X && Other.Max.X <= Max.X && Other.Min.Y >= Min.Y && Other.Max.Y <= Max.Y &&
               Other.Min.Z >= Min.Z && Other.Max.Z <= Max.Z;
    }

    [[nodiscard]] constexpr bool Overlaps(const Rect3D& Other) const noexcept
    {
        return Min.X <= Other.Max.X && Max.X >= Other.Min.X && Min.Y <= Other.Max.Y && Max.Y >= Other.Min.Y &&
               Min.Z <= Other.Max.Z && Max.Z >= Other.Min.Z;
    }

    [[nodiscard]] Rect3D Intersect(const Rect3D& Other) const noexcept
    {
        Rect3D Result{Min.Max(Other.Min), Max.Min(Other.Max)};
        if (!Result.IsValid())
        {
            return Rect3D{};
        }
        return Result;
    }

    [[nodiscard]] Rect3D Combined(const Rect3D& Other) const noexcept
    {
        if (IsEmpty())
        {
            return Other;
        }
        if (Other.IsEmpty())
        {
            return *this;
        }
        return {Min.Min(Other.Min), Max.Max(Other.Max)};
    }

    Rect3D& Combine(const Rect3D& Other) noexcept
    {
        *this = Combined(Other);
        return *this;
    }

    [[nodiscard]] Rect3D Expanded(T Amount) const noexcept
    {
        const Vector3<T> Delta{Amount};
        return {Min - Delta, Max + Delta};
    }

    [[nodiscard]] Rect3D Expanded(const Vector3<T>& Amount) const noexcept
    {
        return {Min - Amount, Max + Amount};
    }

    Rect3D& Expand(T Amount) noexcept
    {
        *this = Expanded(Amount);
        return *this;
    }

    Rect3D& Expand(const Vector3<T>& Amount) noexcept
    {
        *this = Expanded(Amount);
        return *this;
    }

    [[nodiscard]] Rect3D Offset(const Vector3<T>& Delta) const noexcept
    {
        return {Min + Delta, Max + Delta};
    }

    Rect3D& operator+=(const Vector3<T>& Delta) noexcept
    {
        Min += Delta;
        Max += Delta;
        return *this;
    }

    Rect3D& operator-=(const Vector3<T>& Delta) noexcept
    {
        Min -= Delta;
        Max -= Delta;
        return *this;
    }

    [[nodiscard]] Rect3D operator+(const Vector3<T>& Delta) const noexcept
    {
        return Offset(Delta);
    }

    [[nodiscard]] Rect3D operator-(const Vector3<T>& Delta) const noexcept
    {
        return Offset(-Delta);
    }

    [[nodiscard]] constexpr bool operator==(const Rect3D& Other) const noexcept
    {
        return Min == Other.Min && Max == Other.Max;
    }

    [[nodiscard]] Vector3<T> GetClosestPoint(const Vector3<T>& Point) const noexcept
    {
        return Point.Clamp(Min, Max);
    }

    [[nodiscard]] String ToString() const
    {
        return String::Format("Rect3D(Min={}, Max={})", Min, Max);
    }
};

using Rectf = Rect<float>;
using Rectd = Rect<double>;
using Recti = Rect<int>;
using Rect3Df = Rect3D<float>;
using Rect3Dd = Rect3D<double>;
using Rect3Di = Rect3D<int>;
