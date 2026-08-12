#pragma once

#include <mimalloc.h>

#include <cstddef>
#include <new>
#include <utility>

[[nodiscard]] inline void* Malloc(std::size_t Size) noexcept
{
    return mi_malloc(Size);
}

[[nodiscard]] inline void* Calloc(std::size_t Count, std::size_t Size) noexcept
{
    return mi_calloc(Count, Size);
}

[[nodiscard]] inline void* Realloc(void* Pointer, std::size_t NewSize) noexcept
{
    return mi_realloc(Pointer, NewSize);
}

inline void Free(void* Pointer) noexcept
{
    mi_free(Pointer);
}

template <typename T, typename... Args>
[[nodiscard]] T* New(Args&&... Arguments)
{
    void* Memory = Malloc(sizeof(T));
    if (Memory == nullptr)
    {
        throw std::bad_alloc();
    }

    try
    {
        return ::new (Memory) T(std::forward<Args>(Arguments)...);
    }
    catch (...)
    {
        Free(Memory);
        throw;
    }
}

template <typename T>
void Delete(T* Pointer) noexcept
{
    if (Pointer == nullptr)
    {
        return;
    }
    Pointer->~T();
    Free(Pointer);
}
