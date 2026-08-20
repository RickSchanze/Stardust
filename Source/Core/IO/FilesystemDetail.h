#pragma once

#include <filesystem>

#include "Core/String/String.h"
#include "Core/String/StringView.h"

namespace FilesystemDetail
{
    [[nodiscard]] inline std::filesystem::path ToStdPath(const StringView Path)
    {
        return std::filesystem::path{Path.Data(), Path.Data() + Path.Size()};
    }

    [[nodiscard]] inline String FromStdPath(const std::filesystem::path& Path)
    {
        const std::string Narrow = Path.string();
        return String{Narrow.data(), Narrow.size()};
    }
} // namespace FilesystemDetail
