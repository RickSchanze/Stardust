#pragma once

#include <cstdint>

#include "Core/Reflection/EnumFlag.h"

namespace RHI
{

    enum class PixelFormat
    {
        Undefined = 0,

        R4G4UnormPack8 = 1,
        R4G4B4A4UnormPack16 = 2,
        B4G4R4A4UnormPack16 = 3,
        R5G6B5UnormPack16 = 4,
        B5G6R5UnormPack16 = 5,
        R5G5B5A1UnormPack16 = 6,
        B5G5R5A1UnormPack16 = 7,
        A1R5G5B5UnormPack16 = 8,

        R8Unorm = 9,
        R8Snorm = 10,
        R8Uint = 13,
        R8Sint = 14,
        R8Srgb = 15,

        RG8Unorm = 16,
        RG8Snorm = 17,
        RG8Uint = 20,
        RG8Sint = 21,
        RG8Srgb = 22,

        RGB8Unorm = 23,
        RGB8Snorm = 24,
        RGB8Uint = 27,
        RGB8Sint = 28,
        RGB8Srgb = 29,

        BGR8Unorm = 30,
        BGR8Snorm = 31,
        BGR8Uint = 34,
        BGR8Sint = 35,
        BGR8Srgb = 36,

        RGBA8Unorm = 37,
        RGBA8Snorm = 38,
        RGBA8Uint = 41,
        RGBA8Sint = 42,
        RGBA8Srgb = 43,

        BGRA8Unorm = 44,
        BGRA8Snorm = 45,
        BGRA8Uint = 48,
        BGRA8Sint = 49,
        BGRA8Srgb = 50,

        ABGR8UnormPack32 = 51,
        ABGR8SnormPack32 = 52,
        ABGR8UintPack32 = 55,
        ABGR8SintPack32 = 56,
        ABGR8SrgbPack32 = 57,

        A2RGB10UnormPack32 = 58,
        A2RGB10UintPack32 = 62,
        A2BGR10UnormPack32 = 64,
        A2BGR10UintPack32 = 68,

        R16Unorm = 70,
        R16Snorm = 71,
        R16Uint = 74,
        R16Sint = 75,
        R16Float = 76,

        RG16Unorm = 77,
        RG16Snorm = 78,
        RG16Uint = 81,
        RG16Sint = 82,
        RG16Float = 83,

        RGB16Unorm = 84,
        RGB16Snorm = 85,
        RGB16Uint = 88,
        RGB16Sint = 89,
        RGB16Float = 90,

        RGBA16Unorm = 91,
        RGBA16Snorm = 92,
        RGBA16Uint = 95,
        RGBA16Sint = 96,
        RGBA16Float = 97,

        R32Uint = 98,
        R32Sint = 99,
        R32Float = 100,

        RG32Uint = 101,
        RG32Sint = 102,
        RG32Float = 103,

        RGB32Uint = 104,
        RGB32Sint = 105,
        RGB32Float = 106,

        RGBA32Uint = 107,
        RGBA32Sint = 108,
        RGBA32Float = 109,

        B10G11R11UfloatPack32 = 122,
        E5B9G9R9UfloatPack32 = 123,

        D16Unorm = 124,
        X8D24UnormPack32 = 125,
        D32Float = 126,
        S8Uint = 127,
        D16UnormS8Uint = 128,
        D24UnormS8Uint = 129,
        D32FloatS8Uint = 130,

        BC1RGBUnorm = 131,
        BC1RGBSrgb = 132,
        BC1RGBAUnorm = 133,
        BC1RGBASrgb = 134,
        BC2Unorm = 135,
        BC2Srgb = 136,
        BC3Unorm = 137,
        BC3Srgb = 138,
        BC4Unorm = 139,
        BC4Snorm = 140,
        BC5Unorm = 141,
        BC5Snorm = 142,
        BC6HUfloat = 143,
        BC6HSfloat = 144,
        BC7Unorm = 145,
        BC7Srgb = 146,

