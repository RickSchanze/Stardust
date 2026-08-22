#pragma once

#include <cstdint>

#include "Core/Number.h"

class EngineLoop;

class EngineGlobalData
{
public:
    [[nodiscard]] UInt64 GetFrameNumber() const noexcept
    {
        return mFrameNumber;
    }

    [[nodiscard]] Double GetDeltaSeconds() const noexcept
    {
        return mDeltaSeconds;
    }

    [[nodiscard]] Double GetElapsedSeconds() const noexcept
    {
        return mElapsedSeconds;
    }

    [[nodiscard]] Double GetFramesPerSecond() const noexcept
    {
        return mFramesPerSecond;
    }

private:
    friend EngineLoop;

    void ResetFrameTiming();
    void BeginFrame();

    UInt64 mFrameNumber{};
    Double mDeltaSeconds{};
    Double mElapsedSeconds{};
    Double mFramesPerSecond{};
    UInt64 mStartCounter = 0;
    UInt64 mPreviousFrameCounter = 0;
    UInt64 mPerformanceFrequency = 0;
};

inline EngineGlobalData gEngineGlobalData = {};

class EngineLoop
{
public:
    EngineLoop() = default;

    EngineLoop(const EngineLoop&) = delete;
    EngineLoop& operator=(const EngineLoop&) = delete;
    EngineLoop(EngineLoop&&) = delete;
    EngineLoop& operator=(EngineLoop&&) = delete;

    int Run();

    [[nodiscard]] bool Startup();
    void Tick();
    void Shutdown();
    void RequestExit() noexcept;

    [[nodiscard]] bool IsRunning() const noexcept
    {
        return mRunning;
    }

    [[nodiscard]] bool IsInitialized() const noexcept
    {
        return mInitialized;
    }

private:
    void TickInput(Double DeltaSeconds);
    void TickLogic(Double DeltaSeconds);
    void TickPhysics(Double DeltaSeconds);
    void TickRender(Double DeltaSeconds);

    bool mRunning = false;
    bool mInitialized = false;
};
