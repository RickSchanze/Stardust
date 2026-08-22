#include "App/Application.h"

#include "Core/Logging/Logger.h"
#include "Core/Profiler/Profiler.h"

int Application::Run()
{
    PerformanceCounter RunCounter{"Application::Run"};
    LogInfo(Engine, "Application starting");

    const int ExitCode = mEngineLoop.Run();
    LogInfo(Engine, "Application stopped with exit code {}", ExitCode);
    return ExitCode;
}
