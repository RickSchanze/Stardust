#include "Enums.h"

namespace RHI
{

    static_assert(Reflection::IsEnumFlag<BufferUsageFlag>);
    static_assert(Reflection::IsEnumFlag<AccessFlag>);
    static_assert(Reflection::IsEnumFlag<DescriptorPoolCreateFlag>);
    static_assert(!Reflection::IsEnumFlag<PixelFormat>);
    static_assert(!Reflection::IsEnumFlag<SampleCount>);
    static_assert(!Reflection::IsEnumFlag<CullMode>);

    static_assert(HasFlag(BufferUsageFlag::Vertex | BufferUsageFlag::Index, BufferUsageFlag::Vertex));
    static_assert(!HasFlag(BufferUsageFlag::Vertex, BufferUsageFlag::Index));
    static_assert(SetFlag(BufferUsageFlag::Vertex, BufferUsageFlag::Index) ==
                  (BufferUsageFlag::Vertex | BufferUsageFlag::Index));
    static_assert(ClearFlag(BufferUsageFlag::Vertex | BufferUsageFlag::Index, BufferUsageFlag::Index) ==
                  BufferUsageFlag::Vertex);
    static_assert((TextureAspectFlag::Depth | TextureAspectFlag::Stencil) != TextureAspectFlag::None);
    static_assert(HasFlag(ColorComponentFlag::All, ColorComponentFlag::R | ColorComponentFlag::A));

} // namespace RHI
