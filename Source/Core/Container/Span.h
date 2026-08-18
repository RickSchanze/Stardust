#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>

#include "Core/CoreConfig.h"
#include "Core/Debug/Debug.h"

template <typename T>
class Span
{
public:
    static constexpr std::size_t Npos = static_cast<std::size_t>(-1);

    using ElementType = T;
    using ValueType = std::remove_cv_t<T>;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using Pointer = T*;
    using ConstPointer = const T*;
    using Reference = T&;
    using ConstReference = const T&;
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = std::reverse_iterator<Iterator>;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    constexpr Span() noexcept = default;

    constexpr Span(std::nullptr_t) noexcept : mData(nullptr), mSize(0) {}

    constexpr Span(T* Data, SizeType Size) noexcept : mData(Data), mSize(Size) {}

    constexpr Span(T* Begin, T* End) noexcept : mData(Begin), mSize(static_cast<SizeType>(End - Begin)) {}

    template <std::size_t N>
    constexpr Span(T (&Array)[N]) noexcept : mData(Array), mSize(N)
    {
    }

    template <typename U>
        requires std::is_convertible_v<U*, T*>
    constexpr Span(const Span<U>& Other) noexcept : mData(Other.Data()), mSize(Other.Num())
    {
    }

    [[nodiscard]] constexpr Pointer Data() const noexcept
    {
        return mData;
    }

    [[nodiscard]] constexpr SizeType Num() const noexcept
    {
        return mSize;
    }

    [[nodiscard]] constexpr SizeType Size() const noexcept
    {
        return mSize;
    }

    [[nodiscard]] constexpr SizeType Length() const noexcept
    {
        return mSize;
    }

    [[nodiscard]] constexpr bool Empty() const noexcept
    {
        return mSize == 0;
    }

    [[nodiscard]] constexpr bool IsValidIndex(SizeType Index) const noexcept
    {
        return Index < mSize;
    }

    [[nodiscard]] constexpr Reference operator[](SizeType Index) const
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] constexpr Reference At(SizeType Index) const
    {
        CheckIndex(Index);
        return mData[Index];
    }

    [[nodiscard]] constexpr Reference Front() const
    {
        CheckNotEmpty();
        return mData[0];
    }

    [[nodiscard]] constexpr Reference First() const
    {
        return Front();
    }

    [[nodiscard]] constexpr Reference Back() const
    {
        CheckNotEmpty();
        return mData[mSize - 1];
    }

    [[nodiscard]] constexpr Reference Last() const
    {
        return Back();
    }

    [[nodiscard]] constexpr Iterator begin() const noexcept
    {
        return mData;
    }

    [[nodiscard]] constexpr Iterator end() const noexcept
    {
        return mData + mSize;
    }

    [[nodiscard]] constexpr Iterator Begin() const noexcept
    {
        return begin();
    }

    [[nodiscard]] constexpr Iterator End() const noexcept
    {
        return end();
    }

    [[nodiscard]] constexpr ReverseIterator rbegin() const noexcept
    {
        return ReverseIterator(end());
    }

    [[nodiscard]] constexpr ReverseIterator rend() const noexcept
    {
        return ReverseIterator(begin());
    }

    [[nodiscard]] constexpr ReverseIterator RBegin() const noexcept
    {
        return rbegin();
    }

    [[nodiscard]] constexpr ReverseIterator REnd() const noexcept
    {
        return rend();
    }

    [[nodiscard]] constexpr Span First(SizeType Count) const
    {
        CheckCount(Count);
        return {mData, Count};
    }

    [[nodiscard]] constexpr Span Last(SizeType Count) const
    {
        CheckCount(Count);
        return {mData + (mSize - Count), Count};
    }

    [[nodiscard]] constexpr Span Subspan(SizeType Offset, SizeType Count = Npos) const
    {
        CheckOffset(Offset);
        const SizeType Remaining = mSize - Offset;
        const SizeType ActualCount = Count == Npos || Count > Remaining ? Remaining : Count;
        return {mData + Offset, ActualCount};
    }

    [[nodiscard]] constexpr Span Slice(SizeType Offset, SizeType Count = Npos) const
    {
        return Subspan(Offset, Count);
    }

    constexpr void Fill(const T& Value) const
        requires(!std::is_const_v<T>)
    {
        for (SizeType Index = 0; Index < mSize; ++Index)
        {
            mData[Index] = Value;
        }
    }

    constexpr void Clear() const
        requires(!std::is_const_v<T>)
    {
        for (SizeType Index = 0; Index < mSize; ++Index)
        {
            mData[Index] = T{};
        }
    }

    constexpr void CopyTo(Span<ValueType> Destination) const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(Destination.Num() >= mSize);
        }
        for (SizeType Index = 0; Index < mSize; ++Index)
        {
            Destination[Index] = mData[Index];
        }
    }

    [[nodiscard]] constexpr SizeType IndexOf(const T& Item) const
        requires requires { Item == Item; }
    {
        for (SizeType Index = 0; Index < mSize; ++Index)
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
        SizeType Index = mSize;
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

    [[nodiscard]] constexpr T* Find(const T& Item) const
        requires requires { Item == Item; }
    {
        const SizeType Index = IndexOf(Item);
        return Index == Npos ? nullptr : mData + Index;
    }

private:
    T* mData = nullptr;
    SizeType mSize = 0;

    constexpr void CheckIndex(SizeType Index) const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(Index < mSize);
        }
    }

    constexpr void CheckNotEmpty() const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(mSize > 0);
        }
    }

    constexpr void CheckCount(SizeType Count) const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(Count <= mSize);
        }
    }

    constexpr void CheckOffset(SizeType Offset) const
    {
        if constexpr (gEnableContainerCheck)
        {
            Assert(Offset <= mSize);
        }
    }
};
