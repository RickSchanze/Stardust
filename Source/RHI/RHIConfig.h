#pragma once

#define STARDUST_RHI_VALIDATE_DESC 1
#define STARDUST_RHI_BUILD_VULKAN 1
#define STARDUST_RHI_ENABLE_VALIDATION 1

namespace RHI
{
#if STARDUST_RHI_ENABLE_VALIDATION
    inline bool gEnableValidation = true;
#endif
} // namespace RHI