        ETC2RGB8Unorm = 147,
        ETC2RGB8Srgb = 148,
        ETC2RGB8A1Unorm = 149,
        ETC2RGB8A1Srgb = 150,
        ETC2RGBA8Unorm = 151,
        ETC2RGBA8Srgb = 152,
        EACR11Unorm = 153,
        EACR11Snorm = 154,
        EACRG11Unorm = 155,
        EACRG11Snorm = 156,

        ASTC4x4Unorm = 157,
        ASTC4x4Srgb = 158,
        ASTC5x4Unorm = 159,
        ASTC5x4Srgb = 160,
        ASTC5x5Unorm = 161,
        ASTC5x5Srgb = 162,
        ASTC6x5Unorm = 163,
        ASTC6x5Srgb = 164,
        ASTC6x6Unorm = 165,
        ASTC6x6Srgb = 166,
        ASTC8x5Unorm = 167,
        ASTC8x5Srgb = 168,
        ASTC8x6Unorm = 169,
        ASTC8x6Srgb = 170,
        ASTC8x8Unorm = 171,
        ASTC8x8Srgb = 172,
        ASTC10x5Unorm = 173,
        ASTC10x5Srgb = 174,
        ASTC10x6Unorm = 175,
        ASTC10x6Srgb = 176,
        ASTC10x8Unorm = 177,
        ASTC10x8Srgb = 178,
        ASTC10x10Unorm = 179,
        ASTC10x10Srgb = 180,
        ASTC12x10Unorm = 181,
        ASTC12x10Srgb = 182,
        ASTC12x12Unorm = 183,
        ASTC12x12Srgb = 184,
    };

    enum class ColorSpace
    {
        SrgbNonlinear = 0,
        DisplayP3Nonlinear = 1000104001,
        ExtendedSrgbLinear = 1000104002,
        DisplayP3Linear = 1000104003,
        DciP3Nonlinear = 1000104004,
        Bt709Linear = 1000104005,
        Bt709Nonlinear = 1000104006,
        Bt2020Linear = 1000104007,
        Hdr10ST2084 = 1000104008,
        DolbyVision = 1000104009,
        Hdr10HLG = 1000104010,
        AdobeRgbLinear = 1000104011,
        AdobeRgbNonlinear = 1000104012,
        PassThrough = 1000104013,
        ExtendedSrgbNonlinear = 1000104014,
    };

    enum class[[= EnumFlag()]] FormatFeatureFlag : std::uint64_t
    {
        None = 0,
        SampledTexture = 1ull << 0,
        StorageTexture = 1ull << 1,
        StorageTextureAtomic = 1ull << 2,
        UniformTexelBuffer = 1ull << 3,
        StorageTexelBuffer = 1ull << 4,
        StorageTexelBufferAtomic = 1ull << 5,
        VertexBuffer = 1ull << 6,
        ColorAttachment = 1ull << 7,
        ColorAttachmentBlend = 1ull << 8,
        DepthStencilAttachment = 1ull << 9,
        BlitSource = 1ull << 10,
        BlitDestination = 1ull << 11,
        SampledTextureFilterLinear = 1ull << 12,
        SampledTextureFilterCubic = 1ull << 13,
        TransferSource = 1ull << 14,
        TransferDestination = 1ull << 15,
        SampledTextureFilterMinMax = 1ull << 16,
        StorageReadWithoutFormat = 1ull << 31,
        StorageWriteWithoutFormat = 1ull << 32,
        SampledTextureDepthComparison = 1ull << 33,
        AccelerationStructureVertexBuffer = 1ull << 29,
        FragmentShadingRateAttachment = 1ull << 30,
    };

    enum class TextureDimension
    {
        Dim1D = 0,
        Dim2D = 1,
        Dim3D = 2,
    };

    enum class TextureViewDimension
    {
        Dim1D = 0,
        Dim2D = 1,
        Dim3D = 2,
        Cube = 3,
        Dim1DArray = 4,
        Dim2DArray = 5,
        CubeArray = 6,
    };

