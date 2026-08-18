#pragma once

#include <cstddef>
#include <new>

#include "Core/Memory/Memory.h"

template <typename T>
class MiAllocator
{
public:
    using ValueType = T;
    using value_type = T;

    constexpr MiAllocator() noexcept = default;

    template <typename U>
    constexpr MiAllocator(const MiAllocator<U>&) noexcept
    {
    }

    [[nodiscard]] T* allocate(std::size_t Count)
    {
        if (Count == 0)
        {
            return nullptr;
        }

        void* Memory = Malloc(Count * sizeof(T));
        if (Memory == nullptr)
        {
            throw std::bad_alloc();
        }
        return static_cast<T*>(Memory);
    }

    void deallocate(T* Pointer, std::size_t) noexcept
    {
        Free(Pointer);
    }

    [[nodiscard]] T* reallocate(T* Pointer, std::size_t OldCount, std::size_t NewCount)
    {
        (void)OldCount;
        if (NewCount == 0)
        {
            Free(Pointer);
            return nullptr;
        }

        void* Memory = Realloc(Pointer, NewCount * sizeof(T));
        if (Memory == nullptr)
        {
            throw std::bad_alloc();
        }
        return static_cast<T*>(Memory);
    }
};

template <typename T, typename U>
[[nodiscard]] constexpr bool operator==(const MiAllocator<T>&, const MiAllocator<U>&) noexcept
{
    return true;
}

template <typename T, typename U>
[[nodiscard]] constexpr bool operator!=(const MiAllocator<T>&, const MiAllocator<U>&) noexcept
{
    return false;
}
