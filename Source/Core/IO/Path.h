#pragma once

#include "Core/String/String.h"
#include "Core/String/StringView.h"

class Path
{
public:
#ifdef _WIN32
    static constexpr char DirectorySeparatorChar = '\\';
    static constexpr char AltDirectorySeparatorChar = '/';
#else
    static constexpr char DirectorySeparatorChar = '/';
    static constexpr char AltDirectorySeparatorChar = '\\';
#endif

    [[nodiscard]] static String Combine(StringView Path1, StringView Path2);
    [[nodiscard]] static String Combine(StringView Path1, StringView Path2, StringView Path3);
    [[nodiscard]] static String GetFileName(StringView Path);
    [[nodiscard]] static String GetDirectoryName(StringView Path);
    [[nodiscard]] static String GetExtension(StringView Path);
    [[nodiscard]] static String ChangeExtension(StringView Path, StringView Extension);
    [[nodiscard]] static String GetFullPath(StringView Path);
};
