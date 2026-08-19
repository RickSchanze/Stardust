#pragma once

#include <cstddef>
#include <cstring>
#include <type_traits>

#include "Core/Container/Array.h"
#include "Core/Container/FixedArray.h"
#include "Core/Container/Span.h"
#include "Core/Debug/Debug.h"
#include "Core/Math/Color.h"
#include "Core/Math/Rect.h"
#include "Core/Number.h"
#include "Core/Ptr.h"
#include "Core/String/String.h"
#include "RHI/Enums.h"
#include "RHI/Resources.h"

namespace RHI
{

    inline constexpr std::size_t gMaxColorAttachments = 8;
    inline constexpr std::size_t gMaxVertexBufferBindings = 8;
    inline constexpr std::size_t gMaxDescriptorSetsPerBind = 8;
    inline constexpr std::size_t gMaxViewports = 16;
    inline constexpr std::size_t gMaxBarriersPerCommand = 8;
    inline constexpr std::size_t gMaxSecondaryCommandBuffers = 8;
    inline constexpr std::size_t gMaxPushConstantSize = 128;
    inline constexpr std::size_t gCommandAlignment = 8;

    struct Viewport
    {
        Float X = 0.0f;
        Float Y = 0.0f;
        Float Width = 0.0f;
        Float Height = 0.0f;
        Float MinDepth = 0.0f;
        Float MaxDepth = 1.0f;
    };

    struct Offset3D
    {
        Int32 X = 0;
        Int32 Y = 0;
        Int32 Z = 0;
    };

    struct Extent3D
    {
        UInt32 Width = 1;
        UInt32 Height = 1;
        UInt32 Depth = 1;
    };

    struct TextureSubresourceLayers
    {
        TextureAspectFlag Aspect = TextureAspectFlag::Color;
        UInt32 MipLevel = 0;
        UInt32 BaseArrayLayer = 0;
        UInt32 LayerCount = 1;
    };

    struct TextureSubresourceRange
    {
        TextureAspectFlag Aspect = TextureAspectFlag::Color;
        UInt32 BaseMipLevel = 0;
        UInt32 MipLevelCount = 1;
        UInt32 BaseArrayLayer = 0;
        UInt32 ArrayLayerCount = 1;
    };

    struct ClearValue
    {
        Colorf Color{};
        Float Depth = 1.0f;
        UInt32 Stencil = 0;
    };

    struct BufferBarrier
    {
        BufferHandle Buffer;
        AccessFlag SourceAccess = AccessFlag::None;
        AccessFlag DestinationAccess = AccessFlag::None;
        UInt64 Offset = 0;
        UInt64 Size = UInt64::MaxValue;
    };

    struct TextureBarrier
    {
        TextureHandle Texture;
        AccessFlag SourceAccess = AccessFlag::None;
        AccessFlag DestinationAccess = AccessFlag::None;
        TextureLayout OldLayout = TextureLayout::Undefined;
        TextureLayout NewLayout = TextureLayout::Undefined;
        TextureSubresourceRange Range{};
    };

    struct CommandPoolDesc
    {
        QueueFlag Queue = QueueFlag::Graphics;
        CommandPoolCreateFlag CreateFlags = CommandPoolCreateFlag::ResetCommandBuffer;
    };

    struct CommandBufferDesc
    {
        CommandPoolHandle Pool;
        CommandBufferType Type = CommandBufferType::Primary;
    };

    struct GPUCommandPool : GPUResourceWithDesc<CommandPoolDesc>
    {
        using GPUResourceWithDesc<CommandPoolDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    struct GPUCommandBuffer : GPUResourceWithDesc<CommandBufferDesc>
    {
        using GPUResourceWithDesc<CommandBufferDesc>::GPUResourceWithDesc;

        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};
    };

