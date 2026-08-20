#pragma once

#include <cstdint>

#include "Core/Container/Array.h"
#include "Core/Container/Span.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

class File
{
public:
    [[nodiscard]] static bool Exists(StringView Path);

    [[nodiscard]] static Array<std::uint8_t> ReadAllBytes(StringView Path);
    [[nodiscard]] static String ReadAllText(StringView Path);

    static void WriteAllBytes(StringView Path, Span<const std::uint8_t> Bytes);
    static void WriteAllText(StringView Path, StringView Contents);

    static void AppendAllBytes(StringView Path, Span<const std::uint8_t> Bytes);
    static void AppendAllText(StringView Path, StringView Contents);

    static void Delete(StringView Path);
    static void Copy(StringView SourcePath, StringView DestinationPath, bool Overwrite = false);
    static void Move(StringView SourcePath, StringView DestinationPath);
};
