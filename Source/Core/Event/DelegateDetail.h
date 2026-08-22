#pragma once

#include <cstddef>
#include <new>
#include <type_traits>
#include <utility>

#include "Core/Debug/Debug.h"
#include "Core/Memory/Memory.h"

namespace DelegateDetail
{

    inline constexpr std::size_t gBoundCallableBufferSize = 32;

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
            BoundCallable Replacement;
            Replacement.Construct<Decayed>(std::forward<Callable>(CallableRef));
            *this = std::move(Replacement);
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
        }

        [[nodiscard]] bool IsBound() const noexcept
        {
            return mInvoke != nullptr;
        }

        Ret operator()(Args&&... Arguments) const
        {
            Assert(IsBound());
            return mInvoke(mPtr, std::forward<Args>(Arguments)...);
        }

    private:
        template <typename Decayed, typename Callable>
        void Construct(Callable&& CallableRef)
        {
            constexpr bool FitsInBuffer = sizeof(Decayed) <= gBoundCallableBufferSize &&
                                          alignof(Decayed) <= alignof(std::max_align_t) &&
                                          std::is_nothrow_move_constructible_v<Decayed>;

            if constexpr (FitsInBuffer)
            {
                new (mBuffer) Decayed(std::forward<Callable>(CallableRef));
                mPtr = mBuffer;
                mDestroy = +[](void* Object) { static_cast<Decayed*>(Object)->~Decayed(); };
                mRelocate = +[](void* Destination, void* Source)
                {
                    new (Destination) Decayed(std::move(*static_cast<Decayed*>(Source)));
                    static_cast<Decayed*>(Source)->~Decayed();
                };
            }
            else
            {
                void* Memory = MallocAligned(sizeof(Decayed), alignof(Decayed));
                if (Memory == nullptr)
                {
                    throw std::bad_alloc();
                }

                try
                {
                    new (Memory) Decayed(std::forward<Callable>(CallableRef));
                }
                catch (...)
                {
                    Free(Memory);
                    throw;
                }

                mPtr = Memory;
                mDestroy = +[](void* Object)
                {
                    static_cast<Decayed*>(Object)->~Decayed();
                    Free(Object);
                };
                mRelocate = nullptr;
            }

            mInvoke = +[](void* Object, Args&&... Arguments) -> Ret
            { return (*static_cast<Decayed*>(Object))(std::forward<Args>(Arguments)...); };
        }

        void MoveFrom(BoundCallable&& Other) noexcept
        {
            mDestroy = Other.mDestroy;
            mRelocate = Other.mRelocate;
            mInvoke = Other.mInvoke;

            if (!Other.IsBound())
            {
                return;
            }

            if (Other.mRelocate != nullptr)
            {
                mPtr = mBuffer;
                mRelocate(mPtr, Other.mPtr);
                Other.mPtr = nullptr;
                Other.mDestroy = nullptr;
                Other.mRelocate = nullptr;
                Other.mInvoke = nullptr;
            }
            else
            {
                mPtr = Other.mPtr;
                Other.mPtr = nullptr;
                Other.mDestroy = nullptr;
                Other.mRelocate = nullptr;
                Other.mInvoke = nullptr;
            }
        }

        alignas(std::max_align_t) unsigned char mBuffer[gBoundCallableBufferSize];
        void* mPtr = nullptr;
        void (*mDestroy)(void*) = nullptr;
        void (*mRelocate)(void*, void*) = nullptr;
        Ret (*mInvoke)(void*, Args&&...) = nullptr;
    };

    template <typename Class, typename Ret, typename... Args>
    struct MemberBinding
    {
        Class* Object = nullptr;
        Ret (Class::*Method)(Args...) = nullptr;

        Ret operator()(Args&&... Arguments) const
        {
            return (Object->*Method)(std::forward<Args>(Arguments)...);
        }
    };

    template <typename Class, typename Ret, typename... Args>
    struct ConstMemberBinding
    {
        const Class* Object = nullptr;
        Ret (Class::*Method)(Args...) const = nullptr;

        Ret operator()(Args&&... Arguments) const
        {
            return (Object->*Method)(std::forward<Args>(Arguments)...);
        }
    };

} // namespace DelegateDetail
