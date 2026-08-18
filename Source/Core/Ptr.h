#pragma once

#include <charconv>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>

#include "Core/String/String.h"

namespace PtrDetail
{

[[nodiscard]] inline String FormatPointer(std::uintptr_t Value)
{
    if (Value == 0)
    {
        return String{"0"};
    }

    char Buffer[2 + sizeof(std::uintptr_t) * 2];
    Buffer[0] = '0';
    Buffer[1] = 'x';
    const auto [End, ErrorCode] = std::to_chars(Buffer + 2, Buffer + std::size(Buffer), Value, 16);
    (void)ErrorCode;
    return String{Buffer, static_cast<String::SizeType>(End - Buffer)};
}

[[nodiscard]] inline String FormatPointer(std::intptr_t Value)
{
    if (Value == 0)
    {
        return String{"0"};
    }

    char Buffer[3 + sizeof(std::uintptr_t) * 2];
    std::size_t PrefixLength = 0;
    if (Value < 0)
    {
        Buffer[0] = '-';
        PrefixLength = 1;
        Value = -Value;
    }

    Buffer[PrefixLength + 0] = '0';
    Buffer[PrefixLength + 1] = 'x';
    const auto [End, ErrorCode] =
        std::to_chars(Buffer + PrefixLength + 2, Buffer + std::size(Buffer), static_cast<std::uintptr_t>(Value), 16);
    (void)ErrorCode;
    return String{Buffer, static_cast<String::SizeType>(End - Buffer)};
}

} // namespace PtrDetail

class UIntPtr
{
public:
    using ValueType = std::uintptr_t;

    std::uintptr_t Value{0};

    [[nodiscard]] static constexpr UIntPtr Null() noexcept
    {
        return UIntPtr{};
    }

    template <typename T>
        requires std::is_pointer_v<T>
    [[nodiscard]] static constexpr UIntPtr FromPtr(T Pointer) noexcept
    {
        return UIntPtr{reinterpret_cast<std::uintptr_t>(Pointer)};
    }

    constexpr UIntPtr() noexcept = default;

    constexpr explicit UIntPtr(std::uintptr_t InValue) noexcept
        : Value(InValue)
    {
    }

    [[nodiscard]] constexpr explicit operator std::uintptr_t() const noexcept
    {
        return Value;
    }

    [[nodiscard]] constexpr bool IsNull() const noexcept
    {
        return Value == 0;
    }

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return Value != 0;
    }

    [[nodiscard]] String ToString() const
    {
        return PtrDetail::FormatPointer(Value);
    }

    template <typename T>
        requires std::is_pointer_v<T>
    [[nodiscard]] constexpr T ToPtr() const noexcept
    {
        return reinterpret_cast<T>(Value);
    }

    [[nodiscard]] constexpr void* ToVoidPtr() const noexcept
    {
        return reinterpret_cast<void*>(Value);
    }

    [[nodiscard]] constexpr auto operator<=>(const UIntPtr&) const noexcept = default;
};

class IntPtr
{
public:
    using ValueType = std::intptr_t;

    std::intptr_t Value{0};

    [[nodiscard]] static constexpr IntPtr Null() noexcept
    {
        return IntPtr{};
    }

    template <typename T>
        requires std::is_pointer_v<T>
    [[nodiscard]] static constexpr IntPtr FromPtr(T Pointer) noexcept
    {
        return IntPtr{reinterpret_cast<std::intptr_t>(Pointer)};
    }

    constexpr IntPtr() noexcept = default;

    constexpr explicit IntPtr(std::intptr_t InValue) noexcept
        : Value(InValue)
    {
    }

    constexpr explicit IntPtr(UIntPtr Pointer) noexcept
        : Value(static_cast<std::intptr_t>(Pointer.Value))
    {
    }

    [[nodiscard]] constexpr explicit operator std::intptr_t() const noexcept
    {
        return Value;
    }

    [[nodiscard]] constexpr bool IsNull() const noexcept
    {
        return Value == 0;
    }

    [[nodiscard]] constexpr bool IsValid() const noexcept
    {
        return Value != 0;
    }

    [[nodiscard]] String ToString() const
    {
        return PtrDetail::FormatPointer(Value);
    }

    template <typename T>
        requires std::is_pointer_v<T>
    [[nodiscard]] constexpr T ToPtr() const noexcept
    {
        return reinterpret_cast<T>(static_cast<std::uintptr_t>(Value));
    }

    [[nodiscard]] constexpr void* ToVoidPtr() const noexcept
    {
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(Value));
    }

    [[nodiscard]] constexpr auto operator<=>(const IntPtr&) const noexcept = default;
};

template <>
struct fmt::formatter<UIntPtr> : fmt::formatter<std::string_view>
{
    auto format(const UIntPtr& Value, fmt::format_context& Context) const
    {
        const String Text = Value.ToString();
        return formatter<std::string_view>::format(Text.View().ToStdStringView(), Context);
    }
};

template <>
struct fmt::formatter<IntPtr> : fmt::formatter<std::string_view>
{
    auto format(const IntPtr& Value, fmt::format_context& Context) const
    {
        const String Text = Value.ToString();
        return formatter<std::string_view>::format(Text.View().ToStdStringView(), Context);
    }
};

template <>
struct std::hash<UIntPtr>
{
    [[nodiscard]] std::size_t operator()(const UIntPtr Value) const noexcept
    {
        return std::hash<std::uintptr_t>{}(Value.Value);
    }
};

template <>
struct std::hash<IntPtr>
{
    [[nodiscard]] std::size_t operator()(const IntPtr Value) const noexcept
    {
        return std::hash<std::intptr_t>{}(Value.Value);
    }
};

static_assert(sizeof(UIntPtr) == sizeof(std::uintptr_t));
static_assert(sizeof(IntPtr) == sizeof(std::intptr_t));
static_assert(std::is_trivially_copyable_v<UIntPtr>);
static_assert(std::is_trivially_copyable_v<IntPtr>);
