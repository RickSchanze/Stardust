#include "Core/IO/Path.h"

#include <filesystem>

#include "Core/IO/FilesystemDetail.h"

namespace
{
    [[nodiscard]] bool IsSeparator(const char Character)
    {
        return Character == Path::DirectorySeparatorChar || Character == Path::AltDirectorySeparatorChar;
    }

    [[nodiscard]] StringView TrimTrailingSeparators(StringView Path)
    {
        while (!Path.Empty() && IsSeparator(Path.Back()))
        {
            Path = Path.First(Path.Size() - 1);
        }
        return Path;
    }

    [[nodiscard]] std::size_t FindFilenameStart(StringView Path)
    {
        for (std::size_t Index = Path.Size(); Index > 0; --Index)
        {
            if (IsSeparator(Path[Index - 1]))
            {
                return Index;
            }
        }
        return 0;
    }

    [[nodiscard]] String CombineTwo(StringView Path1, StringView Path2)
    {
        if (Path1.Empty())
        {
            return Path2.ToString();
        }
        if (Path2.Empty())
        {
            return Path1.ToString();
        }

        Path1 = TrimTrailingSeparators(Path1);

        std::size_t Path2Start = 0;
        while (Path2Start < Path2.Size() && IsSeparator(Path2[Path2Start]))
        {
            ++Path2Start;
        }

        String Result;
        Result.Reserve(Path1.Size() + 1 + Path2.Size() - Path2Start);
        Result.Append(Path1);
        Result.Append(Path::DirectorySeparatorChar);
        Result.Append(Path2.Substr(Path2Start));
        return Result;
    }
} // namespace

String Path::Combine(const StringView Path1, const StringView Path2)
{
    return CombineTwo(Path1, Path2);
}

String Path::Combine(const StringView Path1, const StringView Path2, const StringView Path3)
{
    return CombineTwo(CombineTwo(Path1, Path2), Path3);
}

String Path::GetFileName(const StringView Path)
{
    const auto Trimmed = TrimTrailingSeparators(Path);
    return Trimmed.Substr(FindFilenameStart(Trimmed)).ToString();
}

String Path::GetDirectoryName(const StringView Path)
{
    const auto Trimmed = TrimTrailingSeparators(Path);
    if (Trimmed.Empty())
    {
        return {};
    }

    const auto FilenameStart = FindFilenameStart(Trimmed);
    if (FilenameStart == 0)
    {
        return {};
    }

    return TrimTrailingSeparators(Trimmed.First(FilenameStart - 1)).ToString();
}

String Path::GetExtension(const StringView Path)
{
    const auto Trimmed = TrimTrailingSeparators(Path);
    const auto Filename = Trimmed.Substr(FindFilenameStart(Trimmed));
    const auto Dot = Filename.RFind('.');
    if (Dot == StringView::Npos || Dot == 0)
    {
        return {};
    }
    return Filename.Substr(Dot).ToString();
}

String Path::ChangeExtension(const StringView Path, const StringView Extension)
{
    const auto Trimmed = TrimTrailingSeparators(Path);
    const auto FilenameStart = FindFilenameStart(Trimmed);
    const auto Filename = Trimmed.Substr(FilenameStart);
    const auto Dot = Filename.RFind('.');

    String Result;
    if (Dot == StringView::Npos)
    {
        Result = Trimmed.ToString();
    }
    else
    {
        Result = Trimmed.First(FilenameStart + Dot).ToString();
    }

    if (!Extension.Empty())
    {
        if (Extension.Front() != '.')
        {
            Result.Append('.');
        }
        Result.Append(Extension);
    }
    return Result;
}

String Path::GetFullPath(const StringView Path)
{
    return FilesystemDetail::FromStdPath(std::filesystem::weakly_canonical(FilesystemDetail::ToStdPath(Path)));
}
