#pragma once

#include <charconv>
#include <concepts>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

#include <spdlog/fmt/fmt.h>

#include "Core/Reflection/Utils.h"
#include "Core/String/String.h"

template <typename T>
    requires std::is_enum_v<T>
[[nodiscard]] String ToString(T Value)
{
    return String{Reflection::GetEnumString(Value)};
}

template <typename T>
    requires std::integral<T> && (!std::same_as<T, bool>) && (!std::same_as<T, char>)
[[nodiscard]] String ToString(T Value)
{
    char Buffer[32];
    const auto [End, ErrorCode] = std::to_chars(Buffer, Buffer + sizeof(Buffer), Value);
    (void)ErrorCode;
    return String{Buffer, static_cast<String::SizeType>(End - Buffer)};
}

template <typename T>
    requires std::floating_point<T>
[[nodiscard]] String ToString(T Value)
{
    char Buffer[64];
    const auto [End, ErrorCode] = std::to_chars(Buffer, Buffer + sizeof(Buffer), Value);
    (void)ErrorCode;
    return String{Buffer, static_cast<String::SizeType>(End - Buffer)};
}

[[nodiscard]] inline String ToString(bool Value)
{
    return String{Value ? "true" : "false"};
}

[[nodiscard]] inline String ToString(char Value)
{
    return String{&Value, 1};
}

[[nodiscard]] inline String ToString(const char* Value)
{
    return String{Value == nullptr ? "" : Value};
}

[[nodiscard]] inline const String& ToString(const String& Value)
{
    return Value;
}

[[nodiscard]] inline String ToString(StringView Value)
{
    return Value.ToString();
}

template <typename T>
concept HasMemberToString = requires(const T& Value) {
    { Value.ToString() } -> std::convertible_to<StringView>;
};

template <typename T>
concept HasFreeToString = requires(const T& Value) {
    { ToString(Value) } -> std::convertible_to<StringView>;
};

template <typename T>
concept Stringable = HasMemberToString<std::remove_cvref_t<T>> || HasFreeToString<std::remove_cvref_t<T>>;

template <typename T>
concept UsesToStringFormatter =
    Stringable<T> && !std::is_arithmetic_v<T> && !std::is_same_v<T, String> && !std::is_same_v<T, StringView> &&
    !std::is_convertible_v<T, const char*> && !std::is_same_v<T, std::string_view> && !std::is_same_v<T, std::string>;

template <typename T>
    requires Stringable<T>
[[nodiscard]] inline String ToStringValue(const T& Value)
{
    if constexpr (HasMemberToString<std::remove_cvref_t<T>>)
    {
        return String{Value.ToString()};
    }
    else
    {
        return String{ToString(Value)};
    }
}

template <typename T>
    requires UsesToStringFormatter<T>
struct fmt::formatter<T, char, void> : fmt::formatter<std::string_view>
{
    auto format(const T& Value, fmt::format_context& Context) const
    {
        const String Text = ToStringValue(Value);
        return formatter<std::string_view>::format(Text.View().ToStdStringView(), Context);
    }
};

template <typename... Args>
inline String String::Format(fmt::format_string<Args...> FormatStr, Args&&... Arguments)
{
    static_assert((Stringable<std::remove_cvref_t<Args>> && ...), "String::Format requires Stringable arguments");

    String Result;
    fmt::format_to(std::back_inserter(Result.mData), FormatStr, std::forward<Args>(Arguments)...);
    return Result;
}
