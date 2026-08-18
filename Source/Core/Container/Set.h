#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <unordered_set>
#include <utility>

#include "Core/Memory/MiAllocator.h"

template <typename T, typename THasher = std::hash<T>, typename TKeyEqual = std::equal_to<T>>
class Set
{
    using Storage = std::unordered_set<T, THasher, TKeyEqual, MiAllocator<T>>;

public:
    using ElementType = T;
    using ValueType = T;
    using SizeType = std::size_t;
    using Hasher = THasher;
    using KeyEqual = TKeyEqual;
    using Iterator = typename Storage::iterator;
    using ConstIterator = typename Storage::const_iterator;

    Set() = default;

    Set(std::initializer_list<T> List) : mData(List) {}

    [[nodiscard]] SizeType Num() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] SizeType Size() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] SizeType Count() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] bool Empty() const noexcept
    {
        return mData.empty();
    }

    void Clear() noexcept
    {
        mData.clear();
    }

    void Reset() noexcept
    {
        mData.clear();
    }

    void Reserve(SizeType NewCapacity)
    {
        mData.reserve(NewCapacity);
    }

    bool Add(const T& Item)
    {
        auto [It, Inserted] = mData.insert(Item);
        (void)It;
        return Inserted;
    }

    bool Add(T&& Item)
    {
        auto [It, Inserted] = mData.insert(std::move(Item));
        (void)It;
        return Inserted;
    }

    template <typename... Args>
    bool Emplace(Args&&... Arguments)
    {
        auto [It, Inserted] = mData.emplace(std::forward<Args>(Arguments)...);
        (void)It;
        return Inserted;
    }

    bool Remove(const T& Item)
    {
        return mData.erase(Item) > 0;
    }

    [[nodiscard]] bool Contains(const T& Item) const
    {
        return mData.contains(Item);
    }

    [[nodiscard]] const T* Find(const T& Item) const
    {
        auto It = mData.find(Item);
        if (It == mData.end())
        {
            return nullptr;
        }
        return &(*It);
    }

    [[nodiscard]] Iterator begin() noexcept
    {
        return mData.begin();
    }

    [[nodiscard]] ConstIterator begin() const noexcept
    {
        return mData.begin();
    }

    [[nodiscard]] Iterator end() noexcept
    {
        return mData.end();
    }

    [[nodiscard]] ConstIterator end() const noexcept
    {
        return mData.end();
    }

    [[nodiscard]] Iterator Begin() noexcept
    {
        return begin();
    }

    [[nodiscard]] ConstIterator Begin() const noexcept
    {
        return begin();
    }

    [[nodiscard]] Iterator End() noexcept
    {
        return end();
    }

    [[nodiscard]] ConstIterator End() const noexcept
    {
        return end();
    }

    [[nodiscard]] Iterator FindIterator(const T& Item)
    {
        return mData.find(Item);
    }

    [[nodiscard]] ConstIterator FindIterator(const T& Item) const
    {
        return mData.find(Item);
    }

    void Swap(Set& Other) noexcept
    {
        mData.swap(Other.mData);
    }

private:
    Storage mData;
};
