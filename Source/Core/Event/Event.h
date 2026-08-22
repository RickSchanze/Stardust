#pragma once

#include "Core/Event/MulticastDelegate.h"

template <typename Owner, typename Signature>
class Event;

// 事件
template <typename Owner, typename... Args>
class Event<Owner, void(Args...)> : private MulticastDelegate<void(Args...)>
{
    friend Owner;

    using Base = MulticastDelegate<void(Args...)>;

public:
    using Base::AddLambda;
    using Base::AddRaw;
    using Base::AddStatic;
    using Base::Clear;
    using Base::IsBound;
    using Base::Num;
    using Base::Remove;
    using Base::Reserve;

private:
    using Base::Broadcast;
};