    enum class CommandType : std::uint16_t
    {
        BeginRenderPass,
        NextSubpass,
        EndRenderPass,
        BindGraphicsPipeline,
        BindComputePipeline,
        BindVertexBuffers,
        BindIndexBuffer,
        BindDescriptorSets,
        PushConstants,
        SetViewport,
        SetScissor,
        SetBlendConstants,
        SetDepthBias,
        SetStencilReference,
        Draw,
        DrawIndexed,
        DrawIndirect,
        DrawIndexedIndirect,
        Dispatch,
        DispatchIndirect,
        CopyBuffer,
        CopyTexture,
        CopyBufferToTexture,
        CopyTextureToBuffer,
        BlitTexture,
        ClearColorTexture,
        ClearDepthStencilTexture,
        FillBuffer,
        UpdateBuffer,
        PipelineBarrier,
        ExecuteCommands,
        BeginDebugLabel,
        EndDebugLabel,
        InsertDebugLabel,
        Count,
    };

    struct Command
    {
        CommandType Type = CommandType::Count;
        UInt16 Size = 0;
    };

    struct BeginRenderPassCommand
    {
        static constexpr auto TypeValue = CommandType::BeginRenderPass;
        Command Header{.Type = TypeValue};
        RenderPassHandle RenderPass;
        Recti RenderArea{};
        UInt32 Width = 0;
        UInt32 Height = 0;
        UInt32 Layers = 1;
        UInt32 AttachmentCount = 0;
        SubpassContents Contents = SubpassContents::Inline;
        FixedArray<TextureViewHandle, gMaxColorAttachments> Attachments{};
        FixedArray<ClearValue, gMaxColorAttachments> ClearValues{};
    };

    struct NextSubpassCommand
    {
        static constexpr auto TypeValue = CommandType::NextSubpass;
        Command Header{.Type = TypeValue};
        SubpassContents Contents = SubpassContents::Inline;
    };

    struct EndRenderPassCommand
    {
        static constexpr auto TypeValue = CommandType::EndRenderPass;
        Command Header{.Type = TypeValue};
    };

    struct BindGraphicsPipelineCommand
    {
        static constexpr auto TypeValue = CommandType::BindGraphicsPipeline;
        Command Header{.Type = TypeValue};
        GraphicsPipelineHandle Pipeline;
    };

    struct BindComputePipelineCommand
    {
        static constexpr auto TypeValue = CommandType::BindComputePipeline;
        Command Header{.Type = TypeValue};
        ComputePipelineHandle Pipeline;
    };

    struct BindVertexBuffersCommand
    {
        static constexpr auto TypeValue = CommandType::BindVertexBuffers;
        Command Header{.Type = TypeValue};
        UInt32 FirstBinding = 0;
        UInt32 BindingCount = 0;
        FixedArray<BufferHandle, gMaxVertexBufferBindings> Buffers{};
        FixedArray<UInt64, gMaxVertexBufferBindings> Offsets{};
    };

    struct BindIndexBufferCommand
    {
        static constexpr auto TypeValue = CommandType::BindIndexBuffer;
        Command Header{.Type = TypeValue};
        BufferHandle Buffer;
        UInt64 Offset = 0;
        IndexFormat Format = IndexFormat::Uint32;
    };

    struct BindDescriptorSetsCommand
    {
        static constexpr auto TypeValue = CommandType::BindDescriptorSets;
        Command Header{.Type = TypeValue};
        PipelineType BindPoint = PipelineType::Graphics;
        PipelineLayoutHandle Layout;
        UInt32 FirstSet = 0;
        UInt32 SetCount = 0;
        FixedArray<DescriptorSetHandle, gMaxDescriptorSetsPerBind> Sets{};
        UInt32 DynamicOffsetCount = 0;
        FixedArray<UInt32, gMaxDescriptorSetsPerBind> DynamicOffsets{};
    };

    struct PushConstantsCommand
    {
        static constexpr auto TypeValue = CommandType::PushConstants;
        Command Header{.Type = TypeValue};
        PipelineLayoutHandle Layout;
        ShaderStageFlag Stages = ShaderStageFlag::None;
        UInt32 Offset = 0;
        UInt32 Size = 0;
        FixedArray<std::uint8_t, gMaxPushConstantSize> Data{};
    };

