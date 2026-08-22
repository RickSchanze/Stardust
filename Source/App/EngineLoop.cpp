#include "App/EngineLoop.h"

#include <SDL3/SDL_timer.h>

#include "Core/Debug/Debug.h"
#include "Core/Logging/Logger.h"
#include "Core/Profiler/Profiler.h"
#include "Render/RenderContext.h"

void EngineGlobalData::ResetFrameTiming()
{
    mPerformanceFrequency = SDL_GetPerformanceFrequency();
    Assert(mPerformanceFrequency != 0);

    mStartCounter = SDL_GetPerformanceCounter();
    mPreviousFrameCounter = mStartCounter;
    mFrameNumber = UInt64{};
    mDeltaSeconds = Double{};
    mElapsedSeconds = Double{};
    mFramesPerSecond = Double{};
}

void EngineGlobalData::BeginFrame()
{
    Assert(mPerformanceFrequency != 0);

    const std::uint64_t CurrentCounter = SDL_GetPerformanceCounter();
    const double Frequency = static_cast<double>(mPerformanceFrequency);
    const double DeltaSeconds = static_cast<double>(CurrentCounter - mPreviousFrameCounter) / Frequency;
    const double ElapsedSeconds = static_cast<double>(CurrentCounter - mStartCounter) / Frequency;

    ++mFrameNumber;
    mDeltaSeconds = DeltaSeconds;
    mElapsedSeconds = ElapsedSeconds;
    mFramesPerSecond = DeltaSeconds > 0.0 ? 1.0 / DeltaSeconds : 0.0;
    mPreviousFrameCounter = CurrentCounter;
}

int EngineLoop::Run()
{
    PerformanceCounter RunCounter{"EngineLoop::Run"};

    if (!Startup())
    {
        LogCritical(Engine, "Engine startup failed");
        return 1;
    }

    LogInfo(Engine, "Engine main loop started");

    try
    {
        while (mRunning)
        {
            Tick();
        }
    }
    catch (...)
    {
        LogCritical(Engine, "Unhandled exception escaped the engine tick; shutting down");
        Shutdown();
        throw;
    }

    LogInfo(Engine, "Engine main loop stopped");
    Shutdown();
    return 0;
}

bool EngineLoop::Startup()
{
    PerformanceCounter StartupCounter{"EngineLoop::Startup"};
    Assert(!mInitialized);
    LogInfo(Engine, "Engine startup started");

    Assert(RenderContext::CreateInstance());

    gEngineGlobalData.ResetFrameTiming();
    mInitialized = true;
    mRunning = true;
    LogInfo(Engine, "Engine startup completed");
    return true;
}

void EngineLoop::Tick()
{
    PerformanceCounter TickCounter{"EngineLoop::Tick"};
    Assert(mInitialized);
    Assert(mRunning);

    gEngineGlobalData.BeginFrame();
    const Double DeltaSeconds = gEngineGlobalData.GetDeltaSeconds();

    TickInput(DeltaSeconds);
    TickLogic(DeltaSeconds);
    TickPhysics(DeltaSeconds);
    TickRender(DeltaSeconds);

    MarkFrame();
}

void EngineLoop::Shutdown()
{
    PerformanceCounter ShutdownCounter{"EngineLoop::Shutdown"};

    if (!mInitialized)
    {
        return;
    }

    LogInfo(Engine,
            "Engine shutdown started after {} frames ({:.3f} seconds)",
            gEngineGlobalData.GetFrameNumber().Value,
            gEngineGlobalData.GetElapsedSeconds().Value);

    mRunning = false;
    RenderContext::DestroyInstance();
    mInitialized = false;
    LogInfo(Engine, "Engine shutdown completed");
}

void EngineLoop::RequestExit() noexcept
{
    mRunning = false;
}

void EngineLoop::TickInput(const Double DeltaSeconds)
{
    PerformanceCounter InputCounter{"EngineLoop::TickInput"};
    (void)DeltaSeconds;
}

void EngineLoop::TickLogic(const Double DeltaSeconds)
{
    PerformanceCounter LogicCounter{"EngineLoop::TickLogic"};
    (void)DeltaSeconds;
}

void EngineLoop::TickPhysics(const Double DeltaSeconds)
{
    PerformanceCounter PhysicsCounter{"EngineLoop::TickPhysics"};
    (void)DeltaSeconds;
}

void EngineLoop::TickRender(const Double DeltaSeconds)
{
    PerformanceCounter RenderCounter{"EngineLoop::TickRender"};
    RenderContext::GetInstance().Tick(DeltaSeconds);
}
