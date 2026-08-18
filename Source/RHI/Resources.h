#pragma once

#include "Core/Container/Array.h"
#include "Core/Number.h"
#include "Core/Ptr.h"

#include "RHI/GPUHandlePool.h"

namespace RHI
{

struct TextureDesc
{
};

struct GPUTexture
{
    TextureDesc Desc;
    IntPtr Native;
};

class Device
{
    GPUHandlePool mTextureHandles;
    Array<GPUTexture> mTextures;
};

} // namespace RHI
