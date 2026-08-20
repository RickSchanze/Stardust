#pragma once

#include "Core/Container/Array.h"
#include "Core/String/String.h"
#include "Core/String/StringView.h"

class Directory
{
public:
    [[nodiscard]] static bool Exists(StringView Path);

    static void Create(StringView Path);
    static void Delete(StringView Path, bool Recursive = false);

    [[nodiscard]] static Array<String> GetFiles(StringView Path);
    [[nodiscard]] static Array<String> GetDirectories(StringView Path);

    [[nodiscard]] static String GetWorkingDirectory();
    static void SetWorkingDirectory(StringView Path);
};
