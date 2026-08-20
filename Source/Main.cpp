#include <mimalloc-new-delete.h>
#include "Core/Debug/Debug.h"
#include "Core/Profiler/Profiler.h"
#include "RHI/Device.h"

static int Main()
{
    PerformanceCounter _{"Main"};

    Assert(RHI::CreateVulkanDevice());
    RHI::DestroyVulkanDevice();

    MarkFrame();
    return 0;
}

int main()
{
    return Main();
}