    struct SetViewportCommand
    {
        static constexpr auto TypeValue = CommandType::SetViewport;
        Command Header{.Type = TypeValue};
        UInt32 FirstViewport = 0;
        UInt32 ViewportCount = 0;
        FixedArray<Viewport, gMaxViewports> Viewports{};
    };

    struct SetScissorCommand
    {
        static constexpr auto TypeValue = CommandType::SetScissor;
        Command Header{.Type = TypeValue};
        UInt32 FirstScissor = 0;
        UInt32 ScissorCount = 0;
        FixedArray<Recti, gMaxViewports> Scissors{};
    };

    struct SetBlendConstantsCommand
    {
        static constexpr auto TypeValue = CommandType::SetBlendConstants;
        Command Header{.Type = TypeValue};
        Colorf Constants{};
    };

    struct SetDepthBiasCommand
    {
        static constexpr auto TypeValue = CommandType::SetDepthBias;
        Command Header{.Type = TypeValue};
        Float ConstantFactor = 0.0f;
        Float Clamp = 0.0f;
        Float SlopeFactor = 0.0f;
    };

    struct SetStencilReferenceCommand
    {
        static constexpr auto TypeValue = CommandType::SetStencilReference;
        Command Header{.Type = TypeValue};
        StencilFace Face = StencilFace::FrontAndBack;
        UInt32 Reference = 0;
    };

    struct DrawCommand
    {
        static constexpr auto TypeValue = CommandType::Draw;
        Command Header{.Type = TypeValue};
        UInt32 VertexCount = 0;
        UInt32 InstanceCount = 1;
        UInt32 FirstVertex = 0;
        UInt32 FirstInstance = 0;
    };

    struct DrawIndexedCommand
    {
        static constexpr auto TypeValue = CommandType::DrawIndexed;
        Command Header{.Type = TypeValue};
        UInt32 IndexCount = 0;
        UInt32 InstanceCount = 1;
        UInt32 FirstIndex = 0;
        Int32 VertexOffset = 0;
        UInt32 FirstInstance = 0;
    };

    struct DrawIndirectCommand
    {
        static constexpr auto TypeValue = CommandType::DrawIndirect;
        Command Header{.Type = TypeValue};
        BufferHandle Buffer;
        UInt64 Offset = 0;
        UInt32 DrawCount = 1;
        UInt32 Stride = 0;
    };

    struct DrawIndexedIndirectCommand
    {
        static constexpr auto TypeValue = CommandType::DrawIndexedIndirect;
        Command Header{.Type = TypeValue};
        BufferHandle Buffer;
        UInt64 Offset = 0;
        UInt32 DrawCount = 1;
        UInt32 Stride = 0;
    };

    struct DispatchCommand
    {
        static constexpr auto TypeValue = CommandType::Dispatch;
        Command Header{.Type = TypeValue};
        UInt32 GroupCountX = 1;
        UInt32 GroupCountY = 1;
        UInt32 GroupCountZ = 1;
    };

    struct DispatchIndirectCommand
    {
        static constexpr auto TypeValue = CommandType::DispatchIndirect;
        Command Header{.Type = TypeValue};
        BufferHandle Buffer;
        UInt64 Offset = 0;
    };

    struct CopyBufferCommand
    {
        static constexpr auto TypeValue = CommandType::CopyBuffer;
        Command Header{.Type = TypeValue};
        BufferHandle Source;
        BufferHandle Destination;
        UInt64 SourceOffset = 0;
        UInt64 DestinationOffset = 0;
        UInt64 Size = 0;
    };

    struct CopyTextureCommand
    {
        static constexpr auto TypeValue = CommandType::CopyTexture;
        Command Header{.Type = TypeValue};
        TextureHandle Source;
        TextureHandle Destination;
        TextureLayout SourceLayout = TextureLayout::TransferSource;
        TextureLayout DestinationLayout = TextureLayout::TransferDestination;
        TextureSubresourceLayers SourceSubresource{};
        TextureSubresourceLayers DestinationSubresource{};
        Offset3D SourceOffset{};
        Offset3D DestinationOffset{};
        Extent3D Extent{};
    };

