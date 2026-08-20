#include "Core/IO/Directory.h"

#include <filesystem>

#include "Core/Debug/Debug.h"
#include "Core/IO/FilesystemDetail.h"
#include "Core/Logging/LogMacros.h"

bool Directory::Exists(const StringView Path)
{
    std::error_code ErrorCode;
    return std::filesystem::is_directory(FilesystemDetail::ToStdPath(Path), ErrorCode) && !ErrorCode;
}

void Directory::Create(StringView Path)
{
    if (std::error_code ErrorCode;
        !std::filesystem::create_directories(FilesystemDetail::ToStdPath(Path), ErrorCode) && ErrorCode)
    {
        LogError(IO, "Failed to create directory: {}", Path);
        Assert(false);
    }
}

void Directory::Delete(StringView Path, const bool Recursive)
{
    std::error_code ErrorCode;
    const auto StdPath = FilesystemDetail::ToStdPath(Path);
    const bool Removed =
        Recursive ? std::filesystem::remove_all(StdPath, ErrorCode) > 0 : std::filesystem::remove(StdPath, ErrorCode);
    if (!Removed || ErrorCode)
    {
        LogError(IO, "Failed to delete directory: {}", Path);
        Assert(false);
    }
}

Array<String> Directory::GetFiles(StringView Path)
{
    Array<String> Result;
    std::error_code ErrorCode;
    for (const auto& Entry : std::filesystem::directory_iterator(FilesystemDetail::ToStdPath(Path), ErrorCode))
    {
        if (ErrorCode)
        {
            break;
        }
        if (Entry.is_regular_file())
        {
            Result.Add(FilesystemDetail::FromStdPath(Entry.path()));
        }
    }

    if (ErrorCode)
    {
        LogError(IO, "Failed to enumerate files in directory: {}", Path);
        Assert(false);
    }
    return Result;
}

Array<String> Directory::GetDirectories(StringView Path)
{
    Array<String> Result;
    std::error_code ErrorCode;
    for (const auto& Entry : std::filesystem::directory_iterator(FilesystemDetail::ToStdPath(Path), ErrorCode))
    {
        if (ErrorCode)
        {
            break;
        }
        if (Entry.is_directory())
        {
            Result.Add(FilesystemDetail::FromStdPath(Entry.path()));
        }
    }

    if (ErrorCode)
    {
        LogError(IO, "Failed to enumerate directories in: {}", Path);
        Assert(false);
    }
    return Result;
}

String Directory::GetWorkingDirectory()
{
    std::error_code ErrorCode;
    const auto Current = std::filesystem::current_path(ErrorCode);
    if (ErrorCode)
    {
        LogError(IO, "Failed to get current directory");
        Assert(false);
        return {};
    }
    return FilesystemDetail::FromStdPath(Current);
}

void Directory::SetWorkingDirectory(StringView Path)
{
    std::error_code ErrorCode;
    std::filesystem::current_path(FilesystemDetail::ToStdPath(Path), ErrorCode);
    if (ErrorCode)
    {
        LogError(IO, "Failed to set current directory: {}", Path);
        Assert(false);
    }
}
