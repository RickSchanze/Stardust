#pragma once

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

#include "Core/Container/Span.h"
#include "Core/CoreConfig.h"
#include "Core/Debug/Debug.h"
#include "Core/Memory/MiAllocator.h"

template <typename T>
class Array
{
    static_assert(!std::is_same_v<T, bool>, "Array<bool> is not supported; use Array<uint8_t>");

    using Storage = std::vector<T, MiAllocator<T>>;

public:
    static constexpr std::size_t Npos = static_cast<std::size_t>(-1);

    using ElementType = T;
    using ValueType = T;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using Iterator = typename Storage::iterator;
    using ConstIterator = typename Storage::const_iterator;
    using ReverseIterator = typename Storage::reverse_iterator;
    using ConstReverseIterator = typename Storage::const_reverse_iterator;

    Array() = default;

    explicit Array(SizeType Count) : mData(Count) {}

    Array(SizeType Count, const T& Value) : mData(Count, Value) {}

    Array(std::initializer_list<T> List) : mData(List) {}

    Array(Span<const T> View)
    {
        Assign(View);
    }

    Array(const T* Data, SizeType Count)
    {
        Assign(Data, Count);
    }

    [[nodiscard]] Pointer Data() noexcept
    {
        return mData.data();
    }

    [[nodiscard]] ConstPointer Data() const noexcept
    {
        return mData.data();
    }

