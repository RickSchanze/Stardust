#pragma once

#include "App/EngineLoop.h"

class Application
{
public:
    int Run();

private:
    EngineLoop mEngineLoop;
};
