#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

#include "Core/Debug/Debug.h"

template <typename Signature>
class FunctionRef;

/// Non-owning callable view (like string_view for functions).
/// Safe as a function parameter; do not store beyond the callable's lifetime.
template <typename Ret, typename... Args>
class FunctionRef<Ret(Args...)>
{
public:
    using ResultType = Ret;

    constexpr FunctionRef() noexcept = default;

    constexpr FunctionRef(std::nullptr_t) noexcept : FunctionRef() {}

    template <typename Callable>
        requires(!std::is_same_v<std::decay_t<Callable>, FunctionRef>) &&
                (!std::is_function_v<std::remove_pointer_t<std::decay_t<Callable>>>) &&
                std::is_invocable_r_v<Ret, Callable&, Args...>
    constexpr FunctionRef(Callable&& CallableRef) noexcept :
        mCallable(reinterpret_cast<std::intptr_t>(std::addressof(CallableRef)))
    {
        using Decayed = std::remove_reference_t<Callable>;
        mInvoker = +[](const std::intptr_t Object, Args... Arguments) -> Ret
        { return (*reinterpret_cast<Decayed*>(Object))(std::forward<Args>(Arguments)...); };
    }

    template <typename Ret2, typename... Args2>
        requires std::is_invocable_r_v<Ret, Ret2 (*)(Args2...), Args...>
    constexpr FunctionRef(Ret2 (*Function)(Args2...)) noexcept :
        mCallable(reinterpret_cast<std::intptr_t>(Function)),
        mInvoker(+[](const std::intptr_t Callable, Args... Arguments) -> Ret
                 { return reinterpret_cast<Ret2 (*)(Args2...)>(Callable)(std::forward<Args>(Arguments)...); })
    {
    }

    [[nodiscard]] constexpr bool IsBound() const noexcept
    {
        return mInvoker != nullptr;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return IsBound();
    }

    constexpr void Reset() noexcept
    {
        mCallable = 0;
        mInvoker = nullptr;
    }

    Ret operator()(Args... Arguments) const
    {
        Assert(IsBound());
        return mInvoker(mCallable, std::forward<Args>(Arguments)...);
    }

private:
    std::intptr_t mCallable = 0;
    Ret (*mInvoker)(std::intptr_t, Args...) = nullptr;
};
