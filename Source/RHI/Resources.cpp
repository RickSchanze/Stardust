#include "Resources.h"

#include "Core/Ptr.h"

namespace RHI
{
    namespace
    {
        void SetValidationError(String* ErrorMessage, const char* Message)
        {
            if (ErrorMessage != nullptr)
            {
                *ErrorMessage = String{Message};
            }
        }

        void CopyDebugName(char (&Destination)[64], const char* Name)
        {
            Destination[0] = '\0';
            if (Name == nullptr)
            {
                return;
            }

            std::size_t Index = 0;
            while (Index + 1 < 64 && Name[Index] != '\0')
            {
                Destination[Index] = Name[Index];
                ++Index;
            }
            Destination[Index] = '\0';
        }

        [[nodiscard]] bool IsAttachmentReferenceValid(const RenderPassAttachmentRef& Reference,
                                                      const UInt32 AttachmentCount)
        {
            return Reference.Attachment == gUnusedAttachmentIndex || Reference.Attachment < AttachmentCount;
        }
    } // namespace

    void AssignResourceDebugName(char (&Destination)[64], const char* Name, const void* FallbackPointer)
    {
        if (Name != nullptr && Name[0] != '\0')
        {
            CopyDebugName(Destination, Name);
            return;
        }

        if (FallbackPointer != nullptr)
        {
            const String Formatted = UIntPtr::FromPtr(FallbackPointer).ToString();
            CopyDebugName(Destination, Formatted.CStr());
            return;
        }

        Destination[0] = '\0';
    }

    bool ValidateBufferDesc(const BufferDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Size == 0)
        {
            SetValidationError(ErrorMessage, "BufferDesc.Size must be greater than zero.");
            return false;
        }

        if (Desc.Usage == BufferUsageFlag::None)
        {
            SetValidationError(ErrorMessage, "BufferDesc.Usage must declare at least one usage flag.");
            return false;
        }

        if (Desc.MemoryProperties == MemoryPropertyFlag::None)
        {
            SetValidationError(ErrorMessage, "BufferDesc.MemoryProperties must declare at least one memory property.");
            return false;
        }

