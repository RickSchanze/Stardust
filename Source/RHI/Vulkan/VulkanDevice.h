#pragma once
#include "RHI/Device.h"
#include "RHI/RHIConfig.h"

#if STARDUST_RHI_VALIDATE_DESC

namespace RHI
{
    class VulkanDevice : public Device
    {
    public:
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
    };
} // namespace RHI

#endif
