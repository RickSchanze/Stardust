#pragma once

#include <cstdint>

#include "Core/Number.h"

class DelegateHandle
{
public:
    UInt64 Id{0};

    [[nodiscard]] static DelegateHandle Invalid() noexcept
    {
        return DelegateHandle{};
    }

    [[nodiscard]] bool IsValid() const noexcept
    {
        return Id.Value != 0;
    }

    void Reset() noexcept
    {
        Id = UInt64{0};
    }

    [[nodiscard]] auto operator<=>(const DelegateHandle&) const noexcept = default;
};
