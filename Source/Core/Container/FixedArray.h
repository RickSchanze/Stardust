#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <utility>

#include "Core/Container/Span.h"
#include "Core/CoreConfig.h"
#include "Core/Debug/Debug.h"

template <typename T, std::size_t N>
class FixedArray
{
public:
    static constexpr std::size_t Npos = static_cast<std::size_t>(-1);
    static constexpr std::size_t CapacityValue = N;

    using ElementType = T;
    using ValueType = T;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using Iterator = typename std::array<T, N>::iterator;
    using ConstIterator = typename std::array<T, N>::const_iterator;
    using ReverseIterator = typename std::array<T, N>::reverse_iterator;
    using ConstReverseIterator = typename std::array<T, N>::const_reverse_iterator;

    constexpr FixedArray() = default;

    constexpr FixedArray(const T (&Values)[N])
    {
        for (SizeType Index = 0; Index < N; ++Index)
        {
            mData[Index] = Values[Index];
        }
    }

    constexpr FixedArray(std::initializer_list<T> List)
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(List.size() <= N);
        }
        SizeType Index = 0;
        for (const T& Value : List)
        {
            mData[Index++] = Value;
        }
        for (; Index < N; ++Index)
        {
            mData[Index] = T{};
        }
    }

    [[nodiscard]] constexpr Pointer Data() noexcept
    {
        return mData.data();
    }

    [[nodiscard]] constexpr ConstPointer Data() const noexcept
    {
        return mData.data();
    }

    [[nodiscard]] static constexpr SizeType Num() noexcept
    {
        return N;
    }

    [[nodiscard]] static constexpr SizeType Size() noexcept
    {
        return N;
    }

    [[nodiscard]] static constexpr SizeType Length() noexcept
    {
        return N;
    }

    [[nodiscard]] static constexpr SizeType Max() noexcept
    {
        return N;
    }

    [[nodiscard]] static constexpr SizeType Capacity() noexcept
    {
        return N;
    }

    [[nodiscard]] static constexpr bool Empty() noexcept
    {
        return N == 0;
    }

    [[nodiscard]] constexpr bool IsValidIndex(SizeType Index) const noexcept
    {
        return Index < N;
    }

    [[nodiscard]] constexpr Reference operator[](SizeType Index)
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] constexpr ConstReference operator[](SizeType Index) const
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] constexpr Reference At(SizeType Index)
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] constexpr ConstReference At(SizeType Index) const
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] constexpr Reference Front()
    {
        CheckNotEmpty();
        return mData.front();
    }

    [[nodiscard]] constexpr ConstReference Front() const
    {
        CheckNotEmpty();
        return mData.front();
    }

    [[nodiscard]] constexpr Reference First()
    {
        return Front();
    }

    [[nodiscard]] constexpr ConstReference First() const
    {
        return Front();
    }

    [[nodiscard]] constexpr Reference Back()
    {
        CheckNotEmpty();
        return mData.back();
    }

    [[nodiscard]] constexpr ConstReference Back() const
    {
        CheckNotEmpty();
        return mData.back();
    }

    [[nodiscard]] constexpr Reference Last()
    {
        return Back();
    }

    [[nodiscard]] constexpr ConstReference Last() const
    {
        return Back();
    }

    [[nodiscard]] constexpr Iterator begin() noexcept
    {
        return mData.begin();
    }

    [[nodiscard]] constexpr ConstIterator begin() const noexcept
    {
        return mData.begin();
    }

    [[nodiscard]] constexpr Iterator end() noexcept
    {
        return mData.end();
    }

    [[nodiscard]] constexpr ConstIterator end() const noexcept
    {
        return mData.end();
    }

    [[nodiscard]] constexpr Iterator Begin() noexcept
    {
        return begin();
    }

    [[nodiscard]] constexpr ConstIterator Begin() const noexcept
    {
        return begin();
    }

    [[nodiscard]] constexpr Iterator End() noexcept
    {
        return end();
    }

    [[nodiscard]] constexpr ConstIterator End() const noexcept
    {
        return end();
    }

    [[nodiscard]] constexpr ReverseIterator rbegin() noexcept
    {
        return mData.rbegin();
    }

    [[nodiscard]] constexpr ConstReverseIterator rbegin() const noexcept
    {
        return mData.rbegin();
    }

    [[nodiscard]] constexpr ReverseIterator rend() noexcept
    {
        return mData.rend();
    }

    [[nodiscard]] constexpr ConstReverseIterator rend() const noexcept
    {
        return mData.rend();
    }

    [[nodiscard]] constexpr ReverseIterator RBegin() noexcept
    {
        return rbegin();
    }

    [[nodiscard]] constexpr ConstReverseIterator RBegin() const noexcept
    {
        return rbegin();
    }

    [[nodiscard]] constexpr ReverseIterator REnd() noexcept
    {
        return rend();
    }

    [[nodiscard]] constexpr ConstReverseIterator REnd() const noexcept
    {
        return rend();
    }

    constexpr void Fill(const T& Value)
    {
        mData.fill(Value);
    }

    constexpr void Swap(FixedArray& Other)
    {
        mData.swap(Other.mData);
    }

    [[nodiscard]] constexpr Span<T> AsSpan() noexcept
    {
        return {Data(), N};
    }

    [[nodiscard]] constexpr Span<const T> AsSpan() const noexcept
    {
        return {Data(), N};
    }

    [[nodiscard]] constexpr operator Span<T>() noexcept
    {
        return AsSpan();
    }

    [[nodiscard]] constexpr operator Span<const T>() const noexcept
    {
        return AsSpan();
    }

    [[nodiscard]] constexpr SizeType IndexOf(const T& Item) const
        requires requires { Item == Item; }
    {
        for (SizeType Index = 0; Index < N; ++Index)
        {
            if (mData[Index] == Item)
            {
                return Index;
            }
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType LastIndexOf(const T& Item) const
        requires requires { Item == Item; }
    {
        SizeType Index = N;
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

    [[nodiscard]] constexpr bool Contains(const T& Item) const
        requires requires { Item == Item; }
    {
        return IndexOf(Item) != Npos;
    }

    [[nodiscard]] constexpr T* Find(const T& Item)
        requires requires { Item == Item; }
    {
        const SizeType Index = IndexOf(Item);
        return Index == Npos ? nullptr : Data() + Index;
    }

    [[nodiscard]] constexpr const T* Find(const T& Item) const
        requires requires { Item == Item; }
    {
        const SizeType Index = IndexOf(Item);
        return Index == Npos ? nullptr : Data() + Index;
    }

    constexpr void Sort()
        requires requires(T& Left, T& Right) { Left < Right; }
    {
        std::sort(mData.begin(), mData.end());
    }

    template <typename Predicate>
    constexpr void Sort(Predicate Pred)
    {
        std::sort(mData.begin(), mData.end(), Pred);
    }

    constexpr void Reverse()
    {
        std::reverse(mData.begin(), mData.end());
    }

    [[nodiscard]] constexpr bool operator==(const FixedArray& Other) const
        requires requires(const T& Left, const T& Right) { Left == Right; }
    {
        return mData == Other.mData;
    }

private:
    std::array<T, N> mData{};

    constexpr void CheckIndex(SizeType Index) const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(Index < N);
        }
    }

    constexpr void CheckNotEmpty() const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(N > 0);
        }
    }
};
