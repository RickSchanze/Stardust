#pragma once

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <map>
#include <utility>

#include "Core/CoreConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Memory/MiAllocator.h"

template <typename TKey, typename TValue, typename TCompare = std::less<TKey>>
class Map
{
    using Storage = std::map<TKey, TValue, TCompare, MiAllocator<std::pair<const TKey, TValue>>>;

public:
    using KeyType = TKey;
    using ValueType = TValue;
    using ElementType = typename Storage::value_type;
    using SizeType = std::size_t;
    using Compare = TCompare;
    using Iterator = typename Storage::iterator;
    using ConstIterator = typename Storage::const_iterator;
    using ReverseIterator = typename Storage::reverse_iterator;
    using ConstReverseIterator = typename Storage::const_reverse_iterator;

    Map() = default;

    Map(std::initializer_list<ElementType> List)
        : mData(List)
    {
    }

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

    TValue& Add(const TKey& Key, const TValue& Value)
    {
        auto [It, Inserted] = mData.insert_or_assign(Key, Value);
        (void)Inserted;
        return It->second;
    }

    TValue& Add(TKey&& Key, TValue&& Value)
    {
        auto [It, Inserted] = mData.insert_or_assign(std::move(Key), std::move(Value));
        (void)Inserted;
        return It->second;
    }

    bool TryAdd(const TKey& Key, const TValue& Value)
    {
        auto [It, Inserted] = mData.try_emplace(Key, Value);
        (void)It;
        return Inserted;
    }

    template <typename... Args>
    TValue& Emplace(const TKey& Key, Args&&... Arguments)
    {
        auto [It, Inserted] = mData.try_emplace(Key, std::forward<Args>(Arguments)...);
        (void)Inserted;
        return It->second;
    }

    TValue& FindOrAdd(const TKey& Key)
    {
        auto [It, Inserted] = mData.try_emplace(Key);
        (void)Inserted;
        return It->second;
    }

    TValue& FindOrAdd(const TKey& Key, const TValue& DefaultValue)
    {
        auto [It, Inserted] = mData.try_emplace(Key, DefaultValue);
        (void)Inserted;
        return It->second;
    }

    bool Remove(const TKey& Key)
    {
        return mData.erase(Key) > 0;
    }

    [[nodiscard]] bool Contains(const TKey& Key) const
    {
        return mData.contains(Key);
    }

    [[nodiscard]] bool ContainsKey(const TKey& Key) const
    {
        return Contains(Key);
    }

    [[nodiscard]] TValue* Find(const TKey& Key)
    {
        auto It = mData.find(Key);
        if (It == mData.end())
        {
            return nullptr;
        }
        return &It->second;
    }

    [[nodiscard]] const TValue* Find(const TKey& Key) const
    {
        auto It = mData.find(Key);
        if (It == mData.end())
        {
            return nullptr;
        }
        return &It->second;
    }

    bool TryGetValue(const TKey& Key, TValue& OutValue) const
    {
        const TValue* Found = Find(Key);
        if (Found == nullptr)
        {
            return false;
        }
        OutValue = *Found;
        return true;
    }

    [[nodiscard]] TValue& operator[](const TKey& Key)
    {
        return FindOrAdd(Key);
    }

    [[nodiscard]] TValue& operator[](TKey&& Key)
    {
        auto [It, Inserted] = mData.try_emplace(std::move(Key));
        (void)Inserted;
        return It->second;
    }

    [[nodiscard]] TValue& At(const TKey& Key)
    {
        TValue* Found = Find(Key);
        if constexpr (gEnableContainerCheck)
        {
            Assert(Found != nullptr);
        }
        return *Found;
    }

    [[nodiscard]] const TValue& At(const TKey& Key) const
    {
        const TValue* Found = Find(Key);
        if constexpr (gEnableContainerCheck)
        {
            Assert(Found != nullptr);
        }
        return *Found;
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

    [[nodiscard]] ReverseIterator rbegin() noexcept
    {
        return mData.rbegin();
    }

    [[nodiscard]] ConstReverseIterator rbegin() const noexcept
    {
        return mData.rbegin();
    }

    [[nodiscard]] ReverseIterator rend() noexcept
    {
        return mData.rend();
    }

    [[nodiscard]] ConstReverseIterator rend() const noexcept
    {
        return mData.rend();
    }

    [[nodiscard]] ReverseIterator RBegin() noexcept
    {
        return rbegin();
    }

    [[nodiscard]] ConstReverseIterator RBegin() const noexcept
    {
        return rbegin();
    }

    [[nodiscard]] ReverseIterator REnd() noexcept
    {
        return rend();
    }

    [[nodiscard]] ConstReverseIterator REnd() const noexcept
    {
        return rend();
    }

    [[nodiscard]] Iterator FindIterator(const TKey& Key)
    {
        return mData.find(Key);
    }

    [[nodiscard]] ConstIterator FindIterator(const TKey& Key) const
    {
        return mData.find(Key);
    }

    void Swap(Map& Other) noexcept
    {
        mData.swap(Other.mData);
    }

private:
    Storage mData;
};