    [[nodiscard]] SizeType Num() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] SizeType Size() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] SizeType Length() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] SizeType Max() const noexcept
    {
        return mData.capacity();
    }

    [[nodiscard]] SizeType Capacity() const noexcept
    {
        return mData.capacity();
    }

    [[nodiscard]] bool Empty() const noexcept
    {
        return mData.empty();
    }

    [[nodiscard]] bool IsValidIndex(SizeType Index) const noexcept
    {
        return Index < mData.size();
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

    void Resize(SizeType NewSize)
    {
        mData.resize(NewSize);
    }

    void Resize(SizeType NewSize, const T& Value)
    {
        mData.resize(NewSize, Value);
    }

    void SetNum(SizeType NewSize)
    {
        mData.resize(NewSize);
    }

    void SetNum(SizeType NewSize, const T& Value)
    {
        mData.resize(NewSize, Value);
    }

    void Shrink()
    {
        mData.shrink_to_fit();
    }

    void ShrinkToFit()
    {
        mData.shrink_to_fit();
    }

    void Init(const T& Value, SizeType Count)
    {
        mData.assign(Count, Value);
    }

    void Assign(Span<const T> View)
    {
        mData.assign(View.Begin(), View.End());
    }

    void Assign(const T* Data, SizeType Count)
    {
        if (Count == 0 || Data == nullptr)
        {
            mData.clear();
            return;
        }
        mData.assign(Data, Data + Count);
    }

    SizeType Add(const T& Item)
    {
        mData.push_back(Item);
        return mData.size() - 1;
    }

    SizeType Add(T&& Item)
    {
        mData.push_back(std::move(Item));
        return mData.size() - 1;
    }

    SizeType AddUnique(const T& Item)
        requires requires { Item == Item; }
    {
        const SizeType Existing = IndexOf(Item);
        if (Existing != Npos)
        {
            return Existing;
        }
        return Add(Item);
    }

    SizeType AddDefaulted()
    {
        mData.emplace_back();
        return mData.size() - 1;
    }

    template <typename... Args>
    Reference Emplace(Args&&... Arguments)
    {
        return mData.emplace_back(std::forward<Args>(Arguments)...);
    }

    SizeType Push(const T& Item)
    {
        return Add(Item);
    }

    SizeType Push(T&& Item)
    {
        return Add(std::move(Item));
    }

    T Pop()
    {
        CheckNotEmpty();
        T Value = std::move(mData.back());
        mData.pop_back();
        return Value;
    }

    void Append(Span<const T> View)
    {
        mData.insert(mData.end(), View.Begin(), View.End());
    }

    void Append(const Array& Other)
    {
        mData.insert(mData.end(), Other.mData.begin(), Other.mData.end());
    }

    void Append(std::initializer_list<T> List)
    {
        mData.insert(mData.end(), List.begin(), List.end());
    }

    void Insert(SizeType Index, const T& Item)
    {
        CheckInsertIndex(Index);
        mData.insert(mData.begin() + static_cast<DifferenceType>(Index), Item);
    }

    void Insert(SizeType Index, T&& Item)
    {
        CheckInsertIndex(Index);
        mData.insert(mData.begin() + static_cast<DifferenceType>(Index), std::move(Item));
    }

    template <typename... Args>
    Reference EmplaceAt(SizeType Index, Args&&... Arguments)
    {
        CheckInsertIndex(Index);
        auto It = mData.emplace(mData.begin() + static_cast<DifferenceType>(Index), std::forward<Args>(Arguments)...);
        return *It;
    }

    void RemoveAt(SizeType Index)
    {
        CheckIndex(Index);
        mData.erase(mData.begin() + static_cast<DifferenceType>(Index));
    }

    void RemoveAt(SizeType Index, SizeType Count)
    {
        CheckIndex(Index);
        if constexpr (gEnableContainerCheck)
        {
            Assert(Count <= mData.size() - Index);
        }
        mData.erase(mData.begin() + static_cast<DifferenceType>(Index),
                    mData.begin() + static_cast<DifferenceType>(Index + Count));
    }

    void RemoveAtSwap(SizeType Index)
    {
        CheckIndex(Index);
        if (Index + 1 != mData.size())
        {
            mData[Index] = std::move(mData.back());
        }
        mData.pop_back();
    }

    bool Remove(const T& Item)
        requires requires { Item == Item; }
    {
        const SizeType Index = IndexOf(Item);
        if (Index == Npos)
        {
            return false;
        }
        RemoveAt(Index);
        return true;
    }

    bool RemoveSwap(const T& Item)
        requires requires { Item == Item; }
    {
        const SizeType Index = IndexOf(Item);
        if (Index == Npos)
        {
            return false;
        }
        RemoveAtSwap(Index);
        return true;
    }

    SizeType RemoveAll(const T& Item)
        requires requires { Item == Item; }
    {
        const SizeType OldSize = mData.size();
        mData.erase(std::remove(mData.begin(), mData.end(), Item), mData.end());
        return OldSize - mData.size();
    }

    template <typename Predicate>
    SizeType RemoveAll(Predicate Pred)
    {
        const SizeType OldSize = mData.size();
        mData.erase(std::remove_if(mData.begin(), mData.end(), Pred), mData.end());
        return OldSize - mData.size();
    }

    [[nodiscard]] Reference operator[](SizeType Index)
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] ConstReference operator[](SizeType Index) const
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] Reference At(SizeType Index)
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] ConstReference At(SizeType Index) const
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] Reference Front()
    {
        CheckNotEmpty();
        return mData.front();
    }

    [[nodiscard]] ConstReference Front() const
    {
        CheckNotEmpty();
        return mData.front();
    }

    [[nodiscard]] Reference First()
    {
        return Front();
    }

    [[nodiscard]] ConstReference First() const
    {
        return Front();
    }

    [[nodiscard]] Reference Back()
    {
        CheckNotEmpty();
        return mData.back();
    }

    [[nodiscard]] ConstReference Back() const
    {
        CheckNotEmpty();
        return mData.back();
    }

    [[nodiscard]] Reference Last()
    {
        return Back();
    }

    [[nodiscard]] ConstReference Last() const
    {
        return Back();
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

    [[nodiscard]] Span<T> AsSpan() noexcept
    {
        return {Data(), Num()};
    }

    [[nodiscard]] Span<const T> AsSpan() const noexcept
    {
        return {Data(), Num()};
    }

    [[nodiscard]] operator Span<T>() noexcept
    {
        return AsSpan();
    }

    [[nodiscard]] operator Span<const T>() const noexcept
    {
        return AsSpan();
    }

    [[nodiscard]] SizeType IndexOf(const T& Item) const
        requires requires { Item == Item; }
    {
        for (SizeType Index = 0; Index < mData.size(); ++Index)
        {
            if (mData[Index] == Item)
            {
                return Index;
            }
        }
        return Npos;
    }

    [[nodiscard]] SizeType LastIndexOf(const T& Item) const
        requires requires { Item == Item; }
    {
        SizeType Index = mData.size();
        while (Index > 0)
        {
            --Index;
            if (mData[Index] == Item)
            {
                return Index;
            }
        }
        return Npos;
    }

    template <typename Predicate>
    [[nodiscard]] SizeType IndexOfByPredicate(Predicate Pred) const
    {
        for (SizeType Index = 0; Index < mData.size(); ++Index)
        {
            if (Pred(mData[Index]))
            {
                return Index;
            }
        }
        return Npos;
    }

    [[nodiscard]] bool Contains(const T& Item) const
        requires requires { Item == Item; }
    {
        return IndexOf(Item) != Npos;
    }

    [[nodiscard]] T* Find(const T& Item)
        requires requires { Item == Item; }
    {
        const SizeType Index = IndexOf(Item);
        return Index == Npos ? nullptr : Data() + Index;
    }

    [[nodiscard]] const T* Find(const T& Item) const
        requires requires { Item == Item; }
    {
        const SizeType Index = IndexOf(Item);
        return Index == Npos ? nullptr : Data() + Index;
    }

    template <typename Predicate>
    [[nodiscard]] T* FindByPredicate(Predicate Pred)
    {
        const SizeType Index = IndexOfByPredicate(Pred);
        return Index == Npos ? nullptr : Data() + Index;
    }

    template <typename Predicate>
    [[nodiscard]] const T* FindByPredicate(Predicate Pred) const
    {
        const SizeType Index = IndexOfByPredicate(Pred);
        return Index == Npos ? nullptr : Data() + Index;
    }

    void Sort()
        requires requires(T& Left, T& Right) { Left < Right; }
    {
        std::sort(mData.begin(), mData.end());
    }

    template <typename Predicate>
    void Sort(Predicate Pred)
    {
        std::sort(mData.begin(), mData.end(), Pred);
    }

    void Reverse()
    {
        std::reverse(mData.begin(), mData.end());
    }

    void Swap(Array& Other) noexcept
    {
        mData.swap(Other.mData);
    }

    [[nodiscard]] bool operator==(const Array& Other) const
        requires requires(const T& Left, const T& Right) { Left == Right; }
    {
        return mData == Other.mData;
    }

private:
    Storage mData;

    void CheckIndex(SizeType Index) const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(Index < mData.size());
        }
    }

    void CheckInsertIndex(SizeType Index) const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(Index <= mData.size());
        }
    }

    void CheckNotEmpty() const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(!mData.empty());
        }
    }
};