    struct CopyBufferToTextureCommand
    {
        static constexpr auto TypeValue = CommandType::CopyBufferToTexture;
        Command Header{.Type = TypeValue};
        BufferHandle Source;
        TextureHandle Destination;
        TextureLayout DestinationLayout = TextureLayout::TransferDestination;
        UInt64 BufferOffset = 0;
        UInt32 BufferRowLength = 0;
        UInt32 BufferImageHeight = 0;
        TextureSubresourceLayers DestinationSubresource{};
        Offset3D TextureOffset{};
        Extent3D TextureExtent{};
    };

    struct CopyTextureToBufferCommand
    {
        static constexpr auto TypeValue = CommandType::CopyTextureToBuffer;
        Command Header{.Type = TypeValue};
        TextureHandle Source;
        BufferHandle Destination;
        TextureLayout SourceLayout = TextureLayout::TransferSource;
        UInt64 BufferOffset = 0;
        UInt32 BufferRowLength = 0;
        UInt32 BufferImageHeight = 0;
        TextureSubresourceLayers SourceSubresource{};
        Offset3D TextureOffset{};
        Extent3D TextureExtent{};
    };

    struct BlitTextureCommand
    {
        static constexpr auto TypeValue = CommandType::BlitTexture;
        Command Header{.Type = TypeValue};
        TextureHandle Source;
        TextureHandle Destination;
        TextureLayout SourceLayout = TextureLayout::TransferSource;
        TextureLayout DestinationLayout = TextureLayout::TransferDestination;
        TextureSubresourceLayers SourceSubresource{};
        TextureSubresourceLayers DestinationSubresource{};
        Offset3D SourceOffset0{};
        Offset3D SourceOffset1{};
        Offset3D DestinationOffset0{};
        Offset3D DestinationOffset1{};
        Filter BlitFilter = Filter::Linear;
    };

    struct ClearColorTextureCommand
    {
        static constexpr auto TypeValue = CommandType::ClearColorTexture;
        Command Header{.Type = TypeValue};
        TextureHandle Texture;
        TextureLayout Layout = TextureLayout::TransferDestination;
        Colorf Color{};
        TextureSubresourceRange Range{};
    };

    struct ClearDepthStencilTextureCommand
    {
        static constexpr auto TypeValue = CommandType::ClearDepthStencilTexture;
        Command Header{.Type = TypeValue};
        TextureHandle Texture;
        TextureLayout Layout = TextureLayout::TransferDestination;
        Float Depth = 1.0f;
        UInt32 Stencil = 0;
        TextureSubresourceRange Range{};
    };

    struct FillBufferCommand
    {
        static constexpr auto TypeValue = CommandType::FillBuffer;
        Command Header{.Type = TypeValue};
        BufferHandle Buffer;
        UInt64 Offset = 0;
        UInt64 Size = 0;
        UInt32 Data = 0;
    };

    struct UpdateBufferCommand
    {
        static constexpr auto TypeValue = CommandType::UpdateBuffer;
        Command Header{.Type = TypeValue};
        BufferHandle Buffer;
        UInt64 Offset = 0;
        UInt64 Size = 0;
        UIntPtr HostData = UIntPtr::Null();
    };

    struct PipelineBarrierCommand
    {
        static constexpr auto TypeValue = CommandType::PipelineBarrier;
        Command Header{.Type = TypeValue};
        PipelineStageFlag SourceStage = PipelineStageFlag::None;
        PipelineStageFlag DestinationStage = PipelineStageFlag::None;
        DependencyFlag DependencyFlags = DependencyFlag::None;
        UInt32 BufferBarrierCount = 0;
        UInt32 TextureBarrierCount = 0;
        FixedArray<BufferBarrier, gMaxBarriersPerCommand> BufferBarriers{};
        FixedArray<TextureBarrier, gMaxBarriersPerCommand> TextureBarriers{};
    };