    enum class TextureLayout
    {
        Undefined = 0,
        General = 1,
        ColorAttachment = 2,
        DepthStencilAttachment = 3,
        DepthStencilReadOnly = 4,
        ShaderReadOnly = 5,
        TransferSource = 6,
        TransferDestination = 7,
        Preinitialized = 8,
        DepthReadOnlyStencilAttachment = 1000117000,
        DepthAttachmentStencilReadOnly = 1000117001,
        DepthAttachment = 1000241000,
        DepthReadOnly = 1000241001,
        StencilAttachment = 1000241002,
        StencilReadOnly = 1000241003,
        ReadOnly = 1000314000,
        Attachment = 1000314001,
        PresentSource = 1000001002,
        FragmentShadingRateAttachment = 1000164003,
        FragmentDensityMap = 1000218000,
    };

    enum class TextureTiling
    {
        Optimal = 0,
        Linear = 1,
    };

    enum class[[= EnumFlag()]] TextureUsageFlag : std::uint32_t
    {
        None = 0,
        TransferSource = 1u << 0,
        TransferDestination = 1u << 1,
        Sampled = 1u << 2,
        Storage = 1u << 3,
        ColorAttachment = 1u << 4,
        DepthStencilAttachment = 1u << 5,
        TransientAttachment = 1u << 6,
        InputAttachment = 1u << 7,
        FragmentShadingRateAttachment = 1u << 8,
        FragmentDensityMap = 1u << 9,
        HostTransfer = 1u << 22,
    };

    enum class[[= EnumFlag()]] TextureCreateFlag : std::uint32_t
    {
        None = 0,
        SparseBinding = 1u << 0,
        SparseResidency = 1u << 1,
        SparseAliased = 1u << 2,
        CubeCompatible = 1u << 4,
        Array2DCompatible = 1u << 5,
        BlockTexelViewCompatible = 1u << 7,
        Alias = 1u << 10,
        Protected = 1u << 11,
    };

    enum class[[= EnumFlag()]] TextureAspectFlag : std::uint32_t
    {
        None = 0,
        Color = 1u << 0,
        Depth = 1u << 1,
        Stencil = 1u << 2,
        Plane0 = 1u << 4,
        Plane1 = 1u << 5,
        Plane2 = 1u << 6,
    };

    enum class SampleCount
    {
        Count1 = 1,
        Count2 = 2,
        Count4 = 4,
        Count8 = 8,
        Count16 = 16,
        Count32 = 32,
        Count64 = 64,
    };

    enum class ComponentSwizzle
    {
        Identity = 0,
        Zero = 1,
        One = 2,
        R = 3,
        G = 4,
        B = 5,
        A = 6,
    };

    enum class[[= EnumFlag()]] BufferUsageFlag : std::uint32_t
    {
        None = 0,
        TransferSource = 1u << 0,
        TransferDestination = 1u << 1,
        UniformTexel = 1u << 2,
        StorageTexel = 1u << 3,
        Uniform = 1u << 4,
        Storage = 1u << 5,
        Index = 1u << 6,
        Vertex = 1u << 7,
        Indirect = 1u << 8,
        ShaderBindingTable = 1u << 10,
        ShaderDeviceAddress = 1u << 17,
        AccelerationStructureBuildInput = 1u << 19,
        AccelerationStructureStorage = 1u << 20,
    };

    enum class[[= EnumFlag()]] BufferCreateFlag : std::uint32_t
    {
        None = 0,
        SparseBinding = 1u << 0,
        SparseResidency = 1u << 1,
        SparseAliased = 1u << 2,
        Protected = 1u << 3,
        DeviceAddressCaptureReplay = 1u << 4,
    };

    enum class[[= EnumFlag()]] MemoryPropertyFlag : std::uint32_t
    {
        None = 0,
        DeviceLocal = 1u << 0,
        HostVisible = 1u << 1,
        HostCoherent = 1u << 2,
        HostCached = 1u << 3,
        LazilyAllocated = 1u << 4,
        Protected = 1u << 5,
    };

