#pragma once

#include <algorithm>
#include <cstddef>

template <std::size_t N>
struct FixedString
{
    char Data[N]{};

    consteval FixedString(const char (&Literal)[N])
    {
        std::copy_n(Literal, N, Data);
    }

    [[nodiscard]] consteval const char* CStr() const
    {
        return Data;
    }

    [[nodiscard]] static consteval std::size_t Size()
    {
        return N > 0 ? N - 1 : 0;
    }
};