    struct ExecuteCommandsCommand
    {
        static constexpr auto TypeValue = CommandType::ExecuteCommands;
        Command Header{.Type = TypeValue};
        UInt32 Count = 0;
        FixedArray<CommandBufferHandle, gMaxSecondaryCommandBuffers> CommandBuffers{};
    };

    struct BeginDebugLabelCommand
    {
        static constexpr auto TypeValue = CommandType::BeginDebugLabel;
        Command Header{.Type = TypeValue};
        char Name[64]{};
        Colorf Color{};
    };

    struct EndDebugLabelCommand
    {
        static constexpr auto TypeValue = CommandType::EndDebugLabel;
        Command Header{.Type = TypeValue};
    };

    struct InsertDebugLabelCommand
    {
        static constexpr auto TypeValue = CommandType::InsertDebugLabel;
        Command Header{.Type = TypeValue};
        char Name[64]{};
        Colorf Color{};
    };

    template <typename TCommand>
    concept IsCommand = std::is_standard_layout_v<TCommand> && requires(TCommand Command) {
        { TCommand::TypeValue } -> std::convertible_to<CommandType>;
        Command.Header;
    };

    template <IsCommand TCommand>
    [[nodiscard]] const TCommand& CastCommand(const Command& Header)
    {
        Assert(Header.Type == TCommand::TypeValue);
        return *reinterpret_cast<const TCommand*>(&Header);
    }

    template <IsCommand TCommand>
    [[nodiscard]] TCommand& CastCommand(Command& Header)
    {
        Assert(Header.Type == TCommand::TypeValue);
        return *reinterpret_cast<TCommand*>(&Header);
    }

    [[nodiscard]] constexpr std::size_t AlignCommandOffset(const std::size_t Offset) noexcept
    {
        return (Offset + gCommandAlignment - 1) & ~(gCommandAlignment - 1);
    }

    class CommandBuffer
    {
    public:
        CommandBufferDesc Desc{};
        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};

        void Begin(CommandBufferUsageFlag Usage = CommandBufferUsageFlag::None);
        void End();
        void Reset(CommandBufferResetFlag Flags = CommandBufferResetFlag::None);

        template <IsCommand TCommand>
        void Record(TCommand Command)
        {
            Assert(mIsRecording);
            Command.Header.Type = TCommand::TypeValue;
            Command.Header.Size = UInt16{static_cast<std::uint16_t>(sizeof(TCommand))};

            const auto Offset = AlignCommandOffset(mCommands.Num());
            mCommands.Resize(Offset + sizeof(TCommand));
            std::memcpy(mCommands.Data() + Offset, &Command, sizeof(TCommand));
        }

        void BeginRenderPass(const BeginRenderPassCommand& Command)
        {
            Record(Command);
        }

        void NextSubpass(const NextSubpassCommand& Command = NextSubpassCommand{
                             .Header = {.Type = NextSubpassCommand::TypeValue},
                         })
        {
            Record(Command);
        }

        void EndRenderPass()
        {
            Record(EndRenderPassCommand{.Header = {.Type = EndRenderPassCommand::TypeValue}});
        }

        void BindGraphicsPipeline(const GraphicsPipelineHandle Pipeline)
        {
            Record(BindGraphicsPipelineCommand{
                .Header = {.Type = BindGraphicsPipelineCommand::TypeValue},
                .Pipeline = Pipeline,
            });
        }

        void BindComputePipeline(const ComputePipelineHandle Pipeline)
        {
            Record(BindComputePipelineCommand{
                .Header = {.Type = BindComputePipelineCommand::TypeValue},
                .Pipeline = Pipeline,
            });
        }

        void BindVertexBuffers(const BindVertexBuffersCommand& Command)
        {
            Record(Command);
        }

        void BindIndexBuffer(const BufferHandle Buffer, const UInt64 Offset = 0, const IndexFormat Format = IndexFormat::Uint32)
        {
            Record(BindIndexBufferCommand{
                .Header = {.Type = BindIndexBufferCommand::TypeValue},
                .Buffer = Buffer,
                .Offset = Offset,
                .Format = Format,
            });
        }