    enum class[[= EnumFlag()]] ShaderStageFlag : std::uint32_t
    {
        None = 0,
        Vertex = 1u << 0,
        TessellationControl = 1u << 1,
        TessellationEvaluation = 1u << 2,
        Geometry = 1u << 3,
        Fragment = 1u << 4,
        Compute = 1u << 5,
        Task = 1u << 6,
        Mesh = 1u << 7,
        RayGeneration = 1u << 8,
        AnyHit = 1u << 9,
        ClosestHit = 1u << 10,
        Miss = 1u << 11,
        Intersection = 1u << 12,
        Callable = 1u << 13,
        AllGraphics = 0x0000001Fu,
        All = 0x7FFFFFFFu,
    };

    enum class PipelineType
    {
        Graphics = 0,
        Compute = 1,
        RayTracing = 1000165000,
    };

    enum class PrimitiveTopology
    {
        PointList = 0,
        LineList = 1,
        LineStrip = 2,
        TriangleList = 3,
        TriangleStrip = 4,
        TriangleFan = 5,
        LineListWithAdjacency = 6,
        LineStripWithAdjacency = 7,
        TriangleListWithAdjacency = 8,
        TriangleStripWithAdjacency = 9,
        PatchList = 10,
    };

    enum class PolygonMode
    {
        Fill = 0,
        Line = 1,
        Point = 2,
    };

    enum class CullMode
    {
        None = 0,
        Front = 1,
        Back = 2,
        FrontAndBack = 3,
    };

    enum class FrontFace
    {
        CounterClockwise = 0,
        Clockwise = 1,
    };

    enum class CompareOp
    {
        Never = 0,
        Less = 1,
        Equal = 2,
        LessOrEqual = 3,
        Greater = 4,
        NotEqual = 5,
        GreaterOrEqual = 6,
        Always = 7,
    };

    enum class StencilOp
    {
        Keep = 0,
        Zero = 1,
        Replace = 2,
        IncrementAndClamp = 3,
        DecrementAndClamp = 4,
        Invert = 5,
        IncrementAndWrap = 6,
        DecrementAndWrap = 7,
    };

    enum class BlendFactor
    {
        Zero = 0,
        One = 1,
        SrcColor = 2,
        OneMinusSrcColor = 3,
        DstColor = 4,
        OneMinusDstColor = 5,
        SrcAlpha = 6,
        OneMinusSrcAlpha = 7,
        DstAlpha = 8,
        OneMinusDstAlpha = 9,
        ConstantColor = 10,
        OneMinusConstantColor = 11,
        ConstantAlpha = 12,
        OneMinusConstantAlpha = 13,
        SrcAlphaSaturate = 14,
        Src1Color = 15,
        OneMinusSrc1Color = 16,
        Src1Alpha = 17,
        OneMinusSrc1Alpha = 18,
    };

    enum class BlendOp
    {
        Add = 0,
        Subtract = 1,
        ReverseSubtract = 2,
        Min = 3,
        Max = 4,
    };

    enum class[[= EnumFlag()]] ColorComponentFlag : std::uint32_t
    {
        None = 0,
        R = 1u << 0,
        G = 1u << 1,
        B = 1u << 2,
        A = 1u << 3,
        All = 0x0000000Fu,
    };

    enum class LogicOp
    {
        Clear = 0,
        And = 1,
        AndReverse = 2,
        Copy = 3,
        AndInverted = 4,
        NoOp = 5,
        Xor = 6,
        Or = 7,
        Nor = 8,
        Equivalent = 9,
        Invert = 10,
        OrReverse = 11,
        CopyInverted = 12,
        OrInverted = 13,
        Nand = 14,
        Set = 15,
    };

    enum class SamplerFilter
    {
        Nearest = 0,
        Linear = 1,
        Cubic = 1000015000,
    };

    enum class SamplerMipmapMode
    {
        Nearest = 0,
        Linear = 1,
    };

    enum class SamplerAddressMode
    {
        Repeat = 0,
        MirroredRepeat = 1,
        ClampToEdge = 2,
        ClampToBorder = 3,
        MirrorClampToEdge = 4,
    };

    enum class BorderColor
    {
        FloatTransparentBlack = 0,
        IntTransparentBlack = 1,
        FloatOpaqueBlack = 2,
        IntOpaqueBlack = 3,
        FloatOpaqueWhite = 4,
        IntOpaqueWhite = 5,
    };

