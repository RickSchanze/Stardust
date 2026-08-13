#pragma once

#include "Core/Memory/MiAllocator.h"
#include "Core/String/StringView.h"

#include <spdlog/fmt/fmt.h>

#include <functional>
#include <string>
#include <utility>

class String
{
    using Storage = std::basic_string<char, std::char_traits<char>, MiAllocator<char>>;

public:
    static constexpr std::size_t Npos = StringView::Npos;

    using ValueType = char;
    using value_type = char;
    using SizeType = std::size_t;
    using DifferenceType = std::ptrdiff_t;
    using Pointer = char*;
    using ConstPointer = const char*;
    using Reference = char&;
    using ConstReference = const char&;
    using Iterator = Storage::iterator;
    using ConstIterator = Storage::const_iterator;
    using ReverseIterator = Storage::reverse_iterator;
    using ConstReverseIterator = Storage::const_reverse_iterator;

    String() = default;

    String(const String&) = default;
    String(String&&) noexcept = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) noexcept = default;
    ~String() = default;

    // 禁止从字面量 / C 字符串隐式构造；允许显式构造
    explicit String(const char* Data)
        : mData(Data == nullptr ? "" : Data)
    {
    }

    explicit String(const char* Data, SizeType Size)
        : mData(Data == nullptr ? ConstPointer("") : Data, Size)
    {
    }

    template <std::size_t N>
    explicit String(const char (&Literal)[N])
        : mData(Literal, N > 0 ? N - 1 : 0)
    {
    }

    // 从视图拷贝（可隐式：拥有内存的自然转换）
    String(StringView View)
        : mData(View.Data(), View.Size())
    {
    }

    String& operator=(StringView View)
    {
        return Assign(View);
    }

    String& operator=(const char*) = delete;

    [[nodiscard]] Pointer Data() noexcept
    {
        return mData.data();
    }

    [[nodiscard]] ConstPointer Data() const noexcept
    {
        return mData.data();
    }

    [[nodiscard]] ConstPointer CStr() const noexcept
    {
        return mData.c_str();
    }

    [[nodiscard]] SizeType Size() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] SizeType Length() const noexcept
    {
        return mData.size();
    }

    [[nodiscard]] SizeType Capacity() const noexcept
    {
        return mData.capacity();
    }

    [[nodiscard]] bool Empty() const noexcept
    {
        return mData.empty();
    }

    void Clear() noexcept
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

    void Resize(SizeType NewSize, char Fill)
    {
        mData.resize(NewSize, Fill);
    }

    void ShrinkToFit()
    {
        mData.shrink_to_fit();
    }

    [[nodiscard]] Reference operator[](SizeType Index) noexcept
    {
        return mData[Index];
    }

    [[nodiscard]] ConstReference operator[](SizeType Index) const noexcept
    {
        return mData[Index];
    }

    [[nodiscard]] Reference At(SizeType Index)
    {
        return mData.at(Index);
    }

    [[nodiscard]] ConstReference At(SizeType Index) const
    {
        return mData.at(Index);
    }

    [[nodiscard]] Reference Front()
    {
        return mData.front();
    }

    [[nodiscard]] ConstReference Front() const
    {
        return mData.front();
    }

    [[nodiscard]] Reference Back()
    {
        return mData.back();
    }

    [[nodiscard]] ConstReference Back() const
    {
        return mData.back();
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

    [[nodiscard]] StringView View() const noexcept
    {
        return {mData.data(), mData.size()};
    }

    [[nodiscard]] operator StringView() const noexcept
    {
        return View();
    }

    [[nodiscard]] const String& ToString() const noexcept
    {
        return *this;
    }

    template <typename... Args>
    [[nodiscard]] static String Format(fmt::format_string<Args...> FormatStr, Args&&... Arguments);

    [[nodiscard]] StringView Substr(SizeType Offset, SizeType Count = Npos) const noexcept
    {
        return View().Substr(Offset, Count);
    }

    [[nodiscard]] StringView First(SizeType Count) const noexcept
    {
        return View().First(Count);
    }

    [[nodiscard]] StringView Last(SizeType Count) const noexcept
    {
        return View().Last(Count);
    }

    [[nodiscard]] int Compare(StringView Other) const noexcept
    {
        return View().Compare(Other);
    }

    [[nodiscard]] bool Equals(StringView Other) const noexcept
    {
        return View().Equals(Other);
    }

    [[nodiscard]] bool StartsWith(StringView Prefix) const noexcept
    {
        return View().StartsWith(Prefix);
    }

    [[nodiscard]] bool EndsWith(StringView Suffix) const noexcept
    {
        return View().EndsWith(Suffix);
    }

    [[nodiscard]] bool Contains(StringView Pattern) const noexcept
    {
        return View().Contains(Pattern);
    }

    [[nodiscard]] bool Contains(char Character) const noexcept
    {
        return View().Contains(Character);
    }

    [[nodiscard]] SizeType Find(char Character, SizeType Start = 0) const noexcept
    {
        return View().Find(Character, Start);
    }

    [[nodiscard]] SizeType Find(StringView Pattern, SizeType Start = 0) const noexcept
    {
        return View().Find(Pattern, Start);
    }

    [[nodiscard]] SizeType RFind(char Character, SizeType Start = Npos) const noexcept
    {
        return View().RFind(Character, Start);
    }

    [[nodiscard]] SizeType RFind(StringView Pattern, SizeType Start = Npos) const noexcept
    {
        return View().RFind(Pattern, Start);
    }

    [[nodiscard]] SizeType FindFirstOf(StringView Characters, SizeType Start = 0) const noexcept
    {
        return View().FindFirstOf(Characters, Start);
    }

    [[nodiscard]] SizeType FindFirstNotOf(StringView Characters, SizeType Start = 0) const noexcept
    {
        return View().FindFirstNotOf(Characters, Start);
    }

    [[nodiscard]] SizeType FindLastOf(StringView Characters, SizeType Start = Npos) const noexcept
    {
        return View().FindLastOf(Characters, Start);
    }

    [[nodiscard]] SizeType FindLastNotOf(StringView Characters, SizeType Start = Npos) const noexcept
    {
        return View().FindLastNotOf(Characters, Start);
    }

    [[nodiscard]] StringView TrimStart() const noexcept
    {
        return View().TrimStart();
    }

    [[nodiscard]] StringView TrimEnd() const noexcept
    {
        return View().TrimEnd();
    }

    [[nodiscard]] StringView Trim() const noexcept
    {
        return View().Trim();
    }

    String& Assign(StringView View)
    {
        mData.assign(View.Data(), View.Size());
        return *this;
    }

    String& Assign(const char* Data, SizeType Size)
    {
        mData.assign(Data == nullptr ? "" : Data, Size);
        return *this;
    }

    String& Append(StringView View)
    {
        mData.append(View.Data(), View.Size());
        return *this;
    }

    String& Append(char Character)
    {
        mData.push_back(Character);
        return *this;
    }

    String& Append(const char* Data, SizeType Size)
    {
        mData.append(Data == nullptr ? "" : Data, Size);
        return *this;
    }

    String& operator+=(StringView View)
    {
        return Append(View);
    }

    String& operator+=(char Character)
    {
        return Append(Character);
    }

    void PushBack(char Character)
    {
        mData.push_back(Character);
    }

    void push_back(char Character)
    {
        PushBack(Character);
    }

    void PopBack()
    {
        mData.pop_back();
    }

    String& Insert(SizeType Index, StringView View)
    {
        mData.insert(Index, View.Data(), View.Size());
        return *this;
    }

    String& Erase(SizeType Index, SizeType Count = Npos)
    {
        mData.erase(Index, Count);
        return *this;
    }

    String& Replace(SizeType Index, SizeType Count, StringView View)
    {
        mData.replace(Index, Count, View.Data(), View.Size());
        return *this;
    }

    void Swap(String& Other) noexcept
    {
        mData.swap(Other.mData);
    }

    [[nodiscard]] bool operator==(StringView Other) const noexcept
    {
        return Equals(Other);
    }

    [[nodiscard]] auto operator<=>(StringView Other) const noexcept
    {
        return View() <=> Other;
    }

private:
    Storage mData;
};

[[nodiscard]] inline String operator+(StringView Left, StringView Right)
{
    String Result;
    Result.Reserve(Left.Size() + Right.Size());
    Result.Append(Left);
    Result.Append(Right);
    return Result;
}

[[nodiscard]] inline String StringView::ToString() const
{
    return String{*this};
}

template <>
struct fmt::formatter<StringView> : fmt::formatter<std::string_view>
{
    auto format(StringView Value, format_context& Context) const
    {
        return formatter<std::string_view>::format(Value.ToStdStringView(), Context);
    }
};

template <>
struct fmt::formatter<String> : fmt::formatter<StringView>
{
    auto format(const String& Value, format_context& Context) const
    {
        return formatter<StringView>::format(Value.View(), Context);
    }
};

template <>
struct std::hash<StringView>
{
    [[nodiscard]] std::size_t operator()(StringView Value) const noexcept
    {
        return std::hash<std::string_view>{}(Value.ToStdStringView());
    }
};

template <>
struct std::hash<String>
{
    [[nodiscard]] std::size_t operator()(const String& Value) const noexcept
    {
        return std::hash<StringView>{}(Value.View());
    }
};

#include "Core/String/Stringable.h"
