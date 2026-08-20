#pragma once

#include "RHI/Device.h"
#include "RHI/RHIConfig.h"

#if STARDUST_RHI_BUILD_VULKAN

#include "RHI/Vulkan/Vulkan.h"

namespace RHI
{
    class VulkanDevice : public Device
    {
    public:
        bool Init() override;
        void UnInit() override;

        BufferHandle CreateBuffer(const BufferDesc& Desc) override;
        void DestroyBuffer(const BufferHandle Handle) override;

        TextureHandle CreateTexture(const TextureDesc& Desc) override;
        void DestroyTexture(const TextureHandle Handle) override;

        TextureViewHandle CreateTextureView(const TextureViewDesc& Desc) override;
        void DestroyTextureView(const TextureViewHandle Handle) override;

        SamplerHandle CreateSampler(const SamplerDesc& Desc) override;
        void DestroySampler(const SamplerHandle Handle) override;

        ShaderHandle CreateShader(const ShaderDesc& Desc) override;
        void DestroyShader(const ShaderHandle Handle) override;

        DescriptorSetLayoutHandle CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& Desc) override;
        void DestroyDescriptorSetLayout(const DescriptorSetLayoutHandle Handle) override;

        PipelineLayoutHandle CreatePipelineLayout(const PipelineLayoutDesc& Desc) override;
        void DestroyPipelineLayout(const PipelineLayoutHandle Handle) override;

        GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& Desc) override;
        void DestroyGraphicsPipeline(const GraphicsPipelineHandle Handle) override;

        ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& Desc) override;
        void DestroyComputePipeline(const ComputePipelineHandle Handle) override;

        RenderPassHandle CreateRenderPass(const RenderPassDesc& Desc) override;
        void DestroyRenderPass(const RenderPassHandle Handle) override;

        CommandPoolHandle CreateCommandPool(const CommandPoolDesc& Desc) override;
        void DestroyCommandPool(const CommandPoolHandle Handle) override;

        CommandBufferHandle CreateCommandBuffer(const CommandBufferDesc& Desc) override;
        void DestroyCommandBuffer(const CommandBufferHandle Handle) override;

    private:
        [[nodiscard]] bool CreateInstance();
        [[nodiscard]] bool PickPhysicalDevice();
        [[nodiscard]] bool CreateLogicalDevice();
        [[nodiscard]] bool CreateAllocator();
#if STARDUST_RHI_ENABLE_VALIDATION
        void CreateDebugMessenger();
#endif

        VkInstance mInstance = VK_NULL_HANDLE;
        VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
        VkDevice mDevice = VK_NULL_HANDLE;
        VkQueue mGraphicsQueue = VK_NULL_HANDLE;
        UInt32 mGraphicsQueueFamily = 0;
        VmaAllocator mAllocator = nullptr;
#if STARDUST_RHI_ENABLE_VALIDATION
        VkDebugUtilsMessengerEXT mDebugMessenger = VK_NULL_HANDLE;
        bool mValidationEnabled = false;
#endif
    };
} // namespace RHI

#endif
