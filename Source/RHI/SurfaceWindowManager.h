#pragma once

#include "Core/Container/Array.h"
#include "Core/Singleton.h"
#include "RHI/SurfaceWindow.h"

namespace RHI
{
    class SurfaceWindowManager : public Singleton<SurfaceWindowManager>
    {
    public:
        bool Shutdown() override;
        bool Startup() override;

        [[nodiscard]] SurfaceWindow* Create(const SurfaceWindowDesc& Desc, const char* DebugName = nullptr);
        void Destroy(SurfaceWindow* Window);

        [[nodiscard]] UInt32 Count() const noexcept
        {
            return static_cast<UInt32>(mWindows.Num());
        }

        [[nodiscard]] SurfaceWindow* GetMainWindow() const
        {
            return mMainWindow;
        }

    private:
        SurfaceWindow* mMainWindow = nullptr;
        Array<SurfaceWindow*> mWindows;
    };

} // namespace RHI
