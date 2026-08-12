#pragma once

#include "Core/String/StringView.h"

#include <meta>
#include <type_traits>
#include <utility>

namespace Reflection
{

template <typename E>
[[nodiscard]] StringView GetEnumString(E Value)
{
    static_assert(std::is_enum_v<E>);

    template for (constexpr auto Enumerator : std::define_static_array(std::meta::enumerators_of(^^E)))
    {
        if (Value == [:Enumerator:])
        {
            return StringView{std::meta::identifier_of(Enumerator)};
        }
    }

    return StringView{"?"};
}

} // namespace Reflection