        void BindDescriptorSets(const BindDescriptorSetsCommand& Command)
        {
            Record(Command);
        }

        void PushConstants(const PushConstantsCommand& Command)
        {
            Record(Command);
        }

        void SetViewport(const Viewport& ViewportValue, const UInt32 FirstViewport = 0)
        {
            SetViewportCommand Command{
                .Header = {.Type = SetViewportCommand::TypeValue},
                .FirstViewport = FirstViewport,
                .ViewportCount = 1,
            };
            Command.Viewports[0] = ViewportValue;
            Record(Command);
        }

        void SetViewport(const SetViewportCommand& Command)
        {
            Record(Command);
        }

        void SetScissor(const Recti& Scissor, const UInt32 FirstScissor = 0)
        {
            SetScissorCommand Command{
                .Header = {.Type = SetScissorCommand::TypeValue},
                .FirstScissor = FirstScissor,
                .ScissorCount = 1,
            };
            Command.Scissors[0] = Scissor;
            Record(Command);
        }

        void SetScissor(const SetScissorCommand& Command)
        {
            Record(Command);
        }

        void SetBlendConstants(const Colorf& Constants)
        {
            Record(SetBlendConstantsCommand{
                .Header = {.Type = SetBlendConstantsCommand::TypeValue},
                .Constants = Constants,
            });
        }

        void SetDepthBias(const Float ConstantFactor, const Float Clamp, const Float SlopeFactor)
        {
            Record(SetDepthBiasCommand{
                .Header = {.Type = SetDepthBiasCommand::TypeValue},
                .ConstantFactor = ConstantFactor,
                .Clamp = Clamp,
                .SlopeFactor = SlopeFactor,
            });
        }

        void SetStencilReference(const StencilFace Face, const UInt32 Reference)
        {
            Record(SetStencilReferenceCommand{
                .Header = {.Type = SetStencilReferenceCommand::TypeValue},
                .Face = Face,
                .Reference = Reference,
            });
        }

        void Draw(const UInt32 VertexCount, const UInt32 InstanceCount = 1, const UInt32 FirstVertex = 0, const UInt32 FirstInstance = 0)
        {
            Record(DrawCommand{
                .Header = {.Type = DrawCommand::TypeValue},
                .VertexCount = VertexCount,
                .InstanceCount = InstanceCount,
                .FirstVertex = FirstVertex,
                .FirstInstance = FirstInstance,
            });
        }

        void DrawIndexed(const UInt32 IndexCount,
                         const UInt32 InstanceCount = 1,
                         const UInt32 FirstIndex = 0,
                         const Int32 VertexOffset = 0,
                         const UInt32 FirstInstance = 0)
        {
            Record(DrawIndexedCommand{
                .Header = {.Type = DrawIndexedCommand::TypeValue},
                .IndexCount = IndexCount,
                .InstanceCount = InstanceCount,
                .FirstIndex = FirstIndex,
                .VertexOffset = VertexOffset,
                .FirstInstance = FirstInstance,
            });
        }

        void DrawIndirect(const DrawIndirectCommand& Command)
        {
            Record(Command);
        }

        void DrawIndexedIndirect(const DrawIndexedIndirectCommand& Command)
        {
            Record(Command);
        }

        void Dispatch(const UInt32 GroupCountX, const UInt32 GroupCountY = 1, const UInt32 GroupCountZ = 1)
        {
            Record(DispatchCommand{
                .Header = {.Type = DispatchCommand::TypeValue},
                .GroupCountX = GroupCountX,
                .GroupCountY = GroupCountY,
                .GroupCountZ = GroupCountZ,
            });
        }

        void DispatchIndirect(const BufferHandle Buffer, const UInt64 Offset = 0)
        {
            Record(DispatchIndirectCommand{
                .Header = {.Type = DispatchIndirectCommand::TypeValue},
                .Buffer = Buffer,
                .Offset = Offset,
            });
        }

        void CopyBuffer(const CopyBufferCommand& Command)
        {
            Record(Command);
        }

        void CopyTexture(const CopyTextureCommand& Command)
        {
            Record(Command);
        }

