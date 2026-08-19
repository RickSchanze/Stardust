#include "RHI/Commands.h"

#include "Core/Debug/Debug.h"
#include "RHI/GPUResourcePool.h"

namespace RHI
{
    namespace
    {
        void SetValidationError(String* ErrorMessage, const char* Message)
        {
            if (ErrorMessage != nullptr)
            {
                *ErrorMessage = String{Message};
            }
        }
    } // namespace

    void CommandBuffer::Begin(const CommandBufferUsageFlag Usage)
    {
        Assert(!mIsRecording);
        mCommands.Clear();
        mUsage = Usage;
        mIsRecording = true;
    }

    void CommandBuffer::End()
    {
        Assert(mIsRecording);
        mIsRecording = false;
    }

    void CommandBuffer::Reset(const CommandBufferResetFlag Flags)
    {
        (void)Flags;
        mCommands.Clear();
        mUsage = CommandBufferUsageFlag::None;
        mIsRecording = false;
    }

    const Command* CommandBuffer::First() const noexcept
    {
        if (mCommands.Empty())
        {
            return nullptr;
        }

        return reinterpret_cast<const Command*>(mCommands.Data());
    }

    const Command* CommandBuffer::Next(const Command* Current) const noexcept
    {
        if (Current == nullptr || mCommands.Empty())
        {
            return nullptr;
        }

        const auto* Bytes = mCommands.Data();
        const auto Offset = static_cast<std::size_t>(reinterpret_cast<const std::uint8_t*>(Current) - Bytes);
        const auto NextOffset = AlignCommandOffset(Offset + static_cast<std::size_t>(Current->Size));
        if (NextOffset >= mCommands.Num())
        {
            return nullptr;
        }

        return reinterpret_cast<const Command*>(Bytes + NextOffset);
    }

    void CommandBuffer::SetDebugLabelName(char (&Destination)[64], const char* Name)
    {
        Destination[0] = '\0';
        if (Name == nullptr)
        {
            return;
        }

        std::size_t Index = 0;
        while (Index + 1 < 64 && Name[Index] != '\0')
        {
            Destination[Index] = Name[Index];
            ++Index;
        }
        Destination[Index] = '\0';
    }

    CommandBuffer CommandPool::Allocate(const CommandBufferType Type)
    {
        CommandBuffer Buffer{};
        Buffer.Desc = CommandBufferDesc{
            .Pool = Handle,
            .Type = Type,
        };
        return Buffer;
    }

    void CommandPool::Reset(const CommandPoolResetFlag Flags)
    {
        (void)Flags;
    }

    bool ValidateCommandPoolDesc(const CommandPoolDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Queue == QueueFlag::None)
        {
            SetValidationError(ErrorMessage, "CommandPoolDesc.Queue must declare at least one queue flag.");
            return false;
        }

        return true;
    }

    bool ValidateCommandBufferDesc(const CommandBufferDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Pool.IsNull())
        {
            SetValidationError(ErrorMessage, "CommandBufferDesc.Pool must reference a valid command pool.");
            return false;
        }

        return true;
    }
} // namespace RHI

template class RHI::GPUResourcePool<RHI::GPUCommandPool>;
template class RHI::GPUResourcePool<RHI::GPUCommandBuffer>;
