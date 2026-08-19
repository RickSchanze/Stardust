#include "Device.h"

#include "Core/Debug/Debug.h"
#include "RHIConfig.h"

#if STARDUST_RHI_BUILD_VULKAN
    #include "Vulkan/VulkanDevice.h"
#endif

using namespace RHI;

static Device* gDevice = nullptr;

Device& RHI::GetDevice()
{
    Assert(gDevice != nullptr);
    return *gDevice;
}

bool RHI::CreateVulkanDevice()
{
#if STARDUST_RHI_BUILD_VULKAN
    Assert(gDevice == nullptr);
    gDevice = New<VulkanDevice>();
    return gDevice->Init();
#else
    return false;
#endif
}

void RHI::DestroyVulkanDevice()
{
#if STARDUST_RHI_BUILD_VULKAN
    Assert(gDevice != nullptr);
    gDevice->UnInit();
    Delete(gDevice);
#endif
}