        void CopyBufferToTexture(const CopyBufferToTextureCommand& Command)
        {
            Record(Command);
        }

        void CopyTextureToBuffer(const CopyTextureToBufferCommand& Command)
        {
            Record(Command);
        }

        void BlitTexture(const BlitTextureCommand& Command)
        {
            Record(Command);
        }

        void ClearColorTexture(const ClearColorTextureCommand& Command)
        {
            Record(Command);
        }

        void ClearDepthStencilTexture(const ClearDepthStencilTextureCommand& Command)
        {
            Record(Command);
        }

        void FillBuffer(const FillBufferCommand& Command)
        {
            Record(Command);
        }

        void UpdateBuffer(const UpdateBufferCommand& Command)
        {
            Record(Command);
        }

        void PipelineBarrier(const PipelineBarrierCommand& Command)
        {
            Record(Command);
        }

        void ExecuteCommands(const ExecuteCommandsCommand& Command)
        {
            Record(Command);
        }

        void BeginDebugLabel(const char* Name, const Colorf& Color = {})
        {
            BeginDebugLabelCommand Command{
                .Header = {.Type = BeginDebugLabelCommand::TypeValue},
                .Color = Color,
            };
            SetDebugLabelName(Command.Name, Name);
            Record(Command);
        }

        void EndDebugLabel()
        {
            Record(EndDebugLabelCommand{.Header = {.Type = EndDebugLabelCommand::TypeValue}});
        }

        void InsertDebugLabel(const char* Name, const Colorf& Color = {})
        {
            InsertDebugLabelCommand Command{
                .Header = {.Type = InsertDebugLabelCommand::TypeValue},
                .Color = Color,
            };
            SetDebugLabelName(Command.Name, Name);
            Record(Command);
        }

        [[nodiscard]] bool IsRecording() const noexcept
        {
            return mIsRecording;
        }

        [[nodiscard]] CommandBufferUsageFlag GetUsage() const noexcept
        {
            return mUsage;
        }

        [[nodiscard]] Span<const std::uint8_t> GetStream() const noexcept
        {
            return mCommands.AsSpan();
        }

        [[nodiscard]] const Command* First() const noexcept;
        [[nodiscard]] const Command* Next(const Command* Current) const noexcept;

        template <IsCommand TCommand>
        [[nodiscard]] static const TCommand& Cast(const Command& Header)
        {
            return CastCommand<TCommand>(Header);
        }

    private:
        static void SetDebugLabelName(char (&Destination)[64], const char* Name);

        Array<std::uint8_t> mCommands;
        CommandBufferUsageFlag mUsage = CommandBufferUsageFlag::None;
        bool mIsRecording = false;
    };

    class CommandPool
    {
    public:
        CommandPoolHandle Handle;
        CommandPoolDesc Desc{};
        UIntPtr Native = UIntPtr::Null();
        char DebugName[64]{};

        [[nodiscard]] CommandBuffer Allocate(CommandBufferType Type = CommandBufferType::Primary);
        void Reset(CommandPoolResetFlag Flags = CommandPoolResetFlag::None);
    };

    [[nodiscard]] bool ValidateCommandPoolDesc(const CommandPoolDesc& Desc, String* ErrorMessage = nullptr);
    [[nodiscard]] bool ValidateCommandBufferDesc(const CommandBufferDesc& Desc, String* ErrorMessage = nullptr);

    static_assert(std::is_trivially_copyable_v<Command>);
    static_assert(std::is_trivially_copyable_v<DrawCommand>);
    static_assert(std::is_trivially_copyable_v<DrawIndexedCommand>);
    static_assert(std::is_trivially_copyable_v<DispatchCommand>);
    static_assert(std::is_trivially_copyable_v<CopyBufferCommand>);
    static_assert(std::is_trivially_copyable_v<PipelineBarrierCommand>);
    static_assert(std::is_standard_layout_v<Command>);
    static_assert(std::is_standard_layout_v<DrawCommand>);
    static_assert(offsetof(DrawCommand, Header) == 0);

} // namespace RHI
