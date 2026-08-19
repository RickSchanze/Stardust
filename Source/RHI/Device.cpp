//
// Created by hulkzhou on 2026/8/19.
//

#include "Device.h"

#include "Core/Debug/Debug.h"

using namespace RHI;

static Device* gDevice = nullptr;

Device& RHI::GetDevice()
{
    Assert(gDevice != nullptr);
    return *gDevice;
}

bool RHI::CreateVulkanDevice()
{
    return true;
}

void RHI::DestroyVulkanDevice()
{
    return;
}
