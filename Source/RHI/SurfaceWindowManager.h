#pragma once

#include "Core/Container/Array.h"
#include "Core/Singleton.h"
#include "RHI/SurfaceWindow.h"

namespace RHI
{

    class SurfaceWindowManager : public Singleton<SurfaceWindowManager>
    {
    public:
        void Shutdown() override;

        [[nodiscard]] SurfaceWindow* Create(const SurfaceWindowDesc& Desc, const char* DebugName = nullptr);
        void Destroy(SurfaceWindow* Window);

        [[nodiscard]] UInt32 Count() const noexcept
        {
            return static_cast<UInt32>(mWindows.Num());
        }

    private:
        Array<SurfaceWindow*> mWindows;
    };

} // namespace RHI
