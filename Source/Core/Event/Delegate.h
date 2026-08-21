#pragma once

#include <type_traits>
#include <utility>

#include "Core/Debug/Debug.h"
#include "Core/Event/DelegateDetail.h"

template <typename Signature>
class Delegate;

// 单播委托
template <typename Ret, typename... Args>
class Delegate<Ret(Args...)>
{
public:
    using ResultType = Ret;

    Delegate() = default;

    Delegate(const Delegate&) = delete;
    Delegate& operator=(const Delegate&) = delete;

    Delegate(Delegate&&) noexcept = default;
    Delegate& operator=(Delegate&&) noexcept = default;

    template <typename Callable>
        requires std::is_invocable_r_v<Ret, std::decay_t<Callable>&, Args...>
    void BindLambda(Callable&& CallableRef)
    {
        mCallable.Assign(std::forward<Callable>(CallableRef));
    }

    template <typename Class>
    void BindRaw(Class* Object, Ret (Class::*Method)(Args...))
    {
        Assert(Object != nullptr);
        mCallable.Assign(DelegateDetail::MemberBinding<Class, Ret, Args...>{.Object = Object, .Method = Method});
    }

    template <typename Class>
    void BindRaw(const Class* Object, Ret (Class::*Method)(Args...) const)
    {
        Assert(Object != nullptr);
        mCallable.Assign(DelegateDetail::ConstMemberBinding<Class, Ret, Args...>{.Object = Object, .Method = Method});
    }

    template <typename Ret2, typename... Args2>
        requires std::is_invocable_r_v<Ret, Ret2 (*)(Args2...), Args...>
    void BindStatic(Ret2 (*Function)(Args2...))
    {
        Assert(Function != nullptr);
        mCallable.Assign([Function](Args... Arguments) -> Ret { return Function(std::forward<Args>(Arguments)...); });
    }

    void Unbind() noexcept
    {
        mCallable.Reset();
    }

    [[nodiscard]] bool IsBound() const noexcept
    {
        return mCallable.IsBound();
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return IsBound();
    }

    Ret Execute(Args... Arguments) const
    {
        Assert(IsBound());
        return mCallable(std::forward<Args>(Arguments)...);
    }

    Ret ExecuteIfBound(Args... Arguments) const
        requires std::is_void_v<Ret>
    {
        if (IsBound())
        {
            mCallable(std::forward<Args>(Arguments)...);
        }
    }

    [[nodiscard]] bool ExecuteIfBound(Args... Arguments) const
        requires(!std::is_void_v<Ret>)
    {
        if (!IsBound())
        {
            return false;
        }
        (void)mCallable(std::forward<Args>(Arguments)...);
        return true;
    }

private:
    DelegateDetail::BoundCallable<Ret, Args...> mCallable;
};
