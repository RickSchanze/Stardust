#pragma once

#include "Core/Singleton.h"
#include "Resources.h"

namespace RHI
{
    enum class API
    {
        Vulkan,
        Count,
    };

    class Device
    {
    public:
        [[nodiscard]] API GetAPI() const
        {
            return mAPI;
        }

        virtual TextureHandle CreateTexture(const TextureDesc& Desc) = 0;
        virtual void DestroyTexture(TextureHandle Handle) = 0;

    private:
        API mAPI = API::Count;
    };

    static Device& GetDevice();
    static bool CreateVulkanDevice();
    static void DestroyVulkanDevice();
} // namespace RHI
