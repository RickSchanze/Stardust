#pragma once

#include <compare>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <string_view>

class String;

class StringView
{
public:
    static constexpr std::size_t Npos = static_cast<std::size_t>(-1);

    using ValueType = char;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using Pointer = char*;
    using ConstPointer = const char*;
    using Reference = char&;
    using ConstReference = const char&;
    using ConstIterator = const char*;
    using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    constexpr StringView() noexcept = default;

    constexpr StringView(std::nullptr_t) = delete;

    constexpr StringView(const char* Data) noexcept
        : mData(Data)
        , mSize(Data == nullptr ? 0 : CStringLength(Data))
    {
    }

    constexpr StringView(const char* Data, SizeType Size) noexcept
        : mData(Data)
        , mSize(Size)
    {
    }

    constexpr StringView(const char* Begin, const char* End) noexcept
        : mData(Begin)
        , mSize(static_cast<SizeType>(End - Begin))
    {
    }

    template <std::size_t N>
    constexpr StringView(const char (&Literal)[N]) noexcept
        : mData(Literal)
        , mSize(N > 0 ? N - 1 : 0)
    {
    }

    constexpr StringView(std::string_view View) noexcept
        : mData(View.data())
        , mSize(View.size())
    {
    }

    [[nodiscard]] constexpr ConstPointer Data() const noexcept
    {
        return mData;
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

    [[nodiscard]] constexpr ConstReference operator[](SizeType Index) const noexcept
    {
        return mData[Index];
    }

    [[nodiscard]] constexpr ConstReference At(SizeType Index) const
    {
        if (Index >= mSize)
        {
            throw std::out_of_range("StringView::At");
        }
        return mData[Index];
    }

    [[nodiscard]] constexpr ConstReference Front() const noexcept
    {
        return mData[0];
    }

    [[nodiscard]] constexpr ConstReference Back() const noexcept
    {
        return mData[mSize - 1];
    }

    [[nodiscard]] constexpr ConstIterator begin() const noexcept
    {
        return mData;
    }

    [[nodiscard]] constexpr ConstIterator end() const noexcept
    {
        return mData + mSize;
    }

    [[nodiscard]] constexpr ConstIterator Begin() const noexcept
    {
        return begin();
    }

    [[nodiscard]] constexpr ConstIterator End() const noexcept
    {
        return end();
    }

    [[nodiscard]] constexpr ConstReverseIterator RBegin() const noexcept
    {
        return ConstReverseIterator(end());
    }

    [[nodiscard]] constexpr ConstReverseIterator REnd() const noexcept
    {
        return ConstReverseIterator(begin());
    }

    constexpr void RemovePrefix(SizeType Count) noexcept
    {
        const SizeType Clamped = Count > mSize ? mSize : Count;
        mData += Clamped;
        mSize -= Clamped;
    }

    constexpr void RemoveSuffix(SizeType Count) noexcept
    {
        const SizeType Clamped = Count > mSize ? mSize : Count;
        mSize -= Clamped;
    }

    [[nodiscard]] constexpr StringView Substr(SizeType Offset, SizeType Count = Npos) const noexcept
    {
        if (Offset > mSize)
        {
            return {};
        }
        const SizeType MaxCount = mSize - Offset;
        return {mData + Offset, Count > MaxCount ? MaxCount : Count};
    }

    [[nodiscard]] constexpr StringView First(SizeType Count) const noexcept
    {
        return Substr(0, Count);
    }

    [[nodiscard]] constexpr StringView Last(SizeType Count) const noexcept
    {
        if (Count >= mSize)
        {
            return *this;
        }
        return Substr(mSize - Count, Count);
    }

    [[nodiscard]] constexpr int Compare(StringView Other) const noexcept
    {
        const SizeType MinSize = mSize < Other.mSize ? mSize : Other.mSize;
        for (SizeType Index = 0; Index < MinSize; ++Index)
        {
            const unsigned char Left = static_cast<unsigned char>(mData[Index]);
            const unsigned char Right = static_cast<unsigned char>(Other.mData[Index]);
            if (Left < Right)
            {
                return -1;
            }
            if (Left > Right)
            {
                return 1;
            }
        }
        if (mSize < Other.mSize)
        {
            return -1;
        }
        if (mSize > Other.mSize)
        {
            return 1;
        }
        return 0;
    }

    [[nodiscard]] constexpr bool Equals(StringView Other) const noexcept
    {
        return Compare(Other) == 0;
    }

    [[nodiscard]] constexpr bool StartsWith(StringView Prefix) const noexcept
    {
        return First(Prefix.mSize).Equals(Prefix);
    }

    [[nodiscard]] constexpr bool EndsWith(StringView Suffix) const noexcept
    {
        return Last(Suffix.mSize).Equals(Suffix);
    }

    [[nodiscard]] constexpr bool Contains(StringView Pattern) const noexcept
    {
        return Find(Pattern) != Npos;
    }

    [[nodiscard]] constexpr bool Contains(char Character) const noexcept
    {
        return Find(Character) != Npos;
    }

    [[nodiscard]] constexpr SizeType Find(char Character, SizeType Start = 0) const noexcept
    {
        if (Start >= mSize)
        {
            return Npos;
        }
        for (SizeType Index = Start; Index < mSize; ++Index)
        {
            if (mData[Index] == Character)
            {
                return Index;
            }
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType Find(StringView Pattern, SizeType Start = 0) const noexcept
    {
        if (Pattern.Empty())
        {
            return Start <= mSize ? Start : Npos;
        }
        if (Pattern.mSize > mSize || Start > mSize - Pattern.mSize)
        {
            return Npos;
        }
        for (SizeType Index = Start; Index + Pattern.mSize <= mSize; ++Index)
        {
            if (Substr(Index, Pattern.mSize).Equals(Pattern))
            {
                return Index;
            }
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType RFind(char Character, SizeType Start = Npos) const noexcept
    {
        if (Empty())
        {
            return Npos;
        }
        SizeType Index = Start < mSize ? Start : mSize - 1;
        for (;;)
        {
            if (mData[Index] == Character)
            {
                return Index;
            }
            if (Index == 0)
            {
                break;
            }
            --Index;
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType RFind(StringView Pattern, SizeType Start = Npos) const noexcept
    {
        if (Pattern.Empty())
        {
            return Start < mSize ? Start : mSize;
        }
        if (Pattern.mSize > mSize)
        {
            return Npos;
        }
        SizeType Index = Start < mSize - Pattern.mSize + 1 ? Start : mSize - Pattern.mSize;
        for (;;)
        {
            if (Substr(Index, Pattern.mSize).Equals(Pattern))
            {
                return Index;
            }
            if (Index == 0)
            {
                break;
            }
            --Index;
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType FindFirstOf(StringView Characters, SizeType Start = 0) const noexcept
    {
        for (SizeType Index = Start; Index < mSize; ++Index)
        {
            if (Characters.Contains(mData[Index]))
            {
                return Index;
            }
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType FindFirstNotOf(StringView Characters, SizeType Start = 0) const noexcept
    {
        for (SizeType Index = Start; Index < mSize; ++Index)
        {
            if (!Characters.Contains(mData[Index]))
            {
                return Index;
            }
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType FindLastOf(StringView Characters, SizeType Start = Npos) const noexcept
    {
        if (Empty())
        {
            return Npos;
        }
        SizeType Index = Start < mSize ? Start : mSize - 1;
        for (;;)
        {
            if (Characters.Contains(mData[Index]))
            {
                return Index;
            }
            if (Index == 0)
            {
                break;
            }
            --Index;
        }
        return Npos;
    }

    [[nodiscard]] constexpr SizeType FindLastNotOf(StringView Characters, SizeType Start = Npos) const noexcept
    {
        if (Empty())
        {
            return Npos;
        }
        SizeType Index = Start < mSize ? Start : mSize - 1;
        for (;;)
        {
            if (!Characters.Contains(mData[Index]))
            {
                return Index;
            }
            if (Index == 0)
            {
                break;
            }
            --Index;
        }
        return Npos;
    }

    [[nodiscard]] constexpr StringView TrimStart() const noexcept
    {
        SizeType Index = 0;
        while (Index < mSize && IsSpace(mData[Index]))
        {
            ++Index;
        }
        return Substr(Index);
    }

    [[nodiscard]] constexpr StringView TrimEnd() const noexcept
    {
        SizeType End = mSize;
        while (End > 0 && IsSpace(mData[End - 1]))
        {
            --End;
        }
        return Substr(0, End);
    }

    [[nodiscard]] constexpr StringView Trim() const noexcept
    {
        return TrimStart().TrimEnd();
    }

    [[nodiscard]] String ToString() const;

    [[nodiscard]] constexpr std::string_view ToStdStringView() const noexcept
    {
        return {mData, mSize};
    }

    [[nodiscard]] constexpr bool operator==(StringView Other) const noexcept
    {
        return Equals(Other);
    }

    [[nodiscard]] constexpr auto operator<=>(StringView Other) const noexcept
    {
        const int Result = Compare(Other);
        if (Result < 0)
        {
            return std::strong_ordering::less;
        }
        if (Result > 0)
        {
            return std::strong_ordering::greater;
        }
        return std::strong_ordering::equal;
    }

private:
    ConstPointer mData = nullptr;
    SizeType mSize = 0;

    [[nodiscard]] static constexpr SizeType CStringLength(const char* Data) noexcept
    {
        SizeType Length = 0;
        while (Data[Length] != '\0')
        {
            ++Length;
        }
        return Length;
    }

    [[nodiscard]] static constexpr bool IsSpace(char Character) noexcept
    {
        return Character == ' ' || Character == '\t' || Character == '\n' || Character == '\v' ||
               Character == '\f' || Character == '\r';
    }
};
