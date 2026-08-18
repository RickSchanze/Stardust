#pragma once

#include <type_traits>

#include "Core/Container/Array.h"
#include "Core/Number.h"

namespace RHI
{

struct GPUHandle
{
    UInt32 Index{};
    UInt32 Generation{};

    [[nodiscard]] constexpr bool IsNull() const noexcept
    {
        return Generation == 0;
    }

    [[nodiscard]] static constexpr GPUHandle Null() noexcept
    {
        return GPUHandle{};
    }

    [[nodiscard]] constexpr auto operator<=>(const GPUHandle&) const noexcept = default;
};

// 池内每个 Index 的元数据：代次 + 空闲链下一项（占用中 NextFree == ~0u）
struct GPUHandleSlot
{
    UInt32 Generation = 0;
    UInt32 NextFree = ~0u;
};

class GPUHandlePool
{
public:
    static constexpr UInt32 InvalidIndex{~0u};

    [[nodiscard]] GPUHandle Allocate();
    void Destroy(GPUHandle Handle);

    [[nodiscard]] bool IsValid(GPUHandle Handle) const noexcept;

    [[nodiscard]] UInt32 LiveCount() const noexcept
    {
        return mLiveCount;
    }

    [[nodiscard]] UInt32 Capacity() const noexcept
    {
        return static_cast<UInt32>(mSlots.Num());
    }

    void Reserve(UInt32 Count);
    void Reset();

private:
    [[nodiscard]] GPUHandle AllocateFromFreeList();
    [[nodiscard]] GPUHandle AllocateNewSlot();

    [[nodiscard]] bool IsIndexInRange(UInt32 Index) const noexcept;
    [[nodiscard]] static bool IsSlotLive(const GPUHandleSlot& Slot, UInt32 Generation) noexcept;

    static void BumpGeneration(UInt32& Generation) noexcept;

    Array<GPUHandleSlot> mSlots;
    UInt32 mFreeHead = InvalidIndex;
    UInt32 mLiveCount = 0;
};

static_assert(std::is_trivially_copyable_v<GPUHandle>);

} // namespace RHI