    enum class SamplerReductionMode
    {
        WeightedAverage = 0,
        Min = 1,
        Max = 2,
    };

    enum class DescriptorType
    {
        Sampler = 0,
        CombinedTextureSampler = 1,
        SampledTexture = 2,
        StorageTexture = 3,
        UniformTexelBuffer = 4,
        StorageTexelBuffer = 5,
        UniformBuffer = 6,
        StorageBuffer = 7,
        UniformBufferDynamic = 8,
        StorageBufferDynamic = 9,
        InputAttachment = 10,
        InlineUniformBlock = 1000138000,
        AccelerationStructure = 1000150000,
    };

    enum class[[= EnumFlag()]] DescriptorBindingFlag : std::uint32_t
    {
        None = 0,
        UpdateAfterBind = 1u << 0,
        UpdateUnusedWhilePending = 1u << 1,
        PartiallyBound = 1u << 2,
        VariableDescriptorCount = 1u << 3,
    };

    enum class[[= EnumFlag()]] DescriptorPoolCreateFlag : std::uint32_t
    {
        None = 0,
        FreeDescriptorSet = 1u << 0,
        UpdateAfterBind = 1u << 1,
    };

    enum class[[= EnumFlag()]] DescriptorSetLayoutCreateFlag : std::uint32_t
    {
        None = 0,
        PushDescriptor = 1u << 0,
        UpdateAfterBindPool = 1u << 1,
    };

    enum class[[= EnumFlag()]] AccessFlag : std::uint64_t
    {
        None = 0,
        IndirectCommandRead = 1ull << 0,
        IndexRead = 1ull << 1,
        VertexAttributeRead = 1ull << 2,
        UniformRead = 1ull << 3,
        InputAttachmentRead = 1ull << 4,
        ShaderRead = 1ull << 5,
        ShaderWrite = 1ull << 6,
        ColorAttachmentRead = 1ull << 7,
        ColorAttachmentWrite = 1ull << 8,
        DepthStencilAttachmentRead = 1ull << 9,
        DepthStencilAttachmentWrite = 1ull << 10,
        TransferRead = 1ull << 11,
        TransferWrite = 1ull << 12,
        HostRead = 1ull << 13,
        HostWrite = 1ull << 14,
        MemoryRead = 1ull << 15,
        MemoryWrite = 1ull << 16,
        ShaderSampledRead = 1ull << 32,
        ShaderStorageRead = 1ull << 33,
        ShaderStorageWrite = 1ull << 34,
        AccelerationStructureRead = 1ull << 21,
        AccelerationStructureWrite = 1ull << 22,
        FragmentShadingRateAttachmentRead = 1ull << 23,
        FragmentDensityMapRead = 1ull << 24,
        ShaderBindingTableRead = 1ull << 40,
    };

    enum class[[= EnumFlag()]] PipelineStageFlag : std::uint64_t
    {
        None = 0,
        TopOfPipe = 1ull << 0,
        DrawIndirect = 1ull << 1,
        VertexInput = 1ull << 2,
        VertexShader = 1ull << 3,
        TessellationControlShader = 1ull << 4,
        TessellationEvaluationShader = 1ull << 5,
        GeometryShader = 1ull << 6,
        FragmentShader = 1ull << 7,
        EarlyFragmentTests = 1ull << 8,
        LateFragmentTests = 1ull << 9,
        ColorAttachmentOutput = 1ull << 10,
        ComputeShader = 1ull << 11,
        Transfer = 1ull << 12,
        BottomOfPipe = 1ull << 13,
        Host = 1ull << 14,
        AllGraphics = 1ull << 15,
        AllCommands = 1ull << 16,
        TaskShader = 1ull << 19,
        MeshShader = 1ull << 20,
        RayTracingShader = 1ull << 21,
        FragmentShadingRateAttachment = 1ull << 22,
        FragmentDensityProcess = 1ull << 23,
        AccelerationStructureBuild = 1ull << 25,
        AccelerationStructureCopy = 1ull << 28,
        Copy = 1ull << 32,
        Resolve = 1ull << 33,
        Blit = 1ull << 34,
        Clear = 1ull << 35,
        IndexInput = 1ull << 36,
        VertexAttributeInput = 1ull << 37,
        PreRasterizationShaders = 1ull << 38,
    };