        return true;
    }

    bool ValidateTextureDesc(const TextureDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Format == PixelFormat::Undefined)
        {
            SetValidationError(ErrorMessage, "TextureDesc.Format must be specified.");
            return false;
        }

        if (Desc.Width == 0 || Desc.Height == 0 || Desc.Depth == 0)
        {
            SetValidationError(ErrorMessage, "TextureDesc dimensions must be greater than zero.");
            return false;
        }

        if (Desc.MipLevels == 0 || Desc.ArrayLayers == 0)
        {
            SetValidationError(ErrorMessage, "TextureDesc mip levels and array layers must be greater than zero.");
            return false;
        }

        if (Desc.Usage == TextureUsageFlag::None)
        {
            SetValidationError(ErrorMessage, "TextureDesc.Usage must declare at least one usage flag.");
            return false;
        }

        if (Desc.Dimension == TextureDimension::Dim1D && (Desc.Height != 1 || Desc.Depth != 1))
        {
            SetValidationError(ErrorMessage, "1D textures must use Height = 1 and Depth = 1.");
            return false;
        }

        if (Desc.Dimension == TextureDimension::Dim2D && Desc.Depth != 1)
        {
            SetValidationError(ErrorMessage, "2D textures must use Depth = 1.");
            return false;
        }

        if (Desc.Dimension == TextureDimension::Dim3D && Desc.ArrayLayers != 1)
        {
            SetValidationError(ErrorMessage, "3D textures must use ArrayLayers = 1.");
            return false;
        }

        if (Desc.Samples != SampleCount::Count1)
        {
            if (Desc.Dimension != TextureDimension::Dim2D || Desc.Depth != 1)
            {
                SetValidationError(ErrorMessage, "Multisampled textures must be 2D.");
                return false;
            }

            if (Desc.MipLevels != 1)
            {
                SetValidationError(ErrorMessage, "Multisampled textures must use exactly one mip level.");
                return false;
            }
        }

        if ((Desc.CreateFlags & TextureCreateFlag::CubeCompatible) == TextureCreateFlag::CubeCompatible)
        {
            if (Desc.Dimension != TextureDimension::Dim2D)
            {
                SetValidationError(ErrorMessage, "Cube-compatible textures must be 2D.");
                return false;
            }

            if ((Desc.ArrayLayers % 6) != 0)
            {
                SetValidationError(ErrorMessage,
                                   "Cube-compatible textures must use an array layer count divisible by 6.");
                return false;
            }
        }

        return true;
    }

    bool ValidateTextureViewDesc(const TextureViewDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Texture.IsNull())
        {
            SetValidationError(ErrorMessage, "TextureViewDesc.Texture must reference a valid texture handle.");
            return false;
        }

        if (Desc.Format == PixelFormat::Undefined)
        {
            SetValidationError(ErrorMessage, "TextureViewDesc.Format must be specified.");
            return false;
        }

        if (Desc.Aspect == TextureAspectFlag::None)
        {
            SetValidationError(ErrorMessage, "TextureViewDesc.Aspect must declare at least one aspect.");
            return false;
        }

        if (Desc.MipLevelCount == 0 || Desc.ArrayLayerCount == 0)
        {
            SetValidationError(ErrorMessage, "TextureViewDesc mip and layer counts must be greater than zero.");
            return false;
        }

        return true;
    }

    bool ValidateSamplerDesc(const SamplerDesc& Desc, String* ErrorMessage)
    {
        if (Desc.MinLod > Desc.MaxLod)
        {
            SetValidationError(ErrorMessage, "SamplerDesc.MinLod must not be greater than MaxLod.");
            return false;
        }

        if (Desc.AnisotropyEnable && Desc.MaxAnisotropy < 1.0f)
        {
            SetValidationError(ErrorMessage,
                               "SamplerDesc.MaxAnisotropy must be at least 1 when anisotropy is enabled.");
            return false;
        }

        return true;
    }

    bool ValidateShaderDesc(const ShaderDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Stage == ShaderStageFlag::None)
        {
            SetValidationError(ErrorMessage, "ShaderDesc.Stage must specify at least one shader stage.");
            return false;
        }

        if (Desc.Code.Empty())
        {
            SetValidationError(ErrorMessage, "ShaderDesc.Code must not be empty.");
            return false;
        }

        if (Desc.EntryPoint[0] == '\0')
        {
            SetValidationError(ErrorMessage, "ShaderDesc.EntryPoint must not be empty.");
            return false;
        }

        return true;
    }

    bool ValidateDescriptorSetLayoutDesc(const DescriptorSetLayoutDesc& Desc, String* ErrorMessage)
    {
        for (std::size_t BindingIndex = 0; BindingIndex < Desc.Bindings.Num(); ++BindingIndex)
        {
            const DescriptorBindingDesc& Binding = Desc.Bindings[BindingIndex];
            if (Binding.DescriptorCount == 0)
            {
                SetValidationError(ErrorMessage,
                                   "DescriptorSetLayoutDesc bindings must use a descriptor count greater than zero.");
                return false;
            }

            if (Binding.Stages == ShaderStageFlag::None)
            {
                SetValidationError(ErrorMessage,
                                   "DescriptorSetLayoutDesc bindings must declare at least one shader stage.");
                return false;
            }

            if (!Binding.ImmutableSamplers.Empty())
            {
                if (Binding.Type != DescriptorType::Sampler && Binding.Type != DescriptorType::CombinedTextureSampler)
                {
                    SetValidationError(
                        ErrorMessage,
                        "Immutable samplers are only valid for Sampler or CombinedTextureSampler bindings.");
                    return false;
                }

                if (Binding.ImmutableSamplers.Num() != Binding.DescriptorCount)
                {
                    SetValidationError(ErrorMessage,
                                       "Immutable sampler count must match DescriptorBindingDesc.DescriptorCount.");
                    return false;
                }
            }

            for (std::size_t OtherBindingIndex = BindingIndex + 1; OtherBindingIndex < Desc.Bindings.Num();
                 ++OtherBindingIndex)
            {
                if (Binding.Binding == Desc.Bindings[OtherBindingIndex].Binding)
                {
                    SetValidationError(ErrorMessage,
                                       "DescriptorSetLayoutDesc bindings must use unique binding numbers.");
                    return false;
                }
            }
        }

        return true;
    }

    bool ValidatePipelineLayoutDesc(const PipelineLayoutDesc& Desc, String* ErrorMessage)
    {
        for (std::size_t RangeIndex = 0; RangeIndex < Desc.PushConstantRanges.Num(); ++RangeIndex)
        {
            const auto& [Stages, Offset, Size] = Desc.PushConstantRanges[RangeIndex];
            if (Stages == ShaderStageFlag::None)
            {
                SetValidationError(ErrorMessage, "Push constant ranges must declare at least one shader stage.");
                return false;
            }

            if (Size == 0)
            {
                SetValidationError(ErrorMessage, "Push constant ranges must use a size greater than zero.");
                return false;
            }

            if ((Offset % 4) != 0 || (Size % 4) != 0)
            {
                SetValidationError(ErrorMessage, "Push constant ranges must use 4-byte aligned offset and size.");
                return false;
            }
        }

        return true;
    }

    bool ValidateRenderPassDesc(const RenderPassDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Attachments.Empty())
        {
            SetValidationError(ErrorMessage, "RenderPassDesc.Attachments must not be empty.");
            return false;
        }

        if (Desc.Subpasses.Empty())
        {
            SetValidationError(ErrorMessage, "RenderPassDesc.Subpasses must not be empty.");
            return false;
        }

        const auto AttachmentCount = static_cast<UInt32>(Desc.Attachments.Num());
        const auto SubpassCount = static_cast<UInt32>(Desc.Subpasses.Num());

        for (const RenderPassAttachmentDesc& Attachment : Desc.Attachments)
        {
            if (Attachment.Format == PixelFormat::Undefined)
            {
                SetValidationError(ErrorMessage, "Render pass attachments must specify a format.");
                return false;
            }
        }

        for (std::size_t SubpassIndex = 0; SubpassIndex < Desc.Subpasses.Num(); ++SubpassIndex)
        {
            const RenderPassSubpassDesc& Subpass = Desc.Subpasses[SubpassIndex];
            if (Subpass.Pipeline != PipelineType::Graphics)
            {
                SetValidationError(ErrorMessage, "Only graphics subpasses are supported in the basic RenderPassDesc.");
                return false;
            }

            if (Subpass.ColorAttachments.Empty() && Subpass.DepthStencilAttachment.Attachment == gUnusedAttachmentIndex)
            {
                SetValidationError(ErrorMessage,
                                   "Each subpass must bind at least one color or depth-stencil attachment.");
                return false;
            }

            if (!Subpass.ResolveAttachments.Empty() &&
                Subpass.ResolveAttachments.Num() != Subpass.ColorAttachments.Num())
            {
                SetValidationError(ErrorMessage, "Resolve attachment count must match color attachment count.");
                return false;
            }

            for (const RenderPassAttachmentRef& Reference : Subpass.InputAttachments)
            {
                if (!IsAttachmentReferenceValid(Reference, AttachmentCount))
                {
                    SetValidationError(ErrorMessage, "RenderPassSubpassDesc input attachment index is out of range.");
                    return false;
                }
            }

            for (const RenderPassAttachmentRef& Reference : Subpass.ColorAttachments)
            {
                if (!IsAttachmentReferenceValid(Reference, AttachmentCount))
                {
                    SetValidationError(ErrorMessage, "RenderPassSubpassDesc color attachment index is out of range.");
                    return false;
                }
            }

            for (const RenderPassAttachmentRef& Reference : Subpass.ResolveAttachments)
            {
                if (!IsAttachmentReferenceValid(Reference, AttachmentCount))
                {
                    SetValidationError(ErrorMessage, "RenderPassSubpassDesc resolve attachment index is out of range.");
                    return false;
                }
            }

            if (!IsAttachmentReferenceValid(Subpass.DepthStencilAttachment, AttachmentCount))
            {
                SetValidationError(ErrorMessage,
                                   "RenderPassSubpassDesc depth-stencil attachment index is out of range.");
                return false;
            }

            for (const auto PreserveAttachment : Subpass.PreserveAttachments)
            {
                if (PreserveAttachment >= AttachmentCount)
                {
                    SetValidationError(ErrorMessage,
                                       "RenderPassSubpassDesc preserve attachment index is out of range.");
                    return false;
                }
            }
        }

        for (const RenderPassSubpassDependencyDesc& Dependency : Desc.Dependencies)
        {
            if (Dependency.SourceSubpass != gExternalSubpassIndex && Dependency.SourceSubpass >= SubpassCount)
            {
                SetValidationError(ErrorMessage, "Render pass dependency source subpass index is out of range.");
                return false;
            }

            if (Dependency.DestinationSubpass != gExternalSubpassIndex && Dependency.DestinationSubpass >= SubpassCount)
            {
                SetValidationError(ErrorMessage, "Render pass dependency destination subpass index is out of range.");
                return false;
            }
        }

        return true;
    }

    bool ValidateGraphicsPipelineDesc(const GraphicsPipelineDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Layout.IsNull())
        {
            SetValidationError(ErrorMessage, "GraphicsPipelineDesc.Layout must reference a valid pipeline layout.");
            return false;
        }

        if (Desc.VertexShader.IsNull())
        {
            SetValidationError(ErrorMessage, "GraphicsPipelineDesc.VertexShader must reference a valid shader handle.");
            return false;
        }

        if (Desc.FragmentShader.IsNull())
        {
            SetValidationError(ErrorMessage,
                               "GraphicsPipelineDesc.FragmentShader must reference a valid shader handle.");
            return false;
        }

        if (Desc.RenderPass.IsNull())
        {
            SetValidationError(ErrorMessage, "GraphicsPipelineDesc.RenderPass must reference a valid render pass.");
            return false;
        }

        if (Desc.ColorAttachments.Empty() && Desc.DepthStencilFormat == PixelFormat::Undefined)
        {
            SetValidationError(
                ErrorMessage,
                "GraphicsPipelineDesc must define at least one color attachment or a depth-stencil format.");
            return false;
        }

        return true;
    }

    bool ValidateComputePipelineDesc(const ComputePipelineDesc& Desc, String* ErrorMessage)
    {
        if (Desc.Layout.IsNull())
        {
            SetValidationError(ErrorMessage, "ComputePipelineDesc.Layout must reference a valid pipeline layout.");
            return false;
        }

        if (Desc.ComputeShader.IsNull())
        {
            SetValidationError(ErrorMessage, "ComputePipelineDesc.ComputeShader must reference a valid shader handle.");
            return false;
        }

        return true;
    }
} // namespace RHI

template class RHI::GPUResourcePool<RHI::GPUBuffer>;
template class RHI::GPUResourcePool<RHI::GPUTexture>;
template class RHI::GPUResourcePool<RHI::GPUTextureView>;
template class RHI::GPUResourcePool<RHI::GPUSampler>;
template class RHI::GPUResourcePool<RHI::GPUShader>;
template class RHI::GPUResourcePool<RHI::GPUDescriptorSetLayout>;
template class RHI::GPUResourcePool<RHI::GPUPipelineLayout>;
template class RHI::GPUResourcePool<RHI::GPUGraphicsPipeline>;
template class RHI::GPUResourcePool<RHI::GPUComputePipeline>;
template class RHI::GPUResourcePool<RHI::GPURenderPass>;
