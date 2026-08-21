#pragma once

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "Core/Debug/Debug.h"
#include "Core/Memory/Memory.h"

namespace DelegateDetail
{

    inline constexpr std::size_t gBoundCallableBufferSize = 64;

    template <typename Ret, typename... Args>
    class BoundCallable
    {
    public:
        BoundCallable() noexcept = default;

        BoundCallable(const BoundCallable&) = delete;
        BoundCallable& operator=(const BoundCallable&) = delete;

        BoundCallable(BoundCallable&& Other) noexcept
        {
            MoveFrom(std::move(Other));
        }

        BoundCallable& operator=(BoundCallable&& Other) noexcept
        {
            if (this != &Other)
            {
                Reset();
                MoveFrom(std::move(Other));
            }
            return *this;
        }

        ~BoundCallable()
        {
            Reset();
        }

        template <typename Callable>
            requires std::is_invocable_r_v<Ret, std::decay_t<Callable>&, Args...>
        void Assign(Callable&& CallableRef)
        {
            using Decayed = std::decay_t<Callable>;
            Reset();

            constexpr auto FitsInBuffer = sizeof(Decayed) <= gBoundCallableBufferSize &&
                                          alignof(Decayed) <= alignof(std::max_align_t) &&
                                          std::is_nothrow_move_constructible_v<Decayed>;

            if constexpr (FitsInBuffer)
            {
                mPtr = mBuffer;
                new (mPtr) Decayed(std::forward<Callable>(CallableRef));
                mDestroy = +[](void* Object) { static_cast<Decayed*>(Object)->~Decayed(); };
                mRelocate = +[](void* Destination, void* Source)
                {
                    new (Destination) Decayed(std::move(*static_cast<Decayed*>(Source)));
                    static_cast<Decayed*>(Source)->~Decayed();
                };
                mInvoke = +[](void* Object, Args... Arguments) -> Ret
                { return (*static_cast<Decayed*>(Object))(std::forward<Args>(Arguments)...); };
                mLocal = true;
            }
            else
            {
                void* Memory = Malloc(sizeof(Decayed));
                Assert(Memory != nullptr);
                mPtr = Memory;
                new (mPtr) Decayed(std::forward<Callable>(CallableRef));
                mDestroy = +[](void* Object)
                {
                    static_cast<Decayed*>(Object)->~Decayed();
                    Free(Object);
                };
                mRelocate = nullptr;
                mInvoke = +[](void* Object, Args... Arguments) -> Ret
                { return (*static_cast<Decayed*>(Object))(std::forward<Args>(Arguments)...); };
                mLocal = false;
            }
        }

        void Reset() noexcept
        {
            if (mPtr != nullptr && mDestroy != nullptr)
            {
                mDestroy(mPtr);
            }
            mPtr = nullptr;
            mDestroy = nullptr;
            mRelocate = nullptr;
            mInvoke = nullptr;
            mLocal = false;
        }

        [[nodiscard]] bool IsBound() const noexcept
        {
            return mInvoke != nullptr;
        }

        Ret operator()(Args... Arguments) const
        {
            Assert(IsBound());
            return mInvoke(mPtr, std::forward<Args>(Arguments)...);
        }

    private:
        void MoveFrom(BoundCallable&& Other) noexcept
        {
            mDestroy = Other.mDestroy;
            mRelocate = Other.mRelocate;
            mInvoke = Other.mInvoke;
            mLocal = Other.mLocal;

            if (!Other.IsBound())
            {
                return;
            }

            if (Other.mLocal)
            {
                mPtr = mBuffer;
                mRelocate(mPtr, Other.mPtr);
                Other.mPtr = nullptr;
                Other.mDestroy = nullptr;
                Other.mRelocate = nullptr;
                Other.mInvoke = nullptr;
                Other.mLocal = false;
            }
            else
            {
                mPtr = Other.mPtr;
                Other.mPtr = nullptr;
                Other.mDestroy = nullptr;
                Other.mRelocate = nullptr;
                Other.mInvoke = nullptr;
                Other.mLocal = false;
            }
        }

        alignas(std::max_align_t) unsigned char mBuffer[gBoundCallableBufferSize]{};
        void* mPtr = nullptr;
        void (*mDestroy)(void*) = nullptr;
        void (*mRelocate)(void*, void*) = nullptr;
        Ret (*mInvoke)(void*, Args...) = nullptr;
        bool mLocal = false;
    };

    template <typename Class, typename Ret, typename... Args>
    struct MemberBinding
    {
        Class* Object = nullptr;
        Ret (Class::*Method)(Args...) = nullptr;

        Ret operator()(Args... Arguments) const
        {
            return (Object->*Method)(std::forward<Args>(Arguments)...);
        }
    };

    template <typename Class, typename Ret, typename... Args>
    struct ConstMemberBinding
    {
        const Class* Object = nullptr;
        Ret (Class::*Method)(Args...) const = nullptr;

        Ret operator()(Args... Arguments) const
        {
            return (Object->*Method)(std::forward<Args>(Arguments)...);
        }
    };

} // namespace DelegateDetail