    enum class LoadOp
    {
        Load = 0,
        Clear = 1,
        DontCare = 2,
        None = 1000400000,
    };

    enum class StoreOp
    {
        Store = 0,
        DontCare = 1,
        None = 1000301000,
    };

    enum class ResolveMode
    {
        None = 0,
        SampleZero = 1,
        Average = 2,
        Min = 4,
        Max = 8,
    };

    enum class CommandBufferType
    {
        Primary = 0,
        Secondary = 1,
    };

    enum class[[= EnumFlag()]] CommandBufferUsageFlag : std::uint32_t
    {
        None = 0,
        OneTimeSubmit = 1u << 0,
        SimultaneousUse = 1u << 2,
    };

    enum class IndexFormat
    {
        Uint16 = 0,
        Uint32 = 1,
        Uint8 = 1000265000,
    };

    enum class VertexInputRate
    {
        Vertex = 0,
        Instance = 1,
    };

    enum class[[= EnumFlag()]] QueueFlag : std::uint32_t
    {
        None = 0,
        Graphics = 1u << 0,
        Compute = 1u << 1,
        Transfer = 1u << 2,
        SparseBinding = 1u << 3,
        Protected = 1u << 4,
    };

    enum class DeviceType
    {
        Other = 0,
        IntegratedGpu = 1,
        DiscreteGpu = 2,
        VirtualGpu = 3,
        Cpu = 4,
    };

    enum class PresentMode
    {
        Immediate = 0,
        Mailbox = 1,
        Fifo = 2,
        FifoRelaxed = 3,
    };

    enum class QueryType
    {
        Occlusion = 0,
        PipelineStatistics = 1,
        Timestamp = 2,
        AccelerationStructureCompactedSize = 1000150000,
        AccelerationStructureSerializationSize = 1000150001,
        AccelerationStructureSize = 1000386001,
        MeshPrimitivesGenerated = 1000328000,
        PrimitivesGenerated = 1000382000,
    };

    enum class[[= EnumFlag()]] QueryControlFlag : std::uint32_t
    {
        None = 0,
        Precise = 1u << 0,
    };

    enum class[[= EnumFlag()]] QueryPipelineStatisticFlag : std::uint32_t
    {
        None = 0,
        InputAssemblyVertices = 1u << 0,
        InputAssemblyPrimitives = 1u << 1,
        VertexShaderInvocations = 1u << 2,
        GeometryShaderInvocations = 1u << 3,
        GeometryShaderPrimitives = 1u << 4,
        ClippingInvocations = 1u << 5,
        ClippingPrimitives = 1u << 6,
        FragmentShaderInvocations = 1u << 7,
        TessellationControlShaderPatches = 1u << 8,
        TessellationEvaluationShaderInvocations = 1u << 9,
        ComputeShaderInvocations = 1u << 10,
        TaskShaderInvocations = 1u << 11,
        MeshShaderInvocations = 1u << 12,
    };

    enum class[[= EnumFlag()]] QueryResultFlag : std::uint32_t
    {
        None = 0,
        Bits64 = 1u << 0,
        Wait = 1u << 1,
        WithAvailability = 1u << 2,
        Partial = 1u << 3,
    };

    enum class ConservativeRasterizationMode
    {
        Disabled = 0,
        Overestimate = 1,
        Underestimate = 2,
    };

    enum class LineRasterizationMode
    {
        Default = 0,
        Rectangular = 1,
        Bresenham = 2,
        RectangularSmooth = 3,
    };

    enum class ProvokingVertexMode
    {
        First = 0,
        Last = 1,
    };

    enum class TessellationDomainOrigin
    {
        UpperLeft = 0,
        LowerLeft = 1,
    };

    enum class StencilFace
    {
        Front = 1,
        Back = 2,
        FrontAndBack = 3,
    };

    enum class[[= EnumFlag()]] SamplerCreateFlag : std::uint32_t
    {
        None = 0,
        NonSeamlessCubeMap = 1u << 2,
    };

} // namespace RHI
