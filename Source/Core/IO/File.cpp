#include "Core/IO/File.h"

#include <filesystem>
#include <fstream>

#include "Core/Debug/Debug.h"
#include "Core/IO/FilesystemDetail.h"
#include "Core/Logging/LogMacros.h"

bool File::Exists(StringView Path)
{
    std::error_code ErrorCode;
    return std::filesystem::is_regular_file(FilesystemDetail::ToStdPath(Path), ErrorCode) && !ErrorCode;
}

Array<std::uint8_t> File::ReadAllBytes(StringView Path)
{
    std::ifstream Stream(FilesystemDetail::ToStdPath(Path), std::ios::binary | std::ios::ate);
    if (!Stream)
    {
        LogError(IO, "Failed to open file for reading: {}", Path);
        Assert(false);
        return {};
    }

    const auto Size = Stream.tellg();
    if (Size < 0)
    {
        LogError(IO, "Failed to read file size: {}", Path);
        Assert(false);
        return {};
    }

    Array<std::uint8_t> Result(static_cast<std::size_t>(Size));
    Stream.seekg(0, std::ios::beg);
    Stream.read(reinterpret_cast<char*>(Result.Data()), Size);
    if (!Stream)
    {
        LogError(IO, "Failed to read file: {}", Path);
        Assert(false);
        return {};
    }
    return Result;
}

String File::ReadAllText(StringView Path)
{
    const auto Bytes = ReadAllBytes(Path);
    return String{reinterpret_cast<const char*>(Bytes.Data()), Bytes.Num()};
}

void File::WriteAllBytes(StringView Path, Span<const std::uint8_t> Bytes)
{
    std::ofstream Stream(FilesystemDetail::ToStdPath(Path), std::ios::binary | std::ios::trunc);
    if (!Stream)
    {
        LogError(IO, "Failed to open file for writing: {}", Path);
        Assert(false);
        return;
    }

    if (!Bytes.Empty())
    {
        Stream.write(reinterpret_cast<const char*>(Bytes.Data()), static_cast<std::streamsize>(Bytes.Num()));
    }

    if (!Stream)
    {
        LogError(IO, "Failed to write file: {}", Path);
        Assert(false);
    }
}

void File::WriteAllText(StringView Path, StringView Contents)
{
    WriteAllBytes(Path, Span<const std::uint8_t>{reinterpret_cast<const std::uint8_t*>(Contents.Data()), Contents.Size()});
}

void File::AppendAllBytes(StringView Path, Span<const std::uint8_t> Bytes)
{
    std::ofstream Stream(FilesystemDetail::ToStdPath(Path), std::ios::binary | std::ios::app);
    if (!Stream)
    {
        LogError(IO, "Failed to open file for appending: {}", Path);
        Assert(false);
        return;
    }

    if (!Bytes.Empty())
    {
        Stream.write(reinterpret_cast<const char*>(Bytes.Data()), static_cast<std::streamsize>(Bytes.Num()));
    }

    if (!Stream)
    {
        LogError(IO, "Failed to append to file: {}", Path);
        Assert(false);
    }
}

void File::AppendAllText(StringView Path, StringView Contents)
{
    AppendAllBytes(Path, Span<const std::uint8_t>{reinterpret_cast<const std::uint8_t*>(Contents.Data()), Contents.Size()});
}

void File::Delete(StringView Path)
{
    std::error_code ErrorCode;
    if (!std::filesystem::remove(FilesystemDetail::ToStdPath(Path), ErrorCode) || ErrorCode)
    {
        LogError(IO, "Failed to delete file: {}", Path);
        Assert(false);
    }
}

void File::Copy(StringView SourcePath, StringView DestinationPath, bool Overwrite)
{
    std::error_code ErrorCode;
    std::filesystem::copy_file(FilesystemDetail::ToStdPath(SourcePath),
                               FilesystemDetail::ToStdPath(DestinationPath),
                               Overwrite ? std::filesystem::copy_options::overwrite_existing
                                         : std::filesystem::copy_options::none,
                               ErrorCode);
    if (ErrorCode)
    {
        LogError(IO, "Failed to copy file from {} to {}", SourcePath, DestinationPath);
        Assert(false);
    }
}

void File::Move(StringView SourcePath, StringView DestinationPath)
{
    std::error_code ErrorCode;
    std::filesystem::rename(FilesystemDetail::ToStdPath(SourcePath), FilesystemDetail::ToStdPath(DestinationPath), ErrorCode);
    if (ErrorCode)
    {
        LogError(IO, "Failed to move file from {} to {}", SourcePath, DestinationPath);
        Assert(false);
    }
}
