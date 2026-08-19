#include "RHI/GPUHandlePool.h"

#include "Core/Debug/Debug.h"

namespace RHI
{

    void GPUHandlePool::BumpGeneration(UInt32& Generation) noexcept
    {
        ++Generation;
        if (Generation == 0)
        {
            Generation = 1;
        }
    }

    bool GPUHandlePool::IsSlotLive(const GPUHandleSlot& Slot, const UInt32 Generation) noexcept
    {
        return Generation != 0 && Slot.Generation == Generation && Slot.NextFree == InvalidIndex;
    }

    bool GPUHandlePool::IsIndexInRange(const UInt32 Index) const noexcept
    {
        return Index < static_cast<UInt32>(mSlots.Num());
    }

    GPUHandle GPUHandlePool::AllocateFromFreeList()
    {
        const auto Index = mFreeHead;
        auto& [Generation, NextFree] = mSlots[Index];
        mFreeHead = NextFree;
        NextFree = InvalidIndex;

        if (Generation == 0)
        {
            Generation = 1;
        }

        ++mLiveCount;
        return GPUHandle{.Index = Index, .Generation = Generation};
    }

    GPUHandle GPUHandlePool::AllocateNewSlot()
    {
        const auto Index = static_cast<UInt32>(mSlots.AddDefaulted());
        auto& [Generation, NextFree] = mSlots[Index];
        Generation = 1;
        NextFree = InvalidIndex;

        ++mLiveCount;
        return GPUHandle{.Index = Index, .Generation = Generation};
    }

    GPUHandle GPUHandlePool::Allocate()
    {
        if (mFreeHead != InvalidIndex)
        {
            return AllocateFromFreeList();
        }

        return AllocateNewSlot();
    }

    void GPUHandlePool::Destroy(const GPUHandle Handle)
    {
        Assert(IsValid(Handle));

        auto& [Generation, NextFree] = mSlots[Handle.Index];
        BumpGeneration(Generation);
        NextFree = mFreeHead;
        mFreeHead = Handle.Index;
        --mLiveCount;
    }

    bool GPUHandlePool::IsValid(const GPUHandle Handle) const noexcept
    {
        if (Handle.IsNull())
        {
            return false;
        }

        if (!IsIndexInRange(Handle.Index))
        {
            return false;
        }

        const GPUHandleSlot& Slot = mSlots[Handle.Index];
        return IsSlotLive(Slot, Handle.Generation);
    }

    void GPUHandlePool::Reserve(const UInt32 Count)
    {
        mSlots.Reserve(Count);
    }

    void GPUHandlePool::Reset()
    {
        mSlots.Clear();
        mFreeHead = InvalidIndex;
        mLiveCount = 0;
    }

} // namespace RHI
