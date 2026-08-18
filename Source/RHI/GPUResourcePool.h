#pragma once

#include "Core/Container/Array.h"
#include "Core/Debug/Debug.h"
#include "RHI/GPUHandlePool.h"

#include <utility>

namespace RHI
{

template <typename T>
class GPUResourcePool
{
public:
    [[nodiscard]] GPUHandle Create(T Resource)
    {
        const GPUHandle Handle = mHandles.Allocate();
        EnsurePayloadCapacity();
        mResources[Handle.Index] = std::move(Resource);
        return Handle;
    }

    void Destroy(GPUHandle Handle)
    {
        Assert(IsValid(Handle));
        mResources[Handle.Index] = T{};
        mHandles.Destroy(Handle);
    }

    [[nodiscard]] bool IsValid(GPUHandle Handle) const noexcept
    {
        return mHandles.IsValid(Handle);
    }

    [[nodiscard]] T& Get(GPUHandle Handle)
    {
        Assert(IsValid(Handle));
        return mResources[Handle.Index];
    }

    [[nodiscard]] const T& Get(GPUHandle Handle) const
    {
        Assert(IsValid(Handle));
        return mResources[Handle.Index];
    }

    [[nodiscard]] UInt32 LiveCount() const noexcept
    {
        return mHandles.LiveCount();
    }

    [[nodiscard]] UInt32 Capacity() const noexcept
    {
        return mHandles.Capacity();
    }

    void Reserve(UInt32 Count)
    {
        mHandles.Reserve(Count);
        mResources.Reserve(Count);
    }

    void Reset()
    {
        mHandles.Reset();
        mResources.Clear();
    }

private:
    void EnsurePayloadCapacity()
    {
        mResources.Resize(mResources.Capacity());
    }

    GPUHandlePool mHandles;
    Array<T> mResources;
};

} // namespace RHI
