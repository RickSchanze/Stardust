#pragma once
#include "Core/Number.h"

namespace RHI
{
    struct GPUHandle
    {
        UInt32 Index;
        UInt32 Generation;
    };

    struct TextureHandle
    {
        GPUHandle Handle;
    };
} // namespace RHI
