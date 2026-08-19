#pragma once

#include <cstdint>
#include <utility>

#include "Core/Container/Array.h"
#include "Core/Number.h"
#include "Core/Ptr.h"
#include "Core/String/String.h"
#include "RHI/Enums.h"
#include "RHI/GPUHandlePool.h"
#include "RHI/GPUResourcePool.h"

namespace RHI
{

    template <typename TDesc>
    class GPUResourceWithDesc
    {
    public:
        GPUResourceWithDesc() = default;

        explicit GPUResourceWithDesc(const TDesc& InDesc) : mDesc(InDesc) {}

        explicit GPUResourceWithDesc(TDesc&& InDesc) : mDesc(std::move(InDesc)) {}

        [[nodiscard]] const TDesc& GetDesc() const noexcept
        {
            return mDesc;
        }

    private:
        TDesc mDesc{};
    };

    struct BufferHandle : GPUHandle
    {
    };

    struct TextureHandle : GPUHandle
    {
    };

    struct TextureViewHandle : GPUHandle
    {
    };

    struct SamplerHandle : GPUHandle
    {
    };

    struct ShaderHandle : GPUHandle
    {
    };

    struct DescriptorSetLayoutHandle : GPUHandle
    {
    };

    struct PipelineLayoutHandle : GPUHandle
    {
    };

    struct GraphicsPipelineHandle : GPUHandle
    {
    };

    struct ComputePipelineHandle : GPUHandle
    {
    };

    struct RenderPassHandle : GPUHandle
    {
    };

    struct DescriptorSetHandle : GPUHandle
    {
    };

    struct CommandPoolHandle : GPUHandle
    {
    };

    struct CommandBufferHandle : GPUHandle
    {
    };

    inline constexpr UInt32 gUnusedAttachmentIndex{~0u};
    inline constexpr UInt32 gExternalSubpassIndex{~0u};

    struct BufferDesc
    {
        UInt64 Size = 0;
        BufferUsageFlag Usage = BufferUsageFlag::None;
        BufferCreateFlag CreateFlags = BufferCreateFlag::None;
        MemoryPropertyFlag MemoryProperties = MemoryPropertyFlag::DeviceLocal;
    };

    struct TextureDesc
    {
        TextureDimension Dimension = TextureDimension::Dim2D;
        PixelFormat Format = PixelFormat::Undefined;

        UInt32 Width = 1;
        UInt32 Height = 1;
        UInt32 Depth = 1;

        UInt32 MipLevels = 1;
        UInt32 ArrayLayers = 1;

        SampleCount Samples = SampleCount::Count1;

        TextureUsageFlag Usage = TextureUsageFlag::None;
        TextureCreateFlag CreateFlags = TextureCreateFlag::None;

        TextureTiling Tiling = TextureTiling::Optimal;
        TextureLayout InitialLayout = TextureLayout::Undefined;
    };

    struct TextureViewDesc
    {
        TextureHandle Texture;
        TextureViewDimension Dimension = TextureViewDimension::Dim2D;
        PixelFormat Format = PixelFormat::Undefined;
        TextureAspectFlag Aspect = TextureAspectFlag::Color;
        UInt32 BaseMipLevel = 0;
        UInt32 MipLevelCount = 1;
        UInt32 BaseArrayLayer = 0;
        UInt32 ArrayLayerCount = 1;
    };

    struct SamplerDesc
    {
        SamplerFilter MinFilter = SamplerFilter::Linear;
        SamplerFilter MagFilter = SamplerFilter::Linear;
        SamplerMipmapMode MipmapMode = SamplerMipmapMode::Linear;
        SamplerAddressMode AddressModeU = SamplerAddressMode::Repeat;
        SamplerAddressMode AddressModeV = SamplerAddressMode::Repeat;
        SamplerAddressMode AddressModeW = SamplerAddressMode::Repeat;
        Float MipLodBias = 0.0f;
        bool AnisotropyEnable = false;
        Float MaxAnisotropy = 1.0f;
        bool CompareEnable = false;
        CompareOp Compare = CompareOp::Always;
        Float MinLod = 0.0f;
        Float MaxLod = 1000.0f;
        BorderColor Border = BorderColor::FloatTransparentBlack;
        SamplerReductionMode ReductionMode = SamplerReductionMode::WeightedAverage;
        SamplerCreateFlag CreateFlags = SamplerCreateFlag::None;
    };

    struct ShaderDesc
    {
        ShaderStageFlag Stage = ShaderStageFlag::None;
        Array<std::uint32_t> Code;
        char EntryPoint[32] = "main";
    };

    struct DescriptorBindingDesc
    {
        UInt32 Binding = 0;
        DescriptorType Type = DescriptorType::UniformBuffer;
        UInt32 DescriptorCount = 1;
        ShaderStageFlag Stages = ShaderStageFlag::None;
        DescriptorBindingFlag Flags = DescriptorBindingFlag::None;
        Array<SamplerHandle> ImmutableSamplers;
    };

    struct DescriptorSetLayoutDesc
    {
        Array<DescriptorBindingDesc> Bindings;
        DescriptorSetLayoutCreateFlag CreateFlags = DescriptorSetLayoutCreateFlag::None;
    };

    struct PushConstantRangeDesc
    {
        ShaderStageFlag Stages = ShaderStageFlag::None;
        UInt32 Offset = 0;
        UInt32 Size = 0;
    };

    struct PipelineLayoutDesc
    {
        Array<DescriptorSetLayoutHandle> SetLayouts;
        Array<PushConstantRangeDesc> PushConstantRanges;
    };

