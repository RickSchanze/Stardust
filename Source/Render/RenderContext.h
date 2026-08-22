#pragma once
#include "Core/Number.h"
#include "Core/Singleton.h"

class RenderContext : Singleton<RenderContext>
{
public:
    void Tick(Double DeltaSeconds);
};