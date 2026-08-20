#pragma once

#include "RHI/Commands.h"
#include "RHI/Resources.h"

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
        virtual ~Device() = default;

        [[nodiscard]] API GetAPI() const
        {
            return mAPI;
        }

        virtual BufferHandle CreateBuffer(const BufferDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroyBuffer(const BufferHandle Handle) = 0;

        virtual TextureHandle CreateTexture(const TextureDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroyTexture(const TextureHandle Handle) = 0;

        virtual TextureViewHandle CreateTextureView(const TextureViewDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroyTextureView(const TextureViewHandle Handle) = 0;

        virtual SamplerHandle CreateSampler(const SamplerDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroySampler(const SamplerHandle Handle) = 0;

        virtual ShaderHandle CreateShader(const ShaderDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroyShader(const ShaderHandle Handle) = 0;

        virtual DescriptorSetLayoutHandle CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& Desc,
                                                                   const char* DebugName = nullptr) = 0;
        virtual void DestroyDescriptorSetLayout(const DescriptorSetLayoutHandle Handle) = 0;

        virtual PipelineLayoutHandle CreatePipelineLayout(const PipelineLayoutDesc& Desc,
                                                          const char* DebugName = nullptr) = 0;
        virtual void DestroyPipelineLayout(const PipelineLayoutHandle Handle) = 0;

        virtual GraphicsPipelineHandle CreateGraphicsPipeline(const GraphicsPipelineDesc& Desc,
                                                              const char* DebugName = nullptr) = 0;
        virtual void DestroyGraphicsPipeline(const GraphicsPipelineHandle Handle) = 0;

        virtual ComputePipelineHandle CreateComputePipeline(const ComputePipelineDesc& Desc,
                                                            const char* DebugName = nullptr) = 0;
        virtual void DestroyComputePipeline(const ComputePipelineHandle Handle) = 0;

        virtual RenderPassHandle CreateRenderPass(const RenderPassDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroyRenderPass(const RenderPassHandle Handle) = 0;

        virtual CommandPoolHandle CreateCommandPool(const CommandPoolDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroyCommandPool(const CommandPoolHandle Handle) = 0;

        virtual CommandBufferHandle CreateCommandBuffer(const CommandBufferDesc& Desc,
                                                        const char* DebugName = nullptr) = 0;
        virtual void DestroyCommandBuffer(const CommandBufferHandle Handle) = 0;

        virtual SurfaceHandle CreateSurface(const SurfaceDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroySurface(const SurfaceHandle Handle) = 0;

        virtual SwapchainHandle CreateSwapchain(const SwapchainDesc& Desc, const char* DebugName = nullptr) = 0;
        virtual void DestroySwapchain(const SwapchainHandle Handle) = 0;

        virtual bool Init() = 0;
        virtual void UnInit() = 0;

        [[nodiscard]] GPUBuffer& GetBuffer(const BufferHandle Handle)
        {
            return mBuffers.Get(Handle);
        }

        [[nodiscard]] const GPUBuffer& GetBuffer(const BufferHandle Handle) const
        {
            return mBuffers.Get(Handle);
        }

        [[nodiscard]] GPUTexture& GetTexture(const TextureHandle Handle)
        {
            return mTextures.Get(Handle);
        }

        [[nodiscard]] const GPUTexture& GetTexture(const TextureHandle Handle) const
        {
            return mTextures.Get(Handle);
        }

        [[nodiscard]] GPUTextureView& GetTextureView(const TextureViewHandle Handle)
        {
            return mTextureViews.Get(Handle);
        }

        [[nodiscard]] const GPUTextureView& GetTextureView(const TextureViewHandle Handle) const
        {
            return mTextureViews.Get(Handle);
        }

        [[nodiscard]] GPUSampler& GetSampler(const SamplerHandle Handle)
        {
            return mSamplers.Get(Handle);
        }

        [[nodiscard]] const GPUSampler& GetSampler(const SamplerHandle Handle) const
        {
            return mSamplers.Get(Handle);
        }

        [[nodiscard]] GPUShader& GetShader(const ShaderHandle Handle)
        {
            return mShaders.Get(Handle);
        }

        [[nodiscard]] const GPUShader& GetShader(const ShaderHandle Handle) const
        {
            return mShaders.Get(Handle);
        }

        [[nodiscard]] GPUDescriptorSetLayout& GetDescriptorSetLayout(const DescriptorSetLayoutHandle Handle)
        {
            return mDescriptorSetLayouts.Get(Handle);
        }

        [[nodiscard]] const GPUDescriptorSetLayout& GetDescriptorSetLayout(const DescriptorSetLayoutHandle Handle) const
        {
            return mDescriptorSetLayouts.Get(Handle);
        }

        [[nodiscard]] GPUPipelineLayout& GetPipelineLayout(const PipelineLayoutHandle Handle)
        {
            return mPipelineLayouts.Get(Handle);
        }

        [[nodiscard]] const GPUPipelineLayout& GetPipelineLayout(const PipelineLayoutHandle Handle) const
        {
            return mPipelineLayouts.Get(Handle);
        }

        [[nodiscard]] GPUGraphicsPipeline& GetGraphicsPipeline(const GraphicsPipelineHandle Handle)
        {
            return mGraphicsPipelines.Get(Handle);
        }

        [[nodiscard]] const GPUGraphicsPipeline& GetGraphicsPipeline(const GraphicsPipelineHandle Handle) const
        {
            return mGraphicsPipelines.Get(Handle);
        }

        [[nodiscard]] GPUComputePipeline& GetComputePipeline(const ComputePipelineHandle Handle)
        {
            return mComputePipelines.Get(Handle);
        }

        [[nodiscard]] const GPUComputePipeline& GetComputePipeline(const ComputePipelineHandle Handle) const
        {
            return mComputePipelines.Get(Handle);
        }

        [[nodiscard]] GPURenderPass& GetRenderPass(const RenderPassHandle Handle)
        {
            return mRenderPasses.Get(Handle);
        }

        [[nodiscard]] const GPURenderPass& GetRenderPass(const RenderPassHandle Handle) const
        {
            return mRenderPasses.Get(Handle);
        }

        [[nodiscard]] GPUCommandPool& GetCommandPool(const CommandPoolHandle Handle)
        {
            return mCommandPools.Get(Handle);
        }

        [[nodiscard]] const GPUCommandPool& GetCommandPool(const CommandPoolHandle Handle) const
        {
            return mCommandPools.Get(Handle);
        }

        [[nodiscard]] bool IsBufferValid(const BufferHandle Handle) const noexcept
        {
            return mBuffers.IsValid(Handle);
        }

        [[nodiscard]] bool IsTextureValid(const TextureHandle Handle) const noexcept
        {
            return mTextures.IsValid(Handle);
        }

        [[nodiscard]] bool IsTextureViewValid(const TextureViewHandle Handle) const noexcept
        {
            return mTextureViews.IsValid(Handle);
        }

        [[nodiscard]] bool IsSamplerValid(const SamplerHandle Handle) const noexcept
        {
            return mSamplers.IsValid(Handle);
        }

        [[nodiscard]] bool IsShaderValid(const ShaderHandle Handle) const noexcept
        {
            return mShaders.IsValid(Handle);
        }

        [[nodiscard]] bool IsDescriptorSetLayoutValid(const DescriptorSetLayoutHandle Handle) const noexcept
        {
            return mDescriptorSetLayouts.IsValid(Handle);
        }

        [[nodiscard]] bool IsPipelineLayoutValid(const PipelineLayoutHandle Handle) const noexcept
        {
            return mPipelineLayouts.IsValid(Handle);
        }

        [[nodiscard]] bool IsGraphicsPipelineValid(const GraphicsPipelineHandle Handle) const noexcept
        {
            return mGraphicsPipelines.IsValid(Handle);
        }

        [[nodiscard]] bool IsComputePipelineValid(const ComputePipelineHandle Handle) const noexcept
        {
            return mComputePipelines.IsValid(Handle);
        }

        [[nodiscard]] bool IsRenderPassValid(const RenderPassHandle Handle) const noexcept
        {
            return mRenderPasses.IsValid(Handle);
        }

        [[nodiscard]] bool IsCommandPoolValid(const CommandPoolHandle Handle) const noexcept
        {
            return mCommandPools.IsValid(Handle);
        }

        [[nodiscard]] GPUCommandBuffer& GetCommandBuffer(const CommandBufferHandle Handle)
        {
            return mCommandBuffers.Get(Handle);
        }

        [[nodiscard]] const GPUCommandBuffer& GetCommandBuffer(const CommandBufferHandle Handle) const
        {
            return mCommandBuffers.Get(Handle);
        }

        [[nodiscard]] bool IsCommandBufferValid(const CommandBufferHandle Handle) const noexcept
        {
            return mCommandBuffers.IsValid(Handle);
        }

        [[nodiscard]] GPUSurface& GetSurface(const SurfaceHandle Handle)
        {
            return mSurfaces.Get(Handle);
        }

        [[nodiscard]] const GPUSurface& GetSurface(const SurfaceHandle Handle) const
        {
            return mSurfaces.Get(Handle);
        }

        [[nodiscard]] GPUSwapchain& GetSwapchain(const SwapchainHandle Handle)
        {
            return mSwapchains.Get(Handle);
        }

        [[nodiscard]] const GPUSwapchain& GetSwapchain(const SwapchainHandle Handle) const
        {
            return mSwapchains.Get(Handle);
        }

        [[nodiscard]] bool IsSurfaceValid(const SurfaceHandle Handle) const noexcept
        {
            return mSurfaces.IsValid(Handle);
        }

        [[nodiscard]] bool IsSwapchainValid(const SwapchainHandle Handle) const noexcept
        {
            return mSwapchains.IsValid(Handle);
        }

    protected:
        GPUResourcePool<GPUBuffer> mBuffers;
        GPUResourcePool<GPUTexture> mTextures;
        GPUResourcePool<GPUTextureView> mTextureViews;
        GPUResourcePool<GPUSampler> mSamplers;
        GPUResourcePool<GPUShader> mShaders;
        GPUResourcePool<GPUDescriptorSetLayout> mDescriptorSetLayouts;
        GPUResourcePool<GPUPipelineLayout> mPipelineLayouts;
        GPUResourcePool<GPUGraphicsPipeline> mGraphicsPipelines;
        GPUResourcePool<GPUComputePipeline> mComputePipelines;
        GPUResourcePool<GPURenderPass> mRenderPasses;
        GPUResourcePool<GPUCommandPool> mCommandPools;
        GPUResourcePool<GPUCommandBuffer> mCommandBuffers;
        GPUResourcePool<GPUSurface> mSurfaces;
        GPUResourcePool<GPUSwapchain> mSwapchains;
        API mAPI = API::Count;
    };

    Device& GetDevice();
    bool CreateVulkanDevice();
    void DestroyVulkanDevice();
} // namespace RHI
