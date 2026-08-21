#pragma once

#include <atomic>
#include <cstdint>
#include <type_traits>
#include <utility>

#include "Core/Container/Array.h"
#include "Core/Debug/Debug.h"
#include "Core/Event/DelegateDetail.h"
#include "Core/Event/DelegateHandle.h"
#include "Core/Number.h"

template <typename Signature>
class MulticastDelegate;

// 多播委托
template <typename... Args>
class MulticastDelegate<void(Args...)>
{
public:
    MulticastDelegate() = default;

    MulticastDelegate(const MulticastDelegate&) = delete;
    MulticastDelegate& operator=(const MulticastDelegate&) = delete;

    MulticastDelegate(MulticastDelegate&&) noexcept = default;
    MulticastDelegate& operator=(MulticastDelegate&&) noexcept = default;

    template <typename Callable>
        requires std::is_invocable_r_v<void, std::decay_t<Callable>&, Args...>
    DelegateHandle AddLambda(Callable&& CallableRef)
    {
        const DelegateHandle Handle = NextHandle();
        Invocation Entry{};
        Entry.Handle = Handle;
        Entry.Callable.Assign(std::forward<Callable>(CallableRef));
        mInvocations.Add(std::move(Entry));
        return Handle;
    }

    template <typename Class>
    DelegateHandle AddRaw(Class* Object, void (Class::*Method)(Args...))
    {
        Assert(Object != nullptr);
        return AddLambda(DelegateDetail::MemberBinding<Class, void, Args...>{.Object = Object, .Method = Method});
    }

    template <typename Class>
    DelegateHandle AddRaw(const Class* Object, void (Class::*Method)(Args...) const)
    {
        Assert(Object != nullptr);
        return AddLambda(DelegateDetail::ConstMemberBinding<Class, void, Args...>{.Object = Object, .Method = Method});
    }

    template <typename... Args2>
        requires std::is_invocable_r_v<void, void (*)(Args2...), Args...>
    DelegateHandle AddStatic(void (*Function)(Args2...))
    {
        Assert(Function != nullptr);
        return AddLambda([Function](Args... Arguments) { Function(std::forward<Args>(Arguments)...); });
    }

    bool Remove(const DelegateHandle Handle)
    {
        if (!Handle.IsValid())
        {
            return false;
        }
        return mInvocations.RemoveAll([&Handle](const Invocation& Entry) { return Entry.Handle == Handle; }) > 0;
    }

    void Clear()
    {
        mInvocations.Clear();
    }

    [[nodiscard]] bool IsBound() const noexcept
    {
        return !mInvocations.Empty();
    }

    [[nodiscard]] std::size_t Num() const noexcept
    {
        return mInvocations.Num();
    }

    void Broadcast(Args... Arguments) const
    {
        Array<DelegateHandle> Handles;
        Handles.Reserve(mInvocations.Num());
        for (const Invocation& Entry : mInvocations)
        {
            Handles.Add(Entry.Handle);
        }

        for (const DelegateHandle Handle : Handles)
        {
            for (const Invocation& Entry : mInvocations)
            {
                if (Entry.Handle == Handle)
                {
                    Entry.Callable(Arguments...);
                    break;
                }
            }
        }
    }

private:
    struct Invocation
    {
        DelegateHandle Handle{};
        DelegateDetail::BoundCallable<void, Args...> Callable;
    };

    static DelegateHandle NextHandle()
    {
        static std::atomic<std::uint64_t> NextId{1};
        DelegateHandle Handle{};
        Handle.Id = UInt64{NextId.fetch_add(1, std::memory_order_relaxed)};
        return Handle;
    }

    Array<Invocation> mInvocations;
};
