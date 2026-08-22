#pragma once

#include <cstdint>
#include <type_traits>
#include <utility>

#include "Core/Container/Array.h"
#include "Core/Debug/Debug.h"
#include "Core/Event/DelegateDetail.h"
#include "Core/Event/DelegateHandle.h"
#include "Core/Number.h"

namespace DelegateDetail
{
    inline UInt64 gNextDelegateHandle = 1;
}

template <typename Signature>
class MulticastDelegate;

// 多播委托
template <typename... Args>
class MulticastDelegate<void(Args...)>
{
    static_assert(((!std::is_rvalue_reference_v<Args>) && ...),
                  "MulticastDelegate does not support rvalue-reference parameters");
    static_assert(((std::is_lvalue_reference_v<Args> || std::is_copy_constructible_v<std::remove_cv_t<Args>>) && ...),
                  "MulticastDelegate value parameters must be copy constructible");

public:
    MulticastDelegate() = default;

    MulticastDelegate(const MulticastDelegate&) = delete;
    MulticastDelegate& operator=(const MulticastDelegate&) = delete;

    MulticastDelegate(MulticastDelegate&& Other) noexcept
    {
        Assert(Other.mBroadcastDepth == 0);
        mInvocations = std::move(Other.mInvocations);
        mPendingInvocations = std::move(Other.mPendingInvocations);
        mBoundCount = std::exchange(Other.mBoundCount, 0);
    }

    MulticastDelegate& operator=(MulticastDelegate&& Other) noexcept
    {
        Assert(mBroadcastDepth == 0);
        Assert(Other.mBroadcastDepth == 0);
        if (this != &Other)
        {
            mInvocations = std::move(Other.mInvocations);
            mPendingInvocations = std::move(Other.mPendingInvocations);
            mBoundCount = std::exchange(Other.mBoundCount, 0);
        }
        return *this;
    }

    template <typename Callable>
        requires std::is_invocable_r_v<void, std::decay_t<Callable>&, Args...>
    DelegateHandle AddLambda(Callable&& CallableRef)
    {
        const DelegateHandle Handle = NextHandle();
        Invocation Entry{};
        Entry.Handle = Handle;
        Entry.Callable.Assign(std::forward<Callable>(CallableRef));

        if (mBroadcastDepth == 0)
        {
            FlushPending();
            mInvocations.Add(std::move(Entry));
        }
        else
        {
            mPendingInvocations.Add(std::move(Entry));
        }

        ++mBoundCount;
        return Handle;
    }

    template <typename Class>
    DelegateHandle AddRaw(Class* Object, void (Class::*Method)(Args...))
    {
        Assert(Object != nullptr);
        Assert(Method != nullptr);
        return AddLambda(DelegateDetail::MemberBinding<Class, void, Args...>{.Object = Object, .Method = Method});
    }

    template <typename Class>
    DelegateHandle AddRaw(const Class* Object, void (Class::*Method)(Args...) const)
    {
        Assert(Object != nullptr);
        Assert(Method != nullptr);
        return AddLambda(DelegateDetail::ConstMemberBinding<Class, void, Args...>{.Object = Object, .Method = Method});
    }

    template <typename... Args2>
        requires std::is_invocable_r_v<void, void (*)(Args2...), Args...>
    DelegateHandle AddStatic(void (*Function)(Args2...))
    {
        Assert(Function != nullptr);
        return AddLambda([Function](Args&&... Arguments) { Function(std::forward<Args>(Arguments)...); });
    }

    bool Remove(const DelegateHandle Handle)
    {
        if (!Handle.IsValid())
        {
            return false;
        }

        for (std::size_t Index = 0; Index < mInvocations.Num(); ++Index)
        {
            Invocation& Entry = mInvocations[Index];
            if (Entry.Active && Entry.Handle == Handle)
            {
                --mBoundCount;
                if (mBroadcastDepth == 0)
                {
                    mInvocations.RemoveAt(Index);
                }
                else
                {
                    Entry.Active = false;
                }
                return true;
            }
        }

        for (std::size_t Index = 0; Index < mPendingInvocations.Num(); ++Index)
        {
            if (mPendingInvocations[Index].Handle == Handle)
            {
                --mBoundCount;
                mPendingInvocations.RemoveAt(Index);
                return true;
            }
        }

        return false;
    }

    void Clear()
    {
        if (mBroadcastDepth == 0)
        {
            mInvocations.Clear();
        }
        else
        {
            for (Invocation& Entry : mInvocations)
            {
                Entry.Active = false;
            }
        }

        mPendingInvocations.Clear();
        mBoundCount = 0;
    }

    void Reserve(const std::size_t Capacity)
    {
        Assert(mBroadcastDepth == 0);
        FlushPending();
        mInvocations.Reserve(Capacity);
    }

    [[nodiscard]] bool IsBound() const noexcept
    {
        return mBoundCount != 0;
    }

    [[nodiscard]] std::size_t Num() const noexcept
    {
        return mBoundCount;
    }

    void Broadcast(Args... Arguments) const
    {
        if (mBroadcastDepth == 0)
        {
            FlushPending();
        }

        ++mBroadcastDepth;
        const std::size_t InvocationCount = mInvocations.Num();

        try
        {
            for (std::size_t Index = 0; Index < InvocationCount; ++Index)
            {
                Invocation& Entry = mInvocations[Index];
                if (Entry.Active)
                {
                    Entry.Callable(BroadcastArgument<Args>(Arguments)...);
                }
            }
        }
        catch (...)
        {
            EndBroadcast();
            throw;
        }

        EndBroadcast();
    }

private:
    struct Invocation
    {
        DelegateHandle Handle{};
        DelegateDetail::BoundCallable<void, Args...> Callable;
        bool Active = true;
    };

    static DelegateHandle NextHandle()
    {
        const UInt64 Id = DelegateDetail::gNextDelegateHandle++;
        Assert(Id != 0);
        DelegateHandle Handle{};
        Handle.Id = UInt64{Id};
        return Handle;
    }

    template <typename Arg>
    static decltype(auto) BroadcastArgument(std::remove_reference_t<Arg>& Argument)
    {
        if constexpr (std::is_lvalue_reference_v<Arg>)
        {
            return (Argument);
        }
        else
        {
            return std::remove_cv_t<Arg>(Argument);
        }
    }

    void EndBroadcast() const
    {
        Assert(mBroadcastDepth > 0);
        --mBroadcastDepth;
        if (mBroadcastDepth == 0)
        {
            mInvocations.RemoveAll([](const Invocation& Entry) { return !Entry.Active; });
            FlushPending();
        }
    }

    void FlushPending() const
    {
        if (mPendingInvocations.Empty())
        {
            return;
        }

        mInvocations.Reserve(mInvocations.Num() + mPendingInvocations.Num());
        for (Invocation& Entry : mPendingInvocations)
        {
            mInvocations.Add(std::move(Entry));
        }
        mPendingInvocations.Clear();
    }

    mutable Array<Invocation> mInvocations;
    mutable Array<Invocation> mPendingInvocations;
    mutable std::size_t mBroadcastDepth = 0;
    std::size_t mBoundCount = 0;
};
