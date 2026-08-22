#pragma once

#include "Core/Number.h"
#include "Core/Singleton.h"

class RenderContext : public Singleton<RenderContext>
{
public:
    bool Startup() override;
    bool Shutdown() override;

    void Tick(const Double DeltaSeconds);
};