    struct GraphicsColorAttachmentDesc
    {
        bool BlendEnable = false;
        BlendFactor SrcColorBlendFactor = BlendFactor::One;
        BlendFactor DstColorBlendFactor = BlendFactor::Zero;
        BlendOp ColorBlendOp = BlendOp::Add;
        BlendFactor SrcAlphaBlendFactor = BlendFactor::One;
        BlendFactor DstAlphaBlendFactor = BlendFactor::Zero;
        BlendOp AlphaBlendOp = BlendOp::Add;
        ColorComponentFlag ColorWriteMask = ColorComponentFlag::All;
    };

    struct GraphicsPipelineDesc
    {
        PipelineLayoutHandle Layout;
        ShaderHandle VertexShader;
        ShaderHandle FragmentShader;
        RenderPassHandle RenderPass;
        UInt32 Subpass = 0;
        PrimitiveTopology Topology = PrimitiveTopology::TriangleList;
        PolygonMode PolygonMode = PolygonMode::Fill;
        CullMode CullMode = CullMode::Back;
        FrontFace FrontFace = FrontFace::CounterClockwise;
        SampleCount Samples = SampleCount::Count1;
        bool DepthTestEnable = false;
        bool DepthWriteEnable = false;
        CompareOp DepthCompareOp = CompareOp::Less;
        Array<GraphicsColorAttachmentDesc> ColorAttachments;
        PixelFormat DepthStencilFormat = PixelFormat::Undefined;
    };

    struct ComputePipelineDesc
    {
        PipelineLayoutHandle Layout;
        ShaderHandle ComputeShader;
    };

    struct RenderPassAttachmentDesc
    {
        PixelFormat Format = PixelFormat::Undefined;
        SampleCount Samples = SampleCount::Count1;
        LoadOp Load = LoadOp::DontCare;
        StoreOp Store = StoreOp::Store;
        LoadOp StencilLoad = LoadOp::DontCare;
        StoreOp StencilStore = StoreOp::DontCare;
        TextureLayout InitialLayout = TextureLayout::Undefined;
        TextureLayout FinalLayout = TextureLayout::Undefined;
    };

    struct RenderPassAttachmentRef
    {
        UInt32 Attachment = gUnusedAttachmentIndex;
        TextureLayout Layout = TextureLayout::Undefined;
    };

    struct RenderPassSubpassDesc
    {
        PipelineType Pipeline = PipelineType::Graphics;
        Array<RenderPassAttachmentRef> InputAttachments;
        Array<RenderPassAttachmentRef> ColorAttachments;
        Array<RenderPassAttachmentRef> ResolveAttachments;
        RenderPassAttachmentRef DepthStencilAttachment;
        Array<UInt32> PreserveAttachments;
    };

    struct RenderPassSubpassDependencyDesc
    {
        UInt32 SourceSubpass = gExternalSubpassIndex;
        UInt32 DestinationSubpass = gExternalSubpassIndex;
        PipelineStageFlag SourceStageMask = PipelineStageFlag::None;
        PipelineStageFlag DestinationStageMask = PipelineStageFlag::None;
        AccessFlag SourceAccessMask = AccessFlag::None;
        AccessFlag DestinationAccessMask = AccessFlag::None;
    };

    struct RenderPassDesc
    {
        Array<RenderPassAttachmentDesc> Attachments;
        Array<RenderPassSubpassDesc> Subpasses;
        Array<RenderPassSubpassDependencyDesc> Dependencies;
    };

    struct GPUBuffer : GPUResourceWithDesc<BufferDesc>
    {
        using GPUResourceWithDesc<BufferDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        UIntPtr Allocation = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUTexture : GPUResourceWithDesc<TextureDesc>
    {
        using GPUResourceWithDesc<TextureDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        UIntPtr Allocation = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUTextureView : GPUResourceWithDesc<TextureViewDesc>
    {
        using GPUResourceWithDesc<TextureViewDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUSampler : GPUResourceWithDesc<SamplerDesc>
    {
        using GPUResourceWithDesc<SamplerDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUShader : GPUResourceWithDesc<ShaderDesc>
    {
        using GPUResourceWithDesc<ShaderDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUDescriptorSetLayout : GPUResourceWithDesc<DescriptorSetLayoutDesc>
    {
        using GPUResourceWithDesc<DescriptorSetLayoutDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUPipelineLayout : GPUResourceWithDesc<PipelineLayoutDesc>
    {
        using GPUResourceWithDesc<PipelineLayoutDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUGraphicsPipeline : GPUResourceWithDesc<GraphicsPipelineDesc>
    {
        using GPUResourceWithDesc<GraphicsPipelineDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUComputePipeline : GPUResourceWithDesc<ComputePipelineDesc>
    {
        using GPUResourceWithDesc<ComputePipelineDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPURenderPass : GPUResourceWithDesc<RenderPassDesc>
    {
        using GPUResourceWithDesc<RenderPassDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    [[nodiscard]] bool ValidateBufferDesc(const BufferDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateTextureDesc(const TextureDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateTextureViewDesc(const TextureViewDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateSamplerDesc(const SamplerDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateShaderDesc(const ShaderDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateDescriptorSetLayoutDesc(const DescriptorSetLayoutDesc& Desc,
                                                       String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidatePipelineLayoutDesc(const PipelineLayoutDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateRenderPassDesc(const RenderPassDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateGraphicsPipelineDesc(const GraphicsPipelineDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateComputePipelineDesc(const ComputePipelineDesc& Desc, String* ErrorMessage = nullptr);

} // namespace RHI
