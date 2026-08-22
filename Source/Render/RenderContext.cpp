#include "RenderContext.h"

#include "Core/Debug/Debug.h"
#include "RHI/Device.h"
#include "RHI/SurfaceWindowManager.h"

bool RenderContext::Startup()
{
    Assert(RHI::CreateVulkanDevice());
    Assert(RHI::SurfaceWindowManager::CreateInstance());
    return true;
}

bool RenderContext::Shutdown()
{
    RHI::SurfaceWindowManager::DestroyInstance();
    RHI::DestroyVulkanDevice();
    return true;
}

void RenderContext::Tick(const Double DeltaSeconds)
{
    (void)DeltaSeconds;
}
